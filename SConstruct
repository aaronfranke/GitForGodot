#!python
import os, subprocess

opts = Variables([], ARGUMENTS)

# Gets the standard flags CC, CCX, etc.
env = DefaultEnvironment()

# Define our options
opts.Add(EnumVariable("target", "Compilation target", "debug", ["d", "debug", "r", "release"]))
opts.Add(EnumVariable("platform", "Compilation platform", "", ["", "windows", "linuxbsd", "macos"]))
opts.Add(EnumVariable("p", "Compilation target, alias for 'platform'", "", ["", "windows", "linuxbsd", "macos"]))
opts.Add(BoolVariable("use_llvm", "Use the LLVM / Clang compiler", "no"))
opts.Add(PathVariable("target_path", "The path where the lib is installed.", "project/addons/git_for_godot/gdnative/"))
opts.Add(PathVariable("target_name", "The library name.", "libsimple", PathVariable.PathAccept))

# Local dependency paths, adapt them to your setup
godot_headers_path = "godot_headers/"

# Only support 64-bit systems.
bits = 64

# Updates the environment with the option variables.
opts.Update(env)

# Process some arguments
if env["use_llvm"]:
    env["CC"] = "clang"
    env["CXX"] = "clang++"

if env["p"] != "":
    env["platform"] = env["p"]

if env["platform"] == "":
    print("No valid target platform selected.")
    quit()

# Check our platform specifics
if env["platform"] == "macos":
    env["target_path"] += "macos/"
    if env["target"] in ("debug", "d"):
        env.Append(CCFLAGS = ["-g","-O2", "-arch", "x86_64"])
        env.Append(LINKFLAGS = ["-arch", "x86_64"])
    else:
        env.Append(CCFLAGS = ["-g","-O3", "-arch", "x86_64"])
        env.Append(LINKFLAGS = ["-arch", "x86_64"])

elif env["platform"] == "linuxbsd":
    env["target_path"] += "linuxbsd/"
    if env["target"] in ("debug", "d"):
        env.Append(CCFLAGS = ["-fPIC", "-g3","-Og"])
    else:
        env.Append(CCFLAGS = ["-fPIC", "-g","-O3"])

elif env["platform"] == "windows":
    env["target_path"] += "windows/"
    # This makes sure to keep the session environment variables on windows,
    # that way you can run scons in a vs 2017 prompt and it will find all the required tools
    env.Append(ENV = os.environ)

    env.Append(CCFLAGS = ["-DWIN32", "-D_WIN32", "-D_WINDOWS", "-W3", "-GR", "-D_CRT_SECURE_NO_WARNINGS"])
    if env["target"] in ("debug", "d"):
        env.Append(CCFLAGS = ["-EHsc", "-D_DEBUG", "-MDd"])
    else:
        env.Append(CCFLAGS = ["-O2", "-EHsc", "-DNDEBUG", "-MD"])

# make sure our binding library is properly includes
env.Append(CPPPATH=[".", godot_headers_path])

# tweak this if you want to use different folders, or more folders, to store your source code in.
env.Append(CPPPATH=["src/"])
sources = Glob("src/*.c")

library = env.SharedLibrary(target=env["target_path"] + env["target_name"] , source=sources)

Default(library)

# Generates help for the -h scons option.
Help(opts.GenerateHelpText(env))