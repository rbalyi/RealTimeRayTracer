#version 330 core
layout (location = 0) in vec4 vVertex;

out vec2 vTexCoords;

uniform mat4 umProjection;

void main() {
    gl_Position = umProjection * vec4(vVertex.xy, 0.0, 1.0);
    vTexCoords = vVertex.zw;
}  