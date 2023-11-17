#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;
in vec2 TextureCoord;

uniform sampler2D myTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform int use_texture;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 proj = fragPosLightSpace.xyz / fragPosLightSpace.w;
    proj = proj * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, proj.xy).r;
    float currentDepth = proj.z;

    float bias = max(0.05 * (1.0 - dot(Normal, lightPos)), 0.005);

    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    // ambient lighting
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
    
    // diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDirection, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
    
    float shadow = ShadowCalculation(FragPosLightSpace);

    vec4 texture_res = texture(myTexture, TextureCoord);

    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * objectColor;

    if (use_texture == 0) {
        FragColor = vec4(result, 1.0);
    } else {
        FragColor = texture_res * vec4(result, 1.0);
    }
}