/**
 * SimpleOS RAM Filesystem
 * Simple in-memory filesystem for bootstrapping
 * Stores files entirely in RAM without disk persistence
 */

#include "kernel_types.h"
#include "fs.h"
#include "mm.h"

#define RAMFS_MAX_FILES 256
#define RAMFS_MAX_SIZE (10 * 1024 * 1024)  /* 10MB max RAM filesystem */

/* File entry in RAM filesystem */
typedef struct ramfs_file {
    char name[256];
    u8 *data;
    u32 size;
    u32 capacity;
    u32 inode_num;
    u32 created;
    u32 modified;
} ramfs_file_t;

static ramfs_file_t files[RAMFS_MAX_FILES];
static u32 file_count = 0;
static u32 total_size = 0;

/**
 * Initialize RAM filesystem
 */
s32 ramfs_init(void) {
    for (u32 i = 0; i < RAMFS_MAX_FILES; i++) {
        files[i].name[0] = '\0';
        files[i].data = NULL;
        files[i].size = 0;
        files[i].capacity = 0;
        files[i].inode_num = 0;
    }
    
    file_count = 0;
    total_size = 0;
    
    return KERNEL_SUCCESS;
}

/**
 * Create a file in RAM filesystem
 */
s32 ramfs_create(const char *name) {
    if (file_count >= RAMFS_MAX_FILES) {
        return KERNEL_ENOMEM;
    }
    
    /* Find existing file */
    for (u32 i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, name) == 0) {
            return KERNEL_EEXIST;  /* File exists */
        }
    }
    
    /* Create new file */
    ramfs_file_t *f = &files[file_count];
    strncpy(f->name, name, sizeof(f->name) - 1);
    f->name[sizeof(f->name) - 1] = '\0';
    
    f->data = NULL;
    f->size = 0;
    f->capacity = 0;
    f->inode_num = file_count + 1;
    f->created = 0;  /* Would use get_ticks() */
    f->modified = 0;
    
    file_count++;
    
    return f->inode_num;
}

/**
 * Write to file in RAM filesystem
 */
s32 ramfs_write(const char *name, const void *data, u32 len, u32 offset) {
    for (u32 i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, name) == 0) {
            ramfs_file_t *f = &files[i];
            
            u32 needed = offset + len;
            
            /* Expand if needed */
            if (needed > f->capacity) {
                u32 new_cap = (needed + 4095) & ~4095;  /* Align to 4KB */
                
                /* Check total size limit */
                if (total_size - f->capacity + new_cap > RAMFS_MAX_SIZE) {
                    return KERNEL_ENOMEM;
                }
                
                u8 *new_data = kernel_malloc(new_cap);
                if (!new_data) {
                    return KERNEL_ENOMEM;
                }
                
                if (f->data) {
                    memcpy(new_data, f->data, f->size);
                    kernel_free(f->data);
                }
                
                f->data = new_data;
                f->capacity = new_cap;
                total_size += (new_cap - f->capacity);
            }
            
            /* Copy data */
            memcpy(f->data + offset, data, len);
            
            if (offset + len > f->size) {
                f->size = offset + len;
            }
            
            f->modified = 0;  /* Would use get_ticks() */
            
            return len;
        }
    }
    
    return KERNEL_ENOENT;
}

/**
 * Read from file in RAM filesystem
 */
s32 ramfs_read(const char *name, void *buffer, u32 len, u32 offset) {
    for (u32 i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, name) == 0) {
            ramfs_file_t *f = &files[i];
            
            if (offset >= f->size) {
                return 0;  /* EOF */
            }
            
            u32 to_read = (offset + len > f->size) ? 
                          (f->size - offset) : len;
            
            memcpy(buffer, f->data + offset, to_read);
            
            return to_read;
        }
    }
    
    return KERNEL_ENOENT;
}

/**
 * Delete file from RAM filesystem
 */
s32 ramfs_unlink(const char *name) {
    for (u32 i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, name) == 0) {
            ramfs_file_t *f = &files[i];
            
            if (f->data) {
                total_size -= f->capacity;
                kernel_free(f->data);
            }
            
            /* Remove from list */
            for (u32 j = i; j < file_count - 1; j++) {
                files[j] = files[j + 1];
            }
            
            file_count--;
            return KERNEL_SUCCESS;
        }
    }
    
    return KERNEL_ENOENT;
}

/**
 * Get file information
 */
s32 ramfs_stat(const char *name, inode_t *inode) {
    for (u32 i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, name) == 0) {
            ramfs_file_t *f = &files[i];
            
            inode->ino = f->inode_num;
            inode->type = INODE_REGULAR;
            inode->mode = 0644;
            inode->uid = 0;
            inode->gid = 0;
            inode->size = f->size;
            inode->atime = f->created;
            inode->mtime = f->modified;
            inode->ctime = f->created;
            inode->nlinks = 1;
            
            return KERNEL_SUCCESS;
        }
    }
    
    return KERNEL_ENOENT;
}

/**
 * List files in RAM filesystem
 */
s32 ramfs_list(const char *dirname, direntry_t *entries, u32 max_entries) {
    if (strlen(dirname) > 0 && strcmp(dirname, "/") != 0) {
        return 0;  /* Only "/" directory exists */
    }
    
    u32 count = 0;
    for (u32 i = 0; i < file_count && count < max_entries; i++) {
        strncpy(entries[count].filename, files[i].name, 
                sizeof(entries[count].filename) - 1);
        entries[count].filename[sizeof(entries[count].filename) - 1] = '\0';
        entries[count].inode = files[i].inode_num;
        entries[count].type = INODE_REGULAR;
        
        count++;
    }
    
    return count;
}

/* String helper functions (if not provided by libc) */
static int strcmp(const char *a, const char *b) {
    while (*a && *b && *a == *b) {
        a++;
        b++;
    }
    return *a - *b;
}

static char *strncpy(char *dst, const char *src, int n) {
    while (n-- && (*dst++ = *src++))
        ;
    return dst;
}

static int strlen(const char *s) {
    int len = 0;
    while (*s++)
        len++;
    return len;
}

static void *memcpy(void *dst, const void *src, u32 n) {
    u8 *d = (u8 *)dst;
    const u8 *s = (const u8 *)src;
    while (n--)
        *d++ = *s++;
    return dst;
}
