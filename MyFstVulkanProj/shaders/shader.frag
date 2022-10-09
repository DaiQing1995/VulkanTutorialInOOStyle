#version 450

// location is binding to shader vert output, not name
layout(location = 0) in vec3 fragColor;

// binding to framebuffer of location 0
layout(location = 0) out vec4 outColor;

void main() {
	outColor = vec4(fragColor, 1.0);
}