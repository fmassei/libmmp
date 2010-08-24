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

/** \todo missing unittest */
void *mmp_mmap(void *ptr, size_t size, int prot, int flags, int fd,
                                                                long offset)
{
#ifndef _WIN32
    return mmap(ptr, size, prot, flags, fd, offset);
#else
    slwait(&g_sl);
    ptr = VirtualAlloc(ptr, size, MEM_RESERVE | MEM_COMMIT | MEM_TOP_DOWN,
                        PAGE_READWRITE);
    if (ptr==NULL)
        ptr = MAP_FAILED;
    slrelease(&g_sl);
    return ptr;
    UNREFERENCED_PARAM(prot);
    UNREFERENCED_PARAM(flags);
    UNREFERENCED_PARAM(fd);
    UNREFERENCED_PARAM(offset);
#endif
}

/** \todo missing unittest */
int mmp_munmap(void *start, size_t length)
{
#ifndef _WIN32
    return munmap(start, length);
#else
    int ret = 0;
    slwait(&g_sl);
    if (!VirtualFree(start, 0, MEM_RELEASE))
        ret = -1;
    slrelease(&g_sl);
    return ret;
    UNREFERENCED_PARAM(length);
#endif
}

