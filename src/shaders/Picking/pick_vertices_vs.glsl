// returns picking ids of vertices

#include "colortranslator.glsl"

uniform mat4 mWVP;
uniform int pickVertexOffset;
//uniform isamplerBuffer vboToInputMap; // maps: vertex id in draw buffer -> vertex id in input buffer

out vec4 vertexPickID;

void main()
{
  // transform
  gl_Position = mWVP * vec4(inPosition.xyz, 1);
  
  // compute picking id
//  int vertexID =  texelFetch(vboToInputMap, gl_VertexID).x;
  int vertexID = gl_VertexID;
  vertexPickID = IntToColor(pickVertexOffset + vertexID);
}

