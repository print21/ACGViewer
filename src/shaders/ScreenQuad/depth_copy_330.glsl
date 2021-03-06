#version 330

uniform sampler2D DepthTex;

uniform float DepthSign;

in vec2 vTexCoord;
out vec4 oColor;

void main() 
{
  oColor = vec4(1.0, 1.0, 1.0, 1.0);
  
  gl_FragDepth = texture(DepthTex, vTexCoord).x * DepthSign;
}