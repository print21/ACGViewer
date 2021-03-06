/*===========================================================================*\
 *                                                                           *
 *                              OpenFlipper                                  *
 *           Copyright (c) 2001-2015, RWTH-Aachen University                 *
 *           Department of Computer Graphics and Multimedia                  *
 *                          All rights reserved.                             *
 *                            www.openflipper.org                            *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * This file is part of OpenFlipper.                                         *
 *---------------------------------------------------------------------------*
 *                                                                           *
 * Redistribution and use in source and binary forms, with or without        *
 * modification, are permitted provided that the following conditions        *
 * are met:                                                                  *
 *                                                                           *
 * 1. Redistributions of source code must retain the above copyright notice, *
 *    this list of conditions and the following disclaimer.                  *
 *                                                                           *
 * 2. Redistributions in binary form must reproduce the above copyright      *
 *    notice, this list of conditions and the following disclaimer in the    *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. Neither the name of the copyright holder nor the names of its          *
 *    contributors may be used to endorse or promote products derived from   *
 *    this software without specific prior written permission.               *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED *
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A           *
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,       *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR        *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              *
 *                                                                           *
\*===========================================================================*/



#pragma once

#include <QStringList>
#include <QString>
#include <string>
#include <list>
#include <map>
#include <vector>

#include <ACG/GL/gl.hh>
#include <ACG/Config/ACGDefines.hh>


namespace ACG
{


// #define MAX_SHADER_INPUT_UNITS 32
// #define MAX_SHADER_CONSTANTS 0x200
// #define MAX_SHADER_GEN_DEFINES 0x100
#define SG_MAX_SHADER_LIGHTS 8

enum ShaderGenLightType
{
  SG_LIGHT_DIRECTIONAL = 0,
  SG_LIGHT_POINT,
  SG_LIGHT_SPOT,
  SG_LIGHT_FORCE_DWORD = 0xFFFFFFFF
};

enum ShaderGenShadeMode
{
  SG_SHADE_UNLIT = 0,
  SG_SHADE_FLAT,
  SG_SHADE_GOURAUD,
  SG_SHADE_PHONG,
  SG_SHADE_FORCE_DWORD = 0xFFFFFFFF
};

class ShaderGenDesc
{

public:
  ShaderGenDesc() :
    version(150),
    numLights(0),
    twoSidedLighting(false),
    shadeMode(SG_SHADE_UNLIT),
    vertexColors(false),
    vertexTemplateFile(""),
    tessControlTemplateFile(""),
    tessEvaluationTemplateFile(""),
    geometryTemplateFile(""),
    fragmentTemplateFile(""),
    normalizeTexColors(true),
    colorMaterialMode(GL_AMBIENT_AND_DIFFUSE),
    clipDistanceMask(0),
    textureTypes_(),
    texGenDim(0),
    texGenMode(GL_EYE_LINEAR),
    texGenPerFragment(false)
  {
    for ( unsigned int i = 0 ; i < SG_MAX_SHADER_LIGHTS ; ++i)
      lightTypes[i] = SG_LIGHT_DIRECTIONAL;
    if(!ACG::openGLVersionTest(3,2))   // version 140 or less
    {
      if(!ACG::openGLVersionTest(3,1)) // assume version 130
      {                            // less is not supported
        version = 130;
        if(ACG::checkExtensionSupported("EXT_texture_buffer"))
          macros.append(QString("#extension GL_EXT_texture_buffer:  enable"));
      }
      else        // version 140 texture buffer is part of spec
      {
        version = 140;
      }
    }
  }

  //In case, something crashes with the light types, try this hammer ;-)
//  const ShaderGenDesc& operator= (const ShaderGenDesc& _rhs) {
// 
//    version = _rhs.version;
// 
//    numLights                         = _rhs.numLights;
//    twoSidedLighting                  = _rhs.twoSidedLighting;
// 
//    std::copy(_rhs.lightTypes,_rhs.lightTypes+SG_MAX_SHADER_LIGHTS,lightTypes);
//    textureTypes_ = _rhs.textureTypes_;
// 
//    shadeMode                         = _rhs.shadeMode;
//    vertexColors                      = _rhs.vertexColors;
//    vertexTemplateFile                = _rhs.vertexTemplateFile;
//    tessControlTemplateFile           = _rhs.tessControlTemplateFile;
//    tessEvaluationTemplateFile        = _rhs.tessEvaluationTemplateFile;
//    geometryTemplateFile              = _rhs.geometryTemplateFile;
//    fragmentTemplateFile              = _rhs.fragmentTemplateFile;
// 
//    textureTypes_ = _rhs.textureTypes_;
// 
//    return *this;
//  }

  // glsl version, i.e. 130, 150, 330 etc.
  // Versions before 130 are not supported (deprecated "varying", "ftransform".. stuff)
  // Specification of each version available on http://www.opengl.org/registry/
  // default: 150 (OpenGL 3.2, supports geometry shaders, msaa samplers, gl_PrimitiveID )
  int version;

  int numLights;
  ShaderGenLightType lightTypes[SG_MAX_SHADER_LIGHTS];

  bool twoSidedLighting;

  ShaderGenShadeMode shadeMode;

  bool vertexColors;

  // optionally specify shader template file names
  QString vertexTemplateFile;
  QString tessControlTemplateFile;
  QString tessEvaluationTemplateFile;
  QString geometryTemplateFile;
  QString fragmentTemplateFile;

  // optionally specify shader mod ids
  std::vector<unsigned int> shaderMods;

