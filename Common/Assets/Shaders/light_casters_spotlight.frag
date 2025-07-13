#version 330 core

struct Material {
    sampler2D   diffuse; 
    sampler2D   specular;
    float       shininess; 
};
struct Light {
    vec3 position; 
    vec3 direction;
    float innerCutOff;   
    float outerCutOff; 

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    // attenuation
    float constant;
    float linear;
    float quadratic; 
};

uniform Material    material; 
uniform Light       light; 
uniform vec3 viewPos; 

in vec2 TexCoord; 
in vec3 FragPos; 
in vec3 Normal; 

out vec4 FragColor;

void main() 
{
    vec3  lightDir  = normalize(light.position - FragPos); 

    // ambient:
    vec3  ambient       = light.ambient * texture(material.diffuse, TexCoord).rgb;
    
    // diffuse:
    vec3  normal        = normalize(Normal);
    float diff          = max(dot(normal, lightDir), 0.0);
    vec3  diffuse       = diff * light.diffuse * texture(material.diffuse, TexCoord).rgb;  
    
    // specular:
    vec3  viewDir       = normalize(viewPos - FragPos);
    vec3  reflectDir    = reflect(-lightDir, normal); 
    float spec          = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);  
    vec3  specular      = light.specular * spec * texture(material.specular, TexCoord).rgb;  

    // attenuation
    float dist          = length(light.position - FragPos);
    float attenuation   = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));
    diffuse             *= attenuation; 
    specular            *= attenuation; 

    // soft edges
    float theta     = dot(lightDir, normalize(-light.direction));
    float epsilon   = light.innerCutOff - light.outerCutOff;    
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0); 
    diffuse         *= intensity;
    specular        *= intensity;

    // result
    vec3 result         = ambient + diffuse + specular; 
    FragColor = vec4(result, 1.0);
}