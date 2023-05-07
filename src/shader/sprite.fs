#version 430 core

subroutine vec3 shadeModelType();
subroutine uniform shadeModelType shadeModel;

layout (location = 0) out vec4 fragColor;

// in vec3 color;
in vec2 texCoord;
in vec3 normal;  
in vec3 fragPos;

//out vec4 fragColor;

uniform sampler2D image;
uniform vec3 viewPos;

struct LightInfo
{
    vec3 Pos;
    vec3 Color;
};
uniform LightInfo Light;

float ambientStrength = 0.1f;
float specularStrength = 0.5f;

subroutine (shadeModelType)
vec3 phongModel()
{
    // Ambient
    vec3 ambient = ambientStrength * Light.Color;

    // Diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(Light.Pos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * Light.Color;
    
    // Specular
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    vec3 specular = vec3(0.0);
    if (diff > 0){
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        specular = specularStrength * spec * Light.Color;
    }

    return vec3(ambient + diffuse + specular);
}

subroutine (shadeModelType)
vec3 diffuseModel()
{
    // Ambient
    vec3 ambient = ambientStrength * Light.Color;

    // Diffuse 
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(Light.Pos - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * Light.Color;

    return vec3(ambient + diffuse);
}

void main(){
    //vec4 result = vec4(phongModel(), 1.0f);
    vec4 result = vec4(shadeModel(), 1.0f);
	fragColor = result * texture(image, texCoord);
}