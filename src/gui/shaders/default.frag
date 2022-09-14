#version 450 core

#define NUM_ADJACENT 3
#define NUM_WATCHES  8  // match to datatypes.h
#define MS_IN_DAY 86400000.0

in vec3 fPos;
in vec3 fNorm;
in vec2 fUv;

uniform int time;
uniform int selectedID;
uniform int soloSelected;

out vec4 fragColor;

layout(std430, binding = 0) buffer PARAM0 {
   float val[]; } param0;

layout(std430, binding = 1) buffer PARAM1 {
   float val[]; } param1;

layout(std430, binding = 2) buffer PARAM2 {
   float val[]; } param2;

layout(std430, binding = 3) buffer PARAM3 {
   float val[]; } param3;

layout(std430, binding = 4) buffer PARAM4 {
   float val[]; } param4;

layout(std430, binding = 5) buffer PARAM5 {
   float val[]; } param5;

layout(std430, binding = 6) buffer ADJ {
   int id[][NUM_ADJACENT]; } adj;

struct ShaderWatch {
   int   ints   [NUM_WATCHES];
   float floats [NUM_WATCHES];
   vec4  vec4s  [NUM_WATCHES];
};

layout(std430, binding = 7) buffer DEBUG {
   ShaderWatch watch; } debug;


void main()
{
   vec3 lightDir = normalize(vec3(10.0f, 10.0f, 10.0f) - fPos);
   vec3 norm = normalize(fNorm);
   float diff = max(dot(norm, lightDir), 0.0);

   vec4 color = {
      fUv.x,
      fUv.y,
      clamp(0, sin(time/MS_IN_DAY*100000), 1),
      1.0f
   };

   fragColor = diff * color;

   if (int(soloSelected) == 1 && adj.id.length() > 0) {
      for (int i = 0; i < NUM_ADJACENT; ++i) {
         int adj_id = adj.id[selectedID][i];
         if (gl_PrimitiveID == int(adj_id)) {
            fragColor = vec4(0.3, 0.0, 0.0, 1.0);
         }
      }
   }

   for (int i = 0; i < NUM_ADJACENT; ++i) {
      int adj_id = adj.id[selectedID][i];
      debug.watch.ints[i]   = adj_id;
      debug.watch.floats[i] = param0.val[adj_id];
      debug.watch.vec4s[i]  = vec4(0.1, 0.2, 0.3, float(adj_id));
   }

   float ambient = 0.2;
   fragColor += ambient;
}
