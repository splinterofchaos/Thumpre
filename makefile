
CXX = g++ -std=c++0x
CFLAGS  = -Wall -Wextra
LDFLAGS = -lncurses

all : rogue mapgen
	
.PHONY: all

compile = ${CXX} ${CFLAGS}

rogue : rogue.cpp
	${compile} rogue.cpp -o rogue ${LDFLAGS}

mapgen : mapgen.cpp
	${compile} mapgen.cpp -o mapgen

