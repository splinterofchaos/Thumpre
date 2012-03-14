
#pragma once

#include "Vec.h"
#include "Item.h"

#include <vector>
#include <list>
#include <string>

// An abstraction of any NPC or PC.
struct Actor;
struct Player;
struct Npc;

typedef std::vector< std::string > Map;
typedef std::vector< Item >        Inventory;
typedef std::list< std::string >   Logger;
typedef std::vector< Npc > NpcList;

// We'll use globals defined in World.cpp to represent the current
// map, the map's inventory, a list of all NPC's and the player.
extern Map map;
extern Inventory items;
extern Logger logger;
extern bool quit;
extern NpcList npcs;
extern Player player;

struct Actor
{
    Vec pos;
    Inventory inventory;

    int hp;
    char image;

    int strength;

    // Used by Player, ignored otherwise.
    bool turnOver;

    Actor( Vec pos, char image );

    virtual void move() = 0;
};

struct Player : public Actor
{
    Player( Vec pos );
    void move();
};

struct Npc : public Actor
{
    Npc( Vec pos, char image );
    void move();
};

bool walk( Actor* a, Vec dir );
Inventory::iterator item_at( Vec pos );
NpcList::iterator npc_at( Vec pos );
void transfer( Inventory* to, Inventory* from, Inventory::iterator what );
