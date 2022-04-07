
find_package(Qt5 COMPONENTS Core Gui)

set(INCLUDE_DIRS ${OPENMESH_INCLUDE_DIRS}
                 ${CMAKE_CURRENT_SOURCE_DIR}
                 ${CMAKE_CURRENT_SOURCE_DIR}/../
                 ${CMAKE_CURRENT_SOURCE_DIR}/ShaderUtils
                 ${CMAKE_CURRENT_BINARY_DIR}
                 ${OPENGL_INCLUDE_DIR} )

set(ADDITIONAL_LINK_LIBRARIES Qt5::Core Qt5::Gui OpenMeshCore GLEW::glew_s)

#===================================================================
# ACG Library files
#===================================================================
set ( headers
    Geometry/Algorithms.hh
    Geometry/AlgorithmsAngleT.hh
    Geometry/AlgorithmsAngleT_impl.hh
    Geometry/GPUCacheOptimizer.hh
    Geometry/Spherical.hh
    Geometry/Triangulator.hh
    Config/ACGDefines.hh
    Geometry/Types/PlaneT.hh
    Geometry/Types/PlaneType.hh
    Geometry/Types/QuadricT.hh
    Geometry/bsp/BSPImplT.hh
    Geometry/bsp/BSPImplT_impl.hh
    Geometry/bsp/BSPTreeNode.hh
    Geometry/bsp/TriangleBSPCoreT.hh
    Geometry/bsp/TriangleBSPCoreT_impl.hh
    Geometry/bsp/TriangleBSPT.hh
    GL/AntiAliasing.hh
    GL/ColorStack.hh
    GL/ColorTranslator.hh
    GL/DrawMesh.hh
    GL/DrawMeshT_impl.hh
    GL/FBO.hh
    GL/FilterKernels.hh
    GL/GLError.hh
    GL/GLFormatInfo.hh
    GL/GLPrimitives.hh
    GL/GLState.hh
    GL/GLTrackball.hh
    GL/IRenderer.hh
    GL/MeshCompiler.hh
    GL/PBuffer.hh
    GL/RenderObject.hh
    GL/ScreenQuad.hh
    GL/ShaderCache.hh
    GL/ShaderGenerator.hh
    GL/VertexDeclaration.hh
    GL/acg_glew.hh
    GL/gl.hh
    GL/globjects.hh
    GL/removedEnums.hh
    GL/stipple_alpha.hh
    Math/BSplineBasis.hh
    Math/BezierCurveT.hh
    Math/BezierCurveT_impl.hh
    Math/DualQuaternionT.hh
    Math/DualQuaternionT_impl.hh
    Math/GLMatrixT.hh
    Math/GLMatrixT_impl.hh
    Math/Matrix3x3T.hh
    Math/Matrix4x4T.hh
    Math/Matrix4x4T_impl.hh
    Math/QuaternionT.hh
    Math/VectorT.hh
    Scenegraph/ArrowNode.hh
    Scenegraph/BaseNode.hh
    Scenegraph/BoundingBoxNode.hh
    Scenegraph/CartesianClippingNode.hh
    Scenegraph/ClippingNode.hh
    Scenegraph/CoordFrameNode.hh
    Scenegraph/CoordsysNode.hh
    Scenegraph/DrawModes.hh
    Scenegraph/EnvMapNode.hh
    Scenegraph/FloatingSubtreeNode.hh
    Scenegraph/GlutPrimitiveNode.hh
    Scenegraph/GridNode.hh
    Scenegraph/LightSourceNode.hh
    Scenegraph/LineNode.hh
    Scenegraph/ManipulatorNode.hh
    Scenegraph/MaterialNode.hh
    Scenegraph/MeshNode2T.hh
    Scenegraph/MeshNode2T_impl.hh
    Scenegraph/MeshNodeDeprecatedT.hh
    Scenegraph/MeshNodeDeprecatedT_impl.hh
    Scenegraph/OBJNode.hh
    Scenegraph/OSDTransformNode.hh
    Scenegraph/PickTarget.hh
    Scenegraph/PointNode.hh
    Scenegraph/PrincipalAxisNode.hh
    Scenegraph/PrincipalAxisNodeT_impl.hh
    Scenegraph/QuadNode.hh
    Scenegraph/ResourceManagerNode.hh
    Scenegraph/SceneGraph.hh
    Scenegraph/SceneGraphAnalysis.hh
    Scenegraph/SeparatorNode.hh
    Scenegraph/ShaderNode.hh
    Scenegraph/SliceNode.hh
    Scenegraph/StatusNodesT.hh
    Scenegraph/StatusNodesT_impl.hh
    Scenegraph/StencilRefNode.hh
    Scenegraph/StripProcessorT.hh
    Scenegraph/StripProcessorT_impl.hh
    Scenegraph/SwitchNode.hh
    Scenegraph/TextNode.hh
    Scenegraph/Texture3DNode.hh
    Scenegraph/TextureNode.hh
    Scenegraph/TrackballNode.hh
    Scenegraph/TransformNode.hh
    Scenegraph/TranslationManipulatorNode.hh
    Scenegraph/TriStripNodeDeprecatedT.hh
    Scenegraph/TriangleNode.hh
    ShaderUtils/GLSLShader.hh
    ShaderUtils/UniformPool.hh
    Utils/ColorCoder.hh
    Utils/ColorConversion.hh
    Utils/ColorGenerator.hh
    Utils/HaltonColors.hh
    Utils/HeapT.hh
    Utils/Histogram.hh
    Utils/HuePartitioningColors.hh
    Utils/IColorCoder.hh
    Utils/ImageConversion.hh
    Utils/LinearTwoColorCoder.hh
    Utils/Noncopyable.hh
    Utils/NumLimitsT.hh
    Utils/Profile.hh
    Utils/Progress.hh
    Utils/SmartPointer.hh
    Utils/StopWatch.hh
    Utils/Tracing.hh
    Utils/VSToolsT.hh
    )

