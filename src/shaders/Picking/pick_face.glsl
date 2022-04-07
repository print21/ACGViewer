// returns picking ids of faces in a n-poly mesh

#include "colortranslator.glsl"

uniform int pickFaceOffset;
uniform isamplerBuffer triToFaceMap; // maps: triangle id in draw buffer -> face id in input buffer

void main()
{
  int faceID = texelFetch(triToFaceMap, gl_PrimitiveID).x;
//  faceID = gl_PrimitiveID; // debug mode: skip reading from texture-buffer
  int pickID = pickFaceOffset + faceID;
  outFragment = IntToColor(pickID);
}
