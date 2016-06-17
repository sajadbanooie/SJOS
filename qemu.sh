#!/bin/sh
set -e
. ./iso.sh

qemu-system-$(./target-triplet-to-arch.sh $HOST) -cdrom sjos.iso -m 128M
# -drive file=sjos.iso,index=0,media=disk
