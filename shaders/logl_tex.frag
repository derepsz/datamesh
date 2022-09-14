#version 450 core
#define NUM_LIGHTS  4
#define NUM_WATCHES 8

struct ShaderWatch {
   int   ints   [NUM_WATCHES];
   float floats [NUM_WATCHES];
   vec4  vec4s  [NUM_WATCHES];
};

layout(std430, binding = 7) buffer DEBUG {
   ShaderWatch watch; } debug;

layout(std430, binding = 0) buffer PARAM0 {
   float val[]; } cloud;

out vec4 FragColor;
in vec2 fUv;
in vec3 fPos;
in vec3 fNorm;

uniform sampler2D albedotex;
uniform sampler2D normaltex;
uniform sampler2D metallictex;
uniform sampler2D roughnesstex;
uniform sampler2D aotex;

uniform vec3 lightPositions[NUM_LIGHTS] = {
  vec3(20.0f, 10.0f, 10.0f),
  vec3(30.0f, 10.0f, 20.0f),
  vec3(-30.0f, 10.0f, 20.0f),
  vec3(-30.0f, -30.0f, 10.0f)
};
uniform vec3 lightColors[NUM_LIGHTS] = {
  vec3(5000.0f, 100.0f, 100.0f),
  vec3(100.0f, 100.0f, 10000.0f),
  vec3(10000.0f, 1000.0f, 100.0f),
  vec3(10000.0f, 100.0f, 1000.0f)
} ;

uniform vec3 camPos;

const float PI = 3.14159265359;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normaltex, fUv).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fPos);
    vec3 Q2  = dFdy(fPos);
    vec2 st1 = dFdx(fUv);
    vec2 st2 = dFdy(fUv);

    vec3 N   = normalize(fNorm);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
void main()
{
    vec3 albedo     = pow(texture(albedotex, fUv).rgb, vec3(2.2));
    float metallic  = texture(metallictex, fUv).r;
    float roughness = texture(roughnesstex, fUv).r;
    float ao        = texture(aotex, fUv).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - fPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < NUM_LIGHTS; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPositions[i] - fPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPositions[i] - fPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = (lightColors[i] - cloud.val[gl_PrimitiveID]) * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;

        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    // ambient lighting (note that the next IBL tutorial will replace
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    FragColor = vec4(color, 1.0);

    debug.watch.floats[0] = cloud.val[gl_PrimitiveID];
}
