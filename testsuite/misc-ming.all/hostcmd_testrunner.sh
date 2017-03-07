#!/bin/sh

#
# hostcmd_testrunner.sh - container-emulated non-predefined FSCommand
#                         invocation test runner generator
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
# The generated test runner checks Gnash for:
#  * Support of FSCommand call via MovieClip.getURL() (bug #46944)
#        <https://savannah.gnu.org/bugs/?46944>
#  * Undefined FSCommand parameter passing via MovieClip.getURL() (bug #50393)
#        <https://savannah.gnu.org/bugs/?50393>
#
# Usage:
#     ./hostcmd_testrunner.sh <builddir> <srcdir> <swfversion> <swf>
#
# Generated test runner's exit codes:
#     0         if tester ran completely
#     non-zero  if tester encountered an error
#
# Note:
#     The generated test file requires a filesystem that supports named pipes.
#

# Check for generation parameters
while getopts "" name
do
	case $name in
		?)
			echo "Usage: $0 <builddir> <srcdir> <swfversion> <swf>" >&2
			exit 1;;
	esac
done
shift $(($OPTIND - 1))
if [ "$#" -ne 4 ]
then
	echo "Usage: $0 <builddir> <srcdir> <swfversion> <swf>" >&2
	exit 1
fi

# Load generation parameters
top_builddir=$1
shift
top_srcdir=$1
shift
swfversion=$1
shift
swf=$1

# Generate the test runner
echo "#!/bin/sh"
echo

echo "# Environment variables"
env | grep '^GNASH' | while read reply
do
	echo "export \"${reply}\""
done

cat << EOF

# Filenames and constants
LOGFILE=${top_builddir}/testoutlog.\$\$
PIPE2CONTAINER=${top_builddir}/tocontainer.\$\$
PIPE2PLAYER=${top_builddir}/toplayer.\$\$
STARTTIMEOUT=10
READTIMEOUT=5

# Test counts
TESTED=0
FAILED=0
PASSED=0

