
#pragma once

#include "Vec.h"
#include "Item.h"
#include "Actor.h"

#include <vector>
#include <list>
#include <string>

typedef std::vector< std::string > Map;
typedef std::list< std::string >   Logger;
typedef std::vector< Actor > ActorList;

// We'll use globals defined in World.cpp to represent the current
// map, the map's inventory, a list of all NPC's and the player.
extern Map map;
extern Inventory items;
extern Logger logger;
extern bool quit;
extern ActorList actors;

bool walk( Actor* a, Vec dir );
Inventory::iterator item_at( Vec pos );
ActorList::iterator actor_at( Vec pos );
void transfer( Inventory* to, Inventory* from, Inventory::iterator what );
void move_player( Actor* );
