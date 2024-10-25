#version 460

//Per-Pixel Linked List

struct Fragment {
    uint nextIndex;
    uint packedColor;
    float depth;
    float unusedPadding;
};

layout (early_fragment_tests) in;
layout (binding = 1) uniform atomic_uint atomicFragmentIndex;
layout (binding = 6, r32ui) uniform coherent uimage2D fragmentStartIndices;
layout (std430, binding = 0) buffer FragmentBlock {
    Fragment fragments[];
};

layout (location = 0) out vec4 FragColor;

//PBR

const float PI = 3.14159265359;
const float EPSILON = 0.0001;

layout (std140) uniform Matrices{
    mat4 projection;
    mat4 view;
    vec3 viewPos;
};

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec2 texCoords;
    vec4 fragPosLightSpace;
} fs_in;

const int MAX_POINT_LIGHTS = 128;
layout (std140) uniform PointLights{
    vec4 amountOfActiveLights_unused_unused_unused;
    vec4 pointPosition_pointRange[MAX_POINT_LIGHTS];
    vec4 pointColor_pointIntensity[MAX_POINT_LIGHTS];
};
float amountOfActiveLights = amountOfActiveLights_unused_unused_unused.x;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D occlusionMap;

uniform vec4 albedoColor;
uniform vec2 textureTiling;
uniform float albedoScalar;
uniform float normalScalar;
uniform float metallicScalar;
uniform float roughnessScalar;
uniform float occlusionScalar;
uniform float skyboxScalar;
uniform int castShadows;
uniform int receivesShadows;
uniform int alphaCutoffEnabled;
uniform float alphaCutoffAmount;


vec3 getNormalFromMap(){
    //    return fs_in.normal;
    vec3 tangentNormal = texture(normalMap, fs_in.texCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fs_in.fragPos);
    vec3 Q2  = dFdy(fs_in.fragPos);
    vec2 st1 = dFdx(fs_in.texCoords);
    vec2 st2 = dFdy(fs_in.texCoords);

    vec3 N   = normalize(fs_in.normal) * normalScalar;
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness){
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness){
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness){
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0){
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


vec4 get_color(){
    vec2 TexCoords = fs_in.texCoords;


    vec4 v4_albedo  = texture(albedoMap, TexCoords);
    v4_albedo = mix(v4_albedo, albedoColor, albedoScalar);
    vec3 albedo     = pow(v4_albedo.rgb, vec3(2.2));
    float metallic = texture(metallicMap, TexCoords).r * metallicScalar;
    float roughness = texture(roughnessMap, TexCoords).r * roughnessScalar;
    float ao = texture(occlusionMap, TexCoords).r * occlusionScalar;

    vec3 camPos = viewPos;
    vec3 WorldPos = fs_in.fragPos;
    // input lighting data
    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - WorldPos);
    vec3 R = reflect(-V, N);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    //    for (int i = 0; i < 4; ++i)

    //    for (int i = 0; i < amountOfActiveLights; ++i){
    //        vec3 lightPos = pointPosition_pointRange[i].xyz;
    //        vec3 lightColor = pointColor_pointIntensity[i].xyz;
    //
    //        vec3 norm = normalize(fs_in.normal);
    //        vec3 lightDir = normalize(lightPos - fs_in.fragPos);
    //        float diff = max(dot(norm, lightDir), 0.0);
    //        diffuse += diff * lightColor;
    //    }
    //    if (i == 0){
    //        fragColor = vec4(1.0, 0.0, 1.0, 1.0);
    //        return;
    //    }
    float attenuationAvg = 0;
    for (int i = 0; i < amountOfActiveLights; ++i){


        vec3 lightPos = pointPosition_pointRange[i].xyz;
        //        vec3 lightColor = pointColor_pointIntensity[i].xyz;
        vec3 lightColor = vec3(300.0f, 300.0f, 300.0);
        // calculate per-light radiance
        vec3 L = normalize(lightPos - WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPos - WorldPos);
        float attenuation = 1.0 / (distance * distance);
        attenuationAvg += attenuation;
        vec3 radiance = lightColor * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 numerator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;// + 0.0001 to prevent divide by zero
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
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    //    vec3 irradiance = texture(irradianceMap, N).rgb;
    //    vec3 diffuse      = irradiance * albedo;
    //
    //    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    //    const float MAX_REFLECTION_LOD = 4.0;
    //    vec3 prefilteredColor = textureLod(prefilterMap, R, roughness * MAX_REFLECTION_LOD).rgb;
    //    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    //    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    //    vec3 ambient = (kD * diffuse + specular) * ao;
    vec3 ambient = vec3(0.03) * albedo * ao;


    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));

    float iorAvg = (F.x + F.y + F.z) / 3;// * roughness;
    attenuationAvg = attenuationAvg / (1 + amountOfActiveLights);

    float alphaAmount = iorAvg * albedoColor.a * roughness;
    float reveal = alphaAmount;
    vec2 uv = fs_in.texCoords;
    vec4 outCol = vec4(color.rgb, alphaAmount);
    reveal = texture(albedoMap, uv).a * alphaAmount;
    outCol.a = reveal;
    return outCol;
}

void gather_pass(){
    vec4 outColor = get_color();
    //    vec4 outColor = albedoColor;

    uint ssboIndex = atomicCounterIncrement(atomicFragmentIndex);
    uint lastIndex = imageAtomicExchange(fragmentStartIndices, ivec2(gl_FragCoord.xy), ssboIndex);

    fragments[ssboIndex].packedColor = packUnorm4x8(outColor);
    fragments[ssboIndex].depth = gl_FragCoord.z;
    fragments[ssboIndex].nextIndex = lastIndex;
}

void main() {
    gather_pass();
}