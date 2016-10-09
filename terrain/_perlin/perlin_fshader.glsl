#version 330 core
uniform sampler1D grad;

uniform float initial_freq;
uniform float H;
uniform float lacunarity;
uniform int octaves;

out vec3 color;
in vec2 uv;

// deep magic happening here
//http://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl
float random(vec2 seed) {
    return fract(sin(dot(seed, vec2(12.9898,78.233))) * 43758.5453);
}

vec2 bottom_left_corner(vec2 pos) {
    return floor(pos);
}

vec2 top_left_corner(vec2 pos) {
    vec2 res = bottom_left_corner(pos);
    res.y = res.y + 1;
    return res;
}

vec2 top_right_corner(vec2 pos) {
    vec2 res = top_left_corner(pos);
    res.x = res.x + 1;
    return res;
}

vec2 bottom_right_corner(vec2 pos) {
    vec2 res = bottom_left_corner(pos);
    res.x = res.x + 1;
    return res;
}

vec2 random_grad_at(vec2 pos) {
    float tex_coord = random(pos);
    return texture(grad, tex_coord).xy;
}

float fade(float t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);    
}

float lerp(float x, float y, float alpha) {
    return (1 - alpha) * x + alpha * y;    
}

float pnoise(vec2 pos) {
    vec2 bottom_left = bottom_left_corner(pos); 
    vec2 top_left = top_left_corner(pos);
    vec2 bottom_right = bottom_right_corner(pos);
    vec2 top_right = top_right_corner(pos);

    vec2 a = pos - bottom_left;
    vec2 b = pos - bottom_right;
    vec2 c = pos - top_left;
    vec2 d = pos - top_right;

    float s = dot(random_grad_at(bottom_left), a);
    float t = dot(random_grad_at(bottom_right), b);
    float u = dot(random_grad_at(top_left), c);
    float v = dot(random_grad_at(top_right), d);

    float st = lerp(s, t, fade(fract(pos.x)));
    float uV = lerp(u, v, fade(fract(pos.x)));
    
    float noise = lerp(st, uV, fade(fract(pos.y)));
    return noise;
}

float fBm(float frequency, float H, float lacunarity, int octaves) {
    float value = 0.0;

    for (int i = 0; i < octaves; i++) {
        value += pnoise(uv * frequency) * pow(lacunarity, -H*i);
        frequency *= lacunarity;
        }
    return value;
}

// Adapted from libnoise library
float ridged_fBm(vec2 uv, float frequency, float H, float lacunarity, int octaves) {
    float signal = 0.0f;
    float value = 0.0f;
    float weight = 1.0f;

    float offset = 1.0f;
    float gain = 1.2f;
    
    for (int i = 0; i < octaves; i++) {
        signal = pnoise(uv * frequency);

        // make the ridges
        signal = abs(signal);
        signal = offset - signal;

        signal *= signal;
        signal *= weight;

        weight = signal * gain;
        weight = clamp(weight, 0, 1);

        value += signal * pow(lacunarity, -H*i);
        frequency *= lacunarity;
    }
    return (value * 0.70f) - 1.0f;
}

void main() {
    float noise = ridged_fBm(uv, initial_freq, H, lacunarity, octaves);
    noise += 0.5;
    color = vec3(noise);
}
