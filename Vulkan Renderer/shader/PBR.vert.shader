#version 450
#pragma shader_stage(vertex)

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec3 inTangent;


layout(location = 0) out float outLightIntensity;
layout(location = 1) out vec2 fragTexCoord; 
layout(location = 2) out vec3 outLightDir;
layout(location = 3) out vec3 outCameraDir;
layout(location = 4) out vec3 outLightColor;
layout(location = 5) out mat3 TBN;


layout(binding = 0) uniform UniformBufferObject{
    float lightIntensity;
    vec3 lightColor;
    vec3 lightPos;
    vec3 cameraPos;
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

/*
vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
    );

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
    );
*/

void main() {
    //gl_Position = vec4(inPosition, 1.0);
    //gl_Position = ubo.model * vec4(inPosition, 1.0);
    //gl_Position = ubo.view * ubo.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
    vec3 fragPosition = (ubo.model * vec4(inPosition, 1.0)).rgb;
    //注意法线应该左乘法线矩阵，此处写法是因为模型矩阵目前没有使用不均匀的缩放(即一直都是正交矩阵)，而法线矩阵恰好是模型矩阵的逆的转置矩阵
    vec3 fragNormal = (ubo.model * vec4(normalize(inNormal), 0.0)).rgb;
    vec3 Tangent = (ubo.model * vec4(normalize(inTangent), 0.0)).rgb;

    outCameraDir = normalize(ubo.cameraPos - fragPosition);
    //outLightDir = normalize(ubo.lightPos - fragPosition);
    //outCameraDir = normalize(ubo.cameraPos);
    //outLightDir = normalize(ubo.lightPos - fragPosition);
    outLightDir = normalize(ubo.lightPos);
    //outLightDir = ubo.lightPos;

    fragTexCoord = inTexCoord;
    vec3 N = normalize(fragNormal);
    vec3 T = normalize(Tangent - fragNormal * dot(fragNormal, Tangent));
    vec3 B = normalize(cross(N, T));
    TBN = mat3(T, B, N);

    outLightIntensity = ubo.lightIntensity;
    outLightColor = ubo.lightColor;
}