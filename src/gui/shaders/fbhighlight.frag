#version 450

out vec4 fragColor;
uniform int projectionOff;

void main()
{
   if (projectionOff == 1){
      fragColor = vec4(1.0, 0.0, 0.0, 1.0);
   } else {
    fragColor = vec4(0.0, 1.0, 0.0, 1.0);
   }
}
