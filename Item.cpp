
#include "Item.h"
#include "Log.h"

#include <algorithm>
#include <climits>
#include <cstdio>
#include <cstring> // for strcpy
#include <cctype> // for isspace

std::map< std::string, Shape > shapes;
std::map< std::string, Material > materials;
Catalogue catalogue;

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
}

Item basic_item( const std::string& m, const std::string& s )
{
    Item i( m, s );

    // Example name: wooded box.
    i.name = materials[m].adjective + " " + s;

    return i;
}

Item container_item( const std::string& m, const std::string& s, const std::string& c )
{
    Item i = basic_item( m, s );

    // Example: wooden box of rocks.
    i.name += " of " + c;
    i.components.push_back( c );

    return i;
}

Item complex_item( const std::string& shape, 
                   const std::string& mainCmp, const std::string& scndCmp )
{
    const auto& ref = catalogue[ mainCmp ];

    // A complex object takes after its main component.
    Item i = basic_item( ref.material, shape );
    i.components.push_back( mainCmp );
    i.components.push_back( scndCmp );
    return i;
}

const std::string& add_to_catalogue( const Item& i )
{
    const auto& entry = catalogue.find( i.name );
    if( entry == catalogue.end() )
        catalogue[ i.name ] = i;
    return i.name;
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
bool _read_subject( char* subject, char* subtopic, const char*const defaultSub, Buf& b )
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


    const char* const def = " ";
    while( _read_subject(subject, subtopic, def, buf) )
    {
        auto& shape = shapes[ subject ];
        shape = { {}, 0, *subtopic };

        while( (line = _next_detail(buf)) )
        {
            if( strncmp(line, "makeup", sizeof "makeup"-1) == 0 )
            {
                while( *line and *line != '\n' )
                {
                    // Skip to the next word.
                    while( *line and not isspace(*line) )
                        line++;
                    while( *line and isspace(*line) )
                        line++;

                    if( *line )
                    {
                        char makeup[30];
                        sscanf( line, "%s", makeup );
                        shape.possibleMakup.push_back( makeup );
                    }
                }
            }

            if( sscanf(line, "volume = %d", &shape.volume) )
            {
            }
        }
    }
}

template< typename F >
int _accumulate_item( const std::string& name, F f )
{
    auto iter = catalogue.find( name );
    if( iter == catalogue.end() )
    {
        log( "_accumulated_item: %s does not exist.", name.c_str() );
        return 0;
    }

    auto& i = iter->second;
    int sum = f( i );
    for( const auto& c : i.components )
        sum += _accumulate_item( c, f );
    return sum;
}

int mass( const std::string& name )
{
    auto m = []( const Item& i ){ 
        return materials[i.material].density * shapes[i.shape].volume; 
    };
    return _accumulate_item( name, m );
}

int durrability( const std::string& name )
{
    auto d = [](const Item& i){ 
        return materials[i.material].durrability; 
    };
    return _accumulate_item( name, d );
}

