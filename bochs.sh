#!/bin/sh
set -e
. ./iso.sh
bochs -q -f bochsrc 
