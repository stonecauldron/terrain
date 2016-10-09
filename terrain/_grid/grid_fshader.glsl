#version 330 core
uniform sampler2D tex;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D sediment;
uniform sampler2D sand;
uniform sampler2D snow;

in vec3 normal;
in vec2 uv;
out vec3 color;

const vec3 light_dir = normalize(vec3(1.0, 1.0, 0.0));
const float WATER_LEVEL = -0.00f;
const float SNOW_LEVEL = 0.4f;

const vec3 water_color = vec3(0.05, 0.3, 0.5);

float fade(float x) {
    return x; 
}

float get_height(vec2 uv) {
    return texture(tex, uv).x;    
}

float compute_slope_factor(vec3 normal) {
    vec3 up = vec3(0,1,0);
    return dot(normal, up);
}

vec3 rock_texture(vec2 uv) {
    return texture(rock, 10*uv).rgb;
}

vec3 grass_texture(vec2 uv) {
    return texture(grass, 10*uv).rgb;
}

vec3 sediment_texture(vec2 uv) {
    return texture(sediment, 30*uv).rgb;
}

vec3 sand_texture(vec2 uv) {
    return texture(sand, 60*uv).rgb;    
}

vec3 snow_texture(vec2 uv) {
    return texture(snow, 30*uv).rgb;    
}

vec3 get_sloped_texture(float height, vec2 uv) {
    float deltaHeight = height - WATER_LEVEL;
    float alpha = exp(8*deltaHeight);
    alpha = clamp(alpha,0,1);
    return mix(sediment_texture(uv), rock_texture(uv), alpha);
}

vec3 get_plane_texture(float height, vec2 uv) {
    if (height < WATER_LEVEL + 0.01f) {
        float deltaHeight = height - WATER_LEVEL;
        float alpha = exp(20*deltaHeight);
        alpha = clamp(alpha,0,1);
        return mix(sand_texture(uv), grass_texture(uv), alpha);
    }
    float deltaHeight = height - SNOW_LEVEL;
    float alpha = exp(8*deltaHeight);
    alpha = clamp(alpha,0,1);
    return mix(grass_texture(uv), snow_texture(uv), alpha);
}

void main() {
    vec3 normal = normalize(normal);
    float intensity = max(dot(normal, light_dir), 0.0);

    // get textures adapted to current height
    float height = get_height(uv);
    vec3 plane_tex = get_plane_texture(height, uv);
    vec3 sloped_tex = get_sloped_texture(height, uv);

    float alpha = fade(compute_slope_factor(normal));
    vec3 tex = mix(sloped_tex, plane_tex, alpha);

    if(height < .0f) {
        tex = mix(tex, water_color, -height*5.0f);
    }

    color = vec3(intensity) * tex;
}
