
CXX = g++ -std=c++0x
CFLAGS  = -Wall -Wextra
LDFLAGS = -lncurses

all : rogue mapgen
	
.PHONY: all

compile = ${CXX} ${CFLAGS}

obj = .world.o

rogue : rogue.cpp mapgen ${obj}
	${compile} rogue.cpp -o rogue ${LDFLAGS} ${obj}
	
.world.o : World.*
	${compile} -c World.cpp -o .world.o

mapgen : mapgen.cpp
	${compile} mapgen.cpp -o mapgen

