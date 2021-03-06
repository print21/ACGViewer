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
//  CLASS MeshNodeT
//
//=============================================================================

#ifndef DOXY_IGNORE_THIS
#ifndef ACG_MESHNODE_HH
#define ACG_MESHNODE_HH

//== INCLUDES =================================================================

#include "BaseNode.hh"
#include <vector>
#include <string>
#include "../ShaderUtils/GLSLShader.hh"


//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {


//== CLASS DEFINITION =========================================================


/** \class MeshNodeT MeshNodeT.hh <ACG/Scenegraph/MeshNodeT.hh>

    This node is used to draw an ACG mesh.

    It uses compile-time-tags for recognizing whether
    the mesh is a TriMeshT or a PolyMeshT.

    The available draw modes are collected based on these mesh
    properties like normal or color for faces or vertices.
*/

template <class Mesh>
class MeshNodeDeprecatedT : public BaseNode
{

public:

   /** Default constructor
   *
   * @param _mesh   The mesh that should be rendered
   * @param _parent parent node
   * @param _name   Name of the node, which defaults to "<MeshNode>"
   */
  MeshNodeDeprecatedT(const Mesh&  _mesh,
	    BaseNode*    _parent=0,
	    std::string  _name="<MeshNode>");

  /// Destructor
  virtual ~MeshNodeDeprecatedT();


  ACG_CLASSNAME(MeshNode);


  /// return available draw modes
  DrawModes::DrawMode  availableDrawModes() const;
  /// update bounding box
  void boundingBox(Vec3d& _bbMin, Vec3d& _bbMax);
  /// drawing
  void draw(GLState& _state, const DrawModes::DrawMode& _drawMode);
  /// picking
  void pick(GLState& _state, PickTarget _target);


  /// get mesh
  const Mesh& mesh() const { return mesh_; }


  /// update geometry buffers (vertex buffer objects)
  void update_geometry();
  /** update face indices (only for triangle meshes).  will be
      overridden by TriStripNodeT. */
  virtual void update_topology();

  /** \brief Setup a mapping between internal texture ids on the mesh and the ids for the loaded textures in opengl
   *
   * @param _map maps between an int index stored in the Mesh describing which texture to use for a face,
   *             and the GluInt name of the texture bound by the TextureNode. \n
   *             If such a map is not available ( =0 ), assume TextureNode has already bound a texture
   *             And render without switching textures
   */
  void setTextureMap( std::map< int, GLuint>* _map){ textureMap_ = _map; };

  /** \brief Setup a mapping between internal texture ids on the mesh and the properties containing texture coordinates
   *
   * @param _map maps between an int index stored in the Mesh describing which texture to use
   *             and a property name giving 2D Texture coordinates for halfedges ( texcoords for to vertex )
   */
  void set_property_map( std::map< int, std::string>* _map){ propertyMap_ = _map; };

  /** \brief Set default property name to get texture coordinates for per face texcoords
   *
   * Property has to be Mesh::TexCoord2D as a halfedge property describing vertex coordinates for
   * to Vertex per face. Defaults to h:texcoords2D
   */
  void setHalfedgeTextcoordPropertyName( std::string _default_halfedge_textcoord_property )
    { default_halfedge_textcoord_property_ = _default_halfedge_textcoord_property; };

  /** \brief Property to use when switching between multiple textures
   *
   * The given mesh property has to contain an int index of textures to use per face.
   * If it is not given, the default face_texture_index property of OpenMesh will be used.
   * If there is no texture map given via set_texture_map, texture switching will be disabled and
   * only the currently bound texture will be used.
   */
  void setIndexPropertyName( std::string _index_property_name) { indexPropertyName_ = _index_property_name; };

protected:


  // types
  enum FaceMode { FACE_NORMALS, FACE_COLORS, PER_VERTEX, FACE_NORMALS_COLORS, FACE_HALFEDGE_TEXTURED };

  // draw vertices
  void draw_vertices();
  // draw polygons. to be overridden by TriStripNodeT
  virtual void draw_faces(FaceMode _mode);

  // pick vertices
  void pick_vertices(GLState& _state, bool _front = false);
  // pick polygons. to be overridden by TriMeshNodeT
  void pick_faces(GLState& _state);
  // pick edges
  void pick_edges(GLState& _state, bool _front = false);
  // pick anything
  void pick_any(GLState& _state);

  // update pick buffer sizes
  void update_pick_buffers ();

  // opengl vertex array stuff
  enum ArrayType
  {
    NONE                    = 0,
    VERTEX_ARRAY            = 1,
    NORMAL_ARRAY            = 2,
    COLOR_ARRAY             = 4,
    TEXTURE_COORD_1D_ARRAY  = 8,
    TEXTURE_COORD_2D_ARRAY  = 16,
    TEXTURE_COORD_3D_ARRAY  = 32
  };

  // enable/disable vertex arrays according to the bits in _arrays
  void enable_arrays(unsigned int _arrays);



protected:

  // mesh reference
  const Mesh&  mesh_;

  // which arrays are currently enabled?
  unsigned int enabled_arrays_;

  // vertex buffer objects
  unsigned int  face_index_buffer_, vertex_buffer_, normal_buffer_;

  // index list for fast rendering (will be accessed by TriStripNodeT
  std::vector<unsigned int>  indices_;

private:

  bool vertexBufferInitialized_;
  bool normalBufferInitialized_;
  bool faceIndexBufferInitialized_;

  // Internal buffer used when rendering non float vertex coordinates
  std::vector< ACG::Vec3f > vertices_;

  // Internal buffer used when rendering non float normals
  std::vector< ACG::Vec3f > normals_;

  // Mapping of mesh face texture indices to gltexture id ( has to be provided externally )
  std::map< int, GLuint>* textureMap_;

  // Mapping of mesh face texture indices to coordinate property names ( has to be provided externally )
  std::map< int, std::string>* propertyMap_;

  // Property name of the per face texture coordinates
  std::string default_halfedge_textcoord_property_;

  // Property name of the per face texture index.
  std::string indexPropertyName_;

  // display Lists used for picking
  GLuint faceList_;
  GLuint vertexList_;
  GLuint edgeList_;
  GLuint anyList_;

  // do we need to update our display lists?
  bool updateFaceList_;
  bool updateVertexList_;
  bool updateEdgeList_;
  bool updateAnyList_;
  unsigned int faceBaseIndex_;
  unsigned int vertexBaseIndex_;
  unsigned int edgeBaseIndex_;
  unsigned int anyBaseIndex_;

  // buffers for draw arrays during picking
  std::vector< ACG::Vec3f > pickVertexBuf_;
  std::vector< ACG::Vec4uc > pickColorBuf_;

  // bounding box
  Vec3d bbMin_;
  Vec3d bbMax_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#if defined(INCLUDE_TEMPLATES) && !defined(ACG_MESHNODE_C)
#define ACG_MESHNODE_TEMPLATES
#include "MeshNodeT.cc"
#endif
//=============================================================================
#endif // ACG_MESHNODE_HH defined
#endif // DOXY_IGNORE_THIS
//=============================================================================
