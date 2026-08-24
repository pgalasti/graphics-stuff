#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 fragPosition;
in vec2 TexCoords;

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shine;
};

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

uniform Material material;
uniform vec3 cameraPosition;

#define MAX_LIGHTS 16
uniform Light lights[MAX_LIGHTS];
uniform int activeLights;

vec3 ColorPass(const vec3 normal, const vec3 cameraDirection, const Light light) {
  vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

  vec3 lightDirection = normalize(light.position - fragPosition);
  float diff = max(dot(normal, lightDirection), 0.0f);
  vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;
  
  vec3 reflectDirection = reflect(-lightDirection, normal);
  float spec = pow(max(dot(cameraDirection, reflectDirection), 0.0), material.shine);
  vec3 specular = light.specular * (spec * texture(material.specular, TexCoords).rgb);

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
