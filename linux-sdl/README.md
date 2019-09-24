# Build the conic-draw demos to run on SDL2 in Linux

_Author:_ Jerry R. VanAken
_Date:_ 9/24/2019

This directory (the linux-sdl subdirectory in your conic-draw installation) contains the files you'll need to build the version of the two conic-draw demo programs that run on [SDL2](https://wiki.libsdl.org/FrontPage) (Simple DirectMedia Layer 2.0) in Linux.

The demo program executables are named demo1.exe and demo2.exe. Each demo runs in a 1280-by-960 window. The demo starts off as a free-running animation, but you can press any key to begin single-stepping through the demo a frame at a time.

## What's in this directory

This directory (i.e., the linux-sdl subdirectory in your conic-draw installation) contains these four files:

* `README.md` -- This README file

* `Makefile` -- A makefile that is invoked with the GNU make utility, and that contains GNU g++ compiler and linker commands

* `demo1.cpp` -- Contains the platform-specific code necessary to run the ellipse-drawing demo program on SDL2 in Linux

* `demo2.cpp` -- Contains the platform-specific code necessary to run the spline-drawing demo program on SDL2 in Linux

## Build the two demo programs

Follow these steps to build and run the conic-draw demos from the command line:

1. Open a terminal window.
2. Install the GNU g++ compiler/linker and GNU make utilities, if you haven't done so already.
3. Install SDL2, if you haven't done so already (more information below).
4. Change to _this_ directory (i.e., the linux-sdl subdirectory in your conic-draw installation).
5. Enter the command "make" to build the demo.
6. To run a demo, enter the command "./demo1" or "./demo2".

## Installing SDL2

The [official SDL2 website](https://wiki.libsdl.org) provides instructions for installing SDL2 on various platforms. The [Installing SDL](https://wiki.libsdl.org/Installation) page at this website explains that

>Debian-based systems (including Ubuntu) can simply do "sudo apt-get install libsdl2-2.0" to get the library installed system-wide, and all sorts of other useful dependencies, too.


.

.

.

.