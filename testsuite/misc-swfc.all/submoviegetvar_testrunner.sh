#!/bin/sh

#
# submoviegetvar_testrunner.sh - container-emulated, automated GetVariable()
#     plugin function test generator for nested MovieClip scenario
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
# This test runner checks Gnash for:
#  * GetVariable() slash-colon variable path issue (bug #42395)
#        <https://savannah.gnu.org/bugs/?42395>
#
# Usage:
#     ./submoviegetvar_testrunner.sh <builddir> <srcdir> <swf>
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
			echo "Usage: $0 <builddir> <srcdir> <swf>" >&2
			exit 1;;
	esac
done
shift $(($OPTIND - 1))
if [ "$#" -ne 3 ]
then
	echo "Usage: $0 <builddir> <srcdir> <swf>" >&2
	exit 1
fi

# Load generation parameters
top_builddir=$1
shift
top_srcdir=$1
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
READTIMEOUT=5

# Test counts
TESTED=0
FAILED=0
PASSED=0
XFAILED=0
XPASSED=0

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

# check_getvariable(\$varpath, \$varvalue)
# Call GetVariable function in the player, and check return value against
# the expected one
check_getvariable() {
	# Call string-returning GetVariable()
	echo '<invoke name="GetVariable" returntype="xml"><arguments><string>'"\$1"'</string></arguments></invoke>' >&4

	# Read for returned value statement
	read_timeout LINE \$READTIMEOUT <&3
	check_equals "\$LINE" '<string>'"\$2"'</string>' "Gnash should return a correct value from GetVariable(\"\$1\")"
}

# xcheck_getvariable(\$varpath, \$varvalue)
# Call GetVariable function in the player, and check return value against
# the expected one - hoping it is equal
xcheck_getvariable() {
	# Call string-returning GetVariable()
	echo '<invoke name="GetVariable" returntype="xml"><arguments><string>'"\$1"'</string></arguments></invoke>' >&4

	# Read for returned value statement
	read_timeout LINE \$READTIMEOUT <&3
	xcheck_equals "\$LINE" '<string>'"\$2"'</string>' "Gnash should return a correct value from GetVariable(\"\$1\")"
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

# Wait until the SWF code finish running, by loop-checking logfile
STARTCOUNTDOWN=\$READTIMEOUT
while [ \$STARTCOUNTDOWN -gt 0 ]
do
	if grep "TRACE: ENDOFTEST" "\$LOGFILE" 2>&1 > /dev/null
	then
		break
	fi
	sleep 1
	STARTCOUNTDOWN=\`expr \$STARTCOUNTDOWN - 1\`
done

[ \$STARTCOUNTDOWN -ne 0 ]
check_equals \$? 0 "Gnash-side ActionScript code should be successfully run"

# Test running GetVariable() on root movie's variable

check_getvariable "movievar" "This is movievar"
xcheck_getvariable ":movievar" "This is movievar"
check_getvariable "/:movievar" "This is movievar"

# Test running GetVariable() on first-level sub-MovieClip's variable

check_getvariable "submovie:submovievar" "This is submovievar"
check_getvariable "submovie/:submovievar" "This is submovievar"
check_getvariable "submovie::submovievar" "This is submovievar"

check_getvariable "/submovie:submovievar" "This is submovievar"
check_getvariable "/submovie/:submovievar" "This is submovievar"
check_getvariable "/submovie::submovievar" "This is submovievar"

check_getvariable ":submovie:submovievar" "This is submovievar"
check_getvariable ":submovie/:submovievar" "This is submovievar"
check_getvariable ":submovie::submovievar" "This is submovievar"

# Test running GetVariable() on second-level sub-MovieClip's variable

check_getvariable "submovie/nestedmovie:nestedmovievar" "This is nestedmovievar"
check_getvariable "submovie/nestedmovie/:nestedmovievar" "This is nestedmovievar"
check_getvariable "submovie/nestedmovie::nestedmovievar" "This is nestedmovievar"
check_getvariable "submovie:nestedmovie:nestedmovievar" "This is nestedmovievar"
check_getvariable "submovie:nestedmovie/:nestedmovievar" "This is nestedmovievar"
check_getvariable "submovie:nestedmovie::nestedmovievar" "This is nestedmovievar"

check_getvariable "/submovie/nestedmovie:nestedmovievar" "This is nestedmovievar"
check_getvariable "/submovie/nestedmovie/:nestedmovievar" "This is nestedmovievar"
check_getvariable "/submovie/nestedmovie::nestedmovievar" "This is nestedmovievar"
check_getvariable "/submovie:nestedmovie:nestedmovievar" "This is nestedmovievar"
check_getvariable "/submovie:nestedmovie/:nestedmovievar" "This is nestedmovievar"
check_getvariable "/submovie:nestedmovie::nestedmovievar" "This is nestedmovievar"

check_getvariable ":submovie/nestedmovie:nestedmovievar" "This is nestedmovievar"
check_getvariable ":submovie/nestedmovie/:nestedmovievar" "This is nestedmovievar"
check_getvariable ":submovie/nestedmovie::nestedmovievar" "This is nestedmovievar"
check_getvariable ":submovie:nestedmovie:nestedmovievar" "This is nestedmovievar"
check_getvariable ":submovie:nestedmovie/:nestedmovievar" "This is nestedmovievar"
check_getvariable ":submovie:nestedmovie::nestedmovievar" "This is nestedmovievar"

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

# Include total number of tests from player side
exec 5< "\$LOGFILE"
PLAYERPASSED=\`grep "TRACE: PASSED:" <&5 | wc -l\`
exec 5<&-
exec 5< "\$LOGFILE"
PLAYERXPASSED=\`grep "TRACE: XPASSED:" <&5 | wc -l\`
exec 5<&-
exec 5< "\$LOGFILE"
PLAYERFAILED=\`grep "TRACE: FAILED:" <&5 | wc -l\`
exec 5<&-
exec 5< "\$LOGFILE"
PLAYERXFAILED=\`grep "TRACE: XFAILED:" <&5 | wc -l\`
exec 5<&-
PASSED=\`expr "\$PASSED" + "\$PLAYERPASSED"\`
XPASSED=\`expr "\$XPASSED" + "\$PLAYERXPASSED"\`
FAILED=\`expr "\$FAILED" + "\$PLAYERFAILED"\`
XFAILED=\`expr "\$XFAILED" + "\$PLAYERXFAILED"\`
TESTED=\`expr "\$TESTED" + "\$PLAYERPASSED" + "\$PLAYERXPASSED" + "\$PLAYERFAILED" + "\$PLAYERXFAILED"\`

# Check for total number of test run
check_totals "34" "There should be 34 tests run"

# Remove temporary files
rm "\$LOGFILE"
rm "\$PIPE2CONTAINER"
rm "\$PIPE2PLAYER"
EOF
