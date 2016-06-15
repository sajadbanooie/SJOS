#!/bin/sh
set -e
. ./build.sh

mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

cp sysroot/boot/sjos.kernel isodir/boot/sjos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "sjos" {
	multiboot /boot/sjos.kernel
}
EOF
grub-mkrescue -o sjos.iso isodir
