
#include "Vec.h"

#include <cmath>

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

Vec operator - ( const Vec& a, const Vec& b )
{
    return { a.x - b.x, a.y - b.y };
}

int rogue_length( const Vec& v )
{
    return v.y > v.x? v.y : v.x;
}

float magnitude( const Vec& v )
{
    return std::sqrt( v.x*v.x + v.y*v.y );
}
