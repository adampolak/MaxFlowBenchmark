#!/bin/sh

mkdir build
cmake -S . -B ./build
make -C ./build
