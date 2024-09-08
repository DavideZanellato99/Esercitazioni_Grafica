#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

uniform vec3 viewPos;

// Material properties
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float     shininess;   
}; 
  
uniform Material material;

// Light intensity
struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;  

void main()
{    
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));     
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    // emission
    vec3 emission = vec3(0.0);
    if (texture(material.specular, TexCoords).r == 0.0)   // rough check for blackbox inside spec texture, if specular texture fragment color = vec3(0,0), then take emission texture fragment to output
    {
        // apply emission texture 
        emission = texture(material.emission, TexCoords).rgb;
    }
        
    
    FragColor = vec4(ambient + diffuse + specular + emission, 1.0);   
}