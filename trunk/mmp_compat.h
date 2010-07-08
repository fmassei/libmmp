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
#ifndef H_MMP_COMPAT_H
#define H_MMP_COMPAT_H

/* restrict features */
#if !(__GNUC__ == 2 && __GNUC_MINOR__ == 95)
#   if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901
#       define __restrict
#       define __inline
#   else
#       define __restrict   restrict
#       define __inline     inline
#   endif
#endif

/* unix strncpy() */
#ifdef _WIN32
#   define xstrncpy(_S1, _S2, _N)   strcpy_s((_S1), (_N), (_S2))
#else
#   define xstrncpy(_S1, _S2, _N)   strncpy((_S1), (_S2), (_N))
#endif

/* unix strerror() */
#define xstrerror(_E)   strerror(_E)

#endif /* H_MMP_COMPAT_H */
