
CXX = g++ -std=c++0x
CFLAGS  = -Wall -Wextra
LDFLAGS = -lncurses

all : rogue mapgen
	
.PHONY: all

compile = ${CXX} ${CFLAGS}

obj = .world.o .vec.o .item.o .actor.o

rogue : rogue.cpp mapgen ${obj}
	${compile} rogue.cpp -o rogue ${LDFLAGS} ${obj}
	
.world.o : World.* .actor.o
	${compile} -c World.cpp -o .world.o

.actor.o : Actor.* .vec.o .item.o
	${compile} -c Actor.cpp -o .actor.o

.item.o : Item.* .vec.o
	${compile} -c Item.cpp -o .item.o

.vec.o : Vec.*
	${compile} -c Vec.cpp -o .vec.o

mapgen : mapgen.cpp
	${compile} mapgen.cpp -o mapgen

