/*
    Copyright 2010 Francesco Massei

    This file is part of the MMP library.

        MMP is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MMP.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "mmp_files.h"

/** \todo missing unittest */
int mmp_open(const char *path, int flags, int mode)
{
#ifndef _WIN32
    return open(path, flags, mode);
#else
    int fd;
    if (_sopen_s(&fd, path, flags, _SH_DENYNO, _S_IREAD|_S_IWRITE)!=0)
        fd = -1;
    return fd;
    UNREFERENCED_PARAM(mode);
#endif
}

/** \todo missing unittest */
int mmp_read(int fd, void *buf, size_t count)
{
#ifndef _WIN32
    return read(fd, buf, count);
#else
    return _read(fd, buf, count);
#endif
}

/** \todo missing unittest */
int mmp_write(int fd, const void *buf, size_t count)
{
#ifndef _WIN32
    return write(fd, buf, count);
#else
    return _write(fd, buf, count);
#endif
}

/** \todo missing unittest */
int mmp_close(int fd)
{
#ifndef _WIN32
    return close(fd);
#else
    return _close(fd);
#endif
}

/** \todo missing unittest */
int mmp_pwrite(int fd, const void *buf, size_t nbyte, long offset)
{
#ifndef _WIN32
    return pwrite(fd, buf, nbyte, offset);
#else
    long pos, ret;
    pos = _tell(fd);
    _lseek(fd, offset, SEEK_SET);
    ret = _write(fd, buf, nbyte);
    _lseek(fd, pos, SEEK_SET);
    return ret;
#endif
}

/** \todo missing unittest */
int mmp_stat(const char * __restrict path, t_mmp_stat_s * __restrict stat_ptr)
{
#ifndef _WIN32
    return stat(path, stat_ptr);
#else
    return _stat(path, stat_ptr);
#endif
}

/** \todo missing unittest */
int mmp_fstat(int fd, t_mmp_stat_s * stat_ptr)
{
#ifndef _WIN32
    return fstat(fd, stat_ptr);
#else
    return _fstat(fd, stat_ptr);
#endif
}

#ifdef UNIT_TESTING
static t_mmp_tap_result_e test_stat(void)
{
    return MMP_TAP_UNTESTED;
}

ret_t mmp_files_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if ((ret = mmp_tap_test(cycle, "mmp_stat", "unimplemented",
                                                test_stat()))!=MMP_ERR_OK)
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */
