#pragma once
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

//=============================================================================
//
//  CLASS DrawMeshT
//
//=============================================================================

//== INCLUDES =================================================================

#include <vector>
#include <list>
#include <OpenMesh/Core/Utils/Property.hh>
#include <OpenMesh/Core/Utils/color_cast.hh>

#include <OpenMesh/Core/Mesh/DefaultTriMesh.hh>
#include <OpenMesh/Core/Mesh/DefaultPolyMesh.hh>

#include <ACG/GL/globjects.hh>
#include <ACG/GL/GLState.hh>
#include <ACG/GL/IRenderer.hh>
#include <ACG/GL/MeshCompiler.hh>
#include <ACG/ShaderUtils/GLSLShader.hh>

#include <ACG/Config/ACGDefines.hh>

//== FORWARDDECLARATIONS ======================================================


//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================

/**
 * This class holds non-templated code and is intended to be inherited
 * by the templated DrawMeshT class.
 */
class ACGDLLEXPORT DrawMeshBase {
    protected:
        DrawMeshBase();
        ~DrawMeshBase();

        void deleteIbo();
        void bindVbo();
        void bindIbo();
        void bindLineIbo();
        void bindHEVbo();
        void unbindHEVbo();
        void bindPickVertexIbo();

        void createIndexBuffer();
        void fillLineBuffer(size_t n_edges, void *data);
        void fillHEVBO(size_t numberOfElements_, size_t sizeOfElements_, void* data_);
        void fillVertexBuffer();
        void fillInvVertexMap(size_t n_vertices, void *data);

    public:
        size_t getNumTris() const { return numTris_; }
        size_t getNumVerts() const { return numVerts_; }

        /** \brief get mesh compiler used to create the draw mesh
        */
        MeshCompiler* getMeshCompiler() {return meshComp_;}
        unsigned int getNumSubsets() const {return meshComp_->getNumSubsets();}

        /** \brief get index type of index buffer
        */
        GLenum getIndexType() const {return indexType_;}

        /** \brief get an index buffer mapping from openmesh vertices to drawmesh vbo vertices
        *
        * @return index buffer object (UNSIGNED_INT), might be 0 if the openmesh object is a point cloud
        */
        GLuint pickVertexIBO_opt() {return pickVertexIBO_;} // does not work



    protected:
        GLuint vbo_, ibo_;
        size_t numTris_, numVerts_;
        MeshCompiler* meshComp_;

        /// index buffer used in Wireframe / Hiddenline mode
        GLuint lineIBO_;
        /// vbo for halfedge rendering, as they are offset
        GLuint heVBO_;
        //previously bound buffer
        GLint prevVBO_;

        /// support for 2 and 4 byte unsigned integers
        GLenum indexType_;

        /** final vertex buffer used for rendering
          * raw byte array, use write__() functions for access
          */
        std::vector<char> vertices_;

        /// vertex buffer layout declaration with per vertex colors
        VertexDeclaration* vertexDecl_;

        /// vertex buffer layout declaration with per edge colors, legacy path
        VertexDeclaration* vertexDeclEdgeCol_;

        /// vertex buffer layout declaration with per edge colors
        VertexDeclaration vertexDeclEdgeNew_;

        /// vertex buffer layout declaration with per halfedge colors
        VertexDeclaration* vertexDeclHalfedgeCol_;

        /// vertex buffer layout declaration with halfedge positions only
        VertexDeclaration* vertexDeclHalfedgePos_;

        /// map from openmesh vertex to vbo vertex id
        GLuint pickVertexIBO_;

};


/** \brief Mesh Drawing Class
 *
 * This class creates a new mesh for efficient rendering based on an OpenMesh object.
 * DrawMesh also supports optimized picking and toggling between vertex/halfedge/face normals and vertex/halfedge texcoords.
 * 
 * It is not recommended to use this class if the input mesh does not implement an OpenMesh kernel.
 * Instead, MeshCompiler can be used directly to build the vertex and index buffer.
 */

template <class Mesh>
class DrawMeshT : public DrawMeshBase
{
private:

  struct Subset
  {
    int  materialID;
    unsigned long startIndex;
    unsigned long numTris;
  };

