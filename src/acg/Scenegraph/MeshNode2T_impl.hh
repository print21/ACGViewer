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
//  CLASS MeshNodeT - IMPLEMENTATION
//
//=============================================================================

#define ACG_MESHNODE_C

#include "MeshNode2T.hh"

#include <ACG/Geometry/GPUCacheOptimizer.hh>
#include <ACG/GL/DrawMesh.hh>
#include <ACG/GL/GLError.hh>
#include <ACG/GL/GLState.hh>

//== NAMESPACES ===============================================================


namespace ACG {
namespace SceneGraph {
    

//== INCLUDES =================================================================



template<class Mesh>
MeshNodeT<Mesh>::
MeshNodeT(Mesh& _mesh,
          BaseNode* _parent,
          const std::string& _name ):
  MeshNodeBase(_parent, _name),
  mesh_(_mesh),
  drawMesh_(0),
  enableNormals_(true),
  enableColors_(true),
  enabled_arrays_(0),
  updateVertexPicking_(true),
  vertexPickingBaseIndex_(0),
  updateEdgePicking_(true),
  edgePickingBaseIndex_(0),
  updateFacePicking_(true),
  facePickingBaseIndex_(0),
  updateAnyPicking_(true),
  anyPickingBaseIndex_(0),
  perFaceTextureIndexAvailable_(false),
  textureMap_(0),
  draw_with_offset_(false)
{
 
  /// \todo : Handle vbo not supported
  
  drawMesh_ = new DrawMeshT<Mesh>(mesh_);

  // Hand draw mesh down to super class.
  MeshNodeBase::supplyDrawMesh(drawMesh_);
}  

template<class Mesh>
MeshNodeT<Mesh>::
~MeshNodeT()
{
  // Delete all allocated buffers
  delete drawMesh_;
}

template<class Mesh>
DrawModes::DrawMode
MeshNodeT<Mesh>::
availableDrawModes() const {
  DrawModes::DrawMode drawModes(DrawModes::NONE);
  
  // We can always render points and a wireframe.
  drawModes |= DrawModes::POINTS;
  drawModes |= DrawModes::HIDDENLINE;
  drawModes |= DrawModes::WIREFRAME;
  drawModes |= DrawModes::HALFEDGES;
  
  if (mesh_.has_vertex_normals())
  {
    drawModes |= DrawModes::POINTS_SHADED;  
    drawModes |= DrawModes::SOLID_SMOOTH_SHADED;
    drawModes |= DrawModes::SOLID_PHONG_SHADED;
  }
  
  if (mesh_.has_face_normals())
    drawModes |= DrawModes::SOLID_FLAT_SHADED;

  if (mesh_.has_halfedge_normals())
    drawModes |= DrawModes::SOLID_SMOOTH_SHADED_FEATURES;
  
  if (mesh_.has_vertex_colors())
  {
    drawModes |= DrawModes::POINTS_COLORED;
    drawModes |= DrawModes::SOLID_POINTS_COLORED;

    if (mesh_.has_vertex_normals())
      drawModes |= DrawModes::SOLID_POINTS_COLORED_SHADED;
  }

  if(mesh_.has_edge_colors())
  {
    drawModes |= DrawModes::EDGES_COLORED;
  }

  if(mesh_.has_halfedge_colors())
  {
    drawModes |= DrawModes::HALFEDGES_COLORED;
  }
  
  bool enableTexturedFaces = drawMesh_->perFaceTextureCoordinateAvailable() != 0;

  if (mesh_.has_face_colors()) {
    drawModes |= DrawModes::SOLID_FACES_COLORED;
    
    if( mesh_.has_face_normals() )
      drawModes |= DrawModes::SOLID_FACES_COLORED_FLAT_SHADED;

    if (mesh().has_vertex_normals()) {
      drawModes |= DrawModes::SOLID_FACES_COLORED_SMOOTH_SHADED;

      if (enableTexturedFaces)
        drawModes |= DrawModes::SOLID_FACES_COLORED_2DTEXTURED_FACE_SMOOTH_SHADED;
    }
  }
  
  if ( mesh_.has_vertex_texcoords2D() ) {
    drawModes |= DrawModes::SOLID_TEXTURED;
    
    if (mesh_.has_vertex_normals())
      drawModes |= DrawModes::SOLID_TEXTURED_SHADED; 
  }
  
  if ( enableTexturedFaces ) {
    drawModes |= DrawModes::SOLID_2DTEXTURED_FACE;
    
    if (mesh_.has_face_normals())
      drawModes |= DrawModes::SOLID_2DTEXTURED_FACE_SHADED;
  }
  
  return drawModes;
}

template<class Mesh>
void
MeshNodeT<Mesh>::
boundingBox(Vec3d& _bbMin, Vec3d& _bbMax) {
  _bbMin.minimize(bbMin_);
  _bbMax.maximize(bbMax_);
}

template<class Mesh>
void
MeshNodeT<Mesh>::
draw(GLState& _state, const DrawModes::DrawMode& _drawMode) {
    
/*  
  if ( ( _drawMode & DrawModes::SOLID_FLAT_SHADED ) ||
    ( _drawMode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED) ||
    ( _drawMode & DrawModes::SOLID_TEXTURED) ||
    ( _drawMode & DrawModes::SOLID_2DTEXTURED_FACE))
  {
    drawMesh_->setFlatShading();
  }
  else
    drawMesh_->setSmoothShading();


  if ( (_drawMode & DrawModes::SOLID_FACES_COLORED ||
    _drawMode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED))
  {
    drawMesh_->usePerFaceColors();
  }
  else
    drawMesh_->usePerVertexColors();
*/
    
  
  glPushAttrib(GL_ENABLE_BIT);
  
  GLenum prev_depth_offset = _state.depthFunc();
  if(draw_with_offset_)
  {
    ACG::GLState::enable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -10.0f);
    ACG::GLState::depthFunc(GL_LEQUAL);
  }
  
  GLenum prev_depth = _state.depthFunc();

  /// get bound texture buffer and target
  GLuint lastBuffer = ACG::GLState::getBoundTextureBuffer();
  GLenum lastTarget = ACG::GLState::getBoundTextureTarget();

  // Unbind to avoid painting textures on non textured primitives
  ACG::GLState::bindTexture(lastTarget,0);


