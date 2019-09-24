# Builds demo1.exe and demo2.exe programs to run on SDL2 in Windows
# This makefile uses the Microsoft C/C++ compiler and linker
# Run the Microsoft nmake utility from the command line in this directory
# Remember to run vcvars32.bat first to set up your build environment
# You may need to edit the include and lib paths for your SDL2 installation
# Before running demo1.exe or demo2.exe, remember to add C:\SDL2\lib\x86 to your path

INCDIR = C:\SDL2\include
LIBDIR = C:\SDL2\lib\x86
LIBFILES = $(LIBDIR)\SDL2main.lib $(LIBDIR)\SDL2.lib
OBJFILES = conic.obj bounce.obj
CC = cl.exe
CDEBUG = -Zi
LINK = link.exe
LFLAGS = /SUBSYSTEM:CONSOLE
LDEBUG = /DEBUG

all : demo1.exe demo2.exe

demo1.exe : demo1.obj $(OBJFILES)
	$(LINK) $(LDEBUG) $(LFLAGS) $** $(LIBFILES) /OUT:$@ /PDB:$*.pdb

demo2.exe : demo2.obj $(OBJFILES)
        $(LINK) $(LDEBUG) $(LFLAGS) $** $(LIBFILES) /OUT:$@ /PDB:$*.pdb

conic.obj : conic.cpp conic.h
	$(CC) $(CDEBUG) -c conic.cpp

bounce.obj : bounce.cpp demo.h
	$(CC) $(CDEBUG) -c bounce.cpp

demo1.obj : demo1.cpp demo.h conic.h
	$(CC) -I$(INCDIR) $(CDEBUG) -c demo1.cpp

demo2.obj : demo2.cpp demo.h conic.h
        $(CC) -I$(INCDIR) $(CDEBUG) -c demo2.cpp
        
demo.h : ..\demo.h
        copy /y ..\demo.h

conic.h : ..\conic.h
        copy /y ..\conic.h
        
conic.cpp : ..\conic.cpp
        copy /y ..\conic.cpp

bounce.cpp : ..\bounce.cpp
        copy /y ..\bounce.cpp
        
clean:
        @del *.exe
        @del *.ilk
        @del *.obj
        @del *.pdb
