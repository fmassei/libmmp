#include "mmp_system.h"

/** \todo missing unittest */
long mmp_system_getPageSize(void)
{
#ifndef _WIN32
    return sysconf(_SC_PAGESIZE);
#else
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwPageSize;
#endif
}

/** \todo missing unittest */
long mmp_system_getPageAlignment(void)
{
#ifndef _WIN32
    return sysconf(_SC_PAGESIZE);
#else
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return si.dwAllocationGranularity;
#endif
}
