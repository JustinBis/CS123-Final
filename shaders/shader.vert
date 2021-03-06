#version 330 core

in vec3 position; // Position of the vertex
in vec3 normal;   // Normal of the vertex
in vec2 texCoord; // UV texture coordinates
in vec3 tangent; // The tangent vector to the normal
in vec3 bitangent; // The bitangent vector to the normal

in float arrowOffset; // Sideways offset for billboarded normal arrows

out vec3 color; // Computed color for this vertex
out vec2 texc;

// For normal mapping
out vec3 lightVec; // Tangent space light vector
out vec3 eyeVec; // Tangent space eye vector
//out vec3 halfVec;

// Transformation matrices
uniform mat4 p;
uniform mat4 v;
uniform mat4 m;

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

uniform bool useLighting;     // Whether to calculate lighting using lighting equation
uniform bool useArrowOffsets; // True if rendering the arrowhead of a normal for Shapes
uniform vec3 allBlack = vec3(1);

void main(){
    texc = texCoord;

    vec4 position_cameraSpace = v * m * vec4(position, 1.0);
    vec4 normal_cameraSpace = vec4(normalize(mat3(transpose(inverse(v * m))) * normal), 0);

    vec4 position_worldSpace = m * vec4(position, 1.0);
    vec4 normal_worldSpace = vec4(normalize(mat3(transpose(inverse(m))) * normal), 0);

    // Normal mapping round two
    mat3 MV3x3 = mat3(transpose(inverse(v * m)));

    vec3 vertexNormal_cameraspace = MV3x3 * normalize(normal);
    vec3 vertexTangent_cameraspace = MV3x3 * normalize(tangent);
    vec3 vertexBitangent_cameraspace = MV3x3 * normalize(bitangent);

    mat3 TBN = transpose(mat3(
            vertexTangent_cameraspace,
            vertexBitangent_cameraspace,
            vertexNormal_cameraspace
        ));

    // Light's direction
    vec4 lightDir_cameraSpace = normalize(v * vec4(1.0 * lightDirections[0], 0.0));
    lightVec = TBN * vec3(lightDir_cameraSpace);
    lightVec = normalize(lightVec);

    //vec4 EyeDirection_tangentspace
    //eyeVec = TBN * vec3(normalize(vec4(0,0,0,1) - position_cameraSpace));
    eyeVec = TBN * vec3(position_cameraSpace);
    eyeVec = normalize(eyeVec);

    if (useArrowOffsets) {
        // Figure out the axis to use in order for the triangle to be billboarded correctly
        vec3 offsetAxis = normalize(cross(vec3(position_cameraSpace), vec3(normal_cameraSpace)));
        position_cameraSpace += arrowOffset * vec4(offsetAxis, 0);
    }

    gl_Position = p * position_cameraSpace;

    // Calculate lighting
    if (useLighting) {
        color = ambient_color.xyz; // Add ambient component

        for(int i = 0; i < MAX_LIGHTS; i++) {
            vec4 vertexToLight = vec4(0);
            // Point Light
            if (lightTypes[i] == 0) {
                vertexToLight = normalize(v * vec4(lightPositions[i], 1) - position_cameraSpace);
            } else if (lightTypes[i] == 1) {
                // Dir Light
                vertexToLight = normalize(v * vec4(-lightDirections[i], 0));
            }

            // Add diffuse component
            float diffuseIntensity = max(0.0, dot(vertexToLight, normal_cameraSpace));
            color += max(vec3(0), lightColors[i] * diffuse_color * diffuseIntensity);

            // Add specular component
            vec4 lightReflection = normalize(-reflect(vertexToLight, normal_cameraSpace));
            vec4 eyeDirection = normalize(vec4(0,0,0,1) - position_cameraSpace);
            float specIntensity = pow(max(0.0, dot(eyeDirection, lightReflection)), shininess);
            color += max (vec3(0), lightColors[i] * specular_color * specIntensity);
        }
    } else {
        color = ambient_color + diffuse_color;
    }
    color = clamp(color, 0.0, 1.0) * allBlack;
}
