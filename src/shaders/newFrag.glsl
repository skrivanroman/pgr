#version 330 core 

struct DirLight 
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight 
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
};

struct SpotLight 
{
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float cutOff;
    float outerCutOff;
    float linear;
    float constant;
};

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
	bool hasTexture;
};

in vec2 fTexCoords;
in vec3 fNormal;
in vec3 FragPos;

uniform sampler2D sampler;
uniform Material material;
uniform vec3 viewPos;

uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;

out vec4 color;

vec3 calcDirLight(DirLight light, vec3 norm, vec3 viewDir, vec3 baseColor)
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * baseColor;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;

    vec3 ambient = material.ambient * baseColor * light.ambient;

    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec3 baseColor)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * baseColor;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance);

    vec3 ambient = material.ambient * baseColor * light.ambient;

    return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec3 baseColor)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * light.diffuse * baseColor;

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * light.specular;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance);

    vec3 ambient = material.ambient * baseColor * light.ambient;

    return (ambient + diffuse + specular) * attenuation * intensity;
}

void main()
{
    float gamma = 2.2;

    vec3 baseColor = material.diffuse;

    if (material.hasTexture) 
    {
        baseColor = texture(sampler, fTexCoords).rgb;
        baseColor = pow(baseColor, vec3(gamma));
    }

    vec3 norm = normalize(fNormal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);

    result += calcDirLight(dirLight, norm, viewDir, baseColor);

    result += calcPointLight(pointLight, norm, FragPos, viewDir, baseColor);

    result += calcSpotLight(spotLight, norm, FragPos, viewDir, baseColor);

    vec3 fogColor = vec3(0.7, 0.8, 0.9);
    float fogDensity = 0.001;
    float dist = length(viewPos - FragPos);
    float fogFactor = exp(-dist * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    result = mix(fogColor, result, fogFactor);

    result = pow(result, vec3(1.0 / gamma));

    color = vec4(result, 1.0);
}