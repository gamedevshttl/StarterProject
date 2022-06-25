#version 430 core
layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec3 vertexColor;
layout (location = 2) in vec2 textureCoord;

out vec3 color;
out vec2 texCoord;

uniform mat4 rotationMatrix;

void main(){
	color = vertexColor;
	texCoord = textureCoord;
	gl_Position = rotationMatrix * vec4(vertexPosition, 1.0);
}
