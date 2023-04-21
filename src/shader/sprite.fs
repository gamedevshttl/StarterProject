#version 430 core

// in vec3 color;
in vec2 texCoord;
in vec3 normal;  
in vec3 fragPos;

out vec4 fragColor;

uniform sampler2D image;
uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 viewPos;

void main(){

    // Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

	//fragColor = vec4(color, 1.0f) * texture(image, texCoord);
    
    
    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 result = (ambient + diffuse + specular);
    
    //vec3 result = (specular);

    vec4 result4 = vec4(result, 1.0f);
	fragColor = result4 * texture(image, texCoord);
}