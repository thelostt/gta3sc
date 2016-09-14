#!/usr/bin/env sh
#
# Discards the exit code of another program, except for crashing programs.
#
"$@"
if [ $? -gt 1 ]; then
    exit $?
else
    exit 0
fi