/*
    Copyright 2010 Francesco Massei

    This file is part of the libmmp library.

        libmmp is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libmmp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libmmp.  If not, see <http://www.gnu.org/licenses/>.
*/
/** \file   mmp_h_utils.h
 * \brief   header file utility
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_H_UTILS_H
#define H_MMP_H_UTILS_H

#ifdef HAVE_CONFIG_H
#   include "config.h"
#endif /* HAVE_CONFIG_H */

/** \def MMP_CEXTERN_BEGIN
 * \brief begin extern "C" if compiling in C++ */
/** \def MMP_CEXTERN_END
 * \brief end exter "C" if compiling in C++ */
#ifdef __cplusplus
#   define MMP_CEXTERN_BEGIN    extern "C" {
#   define MMP_CEXTERN_END      }
#else
#   define MMP_CEXTERN_BEGIN
#   define MMP_CEXTERN_END
#endif

#ifndef _WIN32
#   define MMP_API    extern
#else
#   ifdef MMP_EXPORTS
#       define MMP_API __declspec(dllexport)
#   else
#       define MMP_API __declspec(dllimport)
#   endif
#endif

/** \brief unreferenced parameter macro */
#ifndef UNREFERENCED_PARAM
#   define UNREFERENCED_PARAM(_P) ((void)(_P))
#endif

#endif /* H_MMP_H_UTILS_H */
