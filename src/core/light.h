#ifndef LIGHT_H
#define LIGHT_H

#include "datatypes.h"

class Light
{
   public:
      Light();

      TVec3 position;
      TVec3 ambient;
      TVec3 diffuse;
      TVec3 specular;

};

#endif // LIGHT_H
