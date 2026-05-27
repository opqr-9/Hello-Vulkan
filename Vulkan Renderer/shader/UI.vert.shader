#version 450
#pragma shader_stage(vertex)

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 color; 

layout(push_constant) uniform PushConstants{
    vec2 scale;
    vec2 translate;
}pushConstants;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outuv;


void main() {
	gl_Position = vec4(pos * pushConstants.scale + pushConstants.translate, 0, 1);

	outuv = uv;
	//outuv = vec2(uv.x, 1 - uv.y);
	outColor = color;
}

//vec2 positions[3] = vec2[](
//    vec2(0.0, -0.5),
//    vec2(0.5, 0.5),
//    vec2(-0.5, 0.5)
//    );
//
//vec3 colors[3] = vec3[](
//    vec3(1.0, 0.0, 0.0),
//    vec3(0.0, 1.0, 0.0),
//    vec3(0.0, 0.0, 1.0)
//    );
//
//layout(location = 0) out vec3 outColor;
//
//void main() {
//    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
//    outColor = vec3(1,1,0);
//    //outColor = vec3(pos * pushConstants.scale + pushConstants.translate, 0);
//}