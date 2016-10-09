#version 330 core
uniform sampler2D tex;
uniform sampler2D mirror_tex;

in vec2 uv;
out vec4 color;

const float WATER_LEVEL = -0.00f;
const vec3 water_color = vec3(0.05, 0.3, 0.5);

void main() {
    float transparency = 0.7f;
    float height = texture(tex, uv).x;
    if (height > WATER_LEVEL) {
        transparency = 0.0f;    
    }
    vec4 color_from_water = vec4(water_color, 1.0);    

	vec2 wsize = textureSize(mirror_tex, 0);

    vec2 _uv = vec2((gl_FragCoord.x/wsize.x), 1-(gl_FragCoord.y/wsize.y));

    vec4 color_from_mirror = vec4(texture(mirror_tex, _uv).rgb, 1.0);

   	vec4 mixed = mix(color_from_water, color_from_mirror, 0.3);
    color = vec4(mixed.rgb, transparency);

    
    //color = color_from_mirror;
}
