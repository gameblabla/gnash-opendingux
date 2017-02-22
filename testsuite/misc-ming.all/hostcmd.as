// hostcmd.as - Non-predefined FSCommand invocation tests
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
//     To run this test against proprietary player, use HTML-based
//     test runners "hostcmd_htmltest_v*.html".
//
// Original author: Nutchanon Wetchasit <Nutchanon.Wetchasit@gmail.com>
//

// Once this Flash movie is run, it will issue non-predefined FSCommand
// via MovieClip.getURL() in mulitple combination of command and arguments;
// which will be forwarded to a handling function in host container-side
// scripting language; most likely `*_DoFSCommand(cmd,arg)` function
// in JavaScript (where "*" is the value of `name` attribute on `<object>`
// or `<embed>` tag which the SWF is playing).

var object_customstringarg;

trace("STARTOFTEST");
this.getURL("FSCommand:");
this.getURL("FSCommand:", "This is a string for empty call");
this.getURL("FSCommand:noarg");
this.getURL("FSCommand:stringarg", "This is a string");
this.getURL("FSCommand:weirdstringarg", "!@#$%^&*()_+-={}|[]\\:\";\'<>?,./~`");
this.getURL("FSCommand:integerarg", 9876);
this.getURL("FSCommand:floatarg", 9876.5432);
this.getURL("FSCommand:infinitearg", Infinity);
this.getURL("FSCommand:neginfinitearg", -Infinity);
this.getURL("FSCommand:nanarg", NaN);
this.getURL("FSCommand:booleanarg", true);
this.getURL("FSCommand:nullarg", null);
this.getURL("FSCommand:undefinedarg", undefined);
this.getURL("FSCommand:arrayarg", new Array("The", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog"));
this.getURL("FSCommand:objectarg", new Object());
object_customstringarg = new Object();
object_customstringarg.toString = function():String {
	return "This is a custom Object.toString()";
};
this.getURL("FSCommand:object_customstringarg", object_customstringarg);
trace("ENDOFTEST");