  if ( (_drawMode & DrawModes::POINTS) || (_drawMode & DrawModes::POINTS_COLORED) || (_drawMode & DrawModes::POINTS_SHADED )  ) {
    
    _state.set_color( _state.specular_color() );

    ACG::GLState::shadeModel(GL_FLAT);
    
    if ( _drawMode & DrawModes::POINTS_SHADED  ) {
      ACG::GLState::enable(GL_LIGHTING);
    } else
      ACG::GLState::disable(GL_LIGHTING);
  
    // Use Colors in this mode if allowed
    if ( enableColors_ && (_drawMode & DrawModes::POINTS_COLORED) )
    {
      drawMesh_->usePerVertexColors();

      // If we have colors and lighting with normals, we have to use colormaterial
      if ( enableNormals_ && (_drawMode & DrawModes::POINTS_SHADED ) )
        ACG::GLState::enable(GL_COLOR_MATERIAL);
      else
        ACG::GLState::disable(GL_COLOR_MATERIAL);
    }
    else
      drawMesh_->disableColors();
    
    // Bring the arrays online
//    enable_arrays(arrays);
    
    // Draw vertices
    draw_vertices();
  }
  
  
  /// \todo We can render also wireframe shaded and with vertex colors
  if (_drawMode & DrawModes::WIREFRAME)
  {

    const Vec4f oldColor = _state.color();

    // If the mode is atomic, we use the specular, otherwise we take the overlay color
    if (_drawMode.isAtomic() )
      _state.set_color( _state.specular_color() );
    else
      _state.set_color( _state.overlay_color() );

    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);

    drawMesh_->disableColors();

    draw_lines();

    _state.set_color(oldColor);
  }  
  
  if (_drawMode & DrawModes::HIDDENLINE)
  {
//    enable_arrays(VERTEX_ARRAY);
    
    // First:
    // Render all faces in background color to initialize z-buffer
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

//    drawMesh_->SetFlatShading();
    drawMesh_->disableColors();

    Vec4f  clear_color  = _state.clear_color();
    clear_color[3] = 1.0;
    _state.set_color( clear_color );

    ACG::GLState::depthRange(0.01, 1.0);
    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    
    // Second
    // Render the lines. All lines not on the front will be skipped in z-test
//    enable_arrays(VERTEX_ARRAY|LINE_INDEX_ARRAY);
    ACG::GLState::depthFunc(GL_LEQUAL);

    _state.set_color( _state.specular_color() );

    draw_lines();
    
    //restore depth buffer comparison function for the next draw calls inside this function
    ACG::GLState::depthFunc(prev_depth);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  if (_drawMode & DrawModes::EDGES_COLORED)
  {
    enable_arrays( PER_EDGE_VERTEX_ARRAY | PER_EDGE_COLOR_ARRAY );
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    draw_lines();
  }  

  if (_drawMode & DrawModes::HALFEDGES)
  {
    _state.set_color( _state.specular_color() );

    enable_arrays( PER_HALFEDGE_VERTEX_ARRAY);
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    draw_halfedges();
  }  

  if (_drawMode & DrawModes::HALFEDGES_COLORED)
  {
    enable_arrays( PER_HALFEDGE_VERTEX_ARRAY | PER_HALFEDGE_COLOR_ARRAY );
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    draw_halfedges();
  }  

  if ( ( _drawMode & DrawModes::SOLID_POINTS_COLORED ) && mesh_.has_vertex_colors() )
  {
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->usePerVertexColors();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
  }

  if ( ( _drawMode & DrawModes::SOLID_POINTS_COLORED_SHADED ) && mesh_.has_vertex_colors() && mesh_.has_vertex_normals() )
  {
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);
    if ( enableNormals_ ) {
       ACG::GLState::enable(GL_COLOR_MATERIAL);
    } else {
      ACG::GLState::disable(GL_COLOR_MATERIAL);
    }

    drawMesh_->usePerVertexColors();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
  }

  if ( ( _drawMode & DrawModes::SOLID_FLAT_SHADED ) && mesh_.has_face_normals() && mesh_.n_faces() > 0)
  {
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->setFlatShading();
    drawMesh_->disableColors();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
  }
  
  if ( ( _drawMode & DrawModes::SOLID_SMOOTH_SHADED ) && mesh_.has_vertex_normals() )
  {
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->usePerVertexNormals();
    drawMesh_->setSmoothShading();
    drawMesh_->disableColors();
    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
  }
  
  if ( ( _drawMode & DrawModes::SOLID_PHONG_SHADED ) && mesh_.has_vertex_normals() )
  {
    ///\todo Integrate shader here! 
    //     if ( parent() != 0 ) {
    //       if ( parent()->className() == "ShaderNode" ) {
    //
    //         ShaderNode* node = dynamic_cast< ShaderNode* > ( parent() );
    //
    //         GLSL::PtrProgram program = node->getShader( DrawModes::SOLID_PHONG_SHADED );
    //
    //         // Enable own Phong shader
    //         program->use();
//    enable_arrays(VERTEX_ARRAY | NORMAL_VERTEX_ARRAY );
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->usePerVertexNormals();
    drawMesh_->setSmoothShading();
    drawMesh_->disableColors();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    
    //disable own Phong shader
    //         program->disable();
    //       }
    //     }
  }
  
  
  if ( ( _drawMode & DrawModes::SOLID_FACES_COLORED ) && mesh_.has_face_colors()  && mesh_.n_faces() > 0)
  {
    Vec4f base_color_backup = _state.base_color();
    
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);
//    enable_arrays(PER_FACE_VERTEX_ARRAY | PER_FACE_COLOR_ARRAY);    

