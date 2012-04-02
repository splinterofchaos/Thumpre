
class Tiles
{
    bool flipped;
    char* tiles;

  public:

    unsigned int width, height;

    Tiles();
    Tiles( unsigned int width, unsigned int heigth );
    ~Tiles();

    void flip();
    void reset( unsigned int width, unsigned int heigth );
    
    char& get( unsigned int x, unsigned int y );
    const char& get( unsigned int x, unsigned int y) const;
};
