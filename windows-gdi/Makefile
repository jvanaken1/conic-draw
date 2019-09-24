# Builds demo1.exe and demo2.exe programs to run on the Win32 API in Windows
# This makefile uses the Microsoft C/C++ compiler and linker
# Run the Microsoft nmake utility from the command line in this directory
# Remember to run vcvars32.bat first to set up your build environment

LIBFILES = user32.lib gdi32.lib Winmm.lib
OBJFILES = conic.obj bounce.obj
CC = cl.exe
CDEBUG = -Zi
LINK = link.exe
LFLAGS =
LDEBUG = /DEBUG

all : demo1.exe demo2.exe

demo1.exe : demo1.obj $(OBJFILES)
	$(LINK) $(LDEBUG) $(LFLAGS) $** $(LIBFILES) /OUT:$@ /PDB:$*.pdb

demo2.exe : demo2.obj $(OBJFILES)
	$(LINK) $(LDEBUG) $(LFLAGS) $** $(LIBFILES) /OUT:$@ /PDB:$*.pdb

demo1.obj : demo1.cpp demo.h conic.h
	$(CC) $(CDEBUG) -c demo1.cpp

demo2.obj : demo2.cpp demo.h conic.h
	$(CC) $(CDEBUG) -c demo2.cpp

conic.obj : conic.cpp conic.h
	$(CC) $(CDEBUG) -c conic.cpp

bounce.obj : bounce.cpp demo.h
	$(CC) $(CDEBUG) -c bounce.cpp

conic.h : ..\conic.h
        copy /y ..\conic.h

demo.h : ..\demo.h
        copy /y ..\demo.h

conic.cpp : ..\conic.cpp
        copy /y ..\conic.cpp
        
bounce.cpp : ..\bounce.cpp
        copy /y ..\bounce.cpp
        
clean:
        @del *.exe
        @del *.ilk
        @del *.obj
        @del *.pdb
        