  // preprocessor macros for the shaders
  // these are added to all shaders directly after the #version directive
  QStringList macros;

  /// convert ShaderGenDesc to string format for debugging
  QString toString() const;

  /// Defines if the textureVariable is normalized or not, if multiple textures are used
  bool normalizeTexColors;

  /// interpolation qualifier for input vertex colors: "flat", "smooth", "noperspective"
  QString vertexColorsInterpolator;

  /// interpolation qualifier for vertex shader normal outputs: "flat", "smooth", "noperspective"
  QString vertexNormalInterpolator;

  // color material for input vertex colors: GL_EMISSION, GL_AMBIENT, GL_DIFFUSE, GL_SPECULAR, GL_AMBIENT_AND_DIFFUSE
  // Usage of vertex colors in lighting function, as diffuse, emission, ambient color ..  see glColorMaterial()
  // default: GL_AMBIENT_AND_DIFFUSE
  GLenum colorMaterialMode;

  // Bitmask of enabled hardware clip planes.
  // Bit i represents the i'th clip plane starting at the least significant bit.
  // This is needed to pass the gl_ClipDistance[] array through the geometry shader stage.
  uint clipDistanceMask;

  struct TextureType
  {
    GLenum type;
    bool shadow;
  };
private:
  // TODO: remove this, multitexturing always requires some customization! should be done via custom shader templates or mods. only allow one diffuse texture, as this is something commonly used and intentions are clear
  /// holds the texture types (second) and the stage id (first). if empty, shader does not support textures
  std::map<size_t,TextureType> textureTypes_;

public:
  const std::map<size_t,TextureType>& textureTypes() const {return textureTypes_;}

  /** \brief adds a texture type to the shader and enables texturing.
   *
   */
  void addTextureType(GLenum _type, bool _shadow, size_t _stage)
  {
    TextureType t;
    t.type = _type;
    t.shadow = _shadow;
    textureTypes_[_stage] = t;
  }

  ///disables texture support and removes all texture types
  void clearTextures(){textureTypes_.clear();}

  bool textured()const {return !textureTypes_.empty();}



  // automatic texture coordinate generation, emulation of glTexGen
  // this takes priority over any texcoords provided via addTextureType 

  // dimension of generated texture coordinate: 0,1,2,3,4  (default: 0 - disabled)
  int texGenDim;

  // texture generation mode: GL_OBJECT_LINEAR, GL_EYE_LINEAR, GL_SPHERE_MAP, GL_NORMAL_MAP, GL_REFLECTION_MAP
  GLenum texGenMode; 

  // generate texture coordinates per vertex or fragment
  // default: false -> per vertex
  bool texGenPerFragment;

  void enableTexGenObjectLinear(int _dim = 2)
  {
    texGenDim = std::max(std::min(_dim, 4), 0);
    texGenMode = GL_OBJECT_LINEAR;
  }

  void enableTexGenEyeLinear(int _dim = 2)
  {
    texGenDim = std::max(std::min(_dim, 4), 0);
    texGenMode = GL_EYE_LINEAR;
  }

  void enableTexGenSphericalMap(int _dim = 2)
  {
    texGenDim = std::max(std::min(_dim, 2), 0);
    texGenMode = GL_SPHERE_MAP;
  }

  void enableTexGenNormalMap(int _dim = 3)
  {
    texGenDim = std::max(std::min(_dim, 3), 0);
    texGenMode = GL_NORMAL_MAP;
  }

  void enableTexGenReflectionMap(int _dim = 3)
  {
    texGenDim = std::max(std::min(_dim, 3), 0);
    texGenMode = GL_REFLECTION_MAP;
  }

  void disableTexGen() { texGenDim = 0; }


