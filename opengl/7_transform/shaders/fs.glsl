#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D containerTexture;
uniform sampler2D wallTexture;

void main() {
  FragColor = (mix(texture(containerTexture, TexCoord), texture(wallTexture, TexCoord), 0.5f)*2) * vec4(ourColor, 1.0f);
}
