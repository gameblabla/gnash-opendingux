// submoviegetvar.sc - Built-in GetVariable() plugin function tests
//                     with nested MovieClip
//
//   Copyright (C) 2017 Free Software Foundation, Inc.
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
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
//
// Note:
//     This SWF file does not run any test by itself (apart from
//     few sanity checks), as this is a test on plugin's host interface.
//
//     To run this test against proprietary player, use a HTML-based
//     test runner "submoviegetvar.html".
//
// Original author: Nutchanon Wetchasit <Nutchanon.Wetchasit@gmail.com>
//

// Once this Flash movie is run, it stores string value to variable
// inside root movie, first-level sub-MovieClip, and second-level
// sub-MovieClip; which could be accessed by plugin's `GetVariable(path)`
// function.

.flash bbox=320x240 background=white version=7 fps=12
.frame 1
    .action:
        #include "Dejagnu.sc"
    .end
    .sprite SubMovieClip
        .frame 1
            .sprite NestedMovieClip
                .frame 1
                    .action:
                        var nestedmovievar="This is nestedmovievar";
                        check(true);
                    .end
            .end
            .put nestedmovie=NestedMovieClip
            .action:
                var submovievar="This is submovievar";
                check(true);
            .end
    .end
    .put submovie=SubMovieClip
    .action:
        var movievar="This is movievar";
        check(true);
        trace("ENDOFTEST");
    .end
.end
