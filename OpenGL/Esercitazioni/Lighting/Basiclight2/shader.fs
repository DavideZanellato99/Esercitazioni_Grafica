// MODIFICHE
// Valore di shininess: riga 43

#version 330 core
out vec4 FragColor;
in vec3 Normal;  
in vec3 FragPos;  

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;


void main()
{
    // Luce ambientale
    // ambientStrength impostato a 1, notiamo che quando la luce illumina il cubo, il suo colore diventa un giallo intenso e anche le parti del cubo
    // che non sono direttamente illuminate dalla luce presentano un colore intenso 
    //float ambientStrength = 1.0f;
    // Originale
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;


    // Luce diffusa
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);  

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    
    // Luce speculare
    // Originale
    // float specularStrength = 0.5;
    // specularStrength impostato a 1, notiamo che il riflesso della luce è molto più intenso
    float specularStrength = 1.0;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm); 
    // Originale
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * lightColor;  

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}