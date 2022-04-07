// returns picking ids of vertices

#include "colortranslator.glsl"

uniform int pickVertexOffset;

void main()
{
  int vertexID = gl_PrimitiveID + pickVertexOffset;
  outFragment = IntToColor(vertexID);
}

