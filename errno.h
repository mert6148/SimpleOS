/* Minimal errno.h for SimpleOS */
#if !defined(_ERRNO_H)
#define _ERRNO_H

extern int errno;

#define EINVAL 22
#define ENOMEM 12
#define EACCES 13

#endif /* _ERRNO_H */