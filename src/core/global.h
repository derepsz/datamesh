#ifndef GLOBAL_H
#define GLOBAL_H

#include <string>


enum class SelectMode {
   NEW,
   ADD,
   REMOVE,
};



struct vec2
{
      union
      {
            float x{};
            float u;
      };
      union
      {
            float y{};
            float v;
      };
};

struct vec3
{
      union {
            float x;
            float r;
      };
      union {
            float y;
            float g;
      };
      union {
            float z;
            float b;
      };

      vec3() : x(), y(), z() {};

      vec3(float a, float b,  float c) {
         x = a;
         y = b;
         z = c;
      };

      bool operator==(const vec3& other){
         if (x == other.x
             && y == other.y
             && z == other.z) {
            return true;
         } else { return false; }
      }
};

struct vec4 {
      union {
            float x;
            float r;
      };
      union {
            float y;
            float g;
      };
      union {
            float z;
            float b;
      };
      union {
            float w;
            float a;
      };
};


#endif // GLOBAL_H
