#version 330 core

in vec3 texc;
uniform samplerCube cube_texture;

out vec4 fragColor;

void main () {
    //vec3 color = texture(cube_texture, texc);
    //fragColor = vec4(color, 1.0);
    fragColor = texture(cube_texture, texc);
}
