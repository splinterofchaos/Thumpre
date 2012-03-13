
#include "World.h"

#include <ncurses.h>
#include <algorithm>

Map map;
Inventory items;
Logger logger;
bool quit = false;
NpcList npcs;
Player player( Vec(0,0) );

Vec::Vec()
{
    x = y = 0;
}

Vec::Vec( int x, int y )
    : x(x), y(y)
{
}

bool operator == ( const Vec& a, const Vec& b )
{
    return a.x == b.x and a.y == b.y;
}

bool operator != ( const Vec& a, const Vec& b )
{
    return ! ( a == b );
}

Vec operator + ( const Vec& a, const Vec& b )
{
    return { a.x + b.x, a.y + b.y };
}

Item:: Item( Vec pos, const std::string& name, char image, 
             Material material, Type type )
    : pos(pos), name(name), image(image), material(material), type(type)
{
}

Actor::Actor( Vec pos, char image )
: pos( pos ), image(image)
{
    hp = 5;
}

Player::Player( Vec pos )
    : Actor( pos, '@' )
{
}

void Player::move()
{
    // The direction the user wants to move, if any.
    Vec inputDir = { 0, 0 }; 

    // We need to know if we're standing on an item, but can't construct
    // the object inside the switch, so do it now.
    Inventory::iterator itemHere = item_at( pos );

    int key = getch();
    switch( key )
    {
      case 'l': case '6': case KEY_RIGHT: inputDir = Vec(  1,  0 ); break;
      case 'h': case '4': case KEY_LEFT:  inputDir = Vec( -1,  0 ); break;
      case 'k': case '8': case KEY_UP:    inputDir = Vec(  0, -1 ); break;
      case 'j': case '2': case KEY_DOWN:  inputDir = Vec(  0,  1 ); break;
      case 'y': case '7': inputDir = Vec( -1, -1 ); break;
      case 'u': case '9': inputDir = Vec(  1, -1 ); break;
      case 'b': case '1': inputDir = Vec( -1,  1 ); break;
      case 'n': case '3': inputDir = Vec(  1,  1 ); break;

      case 'p': case 'g':
                          if( itemHere != items.end() )
                          {
                              transfer( &inventory, &items, itemHere );
                              logger.push_back( "Got " + inventory.back().name + "." );
                          }
                          else
                              logger.push_back( "There's nothing here." );

      break;

      case 'c': logger.push_back( "..." ); break;
      case 'q': quit = true; break;
      default: logger.push_back( "Is that key supposed to do something?" ); break;
    }

    if( inputDir.x or inputDir.y )
        if( not walk(this, inputDir) )
            logger.push_back( "Cannot move there." );
}

Npc::Npc( Vec pos, char image )
    : Actor( pos, image )
{
}

void Npc::move()
{
}

void transfer( Inventory* to, Inventory* from, Inventory::iterator what )
{
    to->push_back( *what );
    from->erase( what );
}

Inventory::iterator item_at( Vec pos )
{
    return std::find_if ( 
        items.begin(), items.end(), [&](const Item& i) { return i.pos == pos; }
    );
}

NpcList::iterator npc_at( Vec pos )
{
    return std::find_if ( 
        npcs.begin(), npcs.end(), [&](const Npc& n) { return n.pos == pos; }
    );
}

bool walk( Actor* a, Vec dir )
{
    Vec newPos = a->pos + dir;

    bool inBounds = newPos.x > 0 and newPos.y > 0;
    inBounds &= newPos.y < map.size() and newPos.x < map[newPos.y].size();

    NpcList::iterator npcHere = npc_at( newPos );

    if( npcHere != npcs.end() )
    {
        logger.push_back( "You punch the monster with your fist." );
        if( ! --npcHere->hp )
            npcs.erase( npcHere );
    }
    else if( inBounds and map[newPos.y][newPos.x] != '#' )
    {
        a->pos = newPos;
    }
    else
    {
        return false;
    }

    return true;
}
