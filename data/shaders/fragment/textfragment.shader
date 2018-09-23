#version 330 core
in vec2 vTexCoords;

out vec4 vColor;

uniform sampler2D usText;
uniform vec3 uvTextColor;

void main() {
    vec4 vSampled = vec4(1.0, 1.0, 1.0, texture(usText, vTexCoords).r);
    vColor = vec4(uvTextColor, 1.0) * vSampled;
} 