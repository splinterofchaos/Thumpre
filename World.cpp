
#include "World.h"

#include <ncurses.h>
#include <algorithm>

Map map;
Inventory items;
Logger logger;
bool quit = false;
ActorList actors;

Actor::Actor( Vec pos, char image )
: pos( pos ), image(image)
{
    hp = 5;
    playerControlled = false;
}

void move_player( Actor* pl )
{
    Actor& player = *pl;

    // The direction the user wants to move, if any.
    Vec inputDir = { 0, 0 }; 

    // We need to know if we're standing on an item, but can't construct
    // the object inside the switch, so do it now.
    Inventory::iterator itemHere = item_at( player.pos );

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
                              transfer( &player.inventory, &items, itemHere );
                              logger.push_back( "Got " + player.inventory.back().name + "." );
                          }
                          else
                              logger.push_back( "There's nothing here." );

      break;

      case 'c': logger.push_back( "..." ); break;
      case 'q': quit = true; break;
      default: logger.push_back( "Is that key supposed to do something?" ); break;
    }

    if( inputDir.x or inputDir.y )
        if( not walk(&player, inputDir) )
            logger.push_back( "Cannot move there." );
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

ActorList::iterator actor_at( Vec pos )
{
    return std::find_if ( 
        actors.begin(), actors.end(), [&](const Actor& n) { return n.pos == pos; }
    );
}

bool walk( Actor* a, Vec dir )
{
    Vec newPos = a->pos + dir;

    bool inBounds = newPos.x > 0 and newPos.y > 0;
    inBounds &= newPos.y < map.size() and newPos.x < map[newPos.y].size();

    ActorList::iterator actorHere = actor_at( newPos );

    if( actorHere != actors.end() )
    {
        logger.push_back( "You punch the monster with your fist." );
        if( ! --actorHere->hp )
            actors.erase( actorHere );
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
