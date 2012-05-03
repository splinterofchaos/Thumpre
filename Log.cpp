
#include "Log.h"
#include "Vec.h"

#include <stdio.h>
#include <stdarg.h>
#include <ncurses.h>
#include <algorithm>


typedef std::vector< std::string >   Logger;
Logger logger;

void log_append( const char* const fmt, va_list args )
{
    char buf[ 256 ];
    vsprintf( buf, fmt, args );
    logger.back().append( buf );
}

void log_append( const char* const fmt, ... )
{
    va_list args;
    va_start( args, fmt );

    log_append( fmt, args );

    va_end( args );
}

void log( const char* const fmt, va_list args )
{
    logger.push_back( "" );
    log_append( fmt, args );
}

void log( const char* const fmt, ... )
{
    logger.push_back( "" );

    va_list args;
    va_start( args, fmt );

    log_append( fmt, args );

    va_end( args );
}

extern void print_everything();
char prompt( const char* const fmt, ... )
{
    va_list args;
    va_start( args, fmt );

    log( fmt, args );
    print_everything();

    va_end( args );

    return getch();
}

extern Vec logPos; // Defined in rogue.cpp

// It might make sense to put this in rogue.cpp with the other print_*
// functions, but being here, Logger needn't be known outside this file.
void print_log()
{
    uint row = 0;
    auto print_line = [&]( const std::string& line )
    { mvprintw( logPos.y + row++, logPos.x, line.c_str() ); };

    std::for_each( logger.begin(), logger.end(), print_line );
}

void clear_log()
{
    logger.clear();
}
