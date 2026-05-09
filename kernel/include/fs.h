/**
 * SimpleOS Kernel - File System Interface
 * Provides virtual file system abstraction and basic filesystem operations
 */

#ifndef KERNEL_FS_H
#define KERNEL_FS_H

#include "kernel_types.h"

/* ============================================================================
 * VFS - VIRTUAL FILE SYSTEM
 * =========================================================================== */

/* File types */
typedef enum {
    VFS_TYPE_REGULAR = 0,       /* Regular file */
    VFS_TYPE_DIRECTORY = 1,     /* Directory */
    VFS_TYPE_SYMLINK = 2,       /* Symbolic link */
    VFS_TYPE_CHARDEV = 3,       /* Character device */
    VFS_TYPE_BLOCKDEV = 4,      /* Block device */
    VFS_TYPE_PIPE = 5           /* FIFO/Pipe */
} vfs_type_t;

/* File open flags */
#define VFS_O_READ          0x001
#define VFS_O_WRITE         0x002
#define VFS_O_APPEND        0x004
#define VFS_O_CREAT         0x008
#define VFS_O_EXCL          0x010
#define VFS_O_TRUNC         0x020

/* File seek modes */
#define VFS_SEEK_SET        0           /* Beginning of file */
#define VFS_SEEK_CUR        1           /* Current position */
#define VFS_SEEK_END        2           /* End of file */

/* ============================================================================
 * INODE (File Metadata)
 * =========================================================================== */

typedef struct inode {
    ino_t ino;                          /* Inode number */
    vfs_type_t type;                    /* File type */
    mode_t mode;                        /* File permissions */
    uid_t uid;                          /* User ID */
    gid_t gid;                          /* Group ID */
    u32 size;                           /* File size in bytes */
    u32 block_count;                    /* Number of blocks used */
    u64 created_time;                   /* Creation timestamp */
    u64 modified_time;                  /* Modification timestamp */
    u64 accessed_time;                  /* Access timestamp */
    u32 link_count;                     /* Hard link count */
    
    /* Filesystem specific data */
    void *fs_data;
    struct vfs_ops *ops;                /* Filesystem operations */
} inode_t;

/* ============================================================================
 * FILE DESCRIPTOR
 * =========================================================================== */

typedef struct {
    s32 fd;                             /* File descriptor number */
    inode_t *inode;                     /* Associated inode */
    u32 flags;                          /* Open flags */
    u32 offset;                         /* Current read/write position */
    u32 ref_count;                      /* Reference count */
} file_t;

/* ============================================================================
 * DIRECTORY ENTRY
 * =========================================================================== */

typedef struct {
    char name[256];
    ino_t ino;
    vfs_type_t type;
} dirent_t;

/* ============================================================================
 * FILESYSTEM OPERATIONS (VFS Interface)
 * =========================================================================== */

typedef struct vfs_ops {
    /* Inode operations */
    s32 (*read)(inode_t *inode, char *buf, u32 size, u32 offset);
    s32 (*write)(inode_t *inode, const char *buf, u32 size, u32 offset);
    s32 (*getattr)(inode_t *inode);
    s32 (*setattr)(inode_t *inode);
    void (*truncate)(inode_t *inode, u32 size);
    
    /* Directory operations */
    s32 (*lookup)(inode_t *dir, const char *name, inode_t **result);
    s32 (*mkdir)(inode_t *dir, const char *name, mode_t mode);
    s32 (*rmdir)(inode_t *dir, const char *name);
    s32 (*readdir)(inode_t *dir, u32 offset, dirent_t *entries, u32 count);
    
    /* File operations */
    s32 (*create)(inode_t *dir, const char *name, mode_t mode);
    s32 (*unlink)(inode_t *dir, const char *name);
    s32 (*symlink)(inode_t *dir, const char *name, const char *target);
    
    /* Cleanup */
    void (*release)(inode_t *inode);
} vfs_ops_t;

/* ============================================================================
 * FILESYSTEM INTERFACE
 * =========================================================================== */

/**
 * Initialize file system
 */
void vfs_init(void);

/**
 * Open a file
 * @param path File path
 * @param flags Open flags (VFS_O_*)
 * @param mode Permission mode
 * @return File descriptor or error
 */
s32 vfs_open(const char *path, u32 flags, mode_t mode);

/**
 * Close a file
 */
s32 vfs_close(s32 fd);

/**
 * Read from file
 * @param fd File descriptor
 * @param buf Buffer to read into
 * @param size Number of bytes to read
 * @return Bytes read or error code
 */
s32 vfs_read(s32 fd, void *buf, u32 size);

/**
 * Write to file
 * @param fd File descriptor
 * @param buf Data to write
 * @param size Number of bytes to write
 * @return Bytes written or error code
 */
s32 vfs_write(s32 fd, const void *buf, u32 size);

/**
 * Seek in file
 * @param fd File descriptor
 * @param offset Offset
 * @param whence Seek mode (VFS_SEEK_*)
 * @return New position or error
 */
u32 vfs_seek(s32 fd, s32 offset, u32 whence);

/**
 * Create a directory
 */
s32 vfs_mkdir(const char *path, mode_t mode);

/**
 * Remove a file
 */
s32 vfs_unlink(const char *path);

/**
 * Remove a directory
 */
s32 vfs_rmdir(const char *path);

/**
 * List directory contents
 */
s32 vfs_readdir(s32 fd, dirent_t *entries, u32 count);

/**
 * Get file information
 */
s32 vfs_stat(const char *path, inode_t *inode);

/**
 * Get file information from descriptor
 */
s32 vfs_fstat(s32 fd, inode_t *inode);

/**
 * Register a filesystem driver
 */
void vfs_register_fs(const char *name, vfs_ops_t *ops);

/**
 * Mount a filesystem
 */
s32 vfs_mount(const char *path, const char *fstype, const char *device);

/**
 * Unmount a filesystem
 */
s32 vfs_umount(const char *path);

#endif /* KERNEL_FS_H */
