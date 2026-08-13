#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D containerTexture;
uniform sampler2D wallTexture;

void main() {
  FragColor = mix(texture(containerTexture, TexCoord), texture(wallTexture, TexCoord), 0.5f);
}
