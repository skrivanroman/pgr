#version 330 core

in vec2 TexCoords;
uniform sampler2D spriteTexture;

out vec4 FragColor;

void main()
{
    vec4 color = texture(spriteTexture, TexCoords);
    
    if(color.a < 0.1)
    {
        discard;
    }
        
    FragColor = color;
}