#version 330 core
struct Material {
    sampler2D   diffuse; 
    sampler2D   specular;
    float       shininess; 
};
struct Light {
    vec3 position; 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Material    material; 
uniform Light       light; 

in vec2 TexCoord; 
in vec3 FragPos; 
in vec3 Normal; 

out vec4 FragColor;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

void main() 
{
    float ambientStrength   = 0.1;
    float specularStrength  = 0.5; 

    vec3 normal     = normalize(Normal);
    vec3 lightDir   = normalize(light.position - FragPos);
    
    // ambient:
    vec3 ambient    = light.ambient * vec3(texture(material.diffuse, TexCoord));

    // diffuse:
    float diff      = max(dot(normal, lightDir), 0.0);
    vec3 diffuse    = diff * light.diffuse * vec3(texture(material.diffuse, TexCoord));  
    
    // specular:
    vec3 viewDir    = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal); 
    float spec      = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);  
    vec3 specular   = light.specular * spec * vec3(texture(material.specular, TexCoord));  

    // result
    vec3 result     = ambient + diffuse + specular; 
    FragColor       = vec4(result, 0.0); 
}