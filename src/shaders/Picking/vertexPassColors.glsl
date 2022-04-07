uniform mat4 mWVP;

void main()
{
  gl_Position = mWVP * vec4(inPosition.xyz, 1);
  outVertexColor = inColor;
}