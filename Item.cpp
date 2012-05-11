
#include "Item.h"
#include "Log.h"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstring> // for strcpy
#include <cctype> // for isspace

std::map< std::string, Shape > shapes;
std::map< std::string, Material > materials;

Item::Item()
    : Object({0,0}, ' ')
{
}

char _img( const std::string& shape )
{
    return shapes[ shape ].image;
}

// Leaf item.
Item::Item( const std::string& m, const std::string& s )
    : Object({0,0}, _img(s)), material(m), shape(s)
{
    name = materials[m].adjective + " " + s;
}

// A root item is nothing; just a placeholder for its parts.
Item::Item( const std::string& s, const Item& main, const Item& scnd )
    : Object(main.pos, main.image), 
      material("air"), shape(s)
{
    name = materials[main.material].adjective + " " + s;
    components.push_back( main );
    components.push_back( scnd );
}

Item::Item( const std::string& m, const std::string& s, const Item& i )
    : Object({0,0}, '!'), components(1,i), material(m), shape(s)
{
    name = m + " " + s + " of " + i.material;
}

// Most of the fallowing helper functions require a buffer, its size, and a
// source file. Here it is.
struct Buf
{
    enum { SIZE = 256 };
    char buf[SIZE];
    FILE* src;

    Buf( const char* const file )
    {
        src = nullptr;
        reset( file );
    }

    void reset( const char* const file )
    {
        if( src )
            fclose( src );
        if( file )
            src = fopen( file, "r" );
    }

    ~Buf()
    {
        reset( nullptr );
    }
};

// Returns the next non-blank, non-comment line or null.
// It also skips to the first non-whitespace on that line.
char* _next_line( Buf& b )
{
    while( fgets(b.buf, Buf::SIZE, b.src) )
    {
        char* line = b.buf;
        while( isspace(*line) )
            line++;
        if( *line != '\0' and *line != '#' )
            return line;
    }

    return nullptr;
}

// A subject can be in one of two forms:
//  1) subject {
//         details
//     }
//  2) subject "subtopic" {
//         details
//     }
// In the first case, subtopic = defaultSub.
// _next_detail (see below) can then read the source until the end of the
// subject.
bool _read_subject( char* subject, char* subtopic, char* defaultSub, Buf& b )
{
    char* line;
    while( (line = _next_line(b)) )
    {
        int suc = sscanf( line, "%s \"%s\" {", subject, subtopic );
        if( suc != 2 )
        {
            suc = sscanf( line, "%s {", subject );
            strcpy( subtopic, defaultSub );
        }

        if( suc )
            return true;
    }
    return false;
}

// Like _next_line, but returns null when the subject ends.
char* _next_detail( Buf& b )
{
    char* line = _next_line( b );
    return (line and *line != '}')? line : nullptr;
}

void init_items()
{
    Buf buf( "data/materials" );
    if( not buf.src )
        log( "Warning: Materials not loaded." );

    char subject[30], subtopic[30];
    char* line;
    while( _read_subject(subject, subtopic, subject, buf) )
    {
        auto& mat = materials[ subject ];
        mat = { subtopic, 0, INT_MAX };

        while( (line = _next_detail(buf)) )
        {
            if( sscanf(line, "density = %d", &mat.density) )
            {
            }
            else if( sscanf(line, "durability = %d", &mat.durrability) )
            {
            }
        }
    }

    buf.reset( "data/shapes" );
    if( not buf.src )
        log( "Warning: Shapes not loaded." );


    char* const def = " ";
    while( _read_subject(subject, subtopic, def, buf) )
    {
        auto& shape = shapes[ subject ];
        shape = { 0, *subtopic };

        while( (line = _next_detail(buf)) )
        {
            if( sscanf(line, "volume = %d", &shape.volume) )
                ;
        }
    }
}

template< typename F >
int _accumulate_item( const Item& i, F f )
{
    int sum = f( i );
    for( const auto& c : i.components )
        sum += _accumulate_item( c, f );
    return sum;
}

int mass( const Item& i )
{
    auto m = []( const Item& i ){ 
        return materials[i.material].density * shapes[i.shape].volume; 
    };
    return _accumulate_item( i, m );
}

int durrability( const Item& i )
{
    auto d = [](const Item& i){ 
        return materials[i.material].durrability; 
    };
    return _accumulate_item( i, d );
}
