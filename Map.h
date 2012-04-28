
#include "Vec.h"

#include <vector>
#include <string>

struct Map
{
    struct Tile
    {
        char c;
        bool visible;
    };

    typedef std::vector<Tile> Row;
    typedef std::vector<Row>  Grid;

  private:
    bool flipped;
    Grid tiles;

  public:
    Vec dims;

    Map();
    Map( Vec dims ); // Constructs a blank map of size dims.
    ~Map();

    void flip();

    void clear();
    void add_row( const std::string& );

    std::string row( unsigned int ) const;

    Tile&       tile( const Vec& );
    const Tile& tile( const Vec& ) const;
    bool visible( const Vec& ) const;
    void visible( const Vec&, bool );
    char&       get( const Vec& );
    const char& get( const Vec& ) const;
};