# check_equals(\$op1, \$op2, \$msg)
# Equality checker and counter
check_equals() {
	if [ "\$1" = "\$2" ]
	then
		echo "PASSED: \$3"
		PASSED=\`expr "\$PASSED" + 1\`
	else
		echo "FAILED: \$3 (\"\$1\" != \"\$2\")"
		FAILED=\`expr "\$FAILED" + 1\`
	fi
	TESTED=\`expr "\$TESTED" + 1\`
}

# xcheck_equals(\$op1, \$op2, \$msg)
# Equality checker and counter (for expected failure)
xcheck_equals() {
	if [ "\$1" = "\$2" ]
	then
		echo "XPASSED: \$3"
		PASSED=\`expr "\$PASSED" + 1\`
	else
		echo "XFAILED: \$3 (\"\$1\" != \"\$2\")"
		FAILED=\`expr "\$FAILED" + 1\`
	fi
	TESTED=\`expr "\$TESTED" + 1\`
}

# check_totals(\$op, \$msg)
# Test count checker
check_totals() {
	check_equals "\$TESTED" "\$1" "\$2"
}

# check_error(\$bool, \$msg)
# Assert \$bool is 0; if not, flag error in the test, and exit
check_error() {
	if [ "\$1" -ne 0 ]
	then
		echo "ERROR: \$2" >&2
		exit 1
	fi
}

# read_timeout(\$varname, \$timeout)
# Read one line from standard input, with a specified timeout (in seconds)
read_timeout() {
	trap 'trap - USR1; return 142' USR1
	(sleep "\$2" && kill -USR1 "\$\$" > /dev/null 2>&1) &
	TIMEOUTPID=\$!
	read "\$1"
	READERROR=\$?
	kill "\$TIMEOUTPID" > /dev/null 2>&1
	trap - USR1
	return \$READERROR
}

# Create required named pipes
if [ \! -p "\$PIPE2CONTAINER" ]
then
	mkfifo "\$PIPE2CONTAINER"
	check_error "\$?" "Failed to create a named pipe: \$PIPE2CONTAINER"
fi
if [ \! -p "\$PIPE2PLAYER" ]
then
	mkfifo "\$PIPE2PLAYER"
	check_error "\$?" "Failed to create a named pipe: \$PIPE2PLAYER"
fi

# Open player-to-host pipe
exec 3<> "\$PIPE2CONTAINER"
check_error \$? "Failed to open a named pipe: \$PIPE2CONTAINER"

# Open host-to-player pipe
exec 4<> "\$PIPE2PLAYER"
check_error \$? "Failed to open a named pipe: \$PIPE2PLAYER"

# Start player
"${top_builddir}/gui/gnash" -r 0 -vv -F 3:4 "${swf}" > "\$LOGFILE" 2>&1 &
GNASHPID=\$!

# Wait until the SWF code start running, by loop-checking logfile
STARTCOUNTDOWN=\$STARTTIMEOUT
while [ \$STARTCOUNTDOWN -gt 0 ]
do
	if grep "TRACE: STARTOFTEST" "\$LOGFILE" 2>&1 > /dev/null
	then
		break
	fi
	sleep 1
	STARTCOUNTDOWN=\`expr \$STARTCOUNTDOWN - 1\`
done

[ \$STARTCOUNTDOWN -ne 0 ]
check_equals \$? 0 "Gnash-side ActionScript code should be successfully started"

#
# getURL-based FSCommand tests
#

# Read for no-name no-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string></string><string></string></arguments></invoke>' "Gnash should correctly pass getURL-based no-name FSCommand call with no parameter"

# Read for no-name string-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string></string><string>This is a string for empty call</string></arguments></invoke>' "Gnash should correctly pass getURL-based no-name FSCommand call with string parameter"

# Read for no-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>noarg</string><string></string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with no parameter"

# Read for string-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>stringarg</string><string>This is a string</string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with string parameter"

# Read for full-of-symbols-string-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
xcheck_equals "\$LINE" "<invoke name=\"fsCommand\" returntype=\"xml\"><arguments><string>weirdstringarg</string><string>!@#\\\$%^&amp;*()_+-={}|[]:\";'&lt;&gt;?,./~\\\`</string></arguments></invoke>" "Gnash should correctly pass getURL-based FSCommand call with full-of-symbols string parameter"

# Read for integer-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>integerarg</string><string>9876</string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with integer parameter"

# Read for floating-point-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>floatarg</string><string>9876.5432</string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with floating point parameter"

# Read for infinity-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>infinitearg</string><string>Infinity</string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with infinity parameter"

# Read for negative-infinity-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>neginfinitearg</string><string>-Infinity</string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with negative infinity parameter"

# Read for not-a-number-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>nanarg</string><string>NaN</string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with not-a-number parameter"

# Read for boolean-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>booleanarg</string><string>true</string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with boolean parameter"

# Read for null-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>nullarg</string><string>null</string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with null parameter"

if [ $swfversion -ge 7 ]
then
	# Read for undefined-parameter FSCommand statement (SWF7-above form)
	read_timeout LINE \$READTIMEOUT <&3
	check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>undefinedarg</string><string>undefined</string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with undefined parameter"
else
	# Read for undefined-parameter FSCommand statement (SWF6-below form)
	read_timeout LINE \$READTIMEOUT <&3
	check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>undefinedarg</string><string></string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with undefined parameter"
fi

# Read for array-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>arrayarg</string><string>The,quick,brown,fox,jumps,over,the,lazy,dog</string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with array parameter"

# Read for object-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>objectarg</string><string>[object Object]</string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with object parameter"

# Read for object-with-custom-string-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>object_customstringarg</string><string>This is a custom Object.toString()</string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with object parameter bearing custom toString()"

# Read for function-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>functionarg</string><string>[type Function]</string></arguments></invoke>' "Gnash should correctly pass getURL-based FSCommand call with function parameter"

#
# MovieClip-based FSCommand tests
#

# Read for no-name no-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string></string><string></string></arguments></invoke>' "Gnash should correctly pass MovieClip-based no-name FSCommand call with no parameter"

# Read for no-name string-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string></string><string>This is a string for empty call</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based no-name FSCommand call with string parameter"

# Read for no-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>m_noarg</string><string></string></arguments></invoke>' "Gnash should correctly pass MovieClip-based FSCommand call with no parameter"

# Read for string-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>m_stringarg</string><string>This is a string</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based FSCommand call with string parameter"

# Read for full-of-symbols-string-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
xcheck_equals "\$LINE" "<invoke name=\"fsCommand\" returntype=\"xml\"><arguments><string>m_weirdstringarg</string><string>!@#\\\$%^&amp;*()_+-={}|[]:\";'&lt;&gt;?,./~\\\`</string></arguments></invoke>" "Gnash should correctly pass MovieClip-based FSCommand call with full-of-symbols string parameter"

# Read for integer-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>m_integerarg</string><string>9876</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based FSCommand call with integer parameter"

# Read for floating-point-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>m_floatarg</string><string>9876.5432</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based FSCommand call with floating point parameter"

# Read for infinity-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>m_infinitearg</string><string>Infinity</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based FSCommand call with infinity parameter"

# Read for negative-infinity-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>m_neginfinitearg</string><string>-Infinity</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based FSCommand call with negative infinity parameter"

# Read for not-a-number-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>m_nanarg</string><string>NaN</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based FSCommand call with not-a-number parameter"

# Read for boolean-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>m_booleanarg</string><string>true</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based FSCommand call with boolean parameter"

# Read for null-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>m_nullarg</string><string>null</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based FSCommand call with null parameter"

# Read for undefined-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>m_undefinedarg</string><string></string></arguments></invoke>' "Gnash should pass undefined parameter of MovieClip-based FSCommand call as empty string"

# Read for array-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>m_arrayarg</string><string>The,quick,brown,fox,jumps,over,the,lazy,dog</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based FSCommand call with array parameter"

# Read for object-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>m_objectarg</string><string>[object Object]</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based FSCommand call with object parameter"

# Read for object-with-custom-string-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>m_object_customstringarg</string><string>This is a custom Object.toString()</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based FSCommand call with object parameter bearing custom toString()"

# Read for function-parameter FSCommand statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="fsCommand" returntype="xml"><arguments><string>m_functionarg</string><string>[type Function]</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based FSCommand call with function parameter"

# Close pipes
exec 3<&-
exec 4<&-

# Force Gnash to exit
kill \$GNASHPID
wait \$GNASHPID

# Show player-side output
exec 5< "\$LOGFILE"
cat <&5
exec 5<&-

# Check for total number of test run
check_totals "35" "There should be 35 tests run"

# Remove temporary files
rm "\$LOGFILE"
rm "\$PIPE2CONTAINER"
rm "\$PIPE2PLAYER"
EOF
