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

#define MAX_LIGHTS 16
uniform Light lights[MAX_LIGHTS];
uniform int activeLights;

vec3 ColorPass(const vec3 normal, const vec3 cameraDirection, const Light light) {
  vec3 ambient = (light.ambient * material.ambient);

  vec3 lightDirection = normalize(light.position - fragPosition);
  float diff = max(dot(normal, lightDirection), 0.0f);
  vec3 diffuse = light.diffuse * (diff * material.diffuse);
  
  vec3 reflectDirection = reflect(-lightDirection, normal);
  float spec = pow(max(dot(cameraDirection, reflectDirection), 0.0), material.shine);
  vec3 specular = light.specular * (spec*material.specular);

  return vec3(ambient+diffuse+specular);
}

void main() {

  vec3 norm = normalize(Normal);
  vec3 cameraDirection = normalize(cameraPosition - fragPosition);

  vec3 finalLighting = vec3(0.0f);
  int availableLights = min(activeLights, MAX_LIGHTS);
  
  for(int i = 0; i < availableLights; ++i) {
    finalLighting += ColorPass(norm, cameraDirection, lights[i]);
  }

  FragColor = vec4(finalLighting, 1.0);
}
