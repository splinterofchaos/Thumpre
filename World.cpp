
#include "World.h"

#include <ncurses.h>
#include <algorithm>

Map map;
Inventory items;
Logger logger;
bool quit = false;
NpcList npcs;
Player player( Vec(0,0) );

Actor::Actor( Vec pos, char image )
: pos( pos ), image(image)
{
    hp = 5;
}

Player::Player( Vec pos )
    : Actor( pos, '@' )
{
    turnOver = comboMode = false;
}

extern void print_log( Vec where );

Inventory::iterator inp_inventory_item( Inventory& inventory )
{
    std::string prompt = "Choose an item (";
    if( inventory.size() )
    {
        prompt += "a-";
        prompt.push_back( inventory.size() - 1 + 'a' );
    }

    prompt += ") ";
    
    logger.push_back( prompt );
    print_log( Vec(0,1) );

    int c = getch() - 'a';

    if( c >= 0 and c < inventory.size() )
    {
        Inventory::iterator ret = inventory.begin() + c;
        return ret;
    }
    else
    {
        return inventory.end();
    }
}

bool combine( Inventory* inv )
{
    logger.clear();

    if( inv->size() < 2 )
    {
        logger.push_back( 
            std::string("You have ") 
            + ( ! inv->size() ? "nothing" : "only one thing." )
        );
        return false;
    }


    logger.push_back( "Combine... " );

    Inventory::iterator first  = inp_inventory_item(*inv);
    Inventory::iterator second = inp_inventory_item(*inv);

    if( first == inv->end() or second == inv->end() )
        return false;

    if( first->type == Item::ROD and second->type == Item::WIG )
    {
        Item product( Vec(0,0), "Wooden Broom", '/', Item::WOOD, Item::ROD );
        inv->erase( first );
        inv->erase( second );
        inv->push_back( product );
    }

    return true;
}

void Player::move()
{
    // The direction the user wants to move, if any.
    Vec inputDir = { 0, 0 }; 

    // We need to know if we're standing on an item, but can't construct
    // the object inside the switch, so do it now.
    Inventory::iterator itemHere = item_at( pos );

    // Most actions will end the turn; if one doesn't, it has to unset this.
    turnOver = true;

    if( comboMode )
    {
        comboMode = false;
        
    }

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
          {
              logger.push_back( "There's nothing here." );
          }

          break;

      case 'C': if( ! combine(&inventory) )
                    logger.push_back( "I can't combine them." ); 
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
