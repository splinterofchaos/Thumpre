
CXX = g++ -std=c++0x
CFLAGS  = -Wall -Wextra
LDFLAGS = -lncurses

all : rogue
	
.PHONY: all

compile = ${CXX} ${CFLAGS}

rogue : rogue.cpp
	${compile} rogue.cpp -o rogue ${LDFLAGS}

