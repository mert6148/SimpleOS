/**
 * SimpleOS File System Implementation
 * Basic virtual file system with in-memory filesystem support
 */

#include "fs.h"
#include "mm.h"
#include <string.h>

#define MAX_FILES           256
#define MAX_FILESYSTEMS     16
#define MAX_INODES          1024

static file_t file_table[MAX_FILES];
static inode_t inode_cache[MAX_INODES];
static vfs_ops_t *registered_fs[MAX_FILESYSTEMS];
static u32 next_inode_num = 1;

/* ============================================================================
 * INITIALIZATION
 * =========================================================================== */

void vfs_init(void) {
    memset(file_table, 0, sizeof(file_table));
    memset(&inode_cache, 0, sizeof(inode_cache));
    memset(registered_fs, 0, sizeof(registered_fs));
    
    /* Initialize file descriptors */
    for (int i = 0; i < MAX_FILES; i++) {
        file_table[i].fd = -1;
    }
}

/* ============================================================================
 * FILE OPERATIONS
 * =========================================================================== */

s32 vfs_open(const char *path, u32 flags, mode_t mode) {
    if (!path) return KERNEL_EINVAL;
    
    /* Find free file descriptor */
    s32 fd = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].fd == -1) {
            fd = i;
            break;
        }
    }
    
    if (fd < 0) return KERNEL_EBUSY;
    
    /* TODO: Search filesystem for inode */
    /* For now, create in-memory file */
    inode_t *inode = &inode_cache[next_inode_num];
    inode->ino = next_inode_num++;
    inode->type = VFS_TYPE_REGULAR;
    inode->mode = mode;
    inode->size = 0;
    
    file_table[fd].fd = fd;
    file_table[fd].inode = inode;
    file_table[fd].flags = flags;
    file_table[fd].offset = 0;
    file_table[fd].ref_count = 1;
    
    return fd;
}

s32 vfs_close(s32 fd) {
    if (fd < 0 || fd >= MAX_FILES) return KERNEL_EINVAL;
    if (file_table[fd].fd != fd) return KERNEL_EBADF;
    
    file_table[fd].ref_count--;
    if (file_table[fd].ref_count <= 0) {
        file_table[fd].fd = -1;
        file_table[fd].inode = NULL;
    }
    
    return KERNEL_SUCCESS;
}

s32 vfs_read(s32 fd, void *buf, u32 size) {
    if (fd < 0 || fd >= MAX_FILES || !buf) return KERNEL_EINVAL;
    if (file_table[fd].fd != fd) return KERNEL_EBADF;
    
    file_t *file = &file_table[fd];
    inode_t *inode = file->inode;
    
    if (!inode || !(file->flags & VFS_O_READ)) {
        return KERNEL_EACCES;
    }
    
    /* Check bounds */
    if (file->offset >= inode->size) {
        return 0;  /* EOF */
    }
    
    u32 to_read = MIN(size, inode->size - file->offset);
    
    /* TODO: Read from actual filesystem */
    /* For now, just return 0 bytes */
    
    file->offset += to_read;
    return to_read;
}

s32 vfs_write(s32 fd, const void *buf, u32 size) {
    if (fd < 0 || fd >= MAX_FILES || !buf) return KERNEL_EINVAL;
    if (file_table[fd].fd != fd) return KERNEL_EBADF;
    
    file_t *file = &file_table[fd];
    inode_t *inode = file->inode;
    
    if (!inode || !(file->flags & VFS_O_WRITE)) {
        return KERNEL_EACCES;
    }
    
    /* Handle append mode */
    if (file->flags & VFS_O_APPEND) {
        file->offset = inode->size;
    }
    
    /* TODO: Write to actual filesystem */
    /* For now, just update offset and size */
    
    file->offset += size;
    if (file->offset > inode->size) {
        inode->size = file->offset;
    }
    
    return size;
}

u32 vfs_seek(s32 fd, s32 offset, u32 whence) {
    if (fd < 0 || fd >= MAX_FILES) return KERNEL_EINVAL;
    if (file_table[fd].fd != fd) return KERNEL_EBADF;
    
    file_t *file = &file_table[fd];
    inode_t *inode = file->inode;
    
    u32 new_offset = file->offset;
    
    switch (whence) {
        case VFS_SEEK_SET:
            new_offset = offset;
            break;
        case VFS_SEEK_CUR:
            new_offset += offset;
            break;
        case VFS_SEEK_END:
            new_offset = inode->size + offset;
            break;
        default:
            return KERNEL_EINVAL;
    }
    
    file->offset = new_offset;
    return new_offset;
}

/* ============================================================================
 * DIRECTORY OPERATIONS
 * =========================================================================== */

s32 vfs_mkdir(const char *path, mode_t mode) {
    /* TODO: Create directory in filesystem */
    return KERNEL_SUCCESS;
}

s32 vfs_unlink(const char *path) {
    /* TODO: Delete file from filesystem */
    return KERNEL_SUCCESS;
}

s32 vfs_rmdir(const char *path) {
    /* TODO: Delete directory from filesystem */
    return KERNEL_SUCCESS;
}

s32 vfs_readdir(s32 fd, dirent_t *entries, u32 count) {
    if (fd < 0 || fd >= MAX_FILES) return KERNEL_EINVAL;
    if (file_table[fd].fd != fd) return KERNEL_EBADF;
    
    /* TODO: Read directory entries */
    return 0;
}

/* ============================================================================
 * FILE INFORMATION
 * =========================================================================== */

s32 vfs_stat(const char *path, inode_t *inode) {
    if (!path || !inode) return KERNEL_EINVAL;
    
    /* TODO: Look up inode in filesystem */
    return KERNEL_ENOENT;
}

s32 vfs_fstat(s32 fd, inode_t *inode) {
    if (fd < 0 || fd >= MAX_FILES || !inode) return KERNEL_EINVAL;
    if (file_table[fd].fd != fd) return KERNEL_EBADF;
    
    /* Copy inode info */
    memcpy(inode, file_table[fd].inode, sizeof(inode_t));
    return KERNEL_SUCCESS;
}

/* ============================================================================
 * FILESYSTEM REGISTRATION
 * =========================================================================== */

void vfs_register_fs(const char *name, vfs_ops_t *ops) {
    if (!name || !ops) return;
    
    /* Find free slot */
    for (int i = 0; i < MAX_FILESYSTEMS; i++) {
        if (!registered_fs[i]) {
            registered_fs[i] = ops;
            printk("VFS: Registered filesystem '%s'\n", name);
            return;
        }
    }
}

s32 vfs_mount(const char *path, const char *fstype, const char *device) {
    if (!path || !fstype) return KERNEL_EINVAL;
    
    /* TODO: Find registered filesystem and mount */
    return KERNEL_SUCCESS;
}

s32 vfs_umount(const char *path) {
    if (!path) return KERNEL_EINVAL;
    
    /* TODO: Unmount filesystem */
    return KERNEL_SUCCESS;
}
