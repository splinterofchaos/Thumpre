
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include <vector>
#include <string>

#include <ncurses.h>

typedef std::vector< std::string > Map;
Map map;

bool equal( const char* const a, const char* const b )
{
    return strcmp( a, b ) == 0;
}

struct Vec
{
    int x, y;
    Vec( int x, int y ) : x(x), y(y) {}
};

Vec operator + ( const Vec& a, const Vec& b )
{
    return { a.x + b.x, a.y + b.y };
}

struct Actor
{
    Vec pos;

    Actor( int x, int y )
        : pos( x, y )
    {
    }

    Actor( Vec pos )
        : pos( pos )
    {
    }
};

bool walk( Actor* a, Vec dir )
{
    Vec newPos = a->pos + dir;

    bool inBounds = newPos.x > 0 and newPos.y > 0;
    inBounds = inBounds and newPos.y < map.size() and newPos.x < map[newPos.y].size();

    if( inBounds and map[newPos.y][newPos.x] != '#' )
    {
        a->pos = newPos;
        return true;
    }

    return false;
}

int main( int argc, char** argv )
{
    initscr();
    //cbreak();
    noecho();
    refresh();
    keypad(stdscr, TRUE);

    map.push_back( "####" );
    map.push_back( "#..#" );
    map.push_back( "#..#" );
    map.push_back( "####" );

    Actor player( 1, 1 );

    bool quit = false;
    while( not quit )
    {
        static std::string lastMessage = "Hello";

        erase();
        for( unsigned int row=0; row < map.size(); row++ )
            mvprintw( row, 0, map[row].c_str() );
        mvaddch( player.pos.y, player.pos.x, '@' );
        mvprintw( 10, 5, lastMessage.c_str() );
        refresh(); 

        Vec inputDir = { 0, 0 }; // The direction the user wants to move, if any.

        int key = getch();
        switch( key )
        {
          case 'l': case KEY_RIGHT: inputDir = Vec(  1,  0 ); break;
          case 'h': case KEY_LEFT:  inputDir = Vec( -1,  0 ); break;
          case 'k': case KEY_UP:    inputDir = Vec(  0, -1 ); break;
          case 'j': case KEY_DOWN:  inputDir = Vec(  0,  1 ); break;

          case 'c': lastMessage = "..."; break;
          case 'q': quit = true; break;
          default: lastMessage = "Is that key supposed to do something?"; break;
        }

        if( inputDir.x or inputDir.y )
            if( not walk(&player, inputDir) )
                lastMessage = "Cannot move there.";
    }

    return 0;
}
