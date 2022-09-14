#version 450

layout (location = 3) in vec3 aPos;

uniform mat4 mvp;

uniform int projectionOff;

void main()
{
   if (projectionOff==1) {
      gl_Position = vec4(aPos, 1.0f);

   } else {
   gl_Position = mvp * vec4(aPos, 1.0f);
   }
}
