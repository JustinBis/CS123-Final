#version 330 core

in vec3 color;
in vec2 texc;

// For normal mapping
in vec3 lightVec; // Tangent space light vector
in vec3 eyeVec; // Tangent space eye vector
in vec3 halfVec;

out vec4 fragColor;

uniform sampler2D tex; // Texture sampler
uniform sampler2D normalMap; // Normal map texture
uniform int useTexture = 0;

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

    /*
    // Normal mapping
    vec3 normal = 2.0 * texture(normalMap, texc).rgb - 1.0;
    normal = normalize(normal);

    float lamberFactor= max (dot (lightVec, normal), 0.0);

    fragColor = vec4(0.0);

    if (lamberFactor > 0.0)
    {
            vec4 diffuseLight  = vec4(lightColors[0], 1.0);

            vec4 specularLight = vec4(lightColors[0], 1.0);
            float specIntensity = pow (max (dot (halfVec, normal), 0.0), shininess)  ;

            fragColor = vec4(texColor, 1.0) * diffuseLight * lamberFactor ;
            fragColor += vec4(specular_color, 1.0) * specularLight * specIntensity ;
    }

    fragColor += vec4(ambient_color, 1.0);
    clamp(fragColor, vec4(0), vec4(1));

    // End normal mapping
    */

    fragColor = vec4(color * texColor, 1);
    //fragColor = vec4(0.8, 0.3, 0.6, 1.0); // For debugging
}
