
CXX = g++ -std=c++0x
CFLAGS  = -Wall -Wextra
LDFLAGS = -lncurses

all : rogue mapgen
	
.PHONY: all

compile = ${CXX} ${CFLAGS}

robj = .world.o .vec.o .item.o .actor.o .map.o
mobj = .vec.o .tiles.o

rogue : rogue.cpp mapgen ${robj}
	${compile} rogue.cpp -o rogue ${LDFLAGS} ${robj}

mapgen : mapgen.cpp ${mobj}
	${compile} mapgen.cpp -o mapgen ${mobj}
	
.world.o : World.* .actor.o
	${compile} -c World.cpp -o .world.o

.actor.o : Actor.* .vec.o .item.o
	${compile} -c Actor.cpp -o .actor.o

.item.o : Item.* .vec.o
	${compile} -c Item.cpp -o .item.o

.vec.o : Vec.*
	${compile} -c Vec.cpp -o .vec.o

.tiles.o : Tiles.*
	${compile} -c Tiles.cpp -o .tiles.o

.map.o : Map.* .vec.o
	${compile} -c Map.cpp -o .map.o
