// GnashFileUtilities.h     File handling for Gnash
// 
//   Copyright (C) 2005, 2006, 2007, 2008, 2009 Free Software Foundation, Inc.
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

/// This file should also be included for the following system functions:
//
/// stat()
/// fstat()
/// lstat()
/// dup()
#ifndef GNASH_FILE_UTILITIES_H
#define GNASH_FILE_UTILITIES_H

#include "gnashconfig.h"
#include "dsodefs.h"

#if !defined(_MSC_VER)
# include <unistd.h>
# include <sys/stat.h>
# include <sys/types.h>
#else
#include <io.h>
#define dup _dup
#endif

#include <string>

namespace gnash {

    /// Create a directory, granting owner rwx permissions.
    //
    /// On non-POSIX systems, just create the directory.
    inline int mkdirUserPermissions(const std::string& dirname)
    {
#if !defined(_WIN32) && !defined(_MSC_VER)
        return mkdir(dirname.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
#else
        return mkdir(dirname.c_str());
#endif
    }

    /// Create a directory for a given filename.
    //
    /// Everything after the last '/' is assumed to be the filename.
    DSOEXPORT bool mkdirRecursive(const std::string& filename);

} // namespace gnash

#endif