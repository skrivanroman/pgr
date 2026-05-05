#version 330 core 

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

out vec4 color;

void main()
{
	float gamma = 2.2;

	vec3 baseColor = material.diffuse;

	if (material.hasTexture) 
	{
		baseColor = texture(sampler, fTexCoords).rgb;
		baseColor = pow(baseColor, vec3(gamma));
	}

    vec3 lightDir = normalize(vec3(-0.5, -1.0, -0.3));
    vec3 lightColor = vec3(1.0);

    vec3 ambient = material.ambient * baseColor;

    vec3 norm = normalize(fNormal);
    float diff = max(dot(norm, -lightDir), 0.0);
    vec3 diffuse = diff * lightColor * baseColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(lightDir, norm);

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = material.specular * spec * lightColor;

	vec3 result = ambient + diffuse + specular;

	result = pow(result, vec3(1.0 / gamma));

	color = vec4(result, 1.0);
}