    drawMesh_->usePerFaceColors();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    
    _state.set_base_color(base_color_backup);
  }


  if ( ( _drawMode & DrawModes::SOLID_SMOOTH_SHADED_FEATURES ) && mesh_.has_halfedge_normals()  && mesh_.n_faces() > 0)
  {
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->disableColors();
    drawMesh_->setSmoothShading();
    drawMesh_->usePerHalfedgeNormals();

    draw_faces();

    ACG::GLState::depthRange(0.0, 1.0);
  }
  
  if ( ( _drawMode & DrawModes::SOLID_FACES_COLORED_FLAT_SHADED ) && mesh_.has_face_colors() && mesh_.has_face_normals()  && mesh_.n_faces() > 0 )
  {
    Vec4f base_color_backup = _state.base_color();
    ACG::GLState::enable(GL_LIGHTING);
    
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);
//    enable_arrays(PER_FACE_VERTEX_ARRAY | PER_FACE_COLOR_ARRAY | PER_FACE_NORMAL_ARRAY );

    drawMesh_->setFlatShading();
    drawMesh_->usePerFaceColors();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    
    _state.set_base_color(base_color_backup);
  }

  
  if ( ( _drawMode & DrawModes::SOLID_FACES_COLORED_SMOOTH_SHADED ) && mesh_.has_face_colors() && mesh_.has_vertex_normals() && mesh_.n_faces() > 0)
  {
    Vec4f base_color_backup = _state.base_color();

    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->setSmoothShading();
    drawMesh_->usePerVertexNormals();
    drawMesh_->usePerFaceColors();


    drawMesh_->draw(textureMap_, true);
    ACG::GLState::depthRange(0.0, 1.0);

    _state.set_base_color(base_color_backup);
  }

  // Rebind the previous texture
  ACG::GLState::bindTexture(lastTarget,lastBuffer);
  

  if ((_drawMode & DrawModes::SOLID_FACES_COLORED_2DTEXTURED_FACE_SMOOTH_SHADED) && mesh_.has_face_colors() && mesh_.has_vertex_normals() && mesh_.n_faces() > 0)
  {
    // face colors, texturing via halfedge texcoords, smooth shading of lighting color

    Vec4f base_color_backup = _state.base_color();

    ACG::GLState::enable(GL_TEXTURE_2D);

    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->setSmoothShading();
    drawMesh_->usePerVertexNormals();
    drawMesh_->usePerFaceColors();
    drawMesh_->usePerHalfedgeTexcoords();

    drawMesh_->draw(textureMap_, true);
    ACG::GLState::depthRange(0.0, 1.0);

    ACG::GLState::disable(GL_TEXTURE_2D);

    _state.set_base_color(base_color_backup);
  }

  if ( ( _drawMode & DrawModes::SOLID_TEXTURED )  && mesh_.has_vertex_texcoords2D())
  {
    ACG::GLState::enable(GL_TEXTURE_2D);
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->disableColors();
    drawMesh_->usePerVertexTexcoords();


    // texture environment: fragment color = texture sample
    GLint prevTexEnvMode = 0;
    glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &prevTexEnvMode);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    ACG::GLState::disable(GL_TEXTURE_2D);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, prevTexEnvMode);
  }

  if ((_drawMode & DrawModes::SOLID_ENV_MAPPED) && mesh_.has_vertex_normals())
  {
    ACG::GLState::enable(GL_TEXTURE_2D);
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->disableColors();
    drawMesh_->usePerVertexTexcoords();


    // texture environment: fragment color = texture sample
    GLint prevTexEnvMode = 0;
    glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &prevTexEnvMode);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    ACG::GLState::disable(GL_TEXTURE_2D);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, prevTexEnvMode);
  }
  
  if ( ( _drawMode & DrawModes::SOLID_TEXTURED_SHADED ) && mesh_.has_vertex_texcoords2D() && mesh_.has_vertex_normals())
  {
//    enable_arrays(VERTEX_ARRAY | NORMAL_VERTEX_ARRAY | TEXCOORD_VERTEX_ARRAY);
    ACG::GLState::enable(GL_TEXTURE_2D);
    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_SMOOTH);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->setSmoothShading();
    drawMesh_->disableColors();
    drawMesh_->usePerVertexTexcoords();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    ACG::GLState::disable(GL_TEXTURE_2D);
  }
  
    
  // Textured by using coordinates stored in halfedges ... arrays generated by stripprocessor
  if ( (_drawMode & DrawModes::SOLID_2DTEXTURED_FACE)  && mesh_.n_faces() > 0 )
  {
    ACG::GLState::enable(GL_TEXTURE_2D);
    
//    enable_arrays( PER_FACE_VERTEX_ARRAY | PER_FACE_TEXCOORD_ARRAY );
    
    ACG::GLState::disable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->disableColors();
    drawMesh_->usePerHalfedgeTexcoords();

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    
    ACG::GLState::disable(GL_TEXTURE_2D);
  }
  

  // Textured by using coordinates stored in halfedges
  if ( ( _drawMode & DrawModes::SOLID_2DTEXTURED_FACE_SHADED ) && mesh_.has_face_normals()  && mesh_.n_faces() > 0)
  {
    ACG::GLState::enable(GL_TEXTURE_2D);

    //    enable_arrays( PER_FACE_VERTEX_ARRAY | PER_FACE_TEXCOORD_ARRAY | PER_FACE_PER_VERTEX_NORMAL_ARRAY );

    ACG::GLState::enable(GL_LIGHTING);
    ACG::GLState::shadeModel(GL_FLAT);
    ACG::GLState::depthRange(0.01, 1.0);

    drawMesh_->setFlatShading();
    drawMesh_->disableColors();
    drawMesh_->usePerHalfedgeTexcoords();

    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    ACG::GLState::disable(GL_TEXTURE_2D);

  }

  enable_arrays(0);
  
  // Unbind all remaining buffers
  ACG::GLState::bindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB , 0 );
  
  if(draw_with_offset_)
  {
    ACG::GLState::depthFunc(prev_depth_offset); 
  }
  
  glPopAttrib();
}


