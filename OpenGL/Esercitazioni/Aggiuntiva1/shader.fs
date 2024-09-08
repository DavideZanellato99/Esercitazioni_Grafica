#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
    vec3 TangentLightDirection;
    vec3 TangentLightPosSpotLight;
    vec3 TangentlightDirectionSpotLight;
} fs_in;


struct DirLight {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 1

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;


uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_specular1; 

// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 color, vec3 specularColor, vec3 TangentLightDirection);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 TangentFragPos, vec3 TangentLightPos, vec3 viewDir, vec3 color, vec3 specularColor);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 TangentFragPos, vec3 TangentLightPosSpotLight, vec3 viewDir, vec3 color, vec3 specularColor, vec3 TangentlightDirectionSpotLight);


void main()
{           
    // Obtain normal from normal map in range [0,1]
    vec3 normal = texture(texture_normal1, fs_in.TexCoords).rgb;
    // Transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // This normal is in tangent space
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
   
    // Get diffuse color
    vec3 color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
    // Get specular color
    vec3 specularColor = texture(texture_specular1, fs_in.TexCoords).rgb;

    // Phase 1: Directional lighting
    vec3 result = CalcDirLight(dirLight, normal, viewDir, color, specularColor, fs_in.TangentLightDirection);

    // Phase 2: Point lights
    result += CalcPointLight(pointLights[0], normal, fs_in.TangentFragPos, fs_in.TangentLightPos, viewDir, color, specularColor);

    // Phase 3: Spot light
    result += CalcSpotLight(spotLight, normal, fs_in.TangentFragPos, fs_in.TangentLightPosSpotLight, viewDir, color, specularColor, fs_in.TangentlightDirectionSpotLight);
    
    FragColor = vec4(result, 1.0);
}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 color, vec3 specularColor, vec3 TangentLightDirection)
{
    vec3 lightDir = normalize(-TangentLightDirection);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    // Combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * specularColor;

    return (ambient + diffuse + specular);
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 TangentFragPos, vec3 TangentLightPos, vec3 viewDir, vec3 color, vec3 specularColor)
{
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    // Attenuation
    float distance = length(TangentLightPos - TangentFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  

    // Combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * specularColor;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 TangentFragPos, vec3 TangentLightPosSpotLight, vec3 viewDir, vec3 color, vec3 specularColor, vec3 TangentlightDirectionSpotLight)
{
    vec3 lightDir = normalize(TangentLightPosSpotLight - TangentFragPos);
    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);

    // Attenuation
    float distance = length(TangentLightPosSpotLight - TangentFragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));  

    // Spotlight intensity
    float theta = dot(lightDir, normalize(-TangentlightDirectionSpotLight));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // Combine results
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * spec * specularColor;

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}