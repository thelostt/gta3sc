#!/usr/bin/env sh
md5sum "$1" | grep "$2" >/dev/null || {
    echo "checksum: file $1 checksum is not $2" >&2
    exit 1
}