  enum REBUILD_TYPE {REBUILD_NONE = 0, REBUILD_FULL = 1, REBUILD_GEOMETRY = 2, REBUILD_TOPOLOGY = 4, REBUILD_TEXTURES = 8};


public:

  explicit DrawMeshT(Mesh& _mesh);
  virtual ~DrawMeshT();

  void disableColors()      {colorMode_ = 0;}
  void usePerVertexColors() {colorMode_ = 1;}
  void usePerFaceColors()   {colorMode_ = 2;}

  void setFlatShading()     {flatMode_ = 1;}
  void setSmoothShading()   {flatMode_ = 0;}

  void usePerVertexTexcoords()     {textureMode_ = 0;}
  void usePerHalfedgeTexcoords()   {textureMode_ = 1;}
  void usePerVertexNormals()   {halfedgeNormalMode_ = 0;}
  void usePerHalfedgeNormals()   {halfedgeNormalMode_ = 1;}

  /** \brief eventually rebuilds buffers used for rendering and binds index and vertex buffer
  */
  void bindBuffers();

  /** \brief get opengl vertex buffer id
  */
  GLuint getVBO();

  /** \brief get opengl index buffer id
  */
  GLuint getIBO();

  /**
   * @brief getHEVBO get VBO which stores Halfedges with offset
   * @return GLuint the VBO
   */
  GLuint getHEVBO(){return heVBO_;}

  /** \brief get vertex declaration of the current vbo layout
  */
  VertexDeclaration* getVertexDeclaration();

  /** \brief map from vertex index of the original mesh point buffer to the corresponding vertex index inside the VBO.
  @param _v vertex index into mesh points
  @return vertex index into VBO
  */
  unsigned int mapVertexToVBOIndex(unsigned int _v);


  /** \brief eventually rebuilds buffers used for rendering and binds index and vertex buffer
  */
  void bindBuffersToRenderObject(RenderObject* _obj);

  /** \brief disables vertex, normal, texcoord and color pointers in OpenGL
  */
  void unbindBuffers();

  /** \brief binds index and vertex buffer and executes draw calls
  *
  *   @param _textureMap maps from internally texture-id to OpenGL texture id, may be null to disable textured rendering
  *   @param _nonindexed use unoptimized non-indexed vbo for rendering, not as efficient in terms of memory usage and performance as an indexed draw call.
  */
  void draw(std::map< int, GLuint>* _textureMap, bool _nonindexed = false);

  /** \brief adds RenderObjects to a deferred draw call renderer
  *
  *   @param _renderer renderobjects are added to this renderer
  *   @param _baseObj address of the base renderobject with information about shader generation, gl states, matrices ..
  *   @param _textureMap maps from internally texture-id to OpenGL texture id
  *   @param _nonindexed use non-indexed vbo instead of optimized indexed vbo (should be avoided if possible)
  *   may be null to disable textured rendering
  */
  void addTriRenderObjects(IRenderer* _renderer, const RenderObject* _baseObj, std::map< int, GLuint>* _textureMap, bool _nonindexed = false);

  /** \brief render the mesh in wireframe mode
  */
  void drawLines();

  /** \brief render the mesh in wireframe mode, deferred draw call
  */
  void addLineRenderObjects(IRenderer* _renderer, const RenderObject* _baseObj);


  /** \brief render vertices only
  */
  void drawVertices();

  /** \brief render vertices only, deferred draw call
  */
  void addPointRenderObjects(IRenderer* _renderer, const RenderObject* _baseObj);



  /** \brief measures the size in bytes of allocated memory.
  eventually prints a report to std::cout
  */
  unsigned int getMemoryUsage(bool _printReport = false);

  // The updateX functions give a hint on what to update.
  //  may perform a full rebuild internally!

  /** \brief request an update for the mesh topology
   */
  void updateTopology() {rebuild_ |= REBUILD_TOPOLOGY;}

  /** \brief request an update for the mesh vertices
   */
  void updateGeometry() {rebuild_ |= REBUILD_GEOMETRY;}

  /** \brief request an update for the textures
     */
  void updateTextures() {rebuild_ |= REBUILD_TEXTURES;}

