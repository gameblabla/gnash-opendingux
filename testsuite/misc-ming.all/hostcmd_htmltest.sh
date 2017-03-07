#!/bin/sh

#
# hostcmd_htmltest.sh - HTML-based non-predefined FSCommand invocation
#                       test runner generator
#
# Copyright (C) 2017 Free Software Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#
#
# Original author: Nutchanon Wetchasit <Nutchanon.Wetchasit@gmail.com>
#
# The generated test runner checks Gnash (or proprietary player) for:
#  * Support of FSCommand call via MovieClip.getURL() (bug #46944)
#        <https://savannah.gnu.org/bugs/?46944>
#  * Undefined FSCommand parameter passing via MovieClip.getURL() (bug #50393)
#        <https://savannah.gnu.org/bugs/?50393>
#
# Usage:
#     ./hostcmd_htmltest.sh <swfversion> <swf>
#
#     The <swf> must be a valid URL name and must not contain '&',
#     as this test generator does not escape SWF file name.
#
# Generated test runner's usage:
#     Open the generated test HTML file under web browser with JavaScript and
#     plug-ins enabled, wait few seconds for the test to run. Once finished,
#     the result will be shown on browser screen, with "There should be NN
#     tests run" as the last line.
#
#     When testing with proprietary player, the test directory might need
#     to be added as player's trusted location, or tests would fail due to
#     sandbox violation. Alternative is putting the test directory
#     on a web server and run the test online.
#
# Note:
#     The generated test runner does not count checks run inside SWF file
#     (because there aren't any).
#

# Check for generation parameters
while getopts "" name
do
	case $name in
		?)
			echo "Usage: $0 <swfversion> <swf>" >&2
			exit 1;;
	esac
done
shift $(($OPTIND - 1))
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 <swfversion> <swf>" >&2
	exit 1
fi

# Load generation parameters
swfversion=$1
shift
swf=$1

cat << EOF
<html>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=us-ascii">
	<title>Host Communication FSCommand Test</title>
</head>
<body>
<div id="flashstate">
	<noscript>
		<strong>ERROR: JavaScript is not enabled, tests will not run</strong>
	</noscript>
</div>
<object id="player" name="player" data="${swf}" width="320" height="240" type="application/x-shockwave-flash">
	<param name="movie" value="${swf}">
	<param name="AllowScriptAccess" value="always">
	<div>
		<strong>ERROR: SWF failed to load, tests will not run correctly</strong>
	</div>
