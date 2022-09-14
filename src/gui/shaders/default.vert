#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 aUv;


uniform mat4 mvp;

out vec3 fNorm;
out vec3 fPos;
out vec2 fUv;

void main()
{
   gl_Position = mvp * vec4(aPos, 1.0f);
   fPos = aPos;
   fNorm = aNorm;
   fUv = aUv;
}
