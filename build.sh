#!/bin/sh

mkdir deps/lemon/build

cmake -S ./deps/lemon -B ./deps/lemon/build
make -C ./deps/lemon/build

mkdir build

cmake -S . -B /build
make -C ./build
