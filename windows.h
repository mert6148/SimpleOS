/* Minimal cross-platform windows.h stub for SimpleOS (non-Windows platforms) */
#ifndef _WINDOWS_H
#define _WINDOWS_H

#ifdef _WIN32
#include <windows.h>
#else
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#endif

#endif /* _WINDOWS_H */