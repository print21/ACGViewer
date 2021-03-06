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
//  CLASS BaseNode
//
//=============================================================================


#ifndef ACG_BASE_NODE_HH
#define ACG_BASE_NODE_HH


//== INCLUDES =================================================================

// ACG
#include "../Math/VectorT.hh"
#include "../GL/GLState.hh"
#include "../Config/ACGDefines.hh"
#include "PickTarget.hh"

// Qt
//#include <qgl.h>
#include <QMouseEvent>

// stdc++
#include <vector>
#include <string>
#include <ACG/Scenegraph/DrawModes.hh>
#include <ACG/GL/RenderObject.hh>

//== NAMESPACES ===============================================================


namespace ACG {

class IRenderer;

namespace SceneGraph {

// prototype declaration to avoid include-loop
class Material;


//== CLASS DEFINITION =========================================================


/// Convenience macro that sets up the BaseNode::className() function
#define ACG_CLASSNAME(_className) \
 virtual const std::string& className() const override { \
  static std::string cname( #_className ); return cname; \
}


/** \class BaseNode BaseNode.hh <ACG/Scenegraph/BaseNode.hh>

    This is the base for all scenegraph nodes. All virtual functions
    should be reimplemented when inheriting from this class.  
**/

class ACGDLLEXPORT BaseNode 
{
public:


  /// Default constructor
  BaseNode(BaseNode* _parent=0, std::string _name="<unknown>");

  /// Put this node between _parent and _child
  BaseNode(BaseNode* _parent, BaseNode* _child, std::string _name="<unknown>");
 
  /// Destructor.
  virtual ~BaseNode();

  /** \brief Delete the whole subtree of this node
  *
  * This function will remove the whole subtree below this node.
  * All children in this nodes bubtree will be automatically removed from the tree
  * and their destructor is called.
  * The node itself will be removed from the list of its parents children.
  * Afterwards it will also call its own destructor.
  **/
  void delete_subtree();


  // --- basic interface ---

  /// Return class name (implemented by the ACG_CLASSNAME macro)
  virtual const std::string& className() const = 0;

  /** Return a list of available draw modes for this node: should be OR'ed
      from the items of the enum DrawModeIDs. */
  virtual DrawModes::DrawMode availableDrawModes() const { return DrawModes::NONE; }

  /** Compute the bounding box of this node and update the values
      _bbMin and _bbMax accordingly. Do not initialize _bbMin and
      _bbMax since they may already store values of previous nodes' 
      bounding box computation.
  */
  virtual void boundingBox(Vec3d& /* _bbMin */, Vec3d& /*_bbMax*/ ) {}

  /** This function is called when traversing the scene graph and
      arriving at this node. It can be used to store GL states that
      will be changed in order to restore then in the leave()
      function.

      There are two overloads of the enter() function, one with IRenderer*
      parameter, the other one without. As long the overload with IRenderer*
      parameter is not being overridden, all renderers will make use of the
      overload without the IRenderer* parameter. If the overload with IRenderer*
      parameter is being overridden, only legacy renderers will use the version
      without the additional parameter and new renderers implementing the
      IRenderer interface will use the version with the additional parameter.

      \see MaterialNode 
  */
  virtual void enter(GLState& /*_state */, const DrawModes::DrawMode& /*_drawMode*/ ) {}

  /** This function is called when traversing the scene graph and
      arriving at this node. It can be used to store GL states that
      will be changed in order to restore then in the leave()
      function.

      If you do not need the IRenderer* argument in your override of this
      function, simply use the override without the additional parameter.

      There are two overloads of the enter() function, one with IRenderer*
      parameter, the other one without. As long the overload with IRenderer*
      parameter is not being overridden, all renderers will make use of the
      overload without the IRenderer* parameter. If the overload with IRenderer*
      parameter is being overridden, only legacy renderers will use the version
      without the additional parameter and new renderers implementing the
      IRenderer interface will use the version with the additional parameter.
   */
  virtual void enter(IRenderer* /*_renderer*/, GLState& _state, const DrawModes::DrawMode& _drawMode) {
      enter(_state, _drawMode);
  }

  /** \brief Draw this node using the draw modes _drawMode
   *
   * This function is called when the scenegraph is traversed by the classical draw routines.
   *
   * In this call the node should draw it's content via OpenGL.
   *
   * \note Keep in mind, that you should use
   *       the currently active state and don't change it (at least reset it the original values you got).
   *       Otherwise nodes that are drawn after this node might get an inconsistent state.
   */
  virtual void draw(GLState& /* _state */, const DrawModes::DrawMode& /* _drawMode */)  {}

  /** \brief Deferred draw call with shader based renderer.
   *
   * The renderer calls this function to collect the geometry that should be rendered from the nodes.
   *
   * Add any renderable geometry to the renderer via _renderer->addRenderObject()
   *
   * The material is fetched from the last active material node and may be used to setup Renderobjects,
   * but may as well be ignored.
   *
   * \note You should not draw anything yourself in this function.
   *
   * @param _renderer The renderer which will be used. Add your geometry into this class
   * @param _state    The current GL State when this object is called
   * @param _drawMode The active draw mode
   * @param _mat      Current material
   */
  virtual void getRenderObjects(IRenderer* _renderer, GLState&  _state , const DrawModes::DrawMode&  _drawMode , const Material* _mat)  {}

  /** The leave function is used to restore GL states the have been changed.
      This function must restore the status before enter()!

      There are two overloads of the leave() function, one with IRenderer*
      parameter, the other one without. As long the overload with IRenderer*
      parameter is not being overridden, all renderers will make use of the
      overload without the IRenderer* parameter. If the overload with IRenderer*
      parameter is being overridden, only legacy renderers will use the version
      without the additional parameter and new renderers implementing the
      IRenderer interface will use the version with the additional parameter.
  */
  virtual void leave(GLState& /* _state */, const DrawModes::DrawMode& /* _drawMode */) {}

  /** The leave function is used to restore GL states the have been changed.
      This function must restore the status before enter()!

      If you do not need the IRenderer* argument in your override of this
      function, simply use the override without the additional parameter.

      There are two overloads of the leave() function, one with IRenderer*
      parameter, the other one without. As long the overload with IRenderer*
      parameter is not being overridden, all renderers will make use of the
      overload without the IRenderer* parameter. If the overload with IRenderer*
      parameter is being overridden, only legacy renderers will use the version
      without the additional parameter and new renderers implementing the
      IRenderer interface will use the version with the additional parameter.
   */
  virtual void leave(IRenderer* /*_renderer*/, GLState& _state, const DrawModes::DrawMode& _drawMode) {
      leave(_state, _drawMode);
  }

  /** This function is called when traversing the scene graph during picking
      and arriving at this node. It can be used to store GL states that
      will be changed in order to restore then in the leavePick()
      function. Its default implementation will call the enter() function.
  */
  virtual void enterPick(GLState& _state , PickTarget _target, const DrawModes::DrawMode& _drawMode );
  
  /** Draw the node using the GL picking name stack. The node's ID
      will already be on the name stack, so only names identifying e.g. faces
      should be used ( by pick_set_name() ).
  */
  virtual void pick(GLState& /* _state */, PickTarget /* _target */ ) {}

  /** The leavePick function is used to restore GL states the have been changed.
      This function must restore the status before enterPick() !
      Its default implementation will call the leave() function.
  */
  virtual void leavePick(GLState& _state, PickTarget _target, const DrawModes::DrawMode& _drawMode );
  
  /** Enable or Disable picking for this node
   *  ( default: enabled )
   */
  void enablePicking(bool _enable) { pickingEnabled_ = _enable; };
  
  /** Check if picking is enabled for this node 
   */
  bool pickingEnabled() { return pickingEnabled_; };
  
  /// Handle mouse event (some interaction, e.g. modeling)
  virtual void mouseEvent(GLState& /* _state */, QMouseEvent* /* _event */ ) {}

  /// mark node for redrawn
  void setDirty (bool _dirty = true) { dirty_ = _dirty; }

  /// Check if node should be redrawn
  bool isDirty () const { return dirty_; }


  // --- iterators ---

  /// allows to iterate over children
  typedef std::vector<BaseNode*>::const_iterator ConstChildIter;
  /// allows to iterate over children
  typedef std::vector<BaseNode*>::iterator ChildIter;

  /// allows to reverse iterate over children
  typedef std::vector<BaseNode*>::const_reverse_iterator ConstChildRIter;
  /// allows to reverse iterate over children
  typedef std::vector<BaseNode*>::reverse_iterator ChildRIter;

  /// Returns: begin-iterator of children
  ChildIter childrenBegin() { return children_.begin(); }
  /// Same but \c cont
  ConstChildIter childrenBegin() const { return children_.begin(); }
  /// Returns: end-iterator of children
  ChildIter childrenEnd() { return children_.end(); }
  /// Same but \c const
  ConstChildIter childrenEnd() const { return children_.end(); }
  
  /// Returns: reverse begin-iterator of children
  ChildRIter childrenRBegin() { return children_.rbegin(); }
  /// Same but const
  ConstChildRIter childrenRBegin() const { return children_.rbegin(); }
  /// Returns: reverse end-iterator of children
  ChildRIter childrenREnd() { return children_.rend(); }
  /// Same but \c const
  ConstChildRIter childrenREnd() const { return children_.rend(); }




  
  // --- insert / remove ---

  /// Insert _node at the end of the list of children.
  void push_back(BaseNode* _node) 
  {
    if (_node)
    {
      children_.push_back(_node);
      _node->parent_=this;
    }
  }

  /** Remove child node at position _pos.
      This _pos \a must \a be \a reachable from childrenBegin().<br>
      This method has no effect if called with childrenEnd() as parameter.  */
  void remove(ChildIter _pos) 
  {
    if (_pos == childrenEnd()) return;
    //(*_pos)->parent_=0;
    children_.erase(_pos); 
  }

  /// number of children
  size_t nChildren() const { return children_.size(); }

  /** Find a specific node in the list of children.<br>
      This method is designed to convert a node pointer to an iterator
      that may be used e.g. for insert()'ing a new node at a distinct
      position.<br>
      Returns childrenEnd() if no appropriate node is found.
   */
  ChildIter find(BaseNode* _node) 
  {
    ChildIter i=std::find(children_.begin(),children_.end(),_node);
    return i;
  } 


  /** Find a node of a given name */
  
  BaseNode * find( const std::string & _name )
  {
    if ( name() == _name )
      return this;

    for ( BaseNode::ChildIter cIt = childrenBegin();
	  cIt != childrenEnd(); ++cIt )
    {
      BaseNode * n = (*cIt)->find( _name );
      if ( n ) return n;
    }

    return 0;
  } 
  

  /// Get the nodes parent node
  BaseNode* parent() { return parent_; }

  /// Get the nodes parent node
  const BaseNode* parent() const { return parent_; }
  
  /** \brief Set the parent of this node.
  *
  * This function will remove this node from its original parents children, if the parent exists.
  * And will add it to the new parents children. 
  */
  void set_parent(BaseNode* _parent);


  // --- status info ---

  
  /// Status modi
  enum StatusMode
  {
    /// Draw node & children
    Active = 0x1,
    /// Hide this node, but draw children
    HideNode  = 0x2,
    /// Draw this node, but hide children
    HideChildren = 0x4,
    /// Hide this node and its children
    HideSubtree  = 0x8
  };
  /// Get node's status
  StatusMode status() const { return status_; }
  /// Set the status of this node.
  void set_status(StatusMode _s) { status_ = _s; }
  /// Hide Node: set status to HideNode
  void hide() { set_status(HideNode); }
  /// Show node: set status to Active
  void show() { set_status(Active); }
  /// Is node visible (status == Active)?
  bool visible() { return status_ == Active; }
  /// Is node not visible (status != Active)?
  bool hidden() { return status_ != Active; }


  /// Returns: name of node (needs not be unique)
  std::string name() const { return name_; }
  /// rename a node 
  void name(const std::string& _name) { name_ = _name; }

  
  /** Get unique ID of node. IDs are always positive and may be used
      e.g. for picking.
    */
  unsigned int id() const { return id_; }
  


  //--- draw mode ---

  /// Return the own draw modes of this node
  DrawModes::DrawMode drawMode() const { return drawMode_; }
  /** Set this node's own draw mode. It will be used for drawing instead of 
      the the global draw mode. */
  void drawMode(DrawModes::DrawMode _drawMode) { drawMode_ = _drawMode; }

  //--- traverse type ---

  /// Node traverse types
  enum TraverseMode
  {
    /// Execute action on node first and then on its children
    NodeFirst = 0x1,
    /// Execute action the children first and then on this node
    ChildrenFirst = 0x2,
    /// Draw node in second pass
    SecondPass = 0x4
  };

  /// Return how the node should be traversed
  unsigned int traverseMode () const { return traverseMode_; }

  /// Set traverse mode for node
  void setTraverseMode(unsigned int _mode) { traverseMode_ = _mode; }

  //===========================================================================
  /** @name Render pass controls
  *  The render pass controls are only used during multipass traversal. There
  *  are two types of multipass controls. One type controls if the enter and
  *  leave functions of the nodes are used (RenderStatusPass) or if the actual 
  *  draw function is called (RenderDrawPass). The bitmasks define if the functions
  *  are called by the traverse_multipass operation. The bitmask are initialized
  *  to run in the first path.\n
  *
  * @{ */
  //===========================================================================

public:
  
  /// Multipass pass bit mask type
  typedef unsigned int MultipassBitMask;
  

  /// This enum should be used to enable rendering of a node in different
  enum PASSES {
    NOPASS = 0,
    ALLPASSES = 1 << 0,
    PASS_1    = 1 << 1,
    PASS_2    = 1 << 2,
    PASS_3    = 1 << 3,
    PASS_4    = 1 << 4,
    PASS_5    = 1 << 5,
    PASS_6    = 1 << 6,
    PASS_7    = 1 << 7,
    PASS_8    = 1 << 8
  };
  
  /** \brief Get the current multipass settings for the nodes status functions
  *
  * Get a bitmask defining in which traverse pass the enter and leave nodes are used. Use
  * the PASSES enum above to control multipass rendering!
  *
  * @return Bitmask defining in which traverse pass the enter and leave nodes are used 
  */
  MultipassBitMask multipassStatus() const {return multipassStatus_;};
  
  
  /** \brief Set multipass settings for the nodes status functions
  *
  * Set a bitmask defining in which traverse pass the enter and leave nodes are used. Use
  * the PASSES enum above to control multipass rendering!
  *
  * Set to ALLPASSES if you want to render in all passes  
  *
  * @param _passStatus Bitmask defining in which traverse pass the enter and leave nodes are used 
  */
  void setMultipassStatus(const MultipassBitMask _passStatus) { multipassStatus_ = _passStatus; };
  
  /** \brief Set multipass status to traverse in a specific pass
  *
  * Change multipass setting for the nodes status functions. The node will
  * call its enter and leave functions in the given pass if its set active.
  * Use the PASSES enum above to control multipass rendering!
  *
  * @param _i Pass in which the node should be rendered
  * @param _active Activate or deactivate in this pass?
  */
  void multipassStatusSetActive(const unsigned int _i, bool _active);
 
  /** \brief Get multipass status to traverse in a specific pass
  *
  * Check multipass setting for the nodes status functions if they should
  * be called in the given render pass. 
  * Use the PASSES enum above to control multipass rendering!
  *
  * @param _i Check this pass if the nodes enter/leave functions are active
  */  
  bool multipassStatusActive(const unsigned int _i) const;  
  
  
  
  /** \brief Get the current multipass settings for the node
  *
  * Get a bitmask defining in which traverse path an action is applied to the node. (1-indexed)
  * Use the PASSES enum above to control multipass rendering!
  *
  * @return Bitmask defining in which traverse passes an action is applied to the node.
  */
  MultipassBitMask multipassNode() const {return multipassNode_;};  
  
  
  
  /** \brief Set multipass settings for the node
  *
  * Set a bitmask defining in which traverse path an action is applied to the node. (1-indexed)
  * Set to ALLPASSES if you want to render in all passes.
  * Use the PASSES enum above to control multipass rendering!
  *
  * @param _passNode Bitmask defining in which traverse passes an action is applied to the node.
  */
  void setMultipassNode(const MultipassBitMask _passNode) { multipassNode_ = _passNode; };  
  
  /** \brief Set Node status to traverse in a specific pass
  *
  * Change multipass setting for the node. An action will be
  * applied to this node in the given pass.
  * Use the PASSES enum above to control multipass rendering!
  *
  * @param _i      Pass in which the node should be rendered
  * @param _active Enable or disable node in this pass?
  */
  void multipassNodeSetActive(const unsigned int _i , bool _active);
  
  /** \brief Get Node status to traverse in a specific pass
  *
  * Check multipass setting for the node if an action will be
  * applied in the given pass.
  * Use the PASSES enum above to control multipass rendering!
  *
  * @param _i Check this pass if an action will be applied to the node.
  */  
  bool multipassNodeActive(const unsigned int _i) const;  


  //===========================================================================
  /** @name RenderObject controls
  *  The render pass controls are only used during shader-based rendering with render-objects. 
  *  It is possible to provide shader and state settings that are copied to render-objects.
  *  These functions do not affect the fixed-function pipeline implementation of the scenegraph (enter() draw() leave()).\n
  *
  * @{ */
  //===========================================================================

public:

  /** \brief Set custom shaders
    *
    * Assigns a set of shaders to a primitive type of a node.
    * For instance, it is possible to render faces with a different shaders than lines.
    * Default shaders are used instead if no other shaders are provided.
    * Note: the derived node has to actually make use of shaders provided here
    *
    * Example: set shaders in OpenFlipper/Shaders/MyWireShaders for rendering the line parts of a node:
    *  node->setRenderobjectShaders("MyWireShaders/v.glsl", "MyWireShaders/g.glsl",  "MyWireShaders/f.glsl", true, ACG::SceneGraph::DrawModes::PRIMITIVE_WIREFRAME);
    *
    *
    * @param _vertexShaderFile    filename of vertex shader template compatible with ACG::ShaderGenerator
    * @param _geometryShaderFile  filename of geometry shader template compatible with ACG::ShaderGenerator
    * @param _fragmentShaderFile  filename of fragment shader template compatible with ACG::ShaderGenerator
    * @param _relativePaths       filenames are relative or absolute
    * @param _primitiveType       assign shaders to rendering this type of primitive of the polyline
    *
    */
  void setRenderObjectShaders(const std::string& _vertexShaderFile, const std::string& _geometryShaderFile, const std::string& _fragmentShaderFile, bool _relativePaths = true, DrawModes::DrawModePrimitive _primitiveType = DrawModes::PRIMITIVE_POLYGON);

  /** \brief Set custom shaders
    *
    * Assigns a set of shaders to a primitive type of a node.
    * For instance, it is possible to render faces with a different shaders than lines.
    * Default shaders are used instead if no other shaders are provided.
    * Note: the derived node has to actually make use of shaders provided here
    *
    * Example: set shaders in OpenFlipper/Shaders/MyWireShaders for rendering the line parts of a node:
    *  node->setRenderObjectShaders("MyWireShaders/v.glsl", "MyWireShaders/g.glsl",  "MyWireShaders/f.glsl", true, ACG::SceneGraph::DrawModes::PRIMITIVE_WIREFRAME);
    *
    *
    * @param _vertexShaderFile    filename of vertex shader template compatible with ACG::ShaderGenerator
    * @param _tessControlShaderFile    filename of tessellation-control shader template compatible with ACG::ShaderGenerator
    * @param _tessEvalShaderFile    filename of tessellation-eval shader template compatible with ACG::ShaderGenerator
    * @param _geometryShaderFile  filename of geometry shader template compatible with ACG::ShaderGenerator
    * @param _fragmentShaderFile  filename of fragment shader template compatible with ACG::ShaderGenerator
    * @param _relativePaths       filenames are relative or absolute
    * @param _primitiveType       assign shaders to rendering this type of primitive of the polyline
    *
    */
  void setRenderObjectShaders(const std::string& _vertexShaderFile, const std::string& _tessControlShaderFile, const std::string& _tessEvalShaderFile, const std::string& _geometryShaderFile, const std::string& _fragmentShaderFile, bool _relativePaths = true, DrawModes::DrawModePrimitive _primitiveType = DrawModes::PRIMITIVE_POLYGON);


  /** \brief Set uniforms for shader based rendering
    *
    * Uniforms are copied from a pool when rendering with shader-based render-objects.
    * The specified pool has to be a valid memory address whenever the objects are rendered.
    * It does not make a copy of the pool.
    *
    * @param _pool  pointer to address of a uniform pool
    *
    */
  void setRenderObjectUniformPool(const GLSL::UniformPool* _pool) {uniformPool_ = _pool;}

  /** \brief Get uniforms for shader based rendering
    *
    */
  const GLSL::UniformPool* getRenderObjectUniformPool() {return uniformPool_;}

  /** \brief Set textures for shader based rendering
    *
    * Assign textures to sampler slots.
    * Note: Fixed-function drawing ignores these textures.
    *       Also, it is still necessary to set the according sampler index in a UniformPool to access this texture in a shader.
    *
    * @param _samplerSlot  sampler slot to bind the texture to,  zero-based index
    * @param _texId        gl texture id
    * @param _texType      gl texture type ie. GL_TEXTURE_1D, ..
    *
    */
  void setRenderObjectTexture(int _samplerSlot, GLuint _texId, GLenum _texType = GL_TEXTURE_2D);


  /** \brief Set modifier for render objects
    *
    * Render-objects can be modified, if the implementation of a node supports this.
    * The currently active modifier should be applied directly before adding an object to the renderer.
    *
    * @param _modifier  pointer to address of modifier,  address must be valid whenever the node creates render-objects
    *
    */
  void setRenderObjectModifier(RenderObjectModifier* _modifier) {renderModifier_ = _modifier;}

  /** \brief Get render-object modifier
    *
    */
  RenderObjectModifier* getRenderObjectModifier() {return  renderModifier_;}



  /** \brief Set shaders, textures and uniforms as provided by user to a render-object.
    *
    * A derived node can use this convenience function to copy shader, uniform and texture settings to a render-object.
    *
    * @param _primitive the primitive type of the render-object
    * @param _obj       the render-object [in/out]
    *
    */
  void applyRenderObjectSettings(DrawModes::DrawModePrimitive _primitive, RenderObject* _obj) const;

private:

  /** multi pass bit mask (1-indexed)
  * Defines in which multipass runs the enter and leave functions should be called.
  * (Only applies during multipass traversal!)
  */
  MultipassBitMask multipassStatus_;
  
  /** multi pass bit mask (1-indexed)
  * Defines in which multipass runs an action should be applied to the node.
  * (Only applies during multipass traversal!)
  */  
  MultipassBitMask multipassNode_;

  /** @} */

private:

  /// Copy constructor. Disabled.
  BaseNode(const BaseNode&);

  /// Assignment operator. Disabled.
  void operator=(const BaseNode&);

  
  /// pointer to parent node
  BaseNode* parent_;

  /// name of node
  std::string name_;

  /// node status()
  StatusMode status_;
  
  /// list of children
  std::vector<BaseNode*> children_;

  /// used to provide unique IDs to nodes
  static unsigned int last_id_used__;

  /// ID of node
  unsigned int id_;

  /// private draw mode
  DrawModes::DrawMode drawMode_;
  
  /** Flag indicating if picking should be done for this object
   * This flag has to be checked by your node if you implement picking
   */
  bool pickingEnabled_;

  /// Flag indicating that the node has to be redrawn
  bool dirty_;

  /// traverse mode
  unsigned int traverseMode_;


  // settings for shader-based rendering with render-objects
private:

  struct ShaderSet 
  {
    // shader filenames

    /// vertex shader
    std::string vs_;

    /// tess-control
    std::string tcs_;

    /// tess-eval
    std::string tes_;

    /// geometry
    std::string gs_;

    /// fragment
    std::string fs_;
    
    /// rel or abs path
    bool relativePaths_;
  };
  
  /// shader settings for primitive modes
  std::map<DrawModes::DrawModePrimitive, ShaderSet> shaderSettings_;

  /// texture settings for shader based rendering
  std::map<int, RenderObject::Texture> textureSettings_;

  /// user provided uniform pool for shader constants
  const GLSL::UniformPool* uniformPool_;

  /// render-object modifier
  RenderObjectModifier* renderModifier_;
};


//=============================================================================
} // namespace SceneGraph
} // namespace ACG
//=============================================================================
#endif // ACG_BASE_NODE_HH defined
//=============================================================================
