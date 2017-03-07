#!/bin/sh

#
# hostcmd-geturl_testrunner.sh - container-emulated getURL()/MovieClip.getURL()
#                                target test runner generator
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
#  * Undefined and null target passing via getURL() and MovieClip.getURL()
#    (bug #50393)
#        <https://savannah.gnu.org/bugs/?50393>
#
# Usage:
#     ./hostcmd-geturl_testrunner.sh <builddir> <srcdir> <swfversion> <swf>
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
# Built-in getURL target tests
#

# Read for empty-target getURL statement
read_timeout LINE \$READTIMEOUT <&3
check_equals "\$LINE" '<invoke name="getURL" returntype="xml"><arguments><string>geturl-emptytarget.html</string><string>GET</string></arguments></invoke>' "Gnash should correctly pass built-in getURL call with empty target"

if [ $swfversion -ge 5 ]
then
	# Read for no-target getURL statement
	read_timeout LINE \$READTIMEOUT <&3
	check_equals "\$LINE" '<invoke name="getURL" returntype="xml"><arguments><string>geturl-notarget.html</string><string>GET</string></arguments></invoke>' "Gnash should correctly pass built-in getURL call with no target"

	if [ $swfversion -ge 7 ]
	then
		# Read for undefined-target getURL statement (SWF7-above form)
		read_timeout LINE \$READTIMEOUT <&3
		check_equals "\$LINE" '<invoke name="getURL" returntype="xml"><arguments><string>geturl-undeftarget.html</string><string>GET</string><string>undefined</string></arguments></invoke>' "Gnash should correctly pass built-in getURL call with undefined target"
	else
		# Read for undefined-target getURL statement (SWF6-below form)
		read_timeout LINE \$READTIMEOUT <&3
		check_equals "\$LINE" '<invoke name="getURL" returntype="xml"><arguments><string>geturl-undeftarget.html</string><string>GET</string></arguments></invoke>' "Gnash should correctly pass built-in getURL call with undefined target"
	fi

	# Read for null-target getURL statement
	read_timeout LINE \$READTIMEOUT <&3
	check_equals "\$LINE" '<invoke name="getURL" returntype="xml"><arguments><string>geturl-nulltarget.html</string><string>GET</string><string>null</string></arguments></invoke>' "Gnash should correctly pass built-in getURL call with null target"

	#
	# MovieClip-based getURL() target tests
	#

	# Read for empty-target MovieClip-based getURL statement
	read_timeout LINE \$READTIMEOUT <&3
	check_equals "\$LINE" "<invoke name=\"getURL\" returntype=\"xml\"><arguments><string>mcgeturl-emptytarget.html</string><string>GET</string></arguments></invoke>" "Gnash should correctly pass MovieClip-based getURL call with empty target"

	# Read for no-target MovieClip-based getURL statement
	read_timeout LINE \$READTIMEOUT <&3
	check_equals "\$LINE" '<invoke name="getURL" returntype="xml"><arguments><string>mcgeturl-notarget.html</string><string>GET</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based getURL call with no target"

	# Read for undefined-target MovieClip-based getURL statement
	read_timeout LINE \$READTIMEOUT <&3
	check_equals "\$LINE" '<invoke name="getURL" returntype="xml"><arguments><string>mcgeturl-undeftarget.html</string><string>GET</string></arguments></invoke>' "Gnash should correctly pass MovieClip-based getURL call with undefined target"

	# Read for null-target MovieClip-based getURL statement
	read_timeout LINE \$READTIMEOUT <&3
	check_equals "\$LINE" "<invoke name=\"getURL\" returntype=\"xml\"><arguments><string>mcgeturl-nulltarget.html</string><string>GET</string><string>null</string></arguments></invoke>" "Gnash should correctly pass MovieClip-based getURL call with null target"
fi

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
if [ $swfversion -ge 5 ]
then
	check_totals "9" "There should be 9 tests run"
else
	check_totals "2" "There should be 2 tests run"
fi

# Remove temporary files
rm "\$LOGFILE"
rm "\$PIPE2CONTAINER"
rm "\$PIPE2PLAYER"
EOF
