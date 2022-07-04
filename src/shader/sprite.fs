#version 430 core

// in vec3 color;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D image;

void main(){
	//fragColor = vec4(color, 1.0f) * texture(image, texCoord);
	fragColor = texture(image, texCoord);
}