template <class Mesh>
void ACG::SceneGraph::MeshNodeT<Mesh>::getRenderObjects( IRenderer* _renderer, GLState& _state, const DrawModes::DrawMode& _drawMode, const Material* _mat )
{
  RenderObject ro;

  ro.debugName = "MeshNode";
   
  // shader gen setup (lighting, shademode, vertex-colors..)
  
  for (unsigned int i = 0; i < _drawMode.getNumLayers(); ++i)
  {
    const DrawModes::DrawModeProperties* props = _drawMode.getLayer(i);

    // reset renderobject
    ro.initFromState(&_state);
    ro.priority = 0;
    ro.depthRange = Vec2f(0.0f, 1.0f);
    ro.depthTest = true; // some previous node disabled depth testing
    ro.depthWrite = true;
    ro.depthFunc = GL_LESS;
    ro.setMaterial(_mat);


    ro.shaderDesc.vertexTemplateFile.clear(); //QString(props->vertexShader().c_str());
    ro.shaderDesc.geometryTemplateFile.clear(); //QString(props->geometryShader().c_str());
    ro.shaderDesc.fragmentTemplateFile.clear(); //QString(props->fragmentShader().c_str());

    ro.shaderDesc.vertexColorsInterpolator.clear();

    // ------------------------
    // 1. setup drawMesh based on property source


    ro.shaderDesc.vertexColors = true;

    switch (props->colorSource())
    {
    case DrawModes::COLOR_PER_VERTEX: drawMesh_->usePerVertexColors(); break;
    case DrawModes::COLOR_PER_FACE: drawMesh_->usePerFaceColors(); break;
    default:
      {
        drawMesh_->disableColors(); 
        ro.shaderDesc.vertexColors = false;
      } break;
    }

    // only the polygon primitives can set the normal source
    if (props->primitive() == DrawModes::PRIMITIVE_POLYGON)
    {
      switch (props->normalSource())
      {
      case DrawModes::NORMAL_PER_VERTEX: drawMesh_->usePerVertexNormals(); break;
      case DrawModes::NORMAL_PER_HALFEDGE: drawMesh_->usePerHalfedgeNormals(); break;
      default: break;
      }

      if (props->flatShaded())
        drawMesh_->setFlatShading();
      else
        drawMesh_->setSmoothShading();
    }


    ro.shaderDesc.addTextureType(GL_TEXTURE_2D,false,0);

    switch (props->texcoordSource())
    {
    case DrawModes::TEXCOORD_PER_VERTEX: drawMesh_->usePerVertexTexcoords(); break;
    case DrawModes::TEXCOORD_PER_HALFEDGE: drawMesh_->usePerHalfedgeTexcoords(); break;
    default:
      {
        ro.shaderDesc.clearTextures();
      }break;
    }

    // ------------------------
    // 2. prepare renderobject


    // enable / disable lighting
    ro.shaderDesc.numLights = props->lighting() ? 0 : -1;

    // Enable/Disable twoSided Lighting
    ro.shaderDesc.twoSidedLighting = _state.twosided_lighting();

    // TODO: better handling of attribute sources in shader gen
    switch (props->lightStage())
    {
      case DrawModes::LIGHTSTAGE_SMOOTH:
        ro.shaderDesc.shadeMode = SG_SHADE_GOURAUD;
        break;
      case DrawModes::LIGHTSTAGE_PHONG:
        ro.shaderDesc.shadeMode = SG_SHADE_PHONG;
        break;
      case DrawModes::LIGHTSTAGE_UNLIT:
        ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;
        break;
    }

    if (props->flatShaded())
      ro.shaderDesc.shadeMode = SG_SHADE_FLAT;

    if (props->normalSource() == DrawModes::NORMAL_PER_FACE)
      ro.shaderDesc.vertexNormalInterpolator = "flat";
    else
      ro.shaderDesc.vertexNormalInterpolator.clear();

    // handle 'special' primitives (wireframe, hiddenline, primitives in sysmem buffers)..

    if (props->primitive() == DrawModes::PRIMITIVE_WIREFRAME)
    {
      ro.debugName = "MeshNode.Wireframe";

      ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;
      drawMesh_->disableColors();

      // use specular color for lines
      if (_drawMode.isAtomic() )
        ro.emissive = ro.specular;
      else
        ro.emissive = OpenMesh::color_cast<ACG::Vec3f>(_state.overlay_color());

      // allow wireframe + solid mode
      ro.depthFunc = GL_LEQUAL;
      ro.priority = -1; // render before polygon

      ro.setupLineRendering(_state.line_width(), Vec2f((float)_state.viewport_width(), (float)_state.viewport_height()));

      applyRenderObjectSettings(props->primitive(), &ro);
      drawMesh_->addLineRenderObjects(_renderer, &ro);
    }

    if (props->primitive()  == DrawModes::PRIMITIVE_HIDDENLINE)
    {
      ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;
      drawMesh_->disableColors();

      // use specular color for lines
      if (_drawMode.isAtomic() )
        ro.emissive = ro.specular;
      else
        ro.emissive = OpenMesh::color_cast<ACG::Vec3f>(_state.overlay_color());

      // eventually prepare depthbuffer first
      int polyLayer = _drawMode.getLayerIndexByPrimitive(DrawModes::PRIMITIVE_POLYGON);
      if ( (polyLayer > int(i) || polyLayer < 0) && ( mesh_.n_faces() != 0 ))
      {
        ro.priority = 0;

        applyRenderObjectSettings(DrawModes::PRIMITIVE_POLYGON, &ro);

        // disable color write
        ro.glColorMask(0,0,0,0);

        ro.debugName = "MeshNode.HiddenLine.faces";
        add_face_RenderObjects(_renderer, &ro);
      }


      // draw lines after depth image
      ro.priority = 1;
      ro.glColorMask(1,1,1,1);
      ro.depthFunc = GL_LEQUAL;

      ro.setupLineRendering(_state.line_width(), Vec2f((float)_state.viewport_width(), (float)_state.viewport_height()));

      applyRenderObjectSettings(DrawModes::PRIMITIVE_HIDDENLINE, &ro);

      ro.debugName = "MeshNode.HiddenLine.lines";
      drawMesh_->addLineRenderObjects(_renderer, &ro);
    }

    if (props->colored() && props->primitive()  == DrawModes::PRIMITIVE_EDGE)
    {
      ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;
      ro.shaderDesc.vertexColors = true;

      // use specular color for lines
      ro.emissive = ro.specular;

      // line thickness
      ro.setupLineRendering(_state.line_width(), Vec2f((float)_state.viewport_width(), (float)_state.viewport_height()));

      applyRenderObjectSettings(props->primitive(), &ro);
      ro.debugName = "MeshNode.Edges";
      drawMesh_->addLineRenderObjects(_renderer, &ro);

       // skip other edge primitives for this drawmode layer
      continue;
    }  

    if (props->primitive()  == DrawModes::PRIMITIVE_HALFEDGE)
    {
      ro.shaderDesc.shadeMode = SG_SHADE_UNLIT;

      // buffers in system memory
      drawMesh_->updateEdgeHalfedgeVertexDeclarations();
      halfedgeDecl.clear();
      halfedgeDecl.addElement(GL_FLOAT, 3, VERTEX_USAGE_POSITION, (void *)0); 

      ro.vertexDecl = &halfedgeDecl;
      // use specular color for lines
      ro.emissive = ro.specular;
      ro.vertexBuffer = drawMesh_->getHEVBO();
      ro.indexBuffer = 0;
      ro.glDrawArrays(GL_LINES, 0, int(mesh_.n_halfedges() * 2));

      ro.debugName = "MeshNode.HalfEdges";
      _renderer->addRenderObject(&ro);
    }  


    // -----------------------------------------------------
    // take care of all the other primitives

    ro.depthRange = Vec2f(0.01f, 1.0f);

    switch (props->primitive())
    {
    case DrawModes::PRIMITIVE_POINT:
      {
        if (ro.shaderDesc.shadeMode == SG_SHADE_UNLIT)
        {
          // use specular color for points
          if (_drawMode.isAtomic() )
            ro.emissive = ro.specular;
          else
            ro.emissive = OpenMesh::color_cast<ACG::Vec3f>(_state.overlay_color());
        }

        // use shaders to simulate point size
        ro.setupPointRendering(_mat->pointSize(), Vec2f((float)_state.viewport_width(), (float)_state.viewport_height()));

        applyRenderObjectSettings(props->primitive(), &ro);
        ro.debugName = "MeshNode.Points";
        add_point_RenderObjects(_renderer, &ro);
      } break;
    case DrawModes::PRIMITIVE_EDGE:
      {
        // use specular color for lines
        ro.emissive = ro.specular;

        // use shaders to simulate line width
        ro.setupLineRendering(_state.line_width(), Vec2f((float)_state.viewport_width(), (float)_state.viewport_height()));

        applyRenderObjectSettings(props->primitive(), &ro);
        ro.debugName = "MeshNode.Edges";
        drawMesh_->addLineRenderObjects(_renderer, &ro);
      } break;
    case DrawModes::PRIMITIVE_POLYGON: 
      {
        applyRenderObjectSettings(props->primitive(), &ro);

        if (!ro.shaderDesc.vertexTemplateFile.isEmpty())
          drawMesh_->scanVertexShaderForInput(ro.shaderDesc.vertexTemplateFile.toStdString());

        bool useNonIndexed = (props->colorSource() == DrawModes::COLOR_PER_FACE) && (props->lightStage() == DrawModes::LIGHTSTAGE_SMOOTH) && !props->flatShaded();
        if (!useNonIndexed && props->colorSource() == DrawModes::COLOR_PER_FACE)
          ro.shaderDesc.vertexColorsInterpolator = "flat";

        ro.debugName = "MeshNode.Faces";
        add_face_RenderObjects(_renderer, &ro, useNonIndexed);

        ro.shaderDesc.vertexColorsInterpolator.clear();
      } break;
    default: break;
    }
  }

}