  /** \brief request a full rebuild of the mesh
   *
   */
  void updateFull() {rebuild_ |= REBUILD_FULL;}

  /** \brief returns the number of used textured of this mesh
   *
   * @return Number of different textures used in the mesh
   */
  unsigned int getNumTextures();

  /** \brief set the name of the property used for texture index specification
  *
  * The given property name will define a texture index. The strip processor checks this
  * property and generates strips which contain only the same index. If the property is not
  * found, strips will be independent of this property
  *
  * @param _indexPropertyName
  */
  void setTextureIndexPropertyName( std::string _indexPropertyName );

  /** \brief get the name of the texture index property
   *
   * @return name of the texture index property
   */
  const std::string& getTextureIndexPropertyName() const { return textureIndexPropertyName_; };

  /** \brief set the name of the property used for texture coordinate
  *
  * The given property name will define per face Texture coordinates. This property has to be a
  * halfedge property. The coordinate on each edge is the texture coordinate of the to vertex.
  * If this property is not available, textures will not be processed by the strip processor.
  *
  * @param _perFaceTextureCoordinatePropertyName
  */
  void setPerFaceTextureCoordinatePropertyName( std::string _perFaceTextureCoordinatePropertyName );

  /** \brief Check if per Face Texture coordinates are available
  *
  * If this function returns true, a per face per vertex texture array is available
  *
  * @return zero if not available, nonzero otherwise
  */
  int perFaceTextureCoordinateAvailable();

  /** \brief Check if texture indices are available
  *
  * If this function returns true, the strip processor will respect textures during strip generation.
  * Each returned strip has than an index that has to be used as a texture index during strip rendering.
  *
  * @return zero if not available, nonzero otherwise
  */
  int perFaceTextureIndexAvailable();


  enum PropertySource
  {
    PROPERTY_SOURCE_VERTEX = 0,
    PROPERTY_SOURCE_HALFEDGE,
    PROPERTY_SOURCE_FACE,
  };

  /** \brief Add custom elements to the vertex layout
  *
  * @param _propertyName name id of property in OpenMesh
  * @param _source source of property, ie per vertex, per face or per halfedge
  */
  void addVertexElement( const std::string& _propertyName, PropertySource _source = PROPERTY_SOURCE_VERTEX );

  /** \brief Scan vertex layout from vertex shader
  *
  * Scans a vertex shader for inputs and tries to get the matching properties from OpenMesh.
  * The name of the input attribute in the shader has to match the property name in OpenMesh.
  * Per halfedge properties are preferred over per vertex properties when available.
  * Per face properties are used if the attribute is qualified as "flat" in the shader.
  *
  * @param _vertexShaderFile filename of vertex shader (or vertex shader template)
  * @return true if all requested properties are available, false otherwise
  */
  bool scanVertexShaderForInput( const std::string& _vertexShaderFile );

private:
  // processing pipeline:

  /** \brief  draw_mesh updater
   *
   */
  void rebuild();


  /** \brief reads a vertex from mesh_ and write it to vertex buffer
   *
   * @param _vertex target vertex id in vbo
   * @param _vh mesh vertex handle to read from
   * @param _hh corresponding halfedge handle of this vertex
   * @param _fh corresponding face handle of this vertex
   */
  void readVertex(size_t                               _vertex,
                  const typename Mesh::VertexHandle&   _vh,
                  const typename Mesh::HalfedgeHandle& _hh,
                  const typename Mesh::FaceHandle&     _fh);

  /** \brief return a vertex color from mesh
   *
   * @param _vh mesh vertex handle
   */
  unsigned int getVertexColor(const typename Mesh::VertexHandle&   _vh);

  /** \brief return a face color from mesh
   *
   * @param _fh mesh face handle
   */
  unsigned int getFaceColor(const typename Mesh::FaceHandle&   _fh);

  /** \brief  eventually update vertex and index buffers
   *
   */
  void updateGPUBuffers();

  /** \brief  stores the vertex buffer on the gpu
   *
   */
  void createVBO();

  /** \brief stores the index buffer on the gpu
   *
   */
  void createIBO();

