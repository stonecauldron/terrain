#version 330 core
uniform mat4 MVP;

in vec3 vpoint;

out vec3 texCoords;


void main(){
    gl_Position =  MVP * vec4(vpoint,1); ///< still
    texCoords = vpoint;
}