template<class Mesh>
void
MeshNodeT<Mesh>::
add_point_RenderObjects(IRenderer* _renderer, const RenderObject* _baseObj) {
  drawMesh_->addPointRenderObjects(_renderer, _baseObj);
}

template<class Mesh>
void
MeshNodeT<Mesh>::
draw_vertices() {
  drawMesh_->drawVertices();
}

template<class Mesh>
void
MeshNodeT<Mesh>::
draw_lines() {

  if ((enabled_arrays_ & PER_EDGE_COLOR_ARRAY) && (enabled_arrays_ & PER_EDGE_VERTEX_ARRAY))
  {
    // colored edges still slow
    glDrawArrays(GL_LINES, 0, int(mesh_.n_edges() * 2));
  }
  else
    drawMesh_->drawLines();
}

template<class Mesh>
void
MeshNodeT<Mesh>::
draw_halfedges() {
  // If we are rendering per edge per vertex attributes, we need to use a seperated vertex buffer!
  if ( enabled_arrays_ & PER_HALFEDGE_VERTEX_ARRAY )
    glDrawArrays(GL_LINES, 0, int(mesh_.n_halfedges() * 2));
  // Something went wrong here!
  else
    std::cerr << "Unable to Draw! halfedge array configuration is invalid!!" << std::endl;
}

template<class Mesh>
void
MeshNodeT<Mesh>::
draw_faces() {
  drawMesh_->draw(textureMap_);
}

template<class Mesh>
void
MeshNodeT<Mesh>::
add_face_RenderObjects(IRenderer* _renderer, const RenderObject* _baseObj, bool _nonindexed) {
  drawMesh_->addTriRenderObjects(_renderer, _baseObj, textureMap_, _nonindexed);
}

template<class Mesh>
void
MeshNodeT<Mesh>::
enable_arrays(unsigned int _arrays) {

  // Unbind everything to ensure sane settings
  ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
  ACG::GLState::bindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

  //===================================================================
  // per Edge Vertex Array
  //===================================================================  
  
  // Check if we should enable the per face vertex array
  if (_arrays & PER_EDGE_VERTEX_ARRAY)  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & PER_EDGE_VERTEX_ARRAY)) {
      enabled_arrays_ |= PER_EDGE_VERTEX_ARRAY;
      
      // For this version we load the colors directly not from vbo
      ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
      ACG::GLState::vertexPointer( drawMesh_->perEdgeVertexBuffer() );   
      
      ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
      
    }
  } else if (enabled_arrays_ & PER_EDGE_VERTEX_ARRAY) {
    // Disable Vertex array
    enabled_arrays_ &= ~PER_EDGE_VERTEX_ARRAY;
    ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
  } 
  
  //===================================================================
  // per Edge Color Array
  //===================================================================  
  
  // Check if we should enable the per face vertex array
  if ( mesh_.has_edge_colors()  && ( _arrays & PER_EDGE_COLOR_ARRAY) )  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & PER_EDGE_COLOR_ARRAY)) {
      enabled_arrays_ |= PER_EDGE_COLOR_ARRAY;
      
      // For this version we load the colors directly not from vbo
      ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
      ACG::GLState::colorPointer( drawMesh_->perEdgeColorBuffer() );
      
      ACG::GLState::enableClientState(GL_COLOR_ARRAY);
      
    }
  } else if (enabled_arrays_ & PER_EDGE_COLOR_ARRAY) {
    // Disable Vertex array
    enabled_arrays_ &= ~PER_EDGE_COLOR_ARRAY;
    ACG::GLState::disableClientState(GL_COLOR_ARRAY);
  }   


  //===================================================================
  // per Halfedge Vertex Array
  //===================================================================  
  
  // Check if we should enable the per face vertex array
  if (_arrays & PER_HALFEDGE_VERTEX_ARRAY)  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & PER_HALFEDGE_VERTEX_ARRAY)) {
      enabled_arrays_ |= PER_HALFEDGE_VERTEX_ARRAY;
      
      // For this version we load the colors directly not from vbo
      ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
      ACG::GLState::vertexPointer( drawMesh_->perHalfedgeVertexBuffer() );   
      
      ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
      
    }
  } else if (enabled_arrays_ & PER_HALFEDGE_VERTEX_ARRAY) {
    // Disable Vertex array
    enabled_arrays_ &= ~PER_HALFEDGE_VERTEX_ARRAY;
    ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
  } 
  
  //===================================================================
  // per Halfedge Color Array
  //===================================================================  
  
  // Check if we should enable the per face vertex array
  if ( mesh_.has_halfedge_colors()  && ( _arrays & PER_HALFEDGE_COLOR_ARRAY) )  {
    
    // Check if its already enabled
    if (!(enabled_arrays_ & PER_HALFEDGE_COLOR_ARRAY)) {
      enabled_arrays_ |= PER_HALFEDGE_COLOR_ARRAY;
      
      // For this version we load the colors directly not from vbo
      ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
      ACG::GLState::colorPointer( drawMesh_->perHalfedgeColorBuffer() );
      
      ACG::GLState::enableClientState(GL_COLOR_ARRAY);
      
    }
  } else if (enabled_arrays_ & PER_HALFEDGE_COLOR_ARRAY) {
    // Disable Vertex array
    enabled_arrays_ &= ~PER_HALFEDGE_COLOR_ARRAY;
    ACG::GLState::disableClientState(GL_COLOR_ARRAY);
  }   

  //===================================================================
  // Check for OpenGL Errors
  //===================================================================    
  glCheckErrors();
}

