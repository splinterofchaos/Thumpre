
#pragma once

#include <vector>
#include <list>
#include <string>

// A minimal 2D vector class.
struct Vec;

// An abstraction for all items.
struct Item;

// An abstraction of any NPC or PC.
struct Actor;
struct Player;
struct Npc;

typedef std::vector< std::string > Map;
typedef std::vector< Item >        Inventory;
typedef std::list< std::string >   Logger;
typedef std::vector< Npc > NpcList;

extern Map map;
extern Inventory items;
extern Logger logger;
extern bool quit;
extern NpcList npcs;
extern Player player;

struct Vec
{
    int x, y;

    Vec();
    Vec( int x, int y );
};

struct Item
{
    enum Material
    {
        WOOD,
        HAIR
    };

    enum Type
    {
        ROD,
        WIG
    };

    Vec pos;
    std::string name;
    char     image;
    Material material;
    Type     type;

    Item( Vec pos, const std::string& name, char image, 
          Material material, Type type );
};

struct Actor
{
    Vec pos;
    Inventory inventory;

    int hp;
    char image;

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

bool operator == ( const Vec& a, const Vec& b );
bool operator != ( const Vec& a, const Vec& b );
Vec operator  +  ( const Vec& a, const Vec& b );

bool walk( Actor* a, Vec dir );
Inventory::iterator item_at( Vec pos );
NpcList::iterator npc_at( Vec pos );
void transfer( Inventory* to, Inventory* from, Inventory::iterator what );
