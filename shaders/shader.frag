#version 330 core

in vec3 color;
in vec2 texc;

// For normal mapping
in vec3 lightVec; // Tangent space light vector
in vec3 eyeVec; // Tangent space eye vector
//in vec3 halfVec;

out vec4 fragColor;

uniform sampler2D tex; // Texture sampler
uniform sampler2D normalMap; // Normal map texture
uniform int useTexture = 0;
uniform bool useNormalMap = false;

uniform mat4 v;

// Light data
const int MAX_LIGHTS = 10;
uniform int lightTypes[MAX_LIGHTS];         // 0 for point, 1 for directional
uniform vec3 lightPositions[MAX_LIGHTS];    // For point lights
uniform vec3 lightDirections[MAX_LIGHTS];   // For directional lights
uniform vec3 lightAttenuations[MAX_LIGHTS]; // Constant, linear, and quadratic term
uniform vec3 lightColors[MAX_LIGHTS];

// Material data
uniform vec3 ambient_color;
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float shininess;

void main(){
    vec3 texColor = texture(tex, texc).rgb;
    texColor = clamp(texColor + vec3(1-useTexture), vec3(0), vec3(1));

    // Normal mapping round two
    vec3 TextureNormal_tangentspace = normalize(texture( normalMap, texc ).rgb*2.0 - 1.0);
    float diffuse = clamp( dot(TextureNormal_tangentspace, lightVec), 0.0, 1.0);

    vec3 lightReflection = normalize(-reflect(lightVec, TextureNormal_tangentspace));
    float specular = pow(max(0.0, dot(eyeVec, lightReflection)), shininess);

    vec3 normalcolor = vec3(0);

    // Add diffuse component
    normalcolor += max(vec3(0), lightColors[0] * texColor * diffuse);

    // Add specular component
    //normalcolor += max(vec3(0), lightColors[0] * specular_color * specular);

    // Add ambient color
    normalcolor += ambient_color;

    if(useNormalMap)
    {
        fragColor = vec4(normalcolor, 1.0);
        //fragColor = vec4(vec3(dot(TextureNormal_tangentspace, lightVec)), 1.0); // For debugging

    }
    else
    {
        fragColor = vec4(color * texColor, 1);
    }


    //fragColor = vec4(0.8, 0.3, 0.6, 1.0); // For debugging
}