template<class Mesh>
void
MeshNodeT<Mesh>::
pick(GLState& _state, PickTarget _target) {

  switch (_target)
  {
    case PICK_VERTEX:
    {
      pick_vertices(_state);
      break;
    }
    case PICK_FRONT_VERTEX:
    {
      pick_vertices(_state, true);
      break;
    }
    
    case PICK_ANYTHING:
    {
      pick_any(_state);
      break;
    }
    case PICK_FACE:
    {
      pick_faces(_state);
      break;
    }
    
    case PICK_EDGE:
    {
      pick_edges(_state);
      break;
    }
    
    case PICK_FRONT_EDGE:
    {
      pick_edges(_state, true);
      break;
    }
    
    default:
      break;
  }

}

template<class Mesh>
void
MeshNodeT<Mesh>::
pick_vertices(GLState& _state, bool _front)
{  
  GLenum prev_depth = _state.depthFunc();
  
  if (!_state.pick_set_maximum (static_cast<unsigned int>(mesh_.n_vertices()))) {
    omerr() << "MeshNode::pick_vertices: color range too small, " << "picking failed\n";
    return;
  }
  
  if ( mesh_.n_vertices() == 0 ) {
    std::cerr << "pick_vertices: No vertices in Mesh!" << std::endl;
    return;
  }
  
  if (_front && ( mesh_.n_faces() != 0 ) ) {
    
    Vec4f  clear_color = _state.clear_color();
    Vec4f  base_color  = _state.base_color();
    clear_color[3] = 1.0;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor(clear_color);
    
    ACG::GLState::depthRange(0.01, 1.0);
    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ACG::GLState::depthFunc(GL_LEQUAL);
    glColor(base_color);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }

  // picking implementations:
  //  0 -> render mesh with picking color buffer (compatibility mode, add. mem alloc: 16 bytes per openmesh vertex)
  //  1 -> render mesh with picking shader (optimized, add. mem alloc: none for point-clouds, otherwise 4 bytes per openmesh vertex on gpu)
  int pickImplementationMethod = 0;

  // use optimized picking if supported (the function actually checks whether the shader was linked and bound)
  // which shader should be used only depends on the available OpenGL version
  if (drawMesh_->supportsPickingVertices_opt())
    pickImplementationMethod = openGLVersionTest(3,2) ? 1 : 0;

  if (_state.color_picking () ) {
    
    if ( updateVertexPicking_ || _state.pick_current_index () != vertexPickingBaseIndex_) {
      if (pickImplementationMethod == 0)
        drawMesh_->updatePickingVertices(_state);
      else
        drawMesh_->updatePickingVertices_opt(_state);
      vertexPickingBaseIndex_ = _state.pick_current_index ();
      updateVertexPicking_    = false;
    }

    if (mesh_.n_vertices()) {
      
      if (pickImplementationMethod == 0) {
        // For this version we load the colors directly not from vbo
        ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
        ACG::GLState::colorPointer( drawMesh_->pickVertexColorBuffer() );   
        ACG::GLState::enableClientState(GL_COLOR_ARRAY);    

        // vertex positions
        ACG::GLState::vertexPointer( drawMesh_->pickVertexBuffer() );
        ACG::GLState::enableClientState(GL_VERTEX_ARRAY);

        // Draw color picking
        glDrawArrays(GL_POINTS, 0, int(mesh_.n_vertices()));

        // Disable color array
        ACG::GLState::disableClientState(GL_COLOR_ARRAY);

        // disable vertex array
        ACG::GLState::disableClientState(GL_VERTEX_ARRAY);
      }
      else if (pickImplementationMethod == 1){

        // optimized rendering of picking ids with shaders
        drawMesh_->drawPickingVertices_opt(_state.projection() * _state.modelview(), vertexPickingBaseIndex_);

      }
      
      
    } else {
     std::cerr << "pick_vertices: No vertices in Mesh!" << std::endl; 
    }
    
  } else {
    std::cerr << "No fallback pick_vertices!" << std::endl;
  }
  
  ACG::GLState::depthFunc(prev_depth);
  
}

