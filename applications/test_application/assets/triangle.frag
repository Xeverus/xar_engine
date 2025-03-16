#version 450

#extension GL_EXT_nonuniform_qualifier : enable

layout (location = 0) in vec2 textureCoords;

layout (location = 0) out vec4 outColor;

layout(set = 1, binding = 1) uniform sampler2D textures[];

layout (push_constant) uniform Constants {
    float frame;
} constants;

void main() {
    float multiplier = (1.0 + sin(constants.frame * 0.05)) * 0.5;
    //outColor = texture(texSampler, textureCoords);
    outColor = vec4(vec3(pow(gl_FragCoord.z, 128)), 1);
    outColor.r *= multiplier;

    outColor = texture(textures[0], textureCoords);
}