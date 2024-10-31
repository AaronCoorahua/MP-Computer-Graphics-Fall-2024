#version 410 core

// Uniform inputs
uniform mat4 mvpMatrix;                 // Model-View-Projection Matrix
uniform mat3 normalMatrix;              // Normal matrix
uniform vec3 eyePosition;               // Eye position

// Attribute inputs
layout(location = 0) in vec3 vPos;      // Vertex position
layout(location = 1) in vec3 vNormal;   // Vertex normal

// Material properties
uniform vec3 materialAmbientColor;
uniform vec3 materialDiffuseColor;
uniform vec3 materialSpecularColor;
uniform float materialShininess;

// Directional Light properties
uniform vec3 lightDirection;
uniform vec3 lightAmbientColor;
uniform vec3 lightDiffuseColor;
uniform vec3 lightSpecularColor;

// Point Light properties
uniform vec3 pointLightPos;
uniform vec3 pointLightColor;
uniform float pointLightConstant;
uniform float pointLightLinear;
uniform float pointLightQuadratic;

// Spotlight properties
uniform vec3 spotLightPos;
uniform vec3 spotLightDirection;
uniform vec3 spotLightColor;
uniform float spotLightCutoff;
uniform float spotLightOuterCutoff;
uniform float spotLightExponent;
uniform float spotLightConstant;
uniform float spotLightLinear;
uniform float spotLightQuadratic;

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

vec3 calculatePointLight(vec3 normal, vec3 fragPosition, vec3 viewVector) {
    vec3 lightDirection = normalize(pointLightPos - fragPosition);
    float difference = max(dot(normal, lightDirection), 0.0);
    vec3 reflectDirection = reflect(-lightDirection, normal);
    float specularFactor = pow(max(dot(viewVector, reflectDirection), 0.0), materialShininess);

    float distance = length(pointLightPos - fragPosition);
    float attenuation = 1.0 / (pointLightConstant + pointLightLinear * distance + pointLightQuadratic * (distance * distance));

    vec3 ambient = pointLightColor * materialAmbientColor;
    vec3 diffuse = difference * pointLightColor;
    vec3 specular = specularFactor * pointLightColor;

    return (ambient + diffuse + specular) * attenuation;
}

vec3 calculateSpotlight(vec3 normal, vec3 fragPosition, vec3 viewVector) {
    vec3 lightDirection = normalize(spotLightPos - fragPosition);
    float theta = dot(lightDirection, normalize(-spotLightDirection));

    if (theta > spotLightCutoff) {
        float difference = max(dot(normal, lightDirection), 0.0);
        vec3 reflectDirection = reflect(-lightDirection, normal);
        float specularFactor = pow(max(dot(viewVector, reflectDirection), 0.0), materialShininess);

        float distance = length(spotLightPos - fragPosition);
        float attenuation = 1.0 / (spotLightConstant + spotLightLinear * distance + spotLightQuadratic * (distance * distance));

        float intensity = clamp((theta - spotLightOuterCutoff) / (spotLightCutoff - spotLightOuterCutoff), 0.0, 1.0);
        intensity = pow(intensity, spotLightExponent);

        vec3 ambient = spotLightColor * materialAmbientColor;
        vec3 diffuse = difference * spotLightColor;
        vec3 specular = specularFactor * spotLightColor;

        return (ambient + diffuse + specular) * intensity * attenuation;
    }

    return vec3(0.0); // Outside spotlight cone
}

void main() {
    // Transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    // Transform normal vector
    vec3 normal = normalize(normalMatrix * vNormal);

    // Compute view vector
    vec3 fragmentPos = vec3(vPos);
    vec3 viewVector = normalize(eyePosition - fragmentPos);

    // Calculate all light sources
    vec3 directionalLight = calculateDirectionalLight(normal, viewVector);
    vec3 pointLight = calculatePointLight(normal, fragmentPos, viewVector);
    vec3 spotlight = calculateSpotlight(normal, fragmentPos, viewVector);

    // Combine lighting
    color = directionalLight + pointLight + spotlight;
}
