
#pragma once

struct Vec
{
    int x, y;

    Vec();
    Vec( int x, int y );
};

bool operator == ( const Vec& a, const Vec& b );
bool operator != ( const Vec& a, const Vec& b );
Vec operator  +  ( const Vec& a, const Vec& b );
