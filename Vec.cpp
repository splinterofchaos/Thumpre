
#include "Vec.h"

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
