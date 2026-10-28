#version 460 core

in vec2 uv;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out float WaterMask;

struct Cell
{
    uint type;
    uint padding0;
    uint padding1;
    uint padding2;
};

layout(std430, binding = 0) readonly buffer Grid
{
    Cell cells[];
};

uniform vec2 gridSize;

const vec3 BackgroundColor = vec3(0.055, 0.070, 0.100);

const vec3 WallColor = vec3(0.239, 0.227, 0.341);
const vec3 WallLight = vec3(0.667, 0.663, 0.859);
const vec3 WallDark = vec3(0.318, 0.318, 0.412);

const vec3 SoilColor = vec3(0.949, 0.337, 0.306);
const vec3 SoilLight = vec3(1.0, 0.478, 0.404);
const vec3 SoilDark = vec3(0.42, 0.102, 0.102);

const vec3 LooseSoilColor = vec3(0.612, 0.153, 0.114);
const vec3 LooseSoilLight = vec3(0.749, 0.235, 0.165);
const vec3 LooseSoilDark = vec3(0.424, 0.094, 0.086);

const vec3 WaterLight = vec3(0.588, 1.0, 0.925);
const vec3 WaterMid = vec3(0.306, 0.718, 1);
const vec3 WaterDark = vec3(0.286, 0.192, 0.671);

bool InBounds(ivec2 p)
{
    return p.x >= 0 && p.x < int(gridSize.x) && p.y >= 0 && p.y < int(gridSize.y);
}

float GetWater(ivec2 p)
{
    if (!InBounds(p))
        return 0.0;

    uint type = cells[uint(p.y * int(gridSize.x) + p.x)].type;

    return type == 2u ? 1.0 : 0.0;
}

uint GetType(ivec2 p)
{
    if (!InBounds(p))
        return 0u;

    return cells[uint(p.y * int(gridSize.x) + p.x)].type;
}

vec3 GetWaterColor(float verticalPosition)
{
    vec3 color = mix(WaterDark, WaterMid, smoothstep(0.0, 0.55, verticalPosition));

    color = mix(color, WaterLight, smoothstep(0.55, 1.0, verticalPosition));

    return color;
}

vec3 GetMaterialColor(vec3 darkColor, vec3 baseColor, vec3 lightColor, float verticalPosition)
{
    vec3 color = mix(darkColor, baseColor, smoothstep(0.0, 0.65, 1.0 - verticalPosition));

    color = mix(color, lightColor, smoothstep(0.65, 1.0, 1.0 - verticalPosition));

    return color;
}

void main()
{
    vec2 gridPosition = uv * gridSize;
    ivec2 p = ivec2(gridPosition);
    vec2 f = fract(gridPosition);

    p = clamp(p, ivec2(0), ivec2(gridSize) - 1);

    uint type = GetType(p);

    WaterMask = 0.0;

    float verticalPosition = 1.0 - uv.y;

    if (type == 1u)
    {
        vec3 color = GetMaterialColor(WallDark, WallColor, WallLight, verticalPosition);

        FragColor = vec4(color, 1.0);
        return;
    }

    if (type == 3u)
    {
        vec3 color = GetMaterialColor(SoilDark, SoilColor, SoilLight, verticalPosition);

        FragColor = vec4(color, 1.0);
        return;
    }

    if (type == 4u)
    {
        vec3 color = GetMaterialColor(LooseSoilDark, LooseSoilColor, LooseSoilLight, verticalPosition);

        FragColor = vec4(color, 1.0);
        return;
    }

    float a = GetWater(p);
    float b = GetWater(p + ivec2(1, 0));
    float c = GetWater(p + ivec2(0, 1));
    float d = GetWater(p + ivec2(1, 1));

    float water = mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
    water = smoothstep(0.25, 0.75, water);

    WaterMask = water;

    vec3 waterColor = GetWaterColor(uv.y);
    vec3 color = mix(BackgroundColor, waterColor, water);

    FragColor = vec4(color, 1.0);
}
