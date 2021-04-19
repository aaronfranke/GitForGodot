
[System.IO.Directory]::CreateDirectory("libgit2/build")
Set-Location libgit2/build
cmake ..
cmake --build .

Set-Location ../..

Remove-Item -Force project/addons/git_for_godot/gdnative/windows/*.lib
Copy-Item libgit2/build/Debug/*.lib project/addons/git_for_godot/gdnative/windows/
