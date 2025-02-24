#version 330 core

//TODO: P1bTask5 - Modify shader to use position, normal and light positions to compute lighting.

uniform bool isSelected;

// Output color
out vec4 color;

void main() {
    // Set the fragment color
    vec4 baseColor = vec4(0.8, 0.8, 0.8, 1.0);
    //TODO: P1bTask4 - Find a way to draw the selected part in a brighter color.
    if (isSelected) {
        baseColor = vec4(1.0, 1.0, 0.5, 1.0);
    }

    color = baseColor;
}

