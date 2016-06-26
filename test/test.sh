#!/usr/bin/env sh
set -e

# Test building the GTA3 SCM
wget https://dl.dropboxusercontent.com/u/35476123/Docs/gta3sc/test/gta3sc_main-test-gta3.tar.gz -O gta3sc_main-test-gta3.tar.gz
tar -xzf gta3sc_main-test-gta3.tar.gz -C main-test-gta3
gta3sc main-test-gta3/main.sc --config=gta3
cd main-test-gta3
md5sum -c main.scm.md5
cd ..

