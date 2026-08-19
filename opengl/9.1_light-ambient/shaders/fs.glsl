#version 330 core

out vec4 FragColor;

in vec3 Color;
in vec3 Normal;

uniform vec3 lightColor;
uniform float ambient;

void main() {
  FragColor = vec4(lightColor*Color, 1.0f)*ambient;
}