  // comparison operator
  bool operator == (const ShaderGenDesc& _rhs) const
  {
    if (numLights != _rhs.numLights)
      return false;

    if ( twoSidedLighting != _rhs.twoSidedLighting )
      return false;

    if (shadeMode != _rhs.shadeMode)
      return false;

    if (vertexColors != _rhs.vertexColors)
      return false;

    if (textured() != _rhs.textured())
      return false;

    if (vertexColors)
    {
      if (vertexColorsInterpolator != _rhs.vertexColorsInterpolator)
        return false;

      if (colorMaterialMode != _rhs.colorMaterialMode)
        return false;
    }

    if (!geometryTemplateFile.isEmpty())
    {
      // clip distance mask only affects the geometry shader
      if (clipDistanceMask != _rhs.clipDistanceMask)
        return false;
    }

    if (fragmentTemplateFile != _rhs.fragmentTemplateFile)
      return false;

    if (geometryTemplateFile != _rhs.geometryTemplateFile)
      return false;

    if (vertexTemplateFile != _rhs.vertexTemplateFile)
      return false;

    if (tessControlTemplateFile != _rhs.tessControlTemplateFile)
      return false;

    if (tessEvaluationTemplateFile != _rhs.tessEvaluationTemplateFile)
      return false;

    if (macros != _rhs.macros)
      return false;

    if (texGenDim != _rhs.texGenDim)
      return false;

    if (texGenDim)
    {
      if (texGenMode != _rhs.texGenMode)
        return false;

      if (texGenPerFragment != _rhs.texGenPerFragment)
        return false;
    }

    if (shaderMods != _rhs.shaderMods)
      return false;

    if (vertexNormalInterpolator != _rhs.vertexNormalInterpolator)
      return false;

    if (numLights)
      return memcmp(lightTypes, _rhs.lightTypes, numLights * sizeof(ShaderGenLightType)) == 0;

    return true;
  }
};


/** \page ShaderGenerator_page Shader Generator

\section ShaderGenerator_auto_shader_uniforms Automatically generated Shader Uniforms

\subsection Matrix
\code
  mat4 g_mWVP  - world-view-projection transform
  mat4 g_mWV   - world-view transform
  mat4 g_mWVIT - inverse transpose of world view matrix
  mat4 g_mP    - projection matrix
\endcode

\subsection Color
\code
  vec3 g_cDiffuse - diffuse color
  vec3 g_cAmbient - ambient color
  vec3 g_cEmissive - emissive color
  vec3 g_cSpecular - specular color
  vec4 g_vMaterial - vec4(shininess, alpha, unused, unused)
\endcode


\subsection Lighting

\code
  // light parameters denoted by zero-based index i
  vec3 g_cLightDiffuse_i   - diffuse color
  vec3 g_cLightAmbient_i   - ambient color
  vec3 g_cLightSpecular_i  - specular color
  vec3 g_vLightPos_i       - position in view space (for point and spot lights only)
  vec3 g_vLightAtten_i     - [constant, linear, quadratic] attenuation factors (for point and spot lights only)
  vec3 g_vLightDir_i       - light direction in view space (for spot and directional lights only)
  vec2 g_vLightAngleExp_i  - [cos(spotCutOffAngle), spot exponent] (for spot lights only)
\endcode

\subsection Texturing
\code
  sampler2D g_Texture0     - texture sampler (if textured is enabled in ShaderGenDesc)
\endcode



\section ShaderGenerator_auto_shader_inout Automatically generated shader input/output:

\subsection  ShaderGenerator_vertex_shader_io Vertex shader IO

\code
  in vec4 inPosition - vertex position in model space

  in vec3 inNormal   - vertex normal (only if ShaderGenDesc.shadeMode != unlit)
  in vec4 inColor    - vertex color (only if ShaderGenDesc.vertexColors == true)
  in vec2 inTexCoord - texture coordinate (only if ShaderGenDesc.textured == true)

  out vec4 outPosCS  - position in clip space
  out vec3 outNormal - normal in view space (only if ShaderGenDesc.shadeMode == phong)
  out vec4 outPosVS  - position in view space (only if ShaderGenDesc.shadeMode == phong)
  out vec4 outColor  - vertex color after lighting (only if ShaderGenDesc.shadeMode == flat or gouraud)
  out vec2 outTexCoord - texture coordinate (only if ShaderGenDesc.textured == true)

  // insert input/outputs added from modifiers here
\endcode

\subsection ShaderGenerator_fragment_shader_io Fragment shader IO

See vertex shader outputs for fragment inputs.
\code
  out vec4 outFragment - fragment color

  // insert input/outputs added from modifiers here
\endcode


\section ShaderGenerator_auto_shader_defines Automatically generated shader defines
These come in handy when you need to keep compatibility with custom shaders.

Depending on the shademode of ShaderGenDesc, exactly one of the following defines is active.
 - SG_GOURAUD
 - SG_FLAT
 - SG_UNLIT
 - SG_PHONG


Additional defines:
\code
SG_NORMALS      // defined if vertex normals are available
SG_TEXTURE      // defined if ShaderGenDesc.textured     == true
SG_VERTEX_COLOR // defined if ShaderGenDesc.vertexColors == true

\endcode

Lighting:
\code
 SG_NUM_LIGHTS   // number of lights

  //for each light i:
  SG_LIGHT_TYPE_i // Determine the type of light i; set to one of {SG_LIGHT_DIRECTIONAL, SG_LIGHT_POINT, SG_LIGHT_SPOT}

  // Example:
  #define SG_LIGHT_TYPE_0 SG_LIGHT_DIRECTIONAL
  #define SG_LIGHT_TYPE_1 SG_LIGHT_DIRECTIONAL
  #define SG_LIGHT_TYPE_2 SG_LIGHT_POINT
\endcode

\section ShaderGenerator_general_vertex_shader_structure Vertex shader generation structure

\code
void main()
{
  vec4 sg_vPosPS    = g_mWVP * inPosition;
  vec4 sg_vPosVS    = g_mWV  * inPosition;
  vec3 sg_vNormalVS = vec3(0.0, 1.0, 0.0);
  vec2 sg_vTexCoord = vec2(0.0, 0.0);
  vec4 sg_cColor    = vec4(g_cEmissive, SG_ALPHA);

#if normals available
  sg_vNormalVS = g_mWVIT * inNormal;

#if textured
  sg_vTexCoord = inTexCoord;

#if vertexcolors
  sg_cColor = inColor;


#if vertex-lighting enabled
  lighting code, save lit color in sg_cColor


  // -------------------------------------------------------------------------
  // begin customized code
  // - insert registered begin-code modifiers, that ideally operate on generated sg_* variables
  // - loaded code from template file is added here
  // make sure to use #ifdef #endif if you make use of conditional inputs such as normals, texcoords..
  // end of customized code
  // -------------------------------------------------------------------------

  gl_Position = sv_vPosPS;
  outPosCS = sg_vPosPS;

#if textured
  outTexCoord = sg_vTexCoord;

#if vertex-lighting
  outColor = sg_cColor;

#if fragment-lighting
  outNormal = sg_vNormalVS;
  outPosVS = sg_vPosVS;

  // end-code modifiers are added here
}

\endcode


---------------------------------------------------------------------------------------

\section ShaderGenerator_general_fragment_shader_structure Fragment shader generation structure


\code
void main()
{

  // compute screen-projected coordinates, useful for various post-processing effects
  vec2 sg_vScreenPos = outPosCS.xy / outPosCS.w * 0.5 + vec2(0.5, 0.5);

  vec4 sg_cColor = vec4(g_cEmisive, SG_ALPHA);

#if vertex-lighting
  sg_cColor = outColor;

#if fragment lighting
  vec4 sg_vPosVS = outPosVS;
  vec3 sg_vNormalVS = outNormal;
  lighting code here, save color to sg_cColor


#if textured
  vec4 sg_cTex = texture(g_Texture0, outTexCoord);
  sg_cColor *= sg_cTex;

  // -------------------------------------------------------------------------
  // begin customized code
  // - insert registered begin-code modifiers, that ideally operate on generated sg_* variables
  // - loaded code from template file is added here
  // make sure to use #ifdef #endif if you make use of conditional inputs such as normals, texcoords..
  // end of customized code
  // -------------------------------------------------------------------------

  outFragment = sg_cColor;

  // end-code modifiers are added here
}
\endcode

*/

/**
The ShaderGenerator is used to collect shader io, uniforms, defines and includes.

The shader main-function is not generated here and must be provided
as a parameter to the buildShaderCode function.
*/
class ACGDLLEXPORT ShaderGenerator
{
public:

