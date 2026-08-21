#version 330 core

out vec4 FragColor;

in vec3 Color;
in vec3 Normal;
in vec3 fragPosition;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform float ambient;
uniform float specularStrength;

void main() {
  vec3 norm = normalize(Normal);
  vec3 lightDirection = normalize(lightPosition - fragPosition);

  vec3 viewDirection = normalize(cameraPosition - fragPosition);
  vec3 reflectDirection = reflect(-lightDirection, norm);

  float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32); // Hard code the power for now
  vec3 specular = specularStrength * spec * lightColor;

  float diff = max(dot(norm, lightDirection), 0.0);
  vec3 diffuse = diff*lightColor;
  vec3 result = (ambient*lightColor + diffuse + specular)*Color;
  FragColor = vec4(result, 1.0f);
}