  /** \brief creates all vertex declarations needed for deferred draw call renderer
   *
   */
  void createVertexDeclaration();

public:
  // color picking

   /** Call this function to update the color picking array
  * The _offsett value can be used to shift the color in the resulting arrays.
  * pick Any uses the offset to generate arrays for picking everything.
  *
  * @param _state
  * @param _offset
  */
  void updatePickingVertices(ACG::GLState& _state , uint _offset = 0);

  /** \brief get a pointer to the per vertex picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingVertices to update the buffer before you render it via
  * ACG::GLState::colorPointer.
  *
  * @return pointer to the first element of the picking buffer
  */
  ACG::Vec4uc * pickVertexColorBuffer(){
    if ( !pickVertColBuf_.empty() )
      return &(pickVertColBuf_)[0];
    else {
      std::cerr << "Illegal request to pickVertexColorBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  };

  /** \brief get a pointer to the per vertex picking vertex buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingVertices to update the buffer before you render it via
  * ACG::GLState::vertexPointer.
  *
  * @return pointer to the first element of the picking buffer
  */
  ACG::Vec3f * pickVertexBuffer(){
    if ( !pickVertBuf_.empty() )
      return &(pickVertBuf_)[0];
    else {
      std::cerr << "Illegal request to pickVertexBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  };


  /**  \brief Optimized rendering of vertex picking ids with a shader
   *
   * @param _mvp model view projection transformation
   * @param _pickOffset base picking id of the first vertex
   */
  void drawPickingVertices_opt(const GLMatrixf& _mvp, size_t _pickOffset);


  /**  \brief Check if optimized vertex picking is supported
   *
   */
  bool supportsPickingVertices_opt();

  /**  \brief Update color picking array for the shader implementation
   *
   * @param _state
   */
  void updatePickingVertices_opt(ACG::GLState& _state);


  TextureBuffer* pickVertexMap_opt(){
    if ( pickVertexMapTBO_.is_valid() )
      return &pickVertexMapTBO_;
    else {
      std::cerr << "Illegal request to pickVertexMap_opt when buffer is empty!" << std::endl;
      return 0;
    }
  }


private:

  /// The vertex buffer used for vertex picking
  std::vector< ACG::Vec3f > pickVertBuf_;
  /// The color buffer used for vertex picking
  std::vector< ACG::Vec4uc > pickVertColBuf_;


  // map from vbo vertex id to openmesh vertex id
  TextureBuffer pickVertexMapTBO_;

  // vertex picking shader
  GLSL::Program* pickVertexShader_;


  // selected shader picking method:
  //  0 -> use texturebuffer which maps from vbo id to openmesh vertex id
  //  1 -> draw with indexbuffer mapping from openmesh vertex id to vbo vertex
  int pickVertexMethod_;

public:

  /**  \brief Update color picking array for edges
   *
   * Call this function to update the color picking array
   * The _offsett value can be used to shift the color in the resulting arrays.
   * pick Any uses the offset to generate arrays for picking everything.
   *
   * @param _state
   * @param _offset
   */
  void updatePickingEdges(ACG::GLState& _state , uint _offset = 0 );

  /** \brief get a pointer to the per edge picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingEdges to update the buffer before you render it via
  * ACG::GLState::colorPointer.
  *
  * @return pointer to the first element of the picking buffer
  */
  ACG::Vec4uc * pickEdgeColorBuffer(){
    if ( !pickEdgeBuf_.empty() )
      return &(pickEdgeBuf_)[0];
    else {
      std::cerr << "Illegal request to pickEdgeColorBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  }


  /**  \brief Optimized rendering of edge picking ids with a shader
   *
   * @param _mvp model view projection transformation
   * @param _pickOffset base picking id of the first edge
   */
  void drawPickingEdges_opt(const GLMatrixf& _mvp, size_t _pickOffset);


  /**  \brief Check if optimized face picking is supported
   *
   */
  bool supportsPickingEdges_opt();

  /**  \brief Update color picking array for the shader implementation
   *
   * @param _state
   */
  void updatePickingEdges_opt(ACG::GLState& _state );

private:

  std::vector< ACG::Vec4uc > pickEdgeBuf_;

  // edge picking shader
  GLSL::Program* pickEdgeShader_;


public:

  /**  \brief Update color picking array for faces
   *
   * @param _state
   */
  void updatePickingFaces(ACG::GLState& _state );

  /** \brief get a pointer to the per face picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingFaces to update the buffer before you render it via
  * ACG::GLState::colorPointer.
  *
  * @return pointer to the per face picking color buffer
  */
  ACG::Vec4uc * pickFaceColorBuffer(){
    if ( !pickFaceColBuf_.empty() )
      return &(pickFaceColBuf_)[0];
    else {
      std::cerr << "Illegal request to pickFaceColorBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  }

  /** \brief get a pointer to the per vertex picking color buffer
    *
    * This function will return a pointer to the first element of the picking buffer.
    * Use updatePickingFaces to update the buffer before you render it via
    * ACG::GLState::colorPointer.
    *
    * @return pointer to the first element of the picking buffer
    */
  ACG::Vec3f * pickFaceVertexBuffer(){
    if ( !pickFaceVertexBuf_.empty() )
      return &(pickFaceVertexBuf_)[0];
    else {
      std::cerr << "Illegal request to pickFaceVertexBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  }

  /**  \brief Optimized rendering of face picking ids with a shader
   *
   * @param _mvp model view projection transformation
   * @param _pickOffset base picking id of the first face
   */
  void drawPickingFaces_opt(const GLMatrixf& _mvp, size_t _pickOffset);


  /**  \brief Check if optimized face picking is supported
   *
   */
  bool supportsPickingFaces_opt();

  /**  \brief Update color picking array for the shader implementation
   *
   * @param _state
   */
  void updatePickingFaces_opt(ACG::GLState& _state );


  TextureBuffer* pickFaceTriangleMap_opt(){
    if ( pickFaceTriToFaceMapTBO_.is_valid() )
      return &pickFaceTriToFaceMapTBO_;
    else {
      std::cerr << "Illegal request to pickFaceTriangleMap_opt when buffer is empty!" << std::endl;
      return 0;
    }
  }

private:

  // unoptimized picking buffers
  std::vector< ACG::Vec3f > pickFaceVertexBuf_;
  std::vector< ACG::Vec4uc > pickFaceColBuf_;

  // optimized picking with shaders: maps from triangle id in draw vbo to face id in openmesh
  TextureBuffer pickFaceTriToFaceMapTBO_;

  /// optimized face picking shader
  GLSL::Program* pickFaceShader_;

public:
  /** \brief Call this function to update the color picking array
  *
  * This function calls the updatePickingVertices, updatePickingEdges, updatePickingVertices
  * functions with an appropriate offset so that the standard arrays will be updated.
  *
  * @param _state OpenGL state
  */
  void updatePickingAny(ACG::GLState& _state );

  /** \brief get a pointer to the any picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingAny to update the buffer before you render it via
  * ACG::GLState::colorPointer.
  *
  * @return Pointer to the first element of the picking buffer
  */
  ACG::Vec4uc * pickAnyFaceColorBuffer(){
    if ( !pickAnyFaceColBuf_.empty() )
      return &(pickAnyFaceColBuf_)[0];
    else {
      std::cerr << "Illegal request to pickAnyFaceColorBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  }

  /** \brief get a pointer to the any picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingAny to update the buffer before you render it via
  * ACG::GLState::colorPointer.
  *
  * @return Pointer to the first element of the picking buffer
  */
  ACG::Vec4uc * pickAnyEdgeColorBuffer(){
    if ( !pickAnyEdgeColBuf_.empty() )
      return &(pickAnyEdgeColBuf_)[0];
    else {
      std::cerr << "Illegal request to pickAnyEdgeColorBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  }

  /** \brief get a pointer to the any picking color buffer
  *
  * This function will return a pointer to the first element of the picking buffer.
  * Use updatePickingAny to update the buffer before you render it via
  * ACG::GLState::colorPointer.
  *
  * @return Pointer to the first element of the picking buffer
  */
  ACG::Vec4uc * pickAnyVertexColorBuffer(){
    if ( !pickAnyVertexColBuf_.empty() )
      return &(pickAnyVertexColBuf_)[0];
    else {
      std::cerr << "Illegal request to pickAnyVertexColorBuffer when buffer is empty!" << std::endl;
      return 0;
    }
  }

  /**  \brief Optimized rendering of any picking ids with a shader
   *
   * @param _mvp model view projection transformation
   * @param _pickOffset base picking id of the first element
   */
  void drawPickingAny_opt(const GLMatrixf& _mvp, size_t _pickOffset);

  /**  \brief Check if optimized any picking is supported
   *
   */
  bool supportsPickingAny_opt();

  /**  \brief Update color picking array for the shader implementation
   *
   * @param _state
   */
  void updatePickingAny_opt(ACG::GLState& _state );

private:

  std::vector< ACG::Vec4uc > pickAnyFaceColBuf_;
  std::vector< ACG::Vec4uc > pickAnyEdgeColBuf_;
  std::vector< ACG::Vec4uc > pickAnyVertexColBuf_;


private:

  // small helper functions

  /** \brief Number of triangles after triangulation of the mesh
   *
   * returns the number of tris after triangulation of this mesh
   * if needed, also returns the highest number of vertices of a face
   *
   * @param _pOutMaxPolyVerts max face size
   * @param _pOutNumIndices   total number of indices
   *
   * @return  number of triangles
  */
  unsigned int countTris(unsigned int* _pOutMaxPolyVerts = 0, unsigned int* _pOutNumIndices = 0);

  /** \brief get the texture index of a triangle
   *
   *
   *  @param _tri Triangle index (-1 if not available)
   *
   *  @return Texture index of a triangle
   */
  int getTextureIDofTri(unsigned int _tri);

  /** \brief get the texture index of a face
   *
   *  @param _face Face index
   *  @return Face Texture id of face
   */
  int getTextureIDofFace(unsigned int _face);

  /** \brief get the data type of a mesh property
   *
   *
   *  @param _prop  mesh property
   *  @param _outType [out] data type i.e. GL_FLOAT, GL_DOUBLE
   *  @param _outSize [out] number of atoms in range 1..4
   *  @return address of property data
   */
  const void* getMeshPropertyType(OpenMesh::BaseProperty* _prop, GLuint* _outType, unsigned int* _outSize) const;

  /** \brief test mesh property for type T
   *
   * Test whether property is of type T (eg float) or is a vector of type T.
   * Property vectors up to size 4 are supported (native vertex properties).
   * 
   *  @param _prop  mesh property
   *  @param _outSize [out] number of atoms in range 1..4,  set to 0 if test failed
   *  @return address of property data, 0 if test failed
   */
  template<class T>
  const void* testMeshPropertyTypeT(const OpenMesh::BaseProperty* _prop, unsigned int* _outSize) const;


public:

  /** \brief dump current vertex/index buffer to wavefront obj
   *
   * @param _filename file name of obj file
  */
  void dumpObj(const char* _filename) const;

private:

  /// OpenMesh object to be rendered
  Mesh& mesh_;

  /// final index buffer used for rendering
  unsigned int* indices_;

  /// hint on what to rebuild
  unsigned int rebuild_;

  /** used to track mesh changes, that require a full rebuild
    * values directly taken from Mesh template
    */
  size_t prevNumFaces_,prevNumVerts_;


  /// Color Mode: 0: none, 1: per vertex,  else: per face
  int colorMode_;

  /// Color Mode of vbo
  int curVBOColorMode_;

  /// flat / smooth shade mode toggle
  int flatMode_;

  /// normals in VBO currently in flat / smooth mode
  int bVBOinFlatMode_;

  /// per vertex / halfedge texture mode toggle:  0: per vertex,  1: per halfedge
  int textureMode_;

  /// texcoords in VBO currently in per vertex / halfedge mode toggle
  int bVBOinHalfedgeTexMode_;

  /// per vertex / halfedge normals mode toggle:  0: per vertex,  1: per halfedge
  int halfedgeNormalMode_;

  /// normals in VBO currently in per vertex / halfedge mode toggle
  int bVBOinHalfedgeNormalMode_;


  /** inverse vertex map: original OpenMesh vertex index -> one vertex index in vbo
      this map is ambiguous and only useful for per vertex attributes rendering i.e. lines!
  */
  unsigned int* invVertexMap_;



  //========================================================================
  // flexible vertex layout
  //========================================================================


  struct VertexProperty 
  {
    // get property from vertex, face or halfedge array
    PropertySource source_;

    /// property name in openmesh
    std::string name_;

    /// input name id in vertex shader
    std::string vertexShaderInputName_;

    /// property type as stored in openmesh
    VertexElement sourceType_;

    /// property type as stored in vbo
    VertexElement destType_;

    /// memory address of property data
    const void* propDataPtr_;

    /// element id in vertex declaration
    int declElementID_;
  };
  
  /// fixed vertex elements:
  const size_t offsetPos_,
    offsetNormal_,
    offsetTexc_,
    offsetColor_;

  /// additional optional elements
  std::vector<VertexProperty> additionalElements_;

  //========================================================================
  // texture handling
  //========================================================================

  /** \brief Property for the per face texture index.
  *
  * This property is used by the mesh for texture index specification.
  * If this is invalid, then it is assumed that there is one or no active
  * texture. This means that the generated strips will be independent of texture
  * information.
  */
  std::string textureIndexPropertyName_;

  /** \brief Property for the per face texture coordinates.
  *
  * This property is used by the mesh for texture coordinate specification.
  * If this is invalid, then the strip processor will ignore per face textures during processing.
  */
  std::string perFaceTextureCoordinatePropertyName_;


private:
  //========================================================================
  // write functions for flexible vertex format

  void writeVertexElement(void* _dstBuf, size_t _vertex, size_t _stride, size_t _elementOffset, size_t _elementSize, const void* _elementData);

  void writePosition(size_t _vertex, const ACG::Vec3d& _p);

  void writeNormal(size_t _vertex, const ACG::Vec3d& _n);

  void writeTexcoord(size_t _vertex, const ACG::Vec2f& _uv);

  void writeColor(size_t _vertex, unsigned int _color);

  void writeVertexProperty(size_t _vertex, const VertexElement* _elementDesc, const ACG::Vec4f& _propf);

  void writeVertexProperty(size_t _vertex, const VertexElement* _elementDesc, const ACG::Vec4d& _propd);


  /** \brief Read one vertex from the rendering vbo.
  *
  * @param _vertex vertex id from the rendering vbo (not the original input id from openmesh!)
  * @param _dst [out] pointer to address that will store the vertex. Must have enough space allocated, see vertex declaration stride to get the number of bytes
  */
  void readVertexFromVBO(unsigned int _vertex, void* _dst);


public:

  //===========================================================================
  // fully expanded vbo
  //===========================================================================  


  /** \brief the mesh has been changed
  *
  * call this function if you changed anything of the mesh.
  */
  void invalidateFullVBO();

  /** \brief update the full mesh vbo
  *
  * the full vbo is the non-indexed version for drawing.
  * it's not optimized for rendering at all and it uses lots of memory, so should only be used as last resort.
  */
  void updateFullVBO();


private:
  // fully expanded mesh vbo (not indexed)
  // this is only used for drawmodes with incompatible combinations of interpolation modes (ex. smooth gouraud lighting with flat face colors)
  GeometryBuffer vboFull_;

  // full vbo has been invalidated
  bool updateFullVBO_;

  // (colored) edges
  GeometryBuffer vboEdges_;

public:
  //========================================================================
  // per edge buffers

  /** \brief Update of the buffers
  *
  * This function will set all per edge buffers to invalid and will force an update
  * whe they are requested
  */
  void invalidatePerEdgeBuffers() {updatePerEdgeBuffers_ = 1;}

  /** \brief Update all per edge drawing buffers
  *
  * The updated buffers are: vertex buffer ( 2 vertices per edge ), color buffer.
  * Only used in legacy rendering
  */
  void updatePerEdgeBuffers();

  /** \brief Update the combined position + color buffer for edge rendering
   */
  void updatePerEdgeBuffersNew();

  /** \brief get a pointer to the per edge vertex buffer
  *
  * This function will return a pointer to the first element of the vertex buffer.
  */
  ACG::Vec3f* perEdgeVertexBuffer();

  /** \brief get a pointer to the per edge color buffer
  *
  * This function will return a pointer to the first element of the color buffer.
  */
  ACG::Vec4f* perEdgeColorBuffer();

  /** \brief Update of the buffers
  *
  * This function will set all per edge buffers to invalid and will force an update
  * whe they are requested
  */
  void invalidatePerHalfedgeBuffers() {updatePerHalfedgeBuffers_ = 1;}

  /** \brief Update all per edge drawing buffer
  *n
  * The updated buffers are: per edge vertex buffer ( 2 vertices per edge )
  */
  template<typename Mesh::Normal (DrawMeshT::*NormalLookup)(typename Mesh::FaceHandle)>
  void updatePerHalfedgeBuffers();

  /** \brief get a pointer to the per edge vertex buffer
  *
  * This function will return a pointer to the first element of the vertex buffer.
  */
  ACG::Vec3f* perHalfedgeVertexBuffer();

  /** \brief get a pointer to the per edge color buffer
  *
  * This function will return a pointer to the first element of the color buffer.
  */
  ACG::Vec4f* perHalfedgeColorBuffer();


  /** \brief updates per edge and halfedge vertex declarations
  */
  void updateEdgeHalfedgeVertexDeclarations();


  /** \brief getter for vertex declarations
  */
  const VertexDeclaration* getEdgeColoredVertexDeclaration() const {return vertexDeclEdgeCol_;}

  /** \brief getter for vertex declarations
  */
  const VertexDeclaration* getHalfedgeVertexDeclaration() const {return vertexDeclHalfedgePos_;}

  /** \brief getter for vertex declarations
  */
  const VertexDeclaration* getHalfedgeColoredVertexDeclaration() const {return vertexDeclHalfedgeCol_;}


private:
  int updatePerEdgeBuffers_;
  std::vector<ACG::Vec3f> perEdgeVertexBuf_;
  std::vector<ACG::Vec4f> perEdgeColorBuf_;

  std::vector<float> perEdgeBuf_; // vertex vec3f + color vec4f

  int updatePerHalfedgeBuffers_;
  std::vector<ACG::Vec3f> perHalfedgeVertexBuf_;
  std::vector<ACG::Vec4f> perHalfedgeColorBuf_;

  /** \brief compute halfedge point
  * compute visualization point for halfedge (shifted to interior of face)
  *
  * @param _heh
  *
  * @return
  */
  template<typename Mesh::Normal (DrawMeshT::*NormalLookup)(typename Mesh::FaceHandle)>
  typename Mesh::Point halfedge_point(const typename Mesh::HalfedgeHandle _heh);

  inline
  typename Mesh::Normal cachedNormalLookup(typename Mesh::FaceHandle fh) {
      return mesh_.normal(fh);
  }

  inline
  typename Mesh::Normal computedTriMeshNormal(typename Mesh::FaceHandle fh) {
      typename Mesh::FVIter fv_iter = mesh_.fv_begin(fh);
      const typename Mesh::Point p1 = mesh_.point(*fv_iter);
      const typename Mesh::Point p2 = mesh_.point(*(++fv_iter));
      const typename Mesh::Point p3 = mesh_.point(*(++fv_iter));
      return ( p1 + p2 + p3 ) / 3.0;
  }

  inline
  typename Mesh::Normal computedNormal(typename Mesh::FaceHandle fh) {
      unsigned int count = 0;
      typename Mesh::Normal normal(0, 0, 0);
      for (typename Mesh::FVIter fv_it = mesh_.fv_begin(fh), fv_end = mesh_.fv_end(fh); fv_it != fv_end; ++fv_it) {
          normal += mesh_.point(*fv_it);
          ++count;
      }
      normal /= count;
      return normal;
  }

  typename Mesh::HalfedgeHandle mapToHalfedgeHandle(size_t _vertexId);

};


// defined in DrawMeshT_impl.cc:
extern template class DrawMeshT<::OpenMesh::TriMesh>;
extern template class DrawMeshT<::OpenMesh::PolyMesh>;

} // namespace ACG
