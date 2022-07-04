#version 430 core
layout (location = 0) in vec3 vertexPosition;
//layout (location = 1) in vec3 vertexColor;
layout (location = 1) in vec2 textureCoord;

//out vec3 color;
out vec2 texCoord;

//uniform mat4 rotationMatrix;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
	//color = vertexColor;
	texCoord = textureCoord;
	//gl_Position = rotationMatrix * vec4(vertexPosition, 1.0);
	gl_Position = projection * view * model * vec4(vertexPosition, 1.0);
}
