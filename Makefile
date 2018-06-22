app: hexviewer.c
	gcc -Wall -ggdb hexviewer.c -o hexviewer `pkg-config --libs --cflags gtk+-3.0`
