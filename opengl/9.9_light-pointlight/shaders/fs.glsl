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

struct PointLight {
  vec3 position;
  LightAttributes attributes;
  float constant,
        linear,
        quadratic;
};

uniform Material material;
uniform vec3 cameraPosition;
uniform DirectionalLight directionalLight;
uniform PointLight pointLight;

void getDirectionalLightValues(in DirectionalLight light, in vec3 norm, out vec3 ambient, out vec3 diffuse, out vec3 specular) {
  // Ambient
  ambient = light.attributes.ambient * texture(material.diffuse, TexCoords).rgb;

  // Diffuse
  float diff = max(dot(norm, -light.direction), 0.0);
  diffuse = light.attributes.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

  // Specular
  vec3 cameraDirection = normalize(cameraPosition - fragPosition);
  vec3 reflectDirection = reflect(light.direction, norm);
  float spec = pow(max(dot(cameraDirection, reflectDirection), 0.0), material.shine);
  specular = light.attributes.specular * spec * texture(material.specular, TexCoords).rgb;
}

void getPointLightValues(in PointLight light, in vec3 norm, vec3 lightDirection, out vec3 ambient, out vec3 diffuse, out vec3 specular) {

  // Ambient
  ambient = light.attributes.ambient * texture(material.diffuse, TexCoords).rgb;

  // Diffuse 
  float diff = max(dot(norm, lightDirection), 0.0);
  diffuse = light.attributes.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

  // Specular
  vec3 cameraDirection = normalize(cameraPosition - fragPosition);
  vec3 reflectDirection = reflect(-lightDirection, norm);
  float spec = pow(max(dot(cameraDirection, reflectDirection), 0.0), material.shine);
  specular = light.attributes.specular * spec * texture(material.specular, TexCoords).rgb;

  float distance = length(light.position - fragPosition);
  float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

  ambient  *= attenuation;
  diffuse  *= attenuation;
  specular *= attenuation;
}

void main() {
 
  vec3 norm = normalize(Normal);
  
  vec3 directionalAmbient, directionalDiffuse, directionalSpecular;
  getDirectionalLightValues(directionalLight, norm, directionalAmbient, directionalDiffuse, directionalSpecular);

  vec3 lightDirection = normalize(pointLight.position - fragPosition);
  vec3 pointAmbient, pointDiffuse, pointSpecular;
  getPointLightValues(pointLight, norm, lightDirection, pointAmbient, pointDiffuse, pointSpecular);

  FragColor = vec4((directionalAmbient + directionalDiffuse + directionalSpecular + pointAmbient + pointDiffuse + pointSpecular), 1.0);
}
