#version 330 core

//TODO: P1bTask5 - Modify shader to use position, normal and light positions to compute lighting.
in vec3 FragPos;
in vec3 Normal;

uniform bool isSelected;
uniform vec3 cameraPosition;

// Light properties
uniform vec3 light1Position;
uniform vec3 light1Ambient;
uniform vec3 light1Diffuse;
uniform vec3 light1Specular;

uniform vec3 light2Position;
uniform vec3 light2Ambient;
uniform vec3 light2Diffuse;
uniform vec3 light2Specular;

// Material properties
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float materialShininess;

// Output color
out vec4 color;

void main() {
    // Normalize the normal vector
    vec3 norm = normalize(Normal);

    // Calculate view direction
    vec3 viewDir = normalize(cameraPosition - FragPos);

    // Light 1 calculations
    vec3 light1Dir = normalize(light1Position - FragPos);
    vec3 reflect1Dir = reflect(-light1Dir, norm);

    float diff1 = max(dot(norm, light1Dir), 0.0);
    float spec1 = pow(max(dot(viewDir, reflect1Dir), 0.0), materialShininess);

    vec3 ambient1 = light1Ambient * materialAmbient;
    vec3 diffuse1 = light1Diffuse * diff1 * materialDiffuse;
    vec3 specular1 = light1Specular * spec1 * materialSpecular;

    // Light 2 calculations
    vec3 light2Dir = normalize(light2Position - FragPos);
    vec3 reflect2Dir = reflect(-light2Dir, norm);

    float diff2 = max(dot(norm, light2Dir), 0.0);
    float spec2 = pow(max(dot(viewDir, reflect2Dir), 0.0), materialShininess);

    vec3 ambient2 = light2Ambient * materialAmbient;
    vec3 diffuse2 = light2Diffuse * diff2 * materialDiffuse;
    vec3 specular2 = light2Specular * spec2 * materialSpecular;

    // Combine the results
    vec3 result = (ambient1 + diffuse1 + specular1) + (ambient2 + diffuse2 + specular2);

    // Set the fragment color
    vec4 baseColor = vec4(result, 1.0);
    //TODO: P1bTask4 - Find a way to draw the selected part in a brighter color.
    if (isSelected) {
        baseColor = vec4(1.0, 1.0, 0.5, 1.0);
    }

    color = baseColor;
}

