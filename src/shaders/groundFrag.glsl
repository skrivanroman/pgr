#version 330 core 

in vec2 fTexCoords;

uniform sampler2D sampler;
uniform sampler2D sampler2;

out vec4 color;

void main()
{
	vec3 c1 = texture(sampler, fTexCoords).rgb;
    vec3 c2 = texture(sampler2, fTexCoords).rgb;

    vec3 result = mix(c1, c2, 0.5);

    //color = vec4(result, 1.0);
    color = vec4(1, 0, 0, 1);
}
