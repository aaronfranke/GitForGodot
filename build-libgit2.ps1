
mkdir -Force "libgit2/build"
Set-Location libgit2/build
cmake -DCMAKE_OPTIONS=-A amd64 ..
cmake --build .

Set-Location ../..

Remove-Item -Force project/addons/git_for_godot/gdnative/windows/git2.lib
Remove-Item -Force project/addons/git_for_godot/gdnative/windows/git2.dll
Copy-Item libgit2/build/Debug/git2.lib project/addons/git_for_godot/gdnative/windows/
Copy-Item libgit2/build/Debug/git2.dll project/addons/git_for_godot/gdnative/windows/
