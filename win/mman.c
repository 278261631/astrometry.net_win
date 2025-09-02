#ifdef _WIN32

#include "mman.h"
#include <windows.h>
#include <errno.h>
#include <io.h>
#include <stdio.h>

#ifndef FILE_MAP_EXECUTE
#define FILE_MAP_EXECUTE    0x0020
#endif /* FILE_MAP_EXECUTE */

static int __map_mman_error(const DWORD err, const int deferr)
{
    if (err == 0)
        return 0;

    // Map Windows errors to errno values
    switch (err) {
        case ERROR_FILE_NOT_FOUND:
        case ERROR_PATH_NOT_FOUND:
            return ENOENT;
        case ERROR_ACCESS_DENIED:
            return EACCES;
        case ERROR_NOT_ENOUGH_MEMORY:
        case ERROR_OUTOFMEMORY:
            return ENOMEM;
        case ERROR_INVALID_HANDLE:
            return EBADF;
        case ERROR_INVALID_PARAMETER:
            return EINVAL;
        case ERROR_DISK_FULL:
            return ENOSPC;
        default:
            return deferr;
    }
}

static DWORD __map_mmap_prot_page(const int prot)
{
    DWORD protect = 0;
    
    if (prot == PROT_NONE)
        return protect;
        
    if ((prot & PROT_EXEC) != 0)
    {
        protect = ((prot & PROT_WRITE) != 0) ? 
                    PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ;
    }
    else
    {
        protect = ((prot & PROT_WRITE) != 0) ?
                    PAGE_READWRITE : PAGE_READONLY;
    }
    
    return protect;
}

static DWORD __map_mmap_prot_file(const int prot)
{
    DWORD desiredAccess = 0;
    
    if (prot == PROT_NONE)
        return desiredAccess;
        
    if ((prot & PROT_READ) != 0)
        desiredAccess |= FILE_MAP_READ;
    if ((prot & PROT_WRITE) != 0)
        desiredAccess |= FILE_MAP_WRITE;
    if ((prot & PROT_EXEC) != 0)
        desiredAccess |= FILE_MAP_EXECUTE;
    
    return desiredAccess;
}

void* mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off)
{
    HANDLE fm, h;

    void * map = MAP_FAILED;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4293)
#endif

    // Get system info for allocation granularity
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    // Align offset to allocation granularity (not just page size)
    DWORD dwAllocationGranularity = si.dwAllocationGranularity;
    off_t aligned_off = (off / dwAllocationGranularity) * dwAllocationGranularity;
    size_t offset_adjustment = off - aligned_off;
    size_t adjusted_len = len + offset_adjustment;

    const DWORD dwFileOffsetLow = (sizeof(off_t) <= sizeof(DWORD)) ?
                    (DWORD)aligned_off : (DWORD)(aligned_off & 0xFFFFFFFFL);
    const DWORD dwFileOffsetHigh = (sizeof(off_t) <= sizeof(DWORD)) ?
                    (DWORD)0 : (DWORD)((aligned_off >> 32) & 0xFFFFFFFFL);
    const DWORD protect = __map_mmap_prot_page(prot);
    const DWORD desiredAccess = __map_mmap_prot_file(prot);

    const off_t maxSize = aligned_off + (off_t)adjusted_len;

    const DWORD dwMaxSizeLow = (sizeof(off_t) <= sizeof(DWORD)) ?
                    (DWORD)maxSize : (DWORD)(maxSize & 0xFFFFFFFFL);
    const DWORD dwMaxSizeHigh = (sizeof(off_t) <= sizeof(DWORD)) ?
                    (DWORD)0 : (DWORD)((maxSize >> 32) & 0xFFFFFFFFL);

#ifdef _MSC_VER
#pragma warning(pop)
#endif

    errno = 0;
    
    if (len == 0 
        /* Unsupported flag combinations */
        || (flags & MAP_FIXED) != 0
        /* Usupported protection combinations */
        || prot == PROT_EXEC)
    {
        errno = EINVAL;
        return MAP_FAILED;
    }
    
    h = ((flags & MAP_ANONYMOUS) == 0) ? 
                    (HANDLE)_get_osfhandle(fildes) : INVALID_HANDLE_VALUE;

    if ((flags & MAP_ANONYMOUS) == 0 && h == INVALID_HANDLE_VALUE)
    {
        DWORD lastError = GetLastError();
        fprintf(stderr, "mmap: Invalid file handle, GetLastError()=%lu\n", lastError);
        errno = EBADF;
        return MAP_FAILED;
    }

    fm = CreateFileMapping(h, NULL, protect, dwMaxSizeHigh, dwMaxSizeLow, NULL);

    if (fm == NULL)
    {
        DWORD lastError = GetLastError();
        fprintf(stderr, "mmap: CreateFileMapping failed, GetLastError()=%lu\n", lastError);
        errno = __map_mman_error(lastError, EPERM);
        return MAP_FAILED;
    }

    map = MapViewOfFile(fm, desiredAccess, dwFileOffsetHigh, dwFileOffsetLow, adjusted_len);

    CloseHandle(fm);

    if (map == NULL)
    {
        DWORD lastError = GetLastError();
        fprintf(stderr, "mmap: MapViewOfFile failed, GetLastError()=%lu\n", lastError);
        errno = __map_mman_error(lastError, EPERM);
        return MAP_FAILED;
    }

    // Return pointer adjusted for the original offset
    return (char*)map + offset_adjustment;
}

int munmap(void *addr, size_t len)
{
    if (UnmapViewOfFile(addr))
        return 0;
        
    errno =  __map_mman_error(GetLastError(), EPERM);
    
    return -1;
}

int mprotect(void *addr, size_t len, int prot)
{
    DWORD newProtect = __map_mmap_prot_page(prot);
    DWORD oldProtect = 0;
    
    if (VirtualProtect(addr, len, newProtect, &oldProtect))
        return 0;
    
    errno =  __map_mman_error(GetLastError(), EPERM);
    
    return -1;
}

int msync(void *addr, size_t len, int flags)
{
    if (FlushViewOfFile(addr, len))
        return 0;
    
    errno =  __map_mman_error(GetLastError(), EPERM);
    
    return -1;
}

int mlock(const void *addr, size_t len)
{
    if (VirtualLock((LPVOID)addr, len))
        return 0;
        
    errno =  __map_mman_error(GetLastError(), EPERM);
    
    return -1;
}

int munlock(const void *addr, size_t len)
{
    if (VirtualUnlock((LPVOID)addr, len))
        return 0;
        
    errno =  __map_mman_error(GetLastError(), EPERM);
    
    return -1;
}

#endif
