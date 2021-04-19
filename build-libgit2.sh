#!/usr/bin/env bash

mkdir -p libgit2/build
cd libgit2/build
cmake ..
cmake --build .

# Linux
if [[ $(uname) == "Linux" ]]; then
    rm -f ../../project/addons/git_for_godot/gdnative/linuxbsd/libgit2.so*
    cp libgit2.so* ../../project/addons/git_for_godot/gdnative/linuxbsd/
fi

# macOS
if [[ $(uname) == "Darwin" ]]; then
    rm -f ../../project/addons/git_for_godot/gdnative/macos/libgit2*.dylib
    cp libgit2*.dylib ../../project/addons/git_for_godot/gdnative/macos/
fi
