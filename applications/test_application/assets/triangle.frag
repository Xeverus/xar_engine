#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec2 textureCoords;

layout (location = 0) out vec4 outColor;

layout(set = 0, binding = 1) uniform sampler2D texSampler;

layout (push_constant) uniform Constants {
    float frame;
} constants;

void main() {
    float multiplier = (1.0 + sin(constants.frame * 0.05)) * 0.5;
    outColor = texture(texSampler, textureCoords);
    outColor.r *= multiplier;
}