</object>
<script type="text/javascript">
<!--
	var tested = 0;
	var passed = 0;
	var failed = 0;
	var xpassed = 0;
	var xfailed = 0;

	function check(op, msg) {
		var flashstatus;
		var failureline;

		flashstatus=document.getElementById("flashstate");
		tested++;
		if(op) {
			passed++;
			flashstatus.appendChild(document.createTextNode("PASSED: " + msg));
		} else {
			failed++;
			failureline=document.createElement("strong");
			failureline.appendChild(document.createTextNode("FAILED: " + msg));
			flashstatus.appendChild(failureline);
		}
		flashstatus.appendChild(document.createElement("br"));
	}

	function check_equals(op1, op2, msg) {
		var flashstatus;
		var failureline;

		flashstatus=document.getElementById("flashstate");
		tested++;
		if(op1 == op2) {
			passed++;
			flashstatus.appendChild(document.createTextNode("PASSED: " + msg));
		} else {
			failed++;
			failureline=document.createElement("strong");
			failureline.appendChild(document.createTextNode("FAILED: " + msg + " (\"" + op1 + "\" != \"" + op2 + "\")"));
			flashstatus.appendChild(failureline);
		}
		flashstatus.appendChild(document.createElement("br"));
	}

	function xcheck_equals(op1, op2, msg) {
		var flashstatus;
		var successline;

		flashstatus=document.getElementById("flashstate");
		tested++;
		if(op1 == op2) {
			xpassed++;
			successline=document.createElement("strong");
			successline.appendChild(document.createTextNode("XPASSED: "+msg));
			flashstatus.appendChild(successline);
		} else {
			xfailed++;
			flashstatus.appendChild(document.createTextNode("XFAILED: " + msg + " (\"" + op1 + "\" != \"" + op2 + "\")"));
		}
		flashstatus.appendChild(document.createElement("br"));
	}

	function check_error(msg) {
		var flashstatus;
		var errorline;

		flashstatus=document.getElementById("flashstate");
		errorline=document.createElement("strong");
		errorline.appendChild(document.createTextNode("ERROR: " + msg));
		flashstatus.appendChild(errorline);
		flashstatus.appendChild(document.createElement("br"));
	}

	function check_totals(total) {
		check_equals(tested,total,"There should be "+total+" tests run");
	}

	function xcheck_totals(total) {
		xcheck_equals(tested,total,"There should be "+total+" tests run");
	}

	var noname_call = 0;
	var noarg_call = 0;
	var stringarg_call = 0;
	var weirdstringarg_call = 0;
	var integerarg_call = 0;
	var floatarg_call = 0;
	var infinitearg_call = 0;
	var neginfinitearg_call = 0;
	var nanarg_call = 0;
	var booleanarg_call = 0;
	var nullarg_call = 0;
	var undefinedarg_call = 0;
	var arrayarg_call = 0;
	var objectarg_call = 0;
	var object_customstringarg_call = 0;
	var functionarg_call = 0;
	var m_noarg_call = 0;
	var m_stringarg_call = 0;
	var m_weirdstringarg_call = 0;
	var m_integerarg_call = 0;
	var m_floatarg_call = 0;
	var m_infinitearg_call = 0;
	var m_neginfinitearg_call = 0;
	var m_nanarg_call = 0;
	var m_booleanarg_call = 0;
	var m_nullarg_call = 0;
	var m_undefinedarg_call = 0;
	var m_arrayarg_call = 0;
	var m_objectarg_call = 0;
	var m_object_customstringarg_call = 0;
	var m_functionarg_call = 0;

	function player_DoFSCommand(cmd, arg) {
		if("" == cmd) {
			noname_call++;
			if (noname_call == 1) {
				check_equals(typeof(arg), "string", "getURL-based no-name FSCommand call with no parameter should pass string-type parameter");
				check_equals(arg, "", "getURL-based no-name FSCommand call with no-parameter should pass an empty string parameter");
			} else if(noname_call == 2) {
				check_equals(typeof(arg), "string", "getURL-based no-name FSCommand call with string parameter should pass string-type parameter");
				check_equals(arg, "This is a string for empty call", "getURL-based no-name FSCommand call with string parameter should pass a correct string parameter value");
			} else if(noname_call == 3) {
				check_equals(typeof(arg), "string", "MovieClip-based no-name FSCommand call with no parameter should pass string-type parameter");
				check_equals(arg, "", "MovieClip-based no-name FSCommand call with no-parameter should pass an empty string parameter");
			} else if(noname_call == 4) {
				check_equals(typeof(arg), "string", "MovieClip-based no-name FSCommand call with string parameter should pass string-type parameter");
				check_equals(arg, "This is a string for empty call", "MovieClip-based no-name FSCommand call with string parameter should pass a correct string parameter value");
			} else {
				check_error("Unknown no-name FSCommand issued: \"" + cmd + "\" parameter \"" + arg + "\"");
			}
		} else if("noarg" == cmd) {
			noarg_call++;
			check_equals(typeof(arg), "string", "getURL-based FSCommand call with no parameter should pass string-type parameter");
			check_equals(arg, "", "getURL-based FSCommand call with no parameter should pass an empty string parameter value");
		} else if("stringarg" == cmd) {
			stringarg_call++;
			check_equals(typeof(arg), "string", "getURL-based FSCommand call with string parameter should pass string-type parameter");
			check_equals(arg, "This is a string", "getURL-based FSCommand call with string parameter should pass a correct string parameter value");
		} else if("weirdstringarg" == cmd) {
			// Currently, this FSCommand won't run under libgnashplugin,
			// due to its internal parameter parsing issue.

			weirdstringarg_call++;
			xcheck_equals(typeof(arg),"string", "getURL-based FSCommand call with string parameter full of symbols should pass string-type parameter");
			xcheck_equals(arg,"!@#\$%^&*()_+-={}|[]\\\\:\";\'<>?,./~\`", "Full-of-symbols string parameter value of getURL-based FSCommand call should be passed correctly");
		} else if("integerarg" == cmd) {
			integerarg_call++;
			check_equals(typeof(arg), "string", "getURL-based FSCommand call with integer parameter should pass string-type parameter");
			check_equals(arg, "9876", "getURL-based FSCommand call should pass a correct string representation of integer parameter value");
		} else if("floatarg" == cmd) {
			floatarg_call++;
			check_equals(typeof(arg), "string", "getURL-based FSCommand call with floating point parameter should pass string-type parameter");
			check_equals(arg, "9876.5432", "getURL-based FSCommand call should pass a correct string representation of floating point parameter value");
		} else if("infinitearg" == cmd) {
			infinitearg_call++;
			check_equals(typeof(arg), "string", "getURL-based FSCommand call with infinity parameter should pass string-type parameter");
			check_equals(arg, "Infinity", "getURL-based FSCommand call should pass a correct string representation of infinity parameter");
		} else if("neginfinitearg" == cmd) {
			neginfinitearg_call++;
			check_equals(typeof(arg), "string", "getURL-based FSCommand call with negative infinity parameter should pass string-type parameter");
			check_equals(arg, "-Infinity", "getURL-based FSCommand call should pass a correct string representation of negative infinity parameter value");
		} else if("nanarg" == cmd) {
			nanarg_call++;
			check_equals(typeof(arg), "string", "getURL-based FSCommand call with not-a-number parameter should pass string-type parameter");
			check_equals(arg, "NaN", "getURL-based FSCommand call should pass a correct string representation of not-a-number parameter value");
		} else if("booleanarg" == cmd) {
			booleanarg_call++;
			check_equals(typeof(arg), "string", "getURL-based FSCommand call with boolean parameter should pass string-type parameter");
			check_equals(arg, "true", "getURL-based FSCommand call should pass a correct string representation of boolean parameter value");
		} else if("nullarg" == cmd) {
			nullarg_call++;
			check_equals(typeof(arg), "string", "getURL-based FSCommand call with null parameter should pass string-type parameter");
			check_equals(arg, "null", "getURL-based FSCommand call should pass a correct string representation of null parameter");
		} else if("undefinedarg" == cmd) {
			undefinedarg_call++;
			check_equals(typeof(arg), "string", "getURL-based FSCommand call with undefined parameter should pass string-type parameter");
			if($swfversion >= 7) {
				check_equals(arg, "undefined", "getURL-based FSCommand call should pass a correct string representation of undefined parameter");
			} else {
				check_equals(arg, "", "getURL-based FSCommand call should pass a correct string representation of undefined parameter");
			}
		} else if("arrayarg" == cmd) {
			arrayarg_call++;
			check_equals(typeof(arg), "string", "getURL-based FSCommand call with array parameter should pass string-type parameter");
			check_equals(arg, "The,quick,brown,fox,jumps,over,the,lazy,dog", "Array parameter value of getURL-based FSCommand call should be passed correctly as CSV string");
		} else if("objectarg" == cmd) {
			objectarg_call++;
			check_equals(typeof(arg), "string", "getURL-based FSCommand call with object parameter should pass string-type parameter");
			check_equals(arg, "[object Object]", "String representation of object parameter of getURL-based FSCommand call should be correctly passed");
		} else if("object_customstringarg" == cmd) {
			object_customstringarg_call++;
			check_equals(typeof(arg), "string", "getURL-based FSCommand call with object parameter bearing custom toString() should pass string-type parameter");
			check_equals(arg, "This is a custom Object.toString()", "Custom string representation of object parameter of getURL-based FSCommand call should be correctly passed");
		} else if("functionarg" == cmd) {
			functionarg_call++;
			check_equals(typeof(arg), "string", "getURL-based FSCommand call with function parameter should pass string-type parameter");
			check_equals(arg, "[type Function]", "String representation of function parameter of getURL-based FSCommand call should be correctly passed");
		} else if("m_noarg" == cmd) {
			m_noarg_call++;
			check_equals(typeof(arg), "string", "MovieClip-based FSCommand call with no parameter should pass string-type parameter");
			check_equals(arg, "", "MovieClip-based FSCommand call with no parameter should pass an empty string parameter value");
		} else if("m_stringarg" == cmd) {
			m_stringarg_call++;
			check_equals(typeof(arg), "string", "MovieClip-based FSCommand call with string parameter should pass string-type parameter");
			check_equals(arg, "This is a string", "MovieClip-based FSCommand call with string parameter should pass a correct string parameter value");
		} else if("m_weirdstringarg" == cmd) {
			// Currently, this FSCommand won't run under libgnashplugin,
			// due to its internal parameter parsing issue.

			m_weirdstringarg_call++;
			xcheck_equals(typeof(arg),"string", "MovieClip-based FSCommand call with string parameter full of symbols should pass string-type parameter");
			xcheck_equals(arg,"!@#\$%^&*()_+-={}|[]\\\\:\";\'<>?,./~\`", "Full-of-symbols string parameter value of MovieClip-based FSCommand call should be passed correctly");
		} else if("m_integerarg" == cmd) {
			m_integerarg_call++;
			check_equals(typeof(arg), "string", "MovieClip-based FSCommand call with integer parameter should pass string-type parameter");
			check_equals(arg, "9876", "MovieClip-based FSCommand call should pass a correct string representation of integer parameter value");
		} else if("m_floatarg" == cmd) {
			m_floatarg_call++;
			check_equals(typeof(arg), "string", "MovieClip-based FSCommand call with floating point parameter should pass string-type parameter");
			check_equals(arg, "9876.5432", "MovieClip-based FSCommand call should pass a correct string representation of floating point parameter value");
		} else if("m_infinitearg" == cmd) {
			m_infinitearg_call++;
			check_equals(typeof(arg), "string", "MovieClip-based FSCommand call with infinity parameter should pass string-type parameter");
			check_equals(arg, "Infinity", "MovieClip-based FSCommand call should pass a correct string representation of infinity parameter");
		} else if("m_neginfinitearg" == cmd) {
			m_neginfinitearg_call++;
			check_equals(typeof(arg), "string", "MovieClip-based FSCommand call with negative infinity parameter should pass string-type parameter");
			check_equals(arg, "-Infinity", "MovieClip-based FSCommand call should pass a correct string representation of negative infinity parameter value");
		} else if("m_nanarg" == cmd) {
			m_nanarg_call++;
			check_equals(typeof(arg), "string", "MovieClip-based FSCommand call with not-a-number parameter should pass string-type parameter");
			check_equals(arg, "NaN", "MovieClip-based FSCommand call should pass a correct string representation of not-a-number parameter value");
		} else if("m_booleanarg" == cmd) {
			m_booleanarg_call++;
			check_equals(typeof(arg), "string", "MovieClip-based FSCommand call with boolean parameter should pass string-type parameter");
			check_equals(arg, "true", "MovieClip-based FSCommand call should pass a correct string representation of boolean parameter value");
		} else if("m_nullarg" == cmd) {
			m_nullarg_call++;
			check_equals(typeof(arg), "string", "MovieClip-based FSCommand call with null parameter should pass string-type parameter");
			check_equals(arg, "null", "MovieClip-based FSCommand call should pass a correct string representation of null parameter");
		} else if("m_undefinedarg" == cmd) {
			m_undefinedarg_call++;
			check_equals(typeof(arg), "string", "MovieClip-based FSCommand call with undefined parameter should pass string-type parameter");
			check_equals(arg, "", "MovieClip-based FSCommand call should pass an empty string as a representation of undefined parameter");
		} else if("m_arrayarg" == cmd) {
			m_arrayarg_call++;
			check_equals(typeof(arg), "string", "MovieClip-based FSCommand call with array parameter should pass string-type parameter");
			check_equals(arg, "The,quick,brown,fox,jumps,over,the,lazy,dog", "Array parameter value of MovieClip-based FSCommand call should be passed correctly as CSV string");
		} else if("m_objectarg" == cmd) {
			m_objectarg_call++;
			check_equals(typeof(arg), "string", "MovieClip-based FSCommand call with object parameter should pass string-type parameter");
			check_equals(arg, "[object Object]", "String representation of object parameter of MovieClip-based FSCommand call should be correctly passed");
		} else if("m_object_customstringarg" == cmd) {
			m_object_customstringarg_call++;
			check_equals(typeof(arg), "string", "MovieClip-based FSCommand call with object parameter bearing custom toString() should pass string-type parameter");
			check_equals(arg, "This is a custom Object.toString()", "Custom string representation of object parameter of MovieClip-based FSCommand call should be correctly passed");
		} else if("m_functionarg" == cmd) {
			m_functionarg_call++;
			check_equals(typeof(arg), "string", "MovieClip-based FSCommand call with function parameter should pass string-type parameter");
			check_equals(arg, "[type Function]", "String representation of function parameter of MovieClip-based FSCommand call should be correctly passed");
		} else {
			check_error("Unknown FSCommand issued: \"" + cmd + "\" parameter \"" + arg + "\"");
		}
	}

	function verify_flash() {
		check_equals(noname_call, 4, "No-name FSCommand should be called for 4 times");

		check_equals(noarg_call, 1, "\"noarg\" FSCommand should be called for 1 time");
		check_equals(stringarg_call, 1, "\"stringarg\" FSCommand should be called for 1 time");
		xcheck_equals(weirdstringarg_call, 1, "\"weirdstringarg\" FSCommand should be called for 1 time");
		check_equals(integerarg_call, 1, "\"integerarg\" FSCommand should be called for 1 time");
		check_equals(floatarg_call, 1, "\"floatarg\" FSCommand should be called for 1 time");
		check_equals(infinitearg_call, 1, "\"infinitearg\" FSCommand should be called for 1 time");
		check_equals(neginfinitearg_call, 1, "\"neginfinitearg\" FSCommand should be called for 1 time");
		check_equals(nanarg_call, 1, "\"nanarg\" FSCommand should be called for 1 time");
		check_equals(booleanarg_call, 1, "\"booleanarg\" FSCommand should be called for 1 time");
		check_equals(nullarg_call, 1, "\"nullarg\" FSCommand should be called for 1 time");
		check_equals(undefinedarg_call, 1, "\"undefinedarg\" FSCommand should be called for 1 time");
		check_equals(arrayarg_call, 1, "\"arrayarg\" FSCommand should be called for 1 time");
		check_equals(objectarg_call, 1, "\"objectarg\" FSCommand should be called for 1 time");
		check_equals(object_customstringarg_call, 1, "\"object_customstringarg\" FSCommand should be called for 1 time");
		check_equals(m_functionarg_call, 1, "\"functionarg\" FSCommand should be called for 1 time");

		check_equals(m_noarg_call, 1, "\"m_noarg\" FSCommand should be called for 1 time");
		check_equals(m_stringarg_call, 1, "\"m_stringarg\" FSCommand should be called for 1 time");
		xcheck_equals(m_weirdstringarg_call, 1, "\"m_weirdstringarg\" FSCommand should be called for 1 time");
		check_equals(m_integerarg_call, 1, "\"m_integerarg\" FSCommand should be called for 1 time");
		check_equals(m_floatarg_call, 1, "\"m_floatarg\" FSCommand should be called for 1 time");
		check_equals(m_infinitearg_call, 1, "\"m_infinitearg\" FSCommand should be called for 1 time");
		check_equals(m_neginfinitearg_call, 1, "\"m_neginfinitearg\" FSCommand should be called for 1 time");
		check_equals(m_nanarg_call, 1, "\"m_nanarg\" FSCommand should be called for 1 time");
		check_equals(m_booleanarg_call, 1, "\"m_booleanarg\" FSCommand should be called for 1 time");
		check_equals(m_nullarg_call, 1, "\"m_nullarg\" FSCommand should be called for 1 time");
		check_equals(m_undefinedarg_call, 1, "\"m_undefinedarg\" FSCommand should be called for 1 time");
		check_equals(m_arrayarg_call, 1, "\"m_arrayarg\" FSCommand should be called for 1 time");
		check_equals(m_objectarg_call, 1, "\"m_objectarg\" FSCommand should be called for 1 time");
		check_equals(m_object_customstringarg_call, 1, "\"m_object_customstringarg\" FSCommand should be called for 1 time");
		check_equals(m_functionarg_call, 1, "\"m_functionarg\" FSCommand should be called for 1 time");

		xcheck_totals(99);
	}

	setTimeout("verify_flash()",3000);
//-->
</script>
</body>
</html>
EOF
