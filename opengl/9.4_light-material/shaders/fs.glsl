#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 fragPosition;

uniform vec3 cameraPosition;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shine;
};
uniform Material material;

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};
uniform Light light;

void main() {
  vec3 ambient = light.ambient * material.ambient;

  vec3 norm = normalize(Normal);
  vec3 lightDirection = normalize(light.position - fragPosition);
  float diff = max(dot(norm, lightDirection), 0.0f);
  vec3 diffuse = light.diffuse * (diff * material.diffuse);

  vec3 cameraDirection = normalize(cameraPosition - fragPosition);
  vec3 reflectDirection = reflect(-lightDirection, norm);
  float spec = pow(max(dot(cameraDirection, reflectDirection), 0.0), material.shine);
  vec3 specular = light.specular * (spec*material.specular);

  FragColor = vec4((ambient + diffuse + specular), 1.0);
}
