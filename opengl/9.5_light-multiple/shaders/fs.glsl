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
// Need to learn how to use arrays next
uniform Light light1;
uniform Light light2;

void main() {
  // Ambient light just summed together
  vec3 ambient = (light1.ambient * material.ambient) + (light2.ambient * material.ambient);

  vec3 norm = normalize(Normal);
  
  // Setup diffusal and add together
  vec3 light1Direction = normalize(light1.position - fragPosition);
  float diff = max(dot(norm, light1Direction), 0.0f);
  vec3 diffuse = light1.diffuse * (diff * material.diffuse);
  
  vec3 light2Direction = normalize(light2.position - fragPosition);
  diff = max(dot(norm, light2Direction), 0.0f);
  diffuse += light2.diffuse * (diff * material.diffuse);

  vec3 cameraDirection = normalize(cameraPosition - fragPosition);
 
  // Setup specular light and add together
  vec3 reflectDirection = reflect(-light1Direction, norm);
  float spec = pow(max(dot(cameraDirection, reflectDirection), 0.0), material.shine);
  vec3 specular = light1.specular * (spec*material.specular);

  reflectDirection = reflect(-light2Direction, norm);
  spec = pow(max(dot(cameraDirection, reflectDirection), 0.0), material.shine);
  specular += light2.specular * (spec*material.specular);

  FragColor = vec4((ambient + diffuse + specular), 1.0);
}
