# Build the conic-draw demos to run on SDL2 in Windows

_Author:_ Jerry R. VanAken
_Date:_ 9/24/2019

This directory (i.e., the windows-sdl subdirectory in your conic-draw installation) contains the files you'll need to build the version of the demo programs that run on [SDL2](https://wiki.libsdl.org/FrontPage) (Simple DirectMedia Layer 2.0) in Windows.

The demo program executables are named demo1.exe and demo2.exe. Each demo runs in a 1280-by-960 window. The demo starts off as a free-running animation, but you can press any key to begin single-stepping through the demo a frame at a time.

## What's in this directory

The windows-sdl subdirectory in your conic-draw installation contains these five files:

* `README.md` -- This README file

* `Makefile` -- A makefile that is invoked with the Microsoft nmake utility, and that contains Microsoft Visual Studio C/C++ compiler and linker commands

* `demo1.cpp` -- Contains the platform-specific code necessary to run the ellipse-drawing demo program on SDL2 in Windows

* `demo2.cpp` -- Contains the platform-specific code necessary to run the spline-drawing demo program on SDL2 in Windows

* `sdlpath.bat` -- A batch file that adds the location of the SDL2 DLL files to the "path" environment variable

## Build the two demo programs

Follow these steps to build and run the conic-draw demos from the command line:

1. Download the latest stable build of SDL2, if you haven't done so already. 
>Note: If you choose a directory other than "C:\SDL2" for your SDL2 installation, you'll need to update the Makefile and sdlpath.bat files in _this_ directory (i.e., the windows-sdl directory in your conic-draw installation files).
2. Open a Command Window.
3. Run the vcvars32.bat in your Visual Studio installation files to initialize the "x86" build environment.
4. Change to _this_ directory (i.e., windows-sdl).
5. Enter the command "nmake" to build the demo.exe executable file.
6. Run the sdlpath.bat file in _this_ directory. This file sets the path for the SDL2 DLL files.
7. To run a demo, enter the command "demo1" or "demo2".

If you need more information, keep reading...

## Downloading SDL2

Download the current stable version of SDL2 from [the official SDL2 download](https://www.libsdl.org/download-2.0.php) page. Select the SDL2 development library for Visual Studio C++. Create a directory (for example, "C:\SDL2") to copy the SDL2 files and directories to.

>Note: The Makefile and sdlpath.bat files in _this_ directory (the windows-sdl directory in your conic-draw installation files) assume that the SDL2 files are located in a directory named "C:\SDL2". If you choose a different directory for your SDL2 installation, be sure to update the Makefile and sdlpath.bat files.

For more information, see the [Installing SDL](https://wiki.libsdl.org/Installation) page at the official SDL2 website.

## Downloading Visual Studio

The makefile in _this_ directory (i.e., windows-sdl) was tested on Windows 10 with the following free version of Visual Studio:
* Microsoft Visual Studio Community 2019, Version 16.1.5

You can download the free version from the Visual Studio [download site](https://visualstudio.microsoft.com/vs/).

## Building with Visual Studio

To build the two demo programs from the command line, you'll first need to open a Command Window. To do this, left-click in the search box next to the Windows Start button, type "command prompt", and press enter.

Next, find the vcvars32.bat file in your Visual Studio installation. Here's the default location for this file:

* C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\

Change to the directory just above (or to the corresponding location in your Visual Studio installation), and enter the command "vcvars32". This command executes the vcvars32.bat batch file, which sets up the Visual Studio "x86" build environment.

Next, change to _this_ directory (i.e., the windows-sdl directory in your conic-draw installation).

Enter the command "nmake". (This command will execute the makefile the same as if you entered the command "nmake.exe /f Makefile".) Verify that the nmake utility has successfully built the demo1.exe and demo2.exe executable files.

Before running either demo, enter the command "sdlpath". This command executes the sdlpath.bat batch file, which sets the "path" environment variable in your Command Window so that Windows can find the SDL2 DLL files. 

To run a demo, enter the command "demo1" or "demo2".

.

.

.

.