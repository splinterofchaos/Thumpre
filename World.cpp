
#include "World.h"

#include <ncurses.h>
#include <algorithm>

Map map;
Inventory items;
Logger logger;
bool quit = false;
ActorList actors;

// Defined at main.
extern void print_log();

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
    print_log();

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

      case 'C': if( ! combine(&player.inventory) )
                    logger.push_back( "I can't combine them." ); 
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

bool walk( Actor* a, Vec dir )
{
    Vec newPos = a->pos + dir;

    bool inBounds = newPos.x > 0 and newPos.y > 0;
    inBounds &= newPos.y < map.size() and newPos.x < map[newPos.y].size();

    ActorList::iterator actorHere = actor_at( newPos );

    if( actorHere != actors.end() )
    {
        logger.push_back( "Hit with what? (f=fist, i=item)" );
        print_log();

        static Item fist( a->pos, "fist", 'F', Item::SKIN, Item::HAND );

        Item* weapon = 0;
        Inventory::iterator itWeapon;
        switch( getch() )
        {
          case 'f': weapon = &fist; break;
          case 'i': itWeapon = inp_inventory_item(a->inventory); 
                    weapon = &(*itWeapon);
        }

        AttackValue atk( *a, *weapon );
        if( atk.hit( &(*actorHere) ) )
            logger.push_back( "You hit it." );

        if( actorHere->hp < 0 )
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
