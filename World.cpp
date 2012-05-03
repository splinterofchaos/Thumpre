
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

struct AttackValue
{
    int strength;
    
    AttackValue( const Actor& who, const Item& withWhat )
    {
        strength = who.hp / 8;
        
        int weaponVal = 0;

        switch( withWhat.material )
        {
          case Item::WOOD: weaponVal = 10;
          case Item::HAIR: weaponVal = 2;
          case Item::SKIN: weaponVal = 4;
        }

        switch( withWhat.type )
        { 
          case Item::ROD:  weaponVal *= 5;
          case Item::WIG:  weaponVal *= 1;
          case Item::HAND: weaponVal *= 4;
        }

        strength += weaponVal;
    }

    bool hit( Actor* victim )
    {
        if( ! victim )
            return false;

        victim->hp -= strength;
        return true;
    }
};

bool walk( Actor& a, Vec dir )
{
    Vec newPos = a.pos + dir;

    bool inBounds =
        newPos.x > 0 and newPos.y > 0
        and ( (uint)newPos.y < map.dims.y 
            and (uint)newPos.x < map.dims.x );
    if( not inBounds )
        return false;

    ActorList::iterator actorHere = actor_at( newPos );

    if( actorHere != actors.end() )
    {
        char c = prompt( "Hit with what? (f=fist, i=item)" );

        static Item fist( a.pos, "fist", 'F', Item::SKIN, Item::HAND );

        Item* weapon = 0;
        Inventory::iterator itWeapon;
        switch( c )
        {
          case 'f': weapon = &fist; break;
          case 'i': itWeapon = inp_inventory_item(a.inventory); 
                    weapon = &(*itWeapon);
        }

        AttackValue atk( a, *weapon );
        if( atk.hit( &(*actorHere) ) )
            log( "You hit it." );

        if( actorHere->hp < 0 )
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

