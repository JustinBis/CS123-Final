#version 330 core

in vec3 vp;
uniform mat4 P, V;
out vec3 texc;

void main () {
    texc = vp;
    vec4 pos = P * V * vec4 (vp, 1.0);
    gl_Position = pos;
}
