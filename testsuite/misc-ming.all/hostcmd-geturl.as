// hostcmd-geturl.as - getURL()/MovieClip.getURL() target tests
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
//     This SWF file does not run any test by itself as this is a test on
//     plugin's host interface.
//
// Original author: Nutchanon Wetchasit <Nutchanon.Wetchasit@gmail.com>
//

// Once this Flash movie is run, it will issue `getURL()` call to host
// container via a built-in `getURL(url,target)` function and
// via `MovieClip.getURL(url,target)` with unconventional `target` values;
// each issued call will be checked for correctness by the emulated
// host container.
//
// In real browser environment, this test will not run in a useful manner
// as the early `getURL()` call will redirect test page to other file
// before the later tests could run.

trace("STARTOFTEST");

getURL("geturl-emptytarget.html", "");

#if OUTPUT_VERSION >= 5
getURL("geturl-notarget.html");
getURL("geturl-undeftarget.html", undefined);
getURL("geturl-nulltarget.html", null);

this.getURL("mcgeturl-emptytarget.html", "");
this.getURL("mcgeturl-notarget.html");
this.getURL("mcgeturl-undeftarget.html", undefined);
this.getURL("mcgeturl-nulltarget.html", null);
#endif

trace("ENDOFTEST");
