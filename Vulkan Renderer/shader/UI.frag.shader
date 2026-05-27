#version 450
#pragma shader_stage(fragment)

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform sampler2D texSampler;

void main() {
    //outColor = vec4(1, 0, 0, 1);
    //outColor = vec4(inColor.rgb/2,1);
    outColor = inColor*texture(texSampler, uv);
    //outColor = texture(texSampler,uv);
}

//layout(location = 0) in vec3 fragColor;
//
//layout(location = 0) out vec4 outColor;
//
//void main() {
//    outColor = vec4(fragColor, 1.0);
//}
