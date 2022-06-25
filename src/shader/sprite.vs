#version 430 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor;

out vec3 color;

uniform mat4 rotationMatrix;

void main(){
	color = vertexColor;
	gl_Position = rotationMatrix * vec4(vertexPosition, 1.0);
}
