# gta3sc

[![Build Status](https://travis-ci.org/thelink2012/gta3sc.svg?branch=master)](https://travis-ci.org/thelink2012/gta3sc)

This is a native script compiler/decompiler for the so called [gta3script](http://www.gtamodding.com/wiki/SCM_language) language.

It's very experimental at the moment and works only for GTA III and GTA Vice City, though it is able to compile lots of scripts, including the ones [War Drum Studios](http://gta.wikia.com/wiki/War_Drum_Studios) left on the GTA III 10th Anniversary release.

Due to its design, it can also be used as a framework for working with SCM data. See the README.md in the [src directory](src) for a technical overview. There's also a experimental Dynamic Recompiler in the [dynarec-x86](https://github.com/thelink2012/gta3sc/tree/dynarec-x86) branch.

## Building

Windows (MSVC) and Linux (GCC/Clang) are currently supported, but you need the latest compiler toolchain of your platform due to the use of C++17.

Just follow the standard CMake procedure for generating projects or makefiles.

    mkdir build
    cd build
    cmake ..
    
Then `make` or use the generated project files.

## Using

The compiler/decompiler is invoked by the file extension of the input file _(first argument)_, or from the action `compile` or `decompile`.

**Compilation example:**

    gta3sc main.sc --config=gta3
    gta3sc compile main.sc --config=gta3  # does the same thing as above
    
**Decompilation example:**
    
    gta3sc main.scm --config=gta3
    gta3sc decompile main.scm --config=gta3  # does the same thing as above

The decompiler is still very early and produces _very_ low-levelish code, so low that it cannot be recompiled back. High-level decompilation is supposed to be implemented later.

**Help:**

To get further instructions, try getting help from the utility.

    gta3sc --help

