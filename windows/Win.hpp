#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <BaseTsd.h>
#include <shlobj.h>
#include <io.h>
#include <Shlwapi.h>

typedef SSIZE_T ssize_t;
typedef int socklen_t;

#define NI_MAXHOST 1025
#define NI_MAXSERV 32
#define PATH_MAX 4096

#pragma warning( disable : 4290 )
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shlwapi.lib")

struct timespec {
	long tv_sec;
	long tv_nsec;
};

typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;

static const char *dir_separator = "\\";

/* msvc is really super ugly */
#undef min
#undef max