template<class Mesh>
void
MeshNodeT<Mesh>::
pick_edges(GLState& _state, bool _front)
{  
  GLenum prev_depth = _state.depthFunc();
  
  if (!_state.pick_set_maximum (static_cast<unsigned int>(mesh_.n_edges()))) {
    omerr() << "MeshNode::pick_edges: color range too small, " << "picking failed\n";
    return;
  }
  
  if ( mesh_.n_vertices() == 0 ) {
    std::cerr << "pick_edges: No vertices in Mesh!" << std::endl;
    return;
  }
  
  if ( _front && ( mesh_.n_faces() != 0 ) ) {
    
    Vec4f  clear_color = _state.clear_color();
    Vec4f  base_color  = _state.base_color();
    clear_color[3] = 1.0;
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor(clear_color);
    
    ACG::GLState::depthRange(0.01, 1.0);
    draw_faces();
    ACG::GLState::depthRange(0.0, 1.0);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ACG::GLState::depthFunc(GL_LEQUAL);
    glColor(base_color);
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // disable all other arrays
    enable_arrays(0);
  }
  
  if (_state.color_picking () && drawMesh_ ) {

    // picking implementations:
    //  0 -> render mesh with picking color buffer (compatibility mode, add. mem alloc: 32 bytes per openmesh edge)
    //  1 -> render mesh with picking shader (add. mem alloc: none)
    int pickImplementationMethod = 0;

    // use optimized picking if supported
    if (drawMesh_->supportsPickingEdges_opt())
      pickImplementationMethod = openGLVersionTest(3,2) ? 1 : 0;
    
    if ( updateEdgePicking_ || _state.pick_current_index () != edgePickingBaseIndex_) {
      if (pickImplementationMethod == 0)
        drawMesh_->updatePickingEdges(_state);
      else
        drawMesh_->updatePickingEdges_opt(_state);
      edgePickingBaseIndex_ = _state.pick_current_index ();
      updateEdgePicking_    = false;
    }
    
    if ( mesh_.n_edges() != 0 && drawMesh_) {
      
      if (pickImplementationMethod == 0){
        // For this version we load the colors directly not from vbo
        ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);

        ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
        ACG::GLState::enableClientState(GL_COLOR_ARRAY);

        ACG::GLState::vertexPointer(drawMesh_->perEdgeVertexBuffer());
        ACG::GLState::colorPointer(drawMesh_->pickEdgeColorBuffer());

        glDrawArrays(GL_LINES, 0, int(mesh_.n_edges() * 2));

        ACG::GLState::disableClientState(GL_COLOR_ARRAY);
        ACG::GLState::disableClientState(GL_VERTEX_ARRAY);

        // disable all other arrays
        enable_arrays(0); 
      }
      else if (pickImplementationMethod == 1){
        // optimized rendering of edge ids
        drawMesh_->drawPickingEdges_opt(_state.projection() * _state.modelview(), edgePickingBaseIndex_);
      }

      
    }
    
  } else {
    std::cerr << "No fallback pick_edges!" << std::endl;
  }
  
  ACG::GLState::depthFunc(prev_depth);
  
}

template<class Mesh>
void
MeshNodeT<Mesh>::
pick_faces(GLState& _state)
{
  
  if ( mesh_.n_vertices() == 0 ) {
    std::cerr << "pick_faces: No vertices in Mesh!" << std::endl;
    return;
  }
  
  if ( mesh_.n_faces() > 0 ) {
    if (!_state.pick_set_maximum (static_cast<unsigned int>(mesh_.n_faces()))) {
      omerr() << "MeshNode::pick_faces: color range too small, " << "picking failed\n";
      return;
    }
  } else {
    if (!_state.pick_set_maximum (1)) {
      omerr() << "Strange pickSetMAximum failed for index 1 in MeshNode\n";
      return;
    }
  }

  // picking implementations:
  //  0 -> render mesh with picking color buffer (compatibility mode, add. mem alloc: 48 bytes per triangle)
  //  1 -> render mesh with picking shader (optimized, add. mem alloc: 4 bytes per triangle on gpu)
  int pickImplementationMethod = 0;

  
  // use optimized picking if supported
  if (drawMesh_->supportsPickingFaces_opt())
    pickImplementationMethod = openGLVersionTest(3,2) ? 1 : 0;

//  pickImplementationMethod = 0;

  if (_state.color_picking ()) {

    if (pickImplementationMethod == 0) {
      // compatibility mode (unoptimized)

      if ( updateFacePicking_ || _state.pick_current_index () != facePickingBaseIndex_) {
        drawMesh_->updatePickingFaces(_state);
        facePickingBaseIndex_ = _state.pick_current_index ();
        updateFacePicking_    = false;
      }

      if ( mesh_.n_faces() != 0 ) {

        // For this version we load the colors directly not from vbo
        ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);

        ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
        ACG::GLState::enableClientState(GL_COLOR_ARRAY);

        ACG::GLState::vertexPointer(drawMesh_->pickFaceVertexBuffer());
        ACG::GLState::colorPointer(drawMesh_->pickFaceColorBuffer());

        glDrawArrays(GL_TRIANGLES, 0, int(3 * drawMesh_->getNumTris()));

        ACG::GLState::disableClientState(GL_COLOR_ARRAY);
        ACG::GLState::disableClientState(GL_VERTEX_ARRAY);

        // disable all other arrays
        enable_arrays(0);

      }

    }
    else if (pickImplementationMethod == 1) {
      
      //  render mesh with face picking shader (optimized)

      if ( updateFacePicking_ || _state.pick_current_index () != facePickingBaseIndex_) {
        drawMesh_->updatePickingFaces_opt(_state);
        facePickingBaseIndex_ = _state.pick_current_index ();
        updateFacePicking_    = false;
      }

      if ( mesh_.n_faces() != 0 ) {

        drawMesh_->drawPickingFaces_opt(_state.projection() * _state.modelview(), facePickingBaseIndex_);

        /* debug:  print color id of first face
        Vec4uc facePickingOffsetColor = _state.pick_get_name_color(0);
        
        std::cout << "base_color: " << int(facePickingOffsetColor[0]) << " " <<
          int(facePickingOffsetColor[1]) << " " <<
          int(facePickingOffsetColor[2]) << " " <<
          int(facePickingOffsetColor[3]) << " " << std::endl;
                Vec4uc facePickingOffsetColor = _state.pick_get_name_color(0);
        */
      }
    }
    else
      std::cerr << "Unknown picking method in pick_faces!" << std::endl;
    
    
  } else
    std::cerr << "No fallback pick_faces!" << std::endl;
  
}

