// Common structures to use in shaders relating to light.

// Need to figure out an include mechanism for glsl.
// This will do for now.

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

struct Spotlight {
  // Add fields as I go
  LightAttributes attributes;
};
