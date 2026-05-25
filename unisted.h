/* Minimal placeholder header for unistd-like functions */
#ifndef _UNISTED_H
#define _UNISTED_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Provide a tiny subset of POSIX/unix-style APIs used in the project */
int unlink(const char *pathname);
int rmdir(const char *pathname);

#ifdef __cplusplus
}
#endif

#endif /* _UNISTED_H */