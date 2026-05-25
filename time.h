/* Minimal time.h for SimpleOS */
#ifndef _TIME_H
#define _TIME_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef long time_t;
time_t time(time_t *t);

#ifdef __cplusplus
}
#endif

#endif /* _TIME_H */