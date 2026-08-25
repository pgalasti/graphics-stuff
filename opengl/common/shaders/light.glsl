// Common structures to use in shaders relating to light.

// Need to figure out an include mechanism for glsl.
// This will do for now.

struct LightAttributes {
  vec3 ambient;
  vec3 diffuse;
  vec3 speceular;
};

struct DirectionalLight {
  vec3 direction;
  LightAttributes attributes;
};

struct PointLight {
  // Add fields as I go
  LightAttributes attributes;
};

struct Spotlight {
  // Add fields as I go
  LightAttributes attributes;
};
