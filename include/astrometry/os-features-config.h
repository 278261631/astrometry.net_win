/* This file is generated for Windows build */
#ifndef OS_FEATURES_CONFIG_H
#define OS_FEATURES_CONFIG_H

/* Windows doesn't have netpbm by default */
#define HAVE_NETPBM 0

/* Git information for Windows build */
#define AN_GIT_URL "https://github.com/dstndstn/astrometry.net"
#define AN_GIT_REVISION "windows-build"
#define AN_GIT_DATE "2024-01-01"

/* Windows specific definitions */
#ifdef _WIN32
#define HAVE_WINSOCK2 1
#define HAVE_WINDOWS_H 1
#else
#define HAVE_WINSOCK2 0
#define HAVE_WINDOWS_H 0
#endif

#endif /* OS_FEATURES_CONFIG_H */
