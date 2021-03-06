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

#ifndef ACG_FBO_HH
#define ACG_FBO_HH


//== INCLUDES =================================================================

#include <iostream>
#include <cstdlib>
#include <map>
#include <vector>

#include <ACG/Config/ACGDefines.hh>
#include <ACG/GL/gl.hh>

//== FORWARDDECLARATIONS ======================================================

//== NAMESPACES ===============================================================

namespace ACG {

//== CLASS DEFINITION =========================================================


/** \class FBO FBO.hh <ACG/.../FBO.hh>

    Brief Description.
  
    A more elaborate description follows.
*/
class ACGDLLEXPORT FBO
{
public:
  
  // fbo_ braucht initialen Wert.
  /// Default constructor
  FBO();
  
  /// Destructor
  ~FBO();
  
  /// function to generate the framebuffer object
  void init();

  /// delete fbo and all internally created render textures
  void del();

  /// enable/disable multisampling
  /// returns the MSAA sample count of the fbo, which might not be equal to the requested _samples count but you'll get something close
  GLsizei setMultisampling(GLsizei _samples, GLboolean _fixedsamplelocations = GL_TRUE);

  /// get number of samples
  GLsizei getMultisamplingCount() const {return samples_;}

  /// attach a texture of arbitrary dimension (requires OpenGL 3.2)
  void attachTexture( GLenum _attachment,
    GLuint _texture,
    GLuint _level = 0);

  /// function to attach a texture to fbo
  void attachTexture2D( GLenum _attachment,
    GLsizei _width, GLsizei _height, 
    GLuint _internalFmt, GLenum _format, 
    GLint _wrapMode = GL_CLAMP_TO_EDGE,
    GLint _minFilter = GL_NEAREST,
    GLint _magFilter = GL_NEAREST);


  /// function to attach a texture to fbo
  void attachTexture2D( GLenum _attachment, GLuint _texture, GLenum _target = GL_TEXTURE_2D );

  /// function to attach a depth-buffer texture to fbo (using GL_DEPTH_ATTACHMENT)
  void attachTexture2DDepth( GLsizei _width, GLsizei _height, GLuint _internalFmt = GL_DEPTH_COMPONENT32, GLenum _format = GL_DEPTH_COMPONENT );

  /// function to attach a stencil-buffer texture to fbo (texformat = GL_STENCIL_INDEX8)
  void attachTexture2DStencil(GLsizei _width, GLsizei _height);

  /// attach a 3D texture for render to volume  (requires OpenGL 3.2)
  /// binds all slices at once, target slice has to be chosen via gl_Layer in geometry shader
  void attachTexture3D( GLenum _attachment,
    GLsizei _width, GLsizei _height, GLsizei _depth,
    GLuint _internalFmt, GLenum _format,
    GLint _wrapMode = GL_CLAMP,
    GLint _minFilter = GL_NEAREST,
    GLint _magFilter = GL_NEAREST);

  /// function to add a depth renderbuffer to the fbo
  void addDepthBuffer( GLuint _width, GLuint _height );
  
  /// function to add a stencil renderbuffer to the fbo
  void addStencilBuffer( GLuint _width, GLuint _height );

  /// add a packed depth24_stencil8 renderbuffer
  void addDepthStencilBuffer( GLuint _width, GLuint _height );

  /// return attached texture id
  GLuint getAttachment( GLenum _attachment );

  /// return internal texture format of attachment
  GLuint getInternalFormat( GLenum _attachment );

  /// return opengl id
  GLuint getFboID();

  /// resize function (if textures created by this class)
  void resize(GLsizei _width, GLsizei _height, bool _forceResize = false);

  /// get width of fbo texture
  GLsizei width() const {return width_;}

  /// get height of fbo texture
  GLsizei height() const {return height_;}

  /// get width and height of fbo texture
  ACG::Vec2i size() const {return ACG::Vec2i(width_, height_);}
  
  /// bind the fbo and sets it as rendertarget
  bool bind();
  
  /// unbind fbo, go to normal rendering mode 
  void unbind();
  
  /// function to check the framebuffer status
  bool checkFramebufferStatus();

private:

  /// handle of frame buffer object
  GLuint fbo_;

  /// depthbuffer
  GLuint depthbuffer_;
  
  /// stencilbuffer
  GLuint stencilbuffer_;

  /// attached textures
  struct RenderTexture
  {
    RenderTexture();

    // opengl buf id
    GLuint id;

    // GL_TEXTURE_2D, GL_TEXTURE_2D_MULTISAMPLE..
    GLenum target;

    GLenum internalFormat, format, gltype;

    // tex dimension
    ACG::Vec3i dim;

    // common sampler params
    GLint wrapMode,
      minFilter,
      magFilter;

    // created by FBO class
    bool owner;
  };

  typedef std::map<GLenum, RenderTexture> AttachmentList;
  AttachmentList attachments_; // key: attachment index,  value: RenderTexture


  /// width and height of render textures
  GLsizei width_, height_;

  /// sample count if multisampling
  GLsizei samples_;

  /// enable fixed sample location if multisampling
  GLboolean fixedsamplelocation_;


  /// handle of previously bound fbo
  GLuint prevFbo_;
  GLuint prevDrawBuffer_;
};


//=============================================================================
} // namespace ACG
//=============================================================================
#endif // ACG_FBO_HH defined
//=============================================================================

