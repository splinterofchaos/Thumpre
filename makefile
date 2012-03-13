
CXX = g++ -std=c++0x
CFLAGS  = -Wall -Wextra
LDFLAGS = -lncurses

all : rogue mapgen
	
.PHONY: all

compile = ${CXX} ${CFLAGS}

obj = .world.o .vec.o .item.o

rogue : rogue.cpp mapgen ${obj}
	${compile} rogue.cpp -o rogue ${LDFLAGS} ${obj}
	
.world.o : World.* .vec.o .item.o
	${compile} -c World.cpp -o .world.o

.item.o : Item.* .vec.o
	${compile} -c Item.cpp -o .item.o

.vec.o : Vec.*
	${compile} -c Vec.cpp -o .vec.o

mapgen : mapgen.cpp
	${compile} mapgen.cpp -o mapgen