  ShaderGenerator();
  virtual ~ShaderGenerator();


  struct DefaultIODesc
  {
    DefaultIODesc();

    /// default attributes that should be imported in vertex shader
    bool inputTexCoord_, // texcoords
      inputColor_,    // vertex colors 
      inputNormal_;   // view space normals

    /// default attributes that should be passed down from vertex shader
    bool passPosVS_, // view space position
      passPosOS_, // object space position
      passTexCoord_, // texcoords
      passColor_,    // vertex colors 
      passNormalVS_, // view space normals
      passNormalOS_; // object space normals
  };
  

  /** \brief Set glsl version
  */
  void setGLSLVersion(int _version);

  /** \brief Imports another shader, same as \#include
  */
  void addIncludeFile(QString _fileName);

  /** \brief Adds fitting vertex shader io for a given description
  */
  void initVertexShaderIO(const ShaderGenDesc* _desc, const DefaultIODesc* _iodesc);

  /** \brief Adds fitting tess-control shader io for a given description
  */
  void initTessControlShaderIO(const ShaderGenDesc* _desc, ShaderGenerator* _prevStage, const DefaultIODesc* _iodesc);

  /** \brief Adds fitting tess-evaluation shader io for a given description
  */
  void initTessEvalShaderIO(const ShaderGenDesc* _desc, ShaderGenerator* _prevStage, const DefaultIODesc* _iodesc);

  /** \brief Adds fitting geometry shader io for a given description
  */
  void initGeometryShaderIO(const ShaderGenDesc* _desc, ShaderGenerator* _prevStage, const DefaultIODesc* _iodesc);

  /** \brief Adds fitting fragment shader io for a given description
  */
  void initFragmentShaderIO(const ShaderGenDesc* _desc, ShaderGenerator* _prevStage, const DefaultIODesc* _iodesc);

  /** \brief Adds frequently used uniform parameters
   *
   * Adds frequently used uniform parameters like:
   *  - world, view, projection matrices
   *  - cam pos, view dir
   *  - per object material names: g_cDiffuse, g_cAmbient...
  */
  void initDefaultUniforms();

  /** \brief Add one GLSL input specifier
   *
   * Stores string pointer only
   * Example:
   * \code
   *   in vec4 inPosition;
   * \endcode
  */
  void addInput(const QString& _input);

  /** \brief Add one GLSL input specifier
  *
  * Stores string pointer only
  * Example:
  * \code
  *   in vec4 inPosition;
  * \endcode
  */
  void addInput(const QString& _type, const QString& _varname) { addInput(_type + QString(" ") + _varname); }

  /** \brief Add one GLSL output specifier
   *
   * Stores string pointer only
   * Example:
   * \code
   *   out vec4 inPosition;
   *   \endcode
   */
  void addOutput(const QString& _output);

  /** \brief Add one GLSL output specifier
  *
  * Stores string pointer only
  * Example:
  * \code
  *   out vec4 inPosition;
  * \endcode
  */
  void addOutput(const QString& _type, const QString& _varname) { addOutput(_type + QString(" ") + _varname); }


  /** \brief Add one GLSL uniform specifier
   *
   * Stores string pointer only
   * Example:
   * \code
   *   uniform sampler2D sampAmbient;
   * \endcode
   */
  void addUniform(QString _uniform, QString _comment = "");

  /** \brief Add one define
   *
   * Example:
   * \code
   *   #define SG_GOURAUD 1
   * \endcode
   */
  void addDefine(const QString& _define);

  /** \brief Assign an opaque name to the abstract macro
   *
   */
  void addIODefine(const QString& _macroName, const QString& _resolvedName);

  /** \brief Add a list of preprocessor macros
   *
   */
  void addMacros(const QStringList& _macros);

  /** \brief Check for define
   *
   * Example:
   * \code
   *   hasDefine("#define SG_REQUEST_NORMAL")
   * \endcode
   */
  bool hasDefine(QString _define) const;

  /** \brief Add a layout directive
   *
   * Example:
   * \code
   *   layout(vertices = 3) out;
   * \endcode
   */
  void addLayout(QString _layout);


