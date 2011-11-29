// ext.h
#pragma once
#pragma comment(lib, "shlwapi")
#include <shlwapi.h>
#include <time.h>

#ifdef _exp
#define _dll __declspec(dllexport)
#else
#define _dll __declspec(dllimport)
#endif

void _dll getDir(char* dir, rsize_t _DstSize, const char* file);
void _dll getMonth(char* buf, rsize_t _DstSize, const struct tm* timeptr);
void _dll getName(char* name, rsize_t _DstSize, const char* file);