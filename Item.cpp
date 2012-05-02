
#include "Item.h"

Item:: Item( Vec pos, const std::string& name, char image, 
             Material material, Type type )
    : Object(pos,image), material(material), type(type)
{
    this->name = name;
}
