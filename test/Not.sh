#!/usr/bin/env sh
#
# Negates the exit code of another program
#
# Do note we're checking for exit code 1, not 0.
# That's because we want to still exit 1 in case of a crash
# (which isn't exit code 1, likely 3 or a bigger number).
#
"$@"
if [ $? -eq 1 ]; then
    exit 0
else
    exit 1
fi