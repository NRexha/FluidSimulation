
#version 430 core

in vec2 uv;

out vec4 FragColor;

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
const vec3 WallColor = vec3(0.65, 0.70, 0.77);
const vec3 WaterColor = vec3(0.12, 0.55, 0.92);

const vec3 SoilColor = vec3(0.42, 0.25, 0.13);
const vec3 LooseSoilColor = vec3(0.55, 0.34, 0.18);

bool InBounds(ivec2 p)
{
    return p.x >= 0 && p.x < int(gridSize.x) &&
           p.y >= 0 && p.y < int(gridSize.y);
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

void main()
{
    vec2 gridPosition = uv * gridSize;
    ivec2 p = ivec2(gridPosition);
    vec2 f = fract(gridPosition);

    p = clamp(p, ivec2(0), ivec2(gridSize) - 1);

    uint type = GetType(p);



    if (type == 1u)
    {
        FragColor = vec4(WallColor, 1.0);
        return;
    }

    if (type == 3u)
    {
        FragColor = vec4(SoilColor, 1.0);
        return;
    }

    if (type == 4u)
    {
        FragColor = vec4(LooseSoilColor, 1.0);
        return;
    }

    float a = GetWater(p);
    float b = GetWater(p + ivec2(1, 0));
    float c = GetWater(p + ivec2(0, 1));
    float d = GetWater(p + ivec2(1, 1));

    float water = mix(mix(a, b, f.x), mix(c, d, f.x), f.y);

    water = smoothstep(0.25, 0.75, water);

    vec3 color = mix(BackgroundColor, WaterColor, water);

    FragColor = vec4(color, 1.0);
}
