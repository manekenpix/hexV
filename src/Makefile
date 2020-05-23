BINARIES = hexV
CCGTKMM = `pkg-config gtkmm-3.0 --cflags --libs`
CCOPTIONS = -std=c++14
CCRELEASE = -O2
CCDEBUG = -g
CC = g++

release: HexV.cpp app.cpp
	${CC} $(CCOPTIONS) ${CCRELEASE} HexV.cpp app.cpp -o hexV $(CCGTKMM)

debug: HexV.cpp app.cpp
	${CC} $(CCOPTIONS) ${CCDEBUG} HexV.cpp app.cpp -o hexV ${CCGTKMM}
