#version 460 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform sampler2D waterMaskTexture;
uniform vec2 texelSize;
uniform float blurRadius;

const vec3 BackgroundColor = vec3(0.055, 0.070, 0.100);

const float SofteningStrength = 2.0;
const float MaxFill = 0.35;

const vec3 WaterLight = vec3(0.588, 1.0, 0.925);
const vec3 WaterMid = vec3(0.12, 0.55, 0.85);
const vec3 WaterDark = vec3(0.255, 0.345, 0.812);

float GaussianBlur(vec2 coord)
{
    float mask = 0.0;
    float totalWeight = 0.0;

    for (int y = -2; y <= 2; y++)
    {
        for (int x = -2; x <= 2; x++)
        {
            vec2 offset = vec2(x, y) * texelSize * blurRadius;
            float weight = exp(-float(x * x + y * y) * 0.5);

            mask += texture(waterMaskTexture, coord + offset).r * weight;
            totalWeight += weight;
        }
    }

    return mask / totalWeight;
}

vec3 GetWaterColor(float verticalPosition)
{
    vec3 color = mix(WaterDark, WaterMid, smoothstep(0.0, 0.55, verticalPosition));
    color = mix(color, WaterLight, smoothstep(0.55, 1.0, verticalPosition));
    return color;
}

void main()
{
    vec4 scene = texture(sceneTexture, uv);
    float originalMask = texture(waterMaskTexture, uv).r;
    float blurredMask = GaussianBlur(uv);

    float coverage = smoothstep(0.12, 0.3, blurredMask);
    coverage = mix(originalMask, coverage, 0.75);

    vec3 waterColor = GetWaterColor(uv.y);
    vec3 color = mix(scene.rgb, waterColor, coverage);

    FragColor = vec4(color, scene.a);
}
