#version 330 core

out vec4 FragColor;

in vec3 Color;
in vec3 Normal;
in vec3 fragPosition;

uniform vec3 lightColor;
uniform float ambient;
uniform vec3 lightPosition;

void main() {
  vec3 norm = normalize(Normal);
  vec3 lightDirection = normalize(lightPosition - fragPosition);

  float diff = max(dot(norm, lightDirection), 0.0);
  vec3 diffuse = diff*lightColor;
  vec3 result = (ambient*lightColor + diffuse)*Color;
  FragColor = vec4(result, 1.0f);
}
