#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightCol;
uniform vec3 viewPos;

struct Material {
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    float shininess;
    float transparency;
};

uniform Material material;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform bool hasTexture;


void main()
{
    vec3 norm = normalize(Normal);
    
    vec3 diffuseColor = material.diffuse;
    vec3 specularColor = material.specular;
    
    if (hasTexture) {
        diffuseColor = texture(texture_diffuse1, TexCoords).rgb * material.diffuse;
        specularColor = texture(texture_specular1, TexCoords).rgb * material.specular;
    }
    
    vec3 ambient = material.ambient * lightCol;
    
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor * lightCol;
    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * specularColor * lightCol;
    
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, material.transparency);
};