set (sources
    Geometry/Algorithms.cc
    Geometry/GPUCacheOptimizer.cc
    Geometry/Triangulator.cc
    Geometry/Types/PlaneType.cc
    GL/AntiAliasing.cc
    GL/ColorStack.cc
    GL/ColorTranslator.cc
    GL/DrawMesh.cc
    GL/DrawMeshT_impl.cc
    GL/FBO.cc
    GL/FilterKernels.cc
    GL/GLError.cc
    GL/GLFormatInfo.cc
    GL/GLPrimitives.cc
    GL/GLState.cc
    GL/GLTrackball.cc
    GL/IRenderer.cc
    GL/MeshCompiler.cc
    GL/PBuffer.cc
    GL/RenderObject.cc
    GL/ScreenQuad.cc
    GL/ShaderCache.cc
    GL/ShaderGenerator.cc
    GL/VertexDeclaration.cc
    GL/gl.cc
    GL/globjects.cc
    GL/stipple_alpha.cc
    Math/BSplineBasis.cc
    Scenegraph/ArrowNode.cc
    Scenegraph/BaseNode.cc
    Scenegraph/BoundingBoxNode.cc
    Scenegraph/CartesianClippingNode.cc
    Scenegraph/ClippingNode.cc
    Scenegraph/CoordFrameNode.cc
    Scenegraph/CoordsysNode.cc
    Scenegraph/DrawModes.cc
    Scenegraph/EnvMapNode.cc
    Scenegraph/FloatingSubtreeNode.cc
    Scenegraph/GlutPrimitiveNode.cc
    Scenegraph/GridNode.cc
    Scenegraph/LightSourceNode.cc
    Scenegraph/LineNode.cc
    Scenegraph/LineNodeGLCompat.cc
    Scenegraph/ManipulatorNode.cc
    Scenegraph/MaterialNode.cc
    Scenegraph/MeshNode2.cc
    Scenegraph/MeshNode2T_impl.cc
    Scenegraph/OBJNode.cc
    Scenegraph/OSDTransformNode.cc
    Scenegraph/PointNode.cc
    Scenegraph/PrincipalAxisNode.cc
    Scenegraph/QuadNode.cc
    Scenegraph/SceneGraph.cc
    Scenegraph/SceneGraphAnalysis.cc
    Scenegraph/SeparatorNode.cc
    Scenegraph/ShaderNode.cc
    Scenegraph/SliceNode.cc
    Scenegraph/StatusNodesBase.cc
    Scenegraph/StencilRefNode.cc
    Scenegraph/StripProcessor.cc
    Scenegraph/SwitchNode.cc
    Scenegraph/TextNode.cc
    Scenegraph/TextNodeGLCompat.cc
    Scenegraph/Texture3DNode.cc
    Scenegraph/TextureNode.cc
    Scenegraph/TrackballNode.cc
    Scenegraph/TransformNode.cc
    Scenegraph/TranslationManipulatorNode.cc
    Scenegraph/TriangleNode.cc
    ShaderUtils/GLSLShader.cc
    ShaderUtils/UniformPool.cc
    Utils/ColorCoder.cc
    Utils/HaltonColors.cc
    Utils/Histogram.cc
    Utils/HuePartitioningColors.cc
    Utils/ImageConversion.cc
    Utils/LinearTwoColorCoder.cc
    Utils/Profile.cc
    Utils/Tracing.cc
)

if (OPENMP_FOUND)
    set(INCLUDE_DIRS ${INCLUDE_DIRS} ${OpenMP_CXX_INCLUDE_DIR} )
    set(ADDITIONAL_LINK_LIBRARIES ${ADDITIONAL_LINK_LIBRARIES} ${OpenMP_libomp_LIBRARY})
endif()

include_directories (
  ${INCLUDE_DIRS}
)

# generate dllexport macros on windows
if (WIN32)
  add_definitions(-DACGDLL )
  remove_definitions(-DUSEACG)
endif ()

add_definitions(-D_USE_MATH_DEFINES -DNOMINMAX -DINCLUDE_TEMPLATES)

source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${sources} ${headers} ${ui-files} ${qrc})

add_library(acg SHARED ${headers} ${sources})
target_link_libraries (acg ${ADDITIONAL_LINK_LIBRARIES})

set_target_properties(acg PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/$<CONFIG>)
set_target_properties(acg PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
set_target_properties(acg PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/$<CONFIG>)
