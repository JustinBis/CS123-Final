#version 330 core

in vec3 color;
in vec2 texc;
out vec4 fragColor;

uniform sampler2D tex;
uniform int useTexture = 0;

void main(){
    vec3 texColor = texture(tex, texc).rgb;
    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));
    fragColor = vec4(color * texColor, 1);
    //fragColor = vec4(0.8, 0.3, 0.6, 1.0); // For debugging
}
