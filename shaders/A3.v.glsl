#version 410 core

// Uniform inputs
uniform mat4 mvpMatrix;                 // Model-View-Projection Matrix
uniform mat3 normalMatrix;              // Normal matrix
uniform vec3 eyePosition;               // Eye position

// Light properties
uniform vec3 lightDirection;
uniform vec3 lightAmbientColor;
uniform vec3 lightDiffuseColor;
uniform vec3 lightSpecularColor;

// Material properties
uniform vec3 materialAmbientColor;
uniform vec3 materialDiffuseColor;
uniform vec3 materialSpecularColor;
uniform float materialShininess;

// Attribute inputs
layout(location = 0) in vec3 vPos;      // Vertex position
layout(location = 1) in vec3 vNormal;   // Vertex normal

// Varying outputs
layout(location = 0) out vec3 color;    // Color to pass to fragment shader

vec3 calculateDirectionalLight(vec3 normal, vec3 viewVector) {
    // Compute light vector
    vec3 lightVector = normalize(-lightDirection);

    // Ambient component
    vec3 ambient = lightAmbientColor * materialAmbientColor;

    // Diffuse component
    float diffuseFactor = max(dot(normal, lightVector), 0.0);
    vec3 diffuse = lightDiffuseColor * materialDiffuseColor * diffuseFactor;

    // Specular component
    vec3 reflectVector = reflect(-lightVector, normal);
    float specularFactor = pow(max(dot(viewVector, reflectVector), 0.0), materialShininess);
    vec3 specular = lightSpecularColor * materialSpecularColor * specularFactor;

    // Sum all components
    return ambient + diffuse + specular;
}

void main() {
    // Transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    // Transform normal vector
    vec3 normal = normalize(normalMatrix * vNormal);

    // Compute view vector
    vec3 viewVector = normalize(eyePosition - vec3(gl_Position));

    // Calculate all light sources
    vec3 directionalLight = calculateDirectionalLight(normal, viewVector);

    // Combine lighting
    color = directionalLight;
}
