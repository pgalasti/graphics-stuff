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

struct LightAttributes {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

struct DirectionalLight {
  vec3 direction;
  LightAttributes attributes;
};

uniform Material material;
uniform vec3 cameraPosition;
uniform DirectionalLight directionalLight;

void main() {

 
  // Ambient
  vec3 ambient = directionalLight.attributes.ambient * texture(material.diffuse, TexCoords).rgb;

  // Diffuse
  vec3 norm = normalize(Normal);
  float diff = max(dot(norm, -directionalLight.direction), 0.0);
  vec3 diffuse = directionalLight.attributes.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

  // Specular
  vec3 cameraDirection = normalize(cameraPosition - fragPosition);
  vec3 reflectDirection = reflect(-directionalLight.direction, norm);
  float spec = pow(max(dot(cameraDirection, reflectDirection), 0.0), material.shine);
  vec3 specular = directionalLight.attributes.specular * spec * texture(material.specular, TexCoords).rgb;

  FragColor = vec4((ambient + diffuse + specular), 1.0);
}