template<class Mesh>
void
MeshNodeT<Mesh>::
pick_any(GLState& _state)
{    
  GLenum prev_depth = _state.depthFunc();
  size_t numElements = mesh_.n_faces() + mesh_.n_edges() + mesh_.n_vertices();
  
  if ( mesh_.n_vertices() == 0 ) {
    std::cerr << "pick_any: No vertices in Mesh!" << std::endl;
    return;
  }
  
  // nothing to pick ?
  if (numElements == 0) {
    std::cerr << "pick_any: Number of elements : 0 " << std::endl;
    return;
  }
  
  if (!_state.pick_set_maximum (static_cast<unsigned int>(numElements)))
  {
    omerr() << "MeshNode::pick_any: color range too small, " << "picking failed\n";
    return;
  }

  if (_state.color_picking() && drawMesh_) {

    // picking implementations:
    //  0 -> render mesh with picking color buffer (compatibility mode, add. mem alloc: 48 bytes per triangle + 32 bytes per edge + 16 bytes per vertex)
    //  1 -> render mesh with picking shader (optimized, add. mem alloc: none [ shared memory with optimized vertex,edge,face picking ])
    int pickImplementationMethod = 0;

    // use optimized picking if supported
    if (drawMesh_->supportsPickingAny_opt())
      pickImplementationMethod = openGLVersionTest(3,2) ? 1 : 0;


    if ( updateAnyPicking_ || _state.pick_current_index () != anyPickingBaseIndex_) {
      if (pickImplementationMethod == 0)
        drawMesh_->updatePickingAny(_state);
      else
        drawMesh_->updatePickingAny_opt(_state);
      anyPickingBaseIndex_ = _state.pick_current_index ();
      updateAnyPicking_    = false;
    }

    if (pickImplementationMethod == 0){
      // For this version we load the colors directly, not from vbo
      ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);

      ACG::GLState::disableClientState(GL_NORMAL_ARRAY);
      ACG::GLState::disableClientState(GL_TEXTURE_COORD_ARRAY);

      ACG::GLState::enableClientState(GL_VERTEX_ARRAY);
      ACG::GLState::enableClientState(GL_COLOR_ARRAY);

      // If we do not have any faces, we generate an empty list here.  
      if ( mesh_.n_faces() != 0 && drawMesh_) {

        ACG::GLState::vertexPointer(drawMesh_->pickFaceVertexBuffer());
        ACG::GLState::colorPointer(drawMesh_->pickAnyFaceColorBuffer());

        glDrawArrays(GL_TRIANGLES, 0, int(3 * drawMesh_->getNumTris()));
      }

      ACG::GLState::depthFunc(GL_LEQUAL);

      // If we do not have any edges, we generate an empty list here.  
      if ( mesh_.n_edges() != 0 && drawMesh_) {

        ACG::GLState::vertexPointer(drawMesh_->perEdgeVertexBuffer());
        ACG::GLState::colorPointer(drawMesh_->pickAnyEdgeColorBuffer());

        glDrawArrays(GL_LINES, 0, int(mesh_.n_edges() * 2));
      }

      // For this version we load the colors directly not from vbo
      ACG::GLState::bindBuffer(GL_ARRAY_BUFFER_ARB, 0);
      ACG::GLState::vertexPointer( drawMesh_->pickVertexBuffer() );
      ACG::GLState::colorPointer(drawMesh_->pickAnyVertexColorBuffer());

      // Draw color picking
      glDrawArrays(GL_POINTS, 0, int(mesh_.n_vertices()));

      ACG::GLState::disableClientState(GL_COLOR_ARRAY);
      ACG::GLState::disableClientState(GL_VERTEX_ARRAY);

      // disable all other arrays
      enable_arrays(0);
    } else {
      // optimized version of any picking with shaders
      drawMesh_->drawPickingAny_opt(_state.projection() * _state.modelview(), anyPickingBaseIndex_);
    }
    
  } else
    std::cerr << "No fallback pick_any!" << std::endl;

  //restore depth buffer comparison function for the active display list
  ACG::GLState::depthFunc(prev_depth);
  
  glCheckErrors();
}

template<class Mesh>
void
MeshNodeT<Mesh>::
update_geometry() {
  /// \todo check the following statements. If only geometry changed, the normals,vertices have to be updated nothing else!
  /*
  updateFaceList_ = true;
  updateAnyList_ = true;
  */
  
  updateVertexPicking_ = true;
  updateEdgePicking_   = true;
  updateFacePicking_   = true;
  updateAnyPicking_    = true;
  
  // Set per edge arrays to invalid as they have to be regenerated
  drawMesh_->invalidatePerEdgeBuffers();

  // Set per halfedge arrays to invalid as they have to be regenerated
  drawMesh_->invalidatePerHalfedgeBuffers();
  
  drawMesh_->updateGeometry();

  drawMesh_->invalidateFullVBO();

  // First of all, we update the bounding box:
  bbMin_ = Vec3d(FLT_MAX,  FLT_MAX,  FLT_MAX);
  bbMax_ = Vec3d(-FLT_MAX, -FLT_MAX, -FLT_MAX);
  typename Mesh::ConstVertexIter  v_it(mesh_.vertices_begin()), v_end(mesh_.vertices_end());
  
  for (; v_it!=v_end; ++v_it)
  {
    bbMin_.minimize(mesh_.point(*v_it));
    bbMax_.maximize(mesh_.point(*v_it));
  }
}

template<class Mesh>
void
MeshNodeT<Mesh>::
update_topology() {

  drawMesh_->invalidatePerEdgeBuffers();
  drawMesh_->invalidatePerHalfedgeBuffers();


  drawMesh_->updateTopology();

  // Unbind the buffer after the work has been done
  ACG::GLState::bindBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}

template<class Mesh>
void
MeshNodeT<Mesh>::
update_textures() {
  drawMesh_->updateTextures();

  updateVertexPicking_ = true;
  updateFacePicking_ = true;
  updateAnyPicking_ = true;
}



template<class Mesh>
void
MeshNodeT<Mesh>::
update_color() {
  
  drawMesh_->invalidatePerEdgeBuffers();
  drawMesh_->invalidatePerHalfedgeBuffers();
  
  // TODO: optimize update strategy:
  // if only vertex colors have changed, then call UpdateGeometry() (faster)
  // for face colors we have to use UpdateFull()
  drawMesh_->updateFull();
}

template<class Mesh>
void
MeshNodeT<Mesh>::
setIndexPropertyName( std::string _indexPropertyName ) { 

  drawMesh_->setTextureIndexPropertyName(_indexPropertyName);
  perFaceTextureIndexAvailable_ =  drawMesh_->perFaceTextureIndexAvailable() != 0;
}

template<class Mesh>
const std::string&
MeshNodeT<Mesh>::
indexPropertyName() const {

  return drawMesh_->getTextureIndexPropertyName();
}

template<class Mesh>
void
MeshNodeT<Mesh>::
setHalfedgeTextcoordPropertyName( std::string _halfedgeTextcoordPropertyName ){ 
  drawMesh_->setPerFaceTextureCoordinatePropertyName(_halfedgeTextcoordPropertyName);
}




template<class Mesh>
unsigned int
MeshNodeT<Mesh>::getMemoryUsage()
{
  unsigned int res = 0;

  if (drawMesh_)
    res += drawMesh_->getMemoryUsage();

  return res;
}


template<class Mesh>
DrawMeshT<Mesh>*
MeshNodeT<Mesh>::getDrawMesh()
{
  return drawMesh_;
}




//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
