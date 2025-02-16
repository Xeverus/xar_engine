#version 450

layout (location = 0) in vec3 fragColor;

layout (location = 0) out vec4 outColor;

layout (push_constant) uniform Constants {
    float frame;
} constants;

void main() {
    float multiplier = (1.0 + sin(constants.frame * 0.05)) * 0.5;
    outColor = vec4(fragColor * multiplier, 1.0);
}