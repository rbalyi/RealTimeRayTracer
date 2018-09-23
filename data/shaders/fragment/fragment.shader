#version 330 core
out vec4 vFragColor;

in vec2 vTexCoords;

uniform sampler2D usDiffuseMap;

void main() {
	vFragColor = vec4(texture(usDiffuseMap, vTexCoords).rgb, 1.0);
}