
#include "Vec.h"

#include <vector>
#include <string>

struct Map
{
    typedef std::string Row;
    typedef std::vector<Row> Tiles;

  private:
    bool flipped;
    Tiles tiles;

  public:
    Vec dims;

    Map();
    Map( Vec dims ); // Constructs a blank map of size dims.
    ~Map();

    void flip();

    void clear();
    void add_row( Row );

    Row& row( unsigned int );
    
    char& get( const Vec& );
    const char& get( const Vec& ) const;
};
