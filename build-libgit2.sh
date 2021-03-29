#!/usr/bin/env bash

mkdir -p libgit2/build
cd libgit2/build
cmake ..
cmake --build .
rm -f ../../project/addons/git_for_godot/gdnative/linuxbsd/libgit2.so*
cp libgit2.so* ../../project/addons/git_for_godot/gdnative/linuxbsd/
