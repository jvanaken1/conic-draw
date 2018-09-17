cl /Zi /c conic.cpp
cl /Zi /c demo1.cpp
link /DEBUG demo1.obj conic.obj user32.lib gdi32.lib Winmm.lib /OUT:demo1.exe /PDB:demo1.pdb
cl /Zi /c demo2.cpp
link /DEBUG demo2.obj conic.obj user32.lib gdi32.lib Winmm.lib /OUT:demo2.exe /PDB:demo2.pdb



