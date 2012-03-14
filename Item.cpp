
#include "Item.h"

Item:: Item( Vec pos, const std::string& name, char image, 
             Material material, Type type )
    : pos(pos), name(name), image(image), material(material), type(type)
{
}
