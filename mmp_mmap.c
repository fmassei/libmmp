#include "mmp_mmap.h"

#ifdef _WIN32
/* win32 spinlock functions for VirtualAlloc()/VirtualFree() */
static long g_sl;
static int slwait(long *sl)
{
    while(InterlockedCompareExchange(sl, 1, 0)!=0)
        Sleep(0);
    return 0;
}
static int slrelease(long *sl)
{
    InterlockedExchange(sl, 0);
    return 0;
}
#endif

/** \todo this is WRONG! */
void *mmp_mmap(void *ptr, size_t size, int prot, int flags, int fd,
                                                                long offset)
{
#ifndef _WIN32
    return mmap(ptr, size, prot, flags, fd, offset);
#else
    HANDLE hfile, hmap;
    void *map;
    if ((hfile = (HANDLE)_get_osfhandle(fd))==INVALID_HANDLE_VALUE)
        return NULL;
    if ((hmap = CreateFileMapping(hfile, NULL, PAGE_READWRITE, 0, size+offset, NULL))==NULL)
        return NULL;
    if ((map = MapViewOfFile(hmap, FILE_MAP_WRITE, 0, offset, size))==NULL)
        return NULL;
    return map;
#endif
}

/** \todo this is WRONG! */
int mmp_munmap(void *start, size_t length)
{
#ifndef _WIN32
    return munmap(start, length);
#else
    return UnmapViewOfFile(start);
    UNREFERENCED_PARAM(length);
#endif
}

