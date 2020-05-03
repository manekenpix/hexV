# hexV <img align="left" width="100" height="100" src=https://github.com/manekenpix/hexV/blob/master/Linux/hv.png>

File viewer that displays file content in hexadecimal format


## Installation

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
.