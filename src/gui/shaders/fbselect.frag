#version 450

uniform bool highlight_face;

out highp uvec3 color;

void main()
{

   uint id = uint(gl_PrimitiveID);
   color = uvec3(id, 0, 0);

   //        float id = float(gl_PrimitiveID)/12;
   //        color = vec3(id, 0.0f, 0.0f);
   //        id = uint(gl_PrimitiveID);
   // FragColor = vec3(float(gObjectIndex), float(gDrawIndex),float(gl_PrimitiveID + 1));
}
