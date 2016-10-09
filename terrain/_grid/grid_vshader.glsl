#version 330 core
uniform mat4 mvp;
uniform sampler2D tex;

in vec2 position;
out vec3 normal;
out vec2 uv;
out float gl_ClipDistance[1];


const float WATER_LEVEL = -0.0f;
const float offset = 1/1024.0f;

vec2 convert_uv_to_world(vec2 uv) {
    return uv * 2.0f - vec2(1.0f, 1.0f);
}

float get_height_at(vec2 uv) {
    return texture(tex, uv).x;
}

vec3 vertex_at(vec2 uv) {
    float height = get_height_at(uv);    
    vec2 pos = convert_uv_to_world(uv);
    return vec3(pos.x, height, pos.y);
}

vec3 compute_normal(vec2 uv, float offset) {
    vec3 right = vertex_at(vec2(uv.x + offset, uv.y));
    vec3 left = vertex_at(vec2(uv.x - offset, uv.y));
    vec3 upper = vertex_at(vec2(uv.x, uv.y + offset));
    vec3 lower = vertex_at(vec2(uv.x, uv.y - offset));

    vec3 x_gradient = normalize(right - left);
    vec3 y_gradient = normalize(upper - lower);

    return cross(y_gradient, x_gradient);
}

void main() {
    uv = (position + vec2(1.0, 1.0)) * 0.5;

    normal = compute_normal(uv, offset);

    vec3 pos_3d = vertex_at(uv);

    gl_Position = mvp * vec4(pos_3d, 1.0);
    gl_ClipDistance[0] = get_height_at(uv);
}
