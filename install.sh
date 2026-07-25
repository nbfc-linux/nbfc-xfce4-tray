#!/bin/sh

rm -rf build
meson setup build --prefix=/usr
meson compile -C build
sudo meson install -C build
# DESTDIR="$pkgdir" meson install -C build
