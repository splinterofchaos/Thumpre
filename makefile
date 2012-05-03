
CXX = g++ -std=c++0x
CFLAGS  = -Wall -Wextra
LDFLAGS = -lncurses

all : rogue mapgen
	
.PHONY: all

compile = ${CXX} ${CFLAGS}

robj = .world.o .vec.o .item.o .actor.o .map.o .log.o .object.o
mobj = .vec.o .tiles.o

rogue : rogue.cpp mapgen ${robj}
	${compile} rogue.cpp -o rogue ${LDFLAGS} ${robj}

mapgen : mapgen.cpp ${mobj}
	${compile} mapgen.cpp -o mapgen ${mobj}
	
.world.o : World.* .actor.o .log.o
	${compile} -c World.cpp -o .world.o

.actor.o : Actor.* .item.o .object.o
	${compile} -c Actor.cpp -o .actor.o

.item.o : Item.* .object.o
	${compile} -c Item.cpp -o .item.o

.object.o : Object.* .vec.o
	${compile} -c Object.cpp -o .object.o

.vec.o : Vec.*
	${compile} -c Vec.cpp -o .vec.o

.tiles.o : Tiles.*
	${compile} -c Tiles.cpp -o .tiles.o

.map.o : Map.* .vec.o
	${compile} -c Map.cpp -o .map.o

.log.o : Log.* 
	${compile} -c Log.cpp -o .log.o
