const float QUADRATIC           = 0.032;
const float LINEAR              = 0.09;
const float CONSTANT            = 1.0;
const vec3  AMBIENT             = vec3(0.05f);

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform sampler2D   diffuse;

out vec4 fragColor;

void main()                                         
{
    vec3 diffuseSample  = texture(diffuse, texCoord).rgb;
#ifndef UI_ELEMENT
    vec3 result = vec3(0.0);

    // single ambient
    result += AMBIENT * texture(diffuse, texCoord).rgb;

    vec3 camAhead       = normalize(vec3(camPos) - fragPos);

    for (int i = 0; i < POINT_LIGHT_COUNT; i++)
    {
        if (point.enabled[i].x == 0) continue;

        vec3 lightDir       = normalize(vec3(point.positions[i]) - fragPos); 

        // diffuse:
        vec3 diff           = max(dot(normal, lightDir), 0.0) * vec3(point.diffuses[i]) * diffuseSample;  
       
        // specular:
        vec3 reflectDir     = reflect(-lightDir, normal); 
        vec3 specular       = vec3(point.speculars[i]) * pow(max(dot(camAhead, reflectDir), 0.0), 32.0 /*shininess*/) * diffuseSample.x /*specularity*/;  
        
        // attenuation
        float dist          = length(vec3(point.positions[i]) - fragPos);
        float attenuation   = 1.0 / (CONSTANT + LINEAR * dist + QUADRATIC * (dist * dist));
        diff                *= attenuation; 
        specular            *= attenuation; 
        
        result              += diff + specular;
    }
    for (int i = 0; i < SPOTLIGHT_COUNT; i++)
    {
        if (spot.enabled[i].x == 0) continue;

        vec3 lightDir       = normalize(vec3(spot.positions[i]) - fragPos); 
        
        // diffuse:
        float diff          = max(dot(normal, lightDir), 0.0);
        vec3 diff2          = diff * vec3(spot.diffuses[i]) * diffuseSample;  
       
        // specular:
        vec3 reflectDir     = reflect(-lightDir, normal); 
        vec3 specular       = vec3(spot.speculars[i]) * pow(max(dot(camAhead, reflectDir), 0.0), 32.0 /*shininess*/) * diffuseSample.x /*specularity*/;  
        
        // attenuation
        float dist          = length(vec3(spot.positions[i]) - fragPos);
        float attenuation   = 1.0 / (CONSTANT + LINEAR * dist + QUADRATIC * (dist * dist));
        diff2               *= attenuation; 
        specular            *= attenuation; 
        
        // soft edges
        float theta         = dot(lightDir, normalize(-vec3(spot.directions[i]))); 
        float epsilon       = 0.939692620 - 0.887010833;
        float intensity     = clamp((theta - 0.887010833) / epsilon, 0.0, 1.0); 
        diff2               *= intensity;
        specular            *= intensity;
    
        result              += diff2 + specular;
    }
    fragColor = vec4(result, 1.0); 
    //fragColor = vec4((normal + 1.0) * 0.5, 1.0);
    #else
    fragColor = vec4(diffuseSample, 1.0);
    #endif
}