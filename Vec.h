
#pragma once

struct Vec
{
    int x, y;

    Vec();
    Vec( int x, int y );

    template< typename FType >
    Vec( FType f, Vec v )
    {
        x = f( v.x );
        y = f( v.y );
    }
};

bool operator == ( const Vec& a, const Vec& b );
bool operator != ( const Vec& a, const Vec& b );
Vec operator  +  ( const Vec& a, const Vec& b );
Vec operator  -  ( const Vec& a, const Vec& b );

template< typename N >
Vec operator * ( const Vec& a, const N n )
{
    return { a.x * n, a.y * n };
}

template< typename N >
Vec operator * ( const N n, const Vec& a )
{
    return a * n;
}
