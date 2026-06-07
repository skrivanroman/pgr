#version 330 core

in vec2 TexCoords;

uniform float time;

out vec4 FragColor;

void main()
{
    float DAY_DURATION = 50000.0;
    
    float progress = mod(time, DAY_DURATION) / DAY_DURATION;
    
    int cycleIndex = int(time / DAY_DURATION);
    bool isDay = (cycleIndex % 2 == 0);

    bool inBarY = TexCoords.y > 0.90 && TexCoords.y < 0.95;
    bool inBarX = TexCoords.x > 0.10 && TexCoords.x < 0.90; 
    
    if (inBarX && inBarY)
    {
        float barUV_X = (TexCoords.x - 0.10) / 0.80;
        
        if (barUV_X <= progress)
        {
            vec3 fillColor = isDay ? vec3(0.9, 0.7, 0.1) : vec3(0.2, 0.3, 0.8); 
            FragColor = vec4(fillColor, 1.0);
        }
        else
        {
            FragColor = vec4(0.2, 0.2, 0.2, 0.8); 
        }
    }
    else
    {
        discard;
    }
}