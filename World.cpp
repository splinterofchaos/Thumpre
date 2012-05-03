
#include "World.h"
#include "Log.h"

#include <ncurses.h>
#include <algorithm>

Map map;
Inventory items;
bool quit = false;
ActorList actors;

Inventory::iterator inp_inventory_item( Inventory& inventory )
{
    if( not inventory.size() )
    {
        log( "You have nothing." );
        return inventory.end();
    }

    uint c = prompt( "Choose an item (a-%c)", inventory.size() - 1 + 'a' );

    if( c < inventory.size() )
        return inventory.begin() + c;
    else
        return inventory.end();
}

bool combine( Inventory* inv )
{
    if( inv->size() < 2 )
    {
        log( "You have %s.", inv->size() ? "only one thing" : "nothing" );
        return false;
    }


    log( "Combine... " );
    Inventory::iterator first  = inp_inventory_item(*inv);

    log( "with..." );
    Inventory::iterator second = inp_inventory_item(*inv);

    if( first == inv->end() or second == inv->end() )
        return false;

    if( first->type == Item::ROD and second->type == Item::WIG )
    {
        Item product( {0,0}, "Wooden Broom", '/', Item::WOOD, Item::ROD );
        inv->erase( first );
        inv->erase( second );
        inv->push_back( product );
    }

    return true;
}

void transfer( Inventory* to, Inventory* from, Inventory::iterator what )
{
    to->push_back( *what );
    from->erase( what );
}

Inventory::iterator item_at( Vec pos )
{
    return std::find_if ( 
        items.begin(), items.end(), [=](const Item& i) { return i.pos == pos; }
    );
}

ActorList::iterator actor_at( Vec pos )
{
    return std::find_if ( 
        actors.begin(), actors.end(), [=](const Actor& n) { return n.pos == pos; }
    );
}

bool attack( const Actor& attacker, Actor& defender )
{
    int materialDensity = 0;
    int typePower = 0;

    Item weapon = attacker.unarmed_weapon();

    switch( weapon.material )
    {
      case Item::WOOD: materialDensity = 10; break;
      case Item::HAIR: materialDensity = 2;  break;
      case Item::SKIN: materialDensity = 5;  break;
      default: materialDensity = 1;
    }

    switch( weapon.type )
    {
      case Item::ROD:  typePower = 5; break;
      case Item::WIG:  typePower = 1; break;
      case Item::HAND: typePower = 3; break;
      default: typePower = 1;
    }

    typePower += attacker.strength;

    int attackStrength = materialDensity * typePower;

    defender.hp -= attackStrength;
    if( defender.hp < 1 )
    {
        defender.hp = 0;
        return true;
    }

    return false;
}

bool walk( Actor& a, Vec dir )
{
    auto sgn = [](int x){return x>0? 1 : x<0? -1 : 0;};
    Vec newPos = a.pos + Vec( sgn, dir );

    ActorList::iterator actorHere = actor_at( newPos );

    if( actorHere != actors.end() )
    {
        std::string message( 1, a.image );

        bool killed = attack( a, *actorHere );
        log( "%c %s %c", a.image, killed? "killed":"hit", actorHere->image );
        if( killed )
            actors.erase( actorHere );
    }
    else if( map.get(newPos) != '#' )
    {
        a.pos = newPos;
    }
    else
    {
        return false;
    }

    a.cooldown += 25 / a.speed;

    return true;
}

void move_player( Actor& player )
{
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
              log( "Got %s,", player.inventory.back().name.c_str() );
          }
          else
              log( "There's nothing here." );

          break;

      case 'C': if( ! combine(&player.inventory) )
                    log( "I can't combine them." ); 
                break;

      case 'c': log( "..." ); break;
      case 'q': quit = true; break;
      default: log( "Is that key supposed to do something?" ); break;
    }

    if( inputDir.x or inputDir.y )
        if( not walk(player, inputDir) )
            log( "Cannot move there." );
}

