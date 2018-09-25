# Hexadecimal Viewer

File viewer that displays file content in hexadecimal format.


## Installing

Download or clone the project and compile:

Linux
```
gcc -Wall -o2 hexviewer.c -o hexviewer `pkg-config --libs --cflags gtk+-3.0`
```

Windows
```
cl hexviewer.cpp user32.lib Gdi32.lib Comdlg32.lib
```

## Upcoming features

 - Content edition
 - Content grouping by data type
 - More Languages
