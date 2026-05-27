#version 450
#pragma shader_stage(fragment)


layout(location = 0) in float lightIntensity;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 lightDir;
layout(location = 3) in vec3 cameraDir;
layout(location = 4) in vec3 lightColor;
layout(location = 5) in mat3 TBN;
//layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 outColor;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform sampler2D metallicSampler;
layout(binding = 3) uniform sampler2D normalSampler;
layout(binding = 4) uniform sampler2D roughnessSampler;
/*
vec3 calcNormal(vec3 n)
{
    vec3 detlaV1 = dFdx(fragPos);
    vec3 detlaV1 = dFdx(fragPos);
}


    if (NdotL < 0)
    {
        outColor = baseColor;
        return;
    }

*/

#define PI radians(180.0)

//vec3 lightColor = vec3(1.0, 1.0, 1.0); // °×É«¹â
//float lightIntensity = 20.0;

void main() {
    vec3 baseColor = texture(texSampler, fragTexCoord).rgb;
    //outColor = 2*baseColor;
    //return;

    vec3 textureNormal = texture(normalSampler, fragTexCoord).rgb;
    vec3 N = normalize(TBN * (2 * textureNormal - 1));
    float NdotL = dot(N, lightDir);

    float roughness = texture(roughnessSampler, fragTexCoord).x;
    float metallic = texture(metallicSampler, fragTexCoord).x;
    //vec3 actualNormal = normalize(TBN * textureNormal);

    vec3 H = normalize(lightDir + cameraDir);
    float alpha2 = pow(roughness, 4);
    float NdotH =dot(N, H);
    float NdotV =dot(N, cameraDir);
    float HdotV =dot(H, cameraDir);


    vec3 F0 = mix(vec3(0.04), baseColor, metallic);
    vec3 F = F0 + (1 - F0) * pow((1 - HdotV), 5);

    float D_GGX = alpha2 / (PI * pow(NdotH * NdotH * (alpha2 - 1) + 1, 2));

    //float G = (2 * NdotL * NdotV) / (NdotL * ((1 - roughness) * NdotV + roughness) + NdotV * ((1 - roughness) * NdotL + roughness));

    float correctionfactor = 4 * NdotL * NdotV;

    float a2 = roughness * roughness;
    float lambdaV = NdotL * (NdotV * (1.0 - a2) + a2);
    float lambdaL = NdotV * (NdotL * (1.0 - a2) + a2);

    float G = 0.5 / (lambdaV + lambdaL + 1e-5f);

    vec3 specular = F * D_GGX * 1 / correctionfactor;
    vec3 diffuse = (1 - F) * (1 - metallic) * baseColor / PI;
    vec3 fr = specular + diffuse;

    vec3 radiance = lightColor * lightIntensity;
    vec3 lightContribution = fr * radiance * NdotL;

    //vec3 ambient = vec3(0.03) * baseColor * 0.7;

    outColor = lightContribution;
    //outColor = vec3(NdotL, NdotL, NdotL);
    //outColor = TBN[2];
    
    //outColor = vec4(1, 1, 1, 1);
    //outColor = vec4(0, 0, actualNormal.z > 0.5 ? actualNormal.z : 0, 1);
    //outColor = vec4(0, actualNormal.y > 0.5 ? actualNormal.y : 0, 0, 1);
    //outColor = vec4(actualNormal.x > 0.5 ? actualNormal.x : 0, 0, 0, 1);
    //outColor = texture(texSampler, fragTexCoord);
    //outColor.rgb = pow(outColor.rgb, vec3(1.0 / 2.2));
}