  /** \brief Add a raw glsl IO code block
   *
   * This code block is inserted between the generated IO block and the main function.
   *
   * Example:
   * \code
   *  in vec3 someInput; 
   *  out vec3 outSomeOutput;
   *  uniform vec3 paramA;
   *  #define USE_METHOD_X
   *  #include "imports.h"
   *  ..
   * \endcode
   */
  void addRawIOBlock(QStringList _codeBlock) {rawIO_.append(_codeBlock);}

  /** \brief Add a light description to shader:
  */
  void addLight(int lightIndex_, ShaderGenLightType _light);

  /** \brief Shader assembly function
   *
   * Also scans shader code for references to default lighting functions LitPointLight(), LitDirLight(), LitSpotLight()
   * and eventually adds these to the shader.
  */
  void buildShaderCode(QStringList* _pMainCode, const QStringList& _defaultLightingFunctions);

  /** \brief Get result of buildShaderCode
  */
  const QStringList& getShaderCode();

  /** \brief Save generated shader code to text file
   *
   * @param _fileName Where to save
  */
  void saveToFile(const char* _fileName);


  /** \brief Perform name matching of outputs and inputs between two shader stages.
   *
   * IO of this shader (for instance fragment-shader) and its previous shader (for instance vertex shader) require matching names.
   * Example: The vertex shader has an output "out vec3 outVertexNormal", 
   *          then this function simply adds the input "in vec3 outVertexNormal" to the current shader (either a geometry or fragment shader).
   *          If this is a geometry shader, _passToNextStage should be set to true so that "out vec3 outGeometryNormal" is also added to the outputs.
   *
   * @param _previousShaderStage shader of the previous stage, which should have all outputs setup already
   * @param _passToNextStage all outputs of the previous shader stage are passed to the next stage (should be false for fragment shader for instance)
   * @param _inputPrefix name prefix of inputs to this shader (only necessary if _passToNextStage is true)
   * @param _outputPrefix name prefix of outputs of this shader (only necessary if _passToNextStage is true)
  */
  void matchInputs(const ShaderGenerator* _previousShaderStage, bool _passToNextStage, QString _inputPrefix = "outVertex", QString _outputPrefix = "outGeometry");

  /** \brief Define abstract IO names via shader defines.
   *
   * Default shader inputs and outputs can be conveniently accessed via SG_INPUT_X 
   * and SG_OUTPUT_X defines without having to worry about the combinatoric problem of shader stages.
   * For example, in a fragment-shader view-space normals can always be accessed via SG_INPUT_NORMALVS, regardless whether there is a geometry shader or not.
   *
   * @param _iodesc shader IO descriptor
   * @param _vs IO abstraction for vertex shader
   * @param _fs IO abstraction for fragment shader
  */
  void defineIOAbstraction(const DefaultIODesc* _iodesc, bool _vs, bool _fs);

  /** \brief get number of outputs
  */
  int getNumOutputs() const;

  /** \brief get variable name of output
   *
   * @param _id zero based index of output
  */
  QString getOutputName(int _id) const;

  /** \brief get number of inputs
  */
  int getNumInputs() const;

  /** \brief get variable name of input
   *
   * @param _id zero based index of input
  */
  QString getInputName(int _id) const;

  /** \brief get corresponding output name of an input id
   *
   * @param _inId zero-based index of input id
   * @return matching output name
  */
  QString getIOMapName(int _inId) const;


public:

  struct Keywords 
  {
    Keywords();

    // preprocessor macros that can be defined

    // request vertex attributes that should be available in all shader stages:
    QString macro_requestPosVS, // view space position
      macro_requestPosOS,       // object space position
      macro_requestTexcoord,    // mesh texture coordinate
      macro_requestVertexColor, // vertex color
      macro_requestNormalVS,    // view space normal
      macro_requestNormalOS;    // object space normal

    // input / output abstraction macros
    QString macro_inputPosVS,
      macro_inputPosOS,
      macro_inputPosCS,
      macro_inputNormalVS,
      macro_inputNormalOS,
      macro_inputTexcoord,
      macro_inputVertexColor,

      macro_outputPosVS,
      macro_outputPosOS,
      macro_outputPosCS,
      macro_outputNormalVS,
      macro_outputNormalOS,
      macro_outputTexcoord,
      macro_outputVertexColor;

    // default input/output variable names without prefixes
    QString ioPosCS,
      ioPosOS,
      ioPosVS,
      ioNormalVS,
      ioNormalOS,
      ioTexcoord,
      ioColor;

    // input/output prefix at each shader stage
    QString vs_inputPrefix,
      vs_outputPrefix,
      tcs_outputPrefix,
      tes_outputPrefix,
      gs_outputPrefix,
      fs_outputPrefix;

    // actual input/output of vertex shader
    QString vs_inputPosition,
      vs_inputNormal,
      vs_inputTexCoord,
      vs_inputColor,
      vs_outputPosCS,
      vs_outputPosVS,
      vs_outputPosOS,
      vs_outputTexCoord,
      vs_outputNormalVS,
      vs_outputNormalOS,
      vs_outputVertexColor,
      fs_outputFragmentColor;
  };

  static const Keywords keywords;

private:

  /** aborts if string already present
   *  prefix, postfix functionality is very basic:
   *    only checks for occurrence and disregards location
   */
  void addStringToList(QString _str, QStringList* _list, QString _prefix = "", QString _postfix = "");

  /** Adds command lines to the shader code.
  Eventually appends missing ';'
  */
  void addIOToCode(const QStringList& _cmds);

  QStringList code_;


  /// glsl code imports (with \#include )
  QStringList imports_;

  int version_;

  QStringList inputs_;
  QStringList outputs_;
  QStringList uniforms_;
  QStringList genDefines_;
  QStringList layouts_;

  /// io block as glsl code
  QStringList rawIO_;

  /// inputs of shader are arrays (tess-control, tess-eval, geometry)
  bool inputArrays_;

  /// outputs of shader are arrays (tess-control)
  bool outputArrays_;

  /// prefix of inputs to this shader, same as prefix of ouputs of previous stage
  QString inputPrefix_;

  /// prefix of outputs of this shader
  QString outputPrefix_;
};



/**
 *
 * A shader modifier can modify uniforms, in/outputs
 * and glsl code of vertex and fragment shaders.
 * This is useful for global effects like shadow mapping
 * and depth peeling, where only little changes in code are necessary.
 *
 * Usage:
 *  -# Derive a new subclass of ShaderModifier and implement necessary modify functions.
 *  -# Allocate a static instance of your modifier and register it to ShaderProgGenerator to get it's modifier-ID
 *  -# Create ShaderProgGenerator with a bitwise combination of modifier IDs to activate them.
 *
 * Example:
 * \code
 *
 *   // First modifier
 *   class VertexShaderModifier1 : public ACG::ShaderModifier {
 *   public:
 *     void modifyVertexEndCode(QStringList* _code) {
 *       _code->push_back("// Vertex End Code Modifier begin");
 *       _code->push_back("< Some glsl code >  ");
 *       _code->push_back("// Vertex End Code Modifier end");
 *     }
 *
 *     static VertexShaderModifier1 instance;
 *   };
 *
 *   // Static instance required!
 *   VertexShaderModifier YarnVertexShaderModifier::instance;
 *
 *
 *   class VertexShaderModifier2 : public ACG::ShaderModifier {
 *   public:
 *     void modifyVertexEndCode(QStringList* _code) {
 *       _code->push_back("// Vertex End Code Modifier 2 begin");
 *       _code->push_back("< Some glsl code >  ");
 *       _code->push_back("// Vertex End Code Modifier 2 end");
 *     }
 *
 *     static VertexShaderModifier2 instance;
 *   };
 *
 *   VertexShaderModifier2 VertexShaderModifier2::instance;
 * \endcode
 *
 * To use the modifiers, you have to register them to the shader generator:
 * \code
 *  // Register the modifiers
 *  ACG::ShaderProgGenerator::registerModifier(&VertexShaderModifier::instance);
 *  ACG::ShaderProgGenerator::registerModifier(&VertexShaderModifier2::instance);
 *
 *  // Use them via the shader cache
 *  GLSL::Program* prog = ACG::ShaderCache::getInstance()->getProgram(&shDesc,(VertexShaderModifier::instance.getID() | VertexShaderModifier2::instance.getID() ));
 * \endcode
 */
class ACGDLLEXPORT ShaderModifier
{
  friend class ShaderProgGenerator;

public:
  ShaderModifier(void);
  virtual ~ShaderModifier(void);

  /** \brief Add your own inputs/outputs to the vertex shader.
  *
  * Your implementation may look like this:
  *
  * \code
  *   _shader->addInput("vec4 inTangent");
  *   _shader->addUniform("vec4 shaderParam");
  * \endcode
  *
  * @param _shader shader interface
  */
  virtual void modifyVertexIO(ShaderGenerator* _shader) {}

  /** \brief Append code the the vertex shader.
   *
   * Refer to the generation structure (\ref ShaderGenerator_page ) to see where
   * your code is added and which variables you can modify.
   * Use
   * \code
   *   _code->push_back("...");
   * \endcode
   * to insert your code here.
   *
   * @param _code string list of shader code.
  */
  virtual void modifyVertexBeginCode(QStringList* _code) {}

  /** \brief  Append code the the vertex shader
   *
   * Refer to the generation structure (\ref ShaderGenerator_page ) to see
   * where your code is added and which variables you can modify.
   * Use
   *
   * \code
   *   _code->push_back("...");
   * \endcode
   * to insert your code here
   *
   * @param _code string list of shader code.
   */
  virtual void modifyVertexEndCode(QStringList* _code) {}

  /** \brief Add your own inputs/outputs to the geometry shader.
   *
   * your implementation may look like this:
   *
   * \code
   * _shader->addInput("sampler2D depthSampler");
   * _shader->addUniform("vec4 shaderParam");
   * \endcode
   *
   * @param _shader shader interface
  */
  virtual void modifyGeometryIO(ShaderGenerator* _shader) {}

  /** \brief Add your own inputs/outputs to the tessellation control shader.
   *
   * your implementation may look like this:
   *
   * \code
   * _shader->addInput("sampler2D depthSampler");
   * _shader->addUniform("vec4 shaderParam");
   * \endcode
   *
   * @param _shader shader interface
  */
  virtual void modifyTessControlIO(ShaderGenerator* _shader) {}

    /** \brief Add your own inputs/outputs to the tessellation evaluation shader.
   *
   * your implementation may look like this:
   *
   * \code
   * _shader->addInput("sampler2D depthSampler");
   * _shader->addUniform("vec4 shaderParam");
   * \endcode
   *
   * @param _shader shader interface
  */
  virtual void modifyTessEvalIO(ShaderGenerator* _shader) {}

  /** \brief Add your own inputs/outputs to the fragment shader.
   *
   * your implementation may look like this:
   *
   * \code
   * _shader->addInput("sampler2D depthSampler");
   * _shader->addUniform("vec4 shaderParam");
   * \endcode
   *
   * @param _shader shader interface
  */
  virtual void modifyFragmentIO(ShaderGenerator* _shader) {}

  /** \brief Append code the the fragment shader.
   *
   * Refer to the generation structure (\ref ShaderGenerator_page ) to see where
   * your code is added and which variables you can modify.
   * Use
   *
   * \code
   *   _code->push_back("...");
   * \endcode
   *
   * to insert your code here
   *
   * @param _code string list of shader code.
  */
  virtual void modifyFragmentBeginCode(QStringList* _code) {}

  /** \brief Append code the the fragment shader.
   *
   *  Refer to the generation structure (\ref ShaderGenerator_page )
   *  to see where your code is added and which variables you can modify.
   *  Use
   *
   *  \code
   *    _code->push_back("...");
   *  \endcode
   *
   *  to insert your code here.
   *
   * @param _code string list of shader code.
  */
  virtual void modifyFragmentEndCode(QStringList* _code) {}

  /** \brief Modify the default lighting code of the shader generator.
   *
   *  Refer to the generation structure (\ref ShaderGenerator_page )
   *  to see where your code is added and which variables you can modify.
   *
   *  Use
   *
   *  \code
   *    _code->push_back("...");
   *  \endcode
   *
   *  to insert your code here.
   *
   * @param _code      string list of shader code.
   * @param _lightId   light index, use g_vLightDir__{_lightId}, etc. in shader code to use light parameters
   * @param _lightType light type: point, spot or directional light
  */
  virtual void modifyLightingCode(QStringList* _code, int _lightId, ShaderGenLightType _lightType) {}

  /** \brief Specify whether this modifier replaces or extends the default lighting code.
   *
   * @return return false if this modifier modifies an already computed lighting color, return true if this modifier replaces the default lighting color
  */
  virtual bool replaceDefaultLightingCode() {return false;}

  /** \brief Returns the modifier ID
   *
   * @return Id of the modifier
  */
  unsigned int getID() {return modifierID_;}


  /** \brief Load a modifier from file
   *
   * Shader Mod text file format:
   *
   * The file can begin with a glsl version directive (optionally):
   * #version glsl_version
   *
   * The rest of the file contains glsl code blocks, which are separated by keywords.
   * Each keyword must be at the beginning of a line. Furthermore they end on a colon ':'.
   * The keyword defines the meaning of the following code block.
   *
   * IO code blocks are specified by the keywords:
   *  VertexIO:, TessControlIO:, TessEvalIO:, GeometryIO: and FragmentIO:
   *
   * example
   *
   * VertexIO:
   * in vec3 customInputName;
   * out vec3 custumOutputName;
   * uniform vec4 param0;
   *
   *
   * Shader code modifications are done via the keywords:
   *  VertexBeginCode:, VertexEndCode:, FragmentBeginCode: and FragmentEndCode:
   *
   *
   *
   * The returned modifier should not be deleted manually.
   *
   * @param _filename absolute or relative (from shaderdir) file name
   * @return pointer to shader modifier, 0 if loading failed
  */
  static ShaderModifier* loadFromFile(QString _filename);


//  operator unsigned int() const {return modifierID_;}
  operator std::vector<unsigned int>() const {return std::vector<unsigned int>(1,modifierID_);}

  std::vector<unsigned int> operator | (const std::vector<unsigned int>& _v) const
  {
    std::vector<unsigned int> r(1 + _v.size(), modifierID_);
    for (size_t i = 0; i < _v.size(); ++i)
      r[i + 1] = _v[i];
    return r;
  }

private:
  unsigned int modifierID_;
};


/**
ShaderProgGenerator is responsible for generating a matching pair of vertex and fragment shaders.
*/
class ACGDLLEXPORT ShaderProgGenerator
{
public:

  /**
  The shader directory has to be defined first before making use of the generator!
  For example: setShaderDir(OpenFlipper::Options::shaderDirStr())
  */
  static void setShaderDir(QString _dir);

  /**
  Return shader directory path as previosly specified by user
  */
  static QString getShaderDir();

  /** 
  @param _desc description-set of shader properties.
  */
  explicit ShaderProgGenerator(const ShaderGenDesc* _desc);

  /** 
  @param _desc description-set of shader properties.
  @param _modifierIDs array of modifier-IDs that should be used for the generation.
  */
  ShaderProgGenerator(const ShaderGenDesc* _desc, const std::vector<unsigned int>& _modifierIDs);

  /** 
  @param _desc description-set of shader properties.
  @param _modifierIDs array of modifier-IDs that should be used for the generation.
  */
  ShaderProgGenerator(const ShaderGenDesc* _desc, const std::vector<unsigned int>* _modifierIDs);

  /** 
  @param _desc description-set of shader properties.
  @param _modifierIDs array of modifier-IDs that should be used for the generation.
  @param _numModifiers number of modifiers in _modifierIDs
  */
  ShaderProgGenerator(const ShaderGenDesc* _desc, const unsigned int* _modifierIDs, unsigned int _numModifiers);

  /** 
  @param _desc description-set of shader properties.
  @param _modifiers array of modifiers that should be used for the generation.
  @param _numModifiers number of modifiers in _modifierIDs
  */
  ShaderProgGenerator(const ShaderGenDesc* _desc, ShaderModifier* const* _modifiers, unsigned int _numModifiers);

  /** 
  @param _desc description-set of shader properties.
  @param _modifiers array of modifiers that should be used for the generation.
  */
  ShaderProgGenerator(const ShaderGenDesc* _desc, const std::vector<ShaderModifier*>& _modifiers);

  /** 
  @param _desc description-set of shader properties.
  @param _modifiers array of modifiers that should be used for the generation.
  */
  ShaderProgGenerator(const ShaderGenDesc* _desc, const std::vector<ShaderModifier*>* _modifiers);

  virtual ~ShaderProgGenerator(void);


  void saveVertexShToFile(const char* _fileName);
  void saveGeometryShToFile(const char* _fileName);
  void saveFragmentShToFile(const char* _fileName);

  /** \brief Returns generated vertex shader code
  */
  const QStringList& getVertexShaderCode();

  /** \brief Returns generated vertex shader code
  */
  const QStringList& getTessControlShaderCode();

  /** \brief Returns generated tessellation control shader code
  */
  const QStringList& getTessEvaluationShaderCode();

  /** \brief Returns generated tessellation evaluation shader code
  */
  const QStringList& getGeometryShaderCode();

  /** \brief Returns generated fragment shader code
  */
  const QStringList& getFragmentShaderCode();

  
  /** \brief Get the number of active modifiers.
  */
  int getNumActiveModifiers() const;

  /** \brief Get active modfiers for this program.
  */
  ShaderModifier* getActiveModifier(int _i);


  /** \brief Shader modifiers have to be registered before they can be used.
             They also must remain allocated for the rest of the applications runtime.
             Use a combination of modifier-IDs in the constructor of ShaderProgGen to active them.
  @param _modifier address of a modifier implementation
  @return modifier ID
  */
  static unsigned int registerModifier(ShaderModifier* _modifier);

  /** \brief check whether there is a geometry shader present
  */
  bool hasGeometryShader() const;

  /** \brief check whether there is a tess-control shader present
  */
  bool hasTessControlShader() const;

  /** \brief check whether there is a tess-evaluation shader present
  */
  bool hasTessEvaluationShader() const;

  /** \brief Generates the shader code
  */
  void generateShaders();

  /** \brief Load a text file as string list
  @param _fileName relative (from shader dir) or absolute file name
  @param _out lines from text file
  @return true on success, false otherwise
  */
  static bool loadStringListFromFile(QString _fileName, QStringList* _out);

  /** \brief Convert a filename to an absolute filename
  @param _fileName relative (from shader dir) or absolute file name
  */
  static QString getAbsFilePath(QString _fileName);

private:

  /** \brief Loads external shader templates
  */
  void loadShaderTemplateFromFile();

  /** \brief Scans loaded shader template for requested inputs, glsl version or includes
  */
  void scanShaderTemplate(QStringList& _templateSrc, QString _templateFilename, QStringList* _outLayoutDirectives = 0);

  /** \brief Called in constructor
  */
  void init(const ShaderGenDesc* _desc, ShaderModifier* const* _modifiers, unsigned int _numActiveMods);
  void init(const ShaderGenDesc* _desc, const unsigned int* _modifiers, unsigned int _numActiveMods);

  void buildVertexShader();
  void buildTessControlShader();
  void buildTessEvalShader();
  void buildGeometryShader();
  void buildFragmentShader();

  void addVertexBeginCode(QStringList* _code);
  void addVertexEndCode(QStringList* _code);

  void addFragmentBeginCode(QStringList* _code);
  void addFragmentEndCode(QStringList* _code);

  /** \brief Adds lighting definition functions
   *
   * LitPointLight() ...
   */
  void addLightingFunctions(QStringList* _code);

  /** \brief Adds lighting function calls to code
   */
  void addLightingCode(QStringList* _code);

  /** \brief Calls lighting modifier for each light
   */
  void modifyLightingCode(QStringList* _code, ShaderModifier* _modifier);

  /** \brief Add texture coordinate generation code
   */
  void addTexGenCode(QStringList* _code, bool _fragmentShader);

  /// returns path to _strFileName without last slash
  static QString getPathName(QString _strFileName);

  /// checks if _str is an include directive
  /// eventually imports the included file to the specified generator
  int checkForIncludes(QString _str, ShaderGenerator* _gen, QString _includePath);

  /// checks if _str is an include directive
  /// eventually imports the included file to the specified stringlist
  int checkForIncludes(QString _str, QStringList* _outImport, QString _includePath);


  /// provide generated defines to shader
  void initGenDefines(ShaderGenerator* _gen);

  static void loadLightingFunctions();

  ShaderGenerator* vertex_;
  ShaderGenerator* tessControl_;
  ShaderGenerator* tessEval_;
  ShaderGenerator* geometry_;
  ShaderGenerator* fragment_;

  QStringList vertexTemplate_;
  QStringList tessControlTemplate_;
  QStringList tessEvalTemplate_;
  QStringList geometryTemplate_;
  QStringList fragmentTemplate_;

  ShaderGenDesc   desc_;
  std::vector<ShaderModifier*> activeMods_;


  /// registered shader modifier
  static int numRegisteredModifiers_;
  static std::vector<ShaderModifier*> registeredModifiers_;

  /// path + filename to shader templates
  QString vertexShaderFile_;
  QString tessControlShaderFile_;
  QString tessEvalShaderFile_;
  QString geometryShaderFile_;
  QString fragmentShaderFile_;

  /// layout() directives scanned from loaded templates
  QStringList tessControlLayout_;
  QStringList tessEvalLayout_;


  /// default IO descriptor for the vertex shader
  ShaderGenerator::DefaultIODesc ioDesc_;


  static QString shaderDir_;
  static QStringList lightingCode_;

  
};


//=============================================================================
} // namespace ACG
//=============================================================================
