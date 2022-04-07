#pragma once
#include <vector>
#include <unordered_set>
#include "acg_glew.hh"

/**
* A list of enums, which were removed for core profiles.
* list was obtained by parsing gl.xml from https://github.com/KhronosGroup/OpenGL-Registry/blob/master/xml/gl.xml
* on 27.2.2018
*
* unknown identifiers for OSX were manually defined using ifdef
*/

#ifndef GL_CLAMP_VERTEX_COLOR
#define GL_CLAMP_VERTEX_COLOR 0x891A
#endif
#ifndef GL_CLAMP_FRAGMENT_COLOR
#define GL_CLAMP_FRAGMENT_COLOR 0x891B
#endif
#ifndef GL_ALPHA_INTEGER
#define GL_ALPHA_INTEGER 0x8D97
#endif
#ifndef GL_TEXTURE_LUMINANCE_TYPE
#define GL_TEXTURE_LUMINANCE_TYPE 0x8C14
#endif
#ifndef GL_TEXTURE_INTENSITY_TYPE
#define GL_TEXTURE_INTENSITY_TYPE 0x8C15
#endif

std::unordered_set<GLenum> removedEnums = {
GL_CURRENT_BIT,
GL_POINT_BIT,
GL_LINE_BIT,
GL_POLYGON_BIT,
GL_POLYGON_STIPPLE_BIT,
GL_PIXEL_MODE_BIT,
GL_LIGHTING_BIT,
GL_FOG_BIT,
GL_ACCUM_BUFFER_BIT,
GL_VIEWPORT_BIT,
GL_TRANSFORM_BIT,
GL_ENABLE_BIT,
GL_HINT_BIT,
GL_EVAL_BIT,
GL_LIST_BIT,
GL_TEXTURE_BIT,
GL_SCISSOR_BIT,
GL_ALL_ATTRIB_BITS,
GL_CLIENT_PIXEL_STORE_BIT,
GL_CLIENT_VERTEX_ARRAY_BIT,
GL_CLIENT_ALL_ATTRIB_BITS,
GL_QUAD_STRIP,
GL_QUADS,
GL_POLYGON,
GL_ACCUM,
GL_LOAD,
GL_RETURN,
GL_MULT,
GL_ADD,
GL_STACK_OVERFLOW,
GL_STACK_UNDERFLOW,
GL_AUX0,
GL_AUX1,
GL_AUX2,
GL_AUX3,
GL_2D,
GL_3D,
GL_3D_COLOR,
GL_3D_COLOR_TEXTURE,
GL_4D_COLOR_TEXTURE,
GL_PASS_THROUGH_TOKEN,
GL_POINT_TOKEN,
GL_LINE_TOKEN,
GL_POLYGON_TOKEN,
GL_BITMAP_TOKEN,
GL_DRAW_PIXEL_TOKEN,
GL_COPY_PIXEL_TOKEN,
GL_LINE_RESET_TOKEN,
GL_EXP,
GL_EXP2,
GL_COEFF,
GL_ORDER,
GL_DOMAIN,
GL_PIXEL_MAP_I_TO_I,
GL_PIXEL_MAP_S_TO_S,
GL_PIXEL_MAP_I_TO_R,
GL_PIXEL_MAP_I_TO_G,
GL_PIXEL_MAP_I_TO_B,
GL_PIXEL_MAP_I_TO_A,
GL_PIXEL_MAP_R_TO_R,
GL_PIXEL_MAP_G_TO_G,
GL_PIXEL_MAP_B_TO_B,
GL_PIXEL_MAP_A_TO_A,
GL_VERTEX_ARRAY_POINTER,
GL_NORMAL_ARRAY_POINTER,
GL_COLOR_ARRAY_POINTER,
GL_INDEX_ARRAY_POINTER,
GL_TEXTURE_COORD_ARRAY_POINTER,
GL_EDGE_FLAG_ARRAY_POINTER,
GL_FEEDBACK_BUFFER_POINTER,
GL_SELECTION_BUFFER_POINTER,
GL_CURRENT_COLOR,
GL_CURRENT_INDEX,
GL_CURRENT_NORMAL,
GL_CURRENT_TEXTURE_COORDS,
GL_CURRENT_RASTER_COLOR,
GL_CURRENT_RASTER_INDEX,
GL_CURRENT_RASTER_TEXTURE_COORDS,
GL_CURRENT_RASTER_POSITION,
GL_CURRENT_RASTER_POSITION_VALID,
GL_CURRENT_RASTER_DISTANCE,
GL_POINT_SMOOTH,
GL_LINE_STIPPLE,
GL_LINE_STIPPLE_PATTERN,
GL_LINE_STIPPLE_REPEAT,
GL_LIST_MODE,
GL_MAX_LIST_NESTING,
GL_LIST_BASE,
GL_LIST_INDEX,
GL_POLYGON_STIPPLE,
GL_EDGE_FLAG,
GL_LIGHTING,
GL_LIGHT_MODEL_LOCAL_VIEWER,
GL_LIGHT_MODEL_TWO_SIDE,
GL_LIGHT_MODEL_AMBIENT,
GL_SHADE_MODEL,
GL_COLOR_MATERIAL_FACE,
GL_COLOR_MATERIAL_PARAMETER,
GL_COLOR_MATERIAL,
GL_FOG,
GL_FOG_INDEX,
GL_FOG_DENSITY,
GL_FOG_START,
GL_FOG_END,
GL_FOG_MODE,
GL_FOG_COLOR,
GL_ACCUM_CLEAR_VALUE,
GL_MATRIX_MODE,
GL_NORMALIZE,
GL_MODELVIEW_STACK_DEPTH,
GL_PROJECTION_STACK_DEPTH,
GL_TEXTURE_STACK_DEPTH,
GL_MODELVIEW_MATRIX,
GL_PROJECTION_MATRIX,
GL_TEXTURE_MATRIX,
GL_ATTRIB_STACK_DEPTH,
GL_CLIENT_ATTRIB_STACK_DEPTH,
GL_ALPHA_TEST,
GL_ALPHA_TEST_FUNC,
GL_ALPHA_TEST_REF,
GL_INDEX_LOGIC_OP,
GL_LOGIC_OP,
GL_AUX_BUFFERS,
GL_INDEX_CLEAR_VALUE,
GL_INDEX_WRITEMASK,
GL_INDEX_MODE,
GL_RGBA_MODE,
GL_RENDER_MODE,
GL_PERSPECTIVE_CORRECTION_HINT,
GL_POINT_SMOOTH_HINT,
GL_FOG_HINT,
GL_TEXTURE_GEN_S,
GL_TEXTURE_GEN_T,
GL_TEXTURE_GEN_R,
GL_TEXTURE_GEN_Q,
GL_PIXEL_MAP_I_TO_I_SIZE,
GL_PIXEL_MAP_S_TO_S_SIZE,
GL_PIXEL_MAP_I_TO_R_SIZE,
GL_PIXEL_MAP_I_TO_G_SIZE,
GL_PIXEL_MAP_I_TO_B_SIZE,
GL_PIXEL_MAP_I_TO_A_SIZE,
GL_PIXEL_MAP_R_TO_R_SIZE,
GL_PIXEL_MAP_G_TO_G_SIZE,
GL_PIXEL_MAP_B_TO_B_SIZE,
GL_PIXEL_MAP_A_TO_A_SIZE,
GL_MAP_COLOR,
GL_MAP_STENCIL,
GL_INDEX_SHIFT,
GL_INDEX_OFFSET,
GL_RED_SCALE,
GL_RED_BIAS,
GL_ZOOM_X,
GL_ZOOM_Y,
GL_GREEN_SCALE,
GL_GREEN_BIAS,
GL_BLUE_SCALE,
GL_BLUE_BIAS,
GL_ALPHA_SCALE,
GL_ALPHA_BIAS,
GL_DEPTH_SCALE,
GL_DEPTH_BIAS,
GL_MAX_EVAL_ORDER,
GL_MAX_LIGHTS,
GL_MAX_CLIP_PLANES,
GL_MAX_PIXEL_MAP_TABLE,
GL_MAX_ATTRIB_STACK_DEPTH,
GL_MAX_MODELVIEW_STACK_DEPTH,
GL_MAX_NAME_STACK_DEPTH,
GL_MAX_PROJECTION_STACK_DEPTH,
GL_MAX_TEXTURE_STACK_DEPTH,
GL_MAX_CLIENT_ATTRIB_STACK_DEPTH,
GL_INDEX_BITS,
GL_RED_BITS,
GL_GREEN_BITS,
GL_BLUE_BITS,
GL_ALPHA_BITS,
GL_DEPTH_BITS,
GL_STENCIL_BITS,
GL_ACCUM_RED_BITS,
GL_ACCUM_GREEN_BITS,
GL_ACCUM_BLUE_BITS,
GL_ACCUM_ALPHA_BITS,
GL_NAME_STACK_DEPTH,
GL_AUTO_NORMAL,
GL_MAP1_COLOR_4,
GL_MAP1_INDEX,
GL_MAP1_NORMAL,
GL_MAP1_TEXTURE_COORD_1,
GL_MAP1_TEXTURE_COORD_2,
GL_MAP1_TEXTURE_COORD_3,
GL_MAP1_TEXTURE_COORD_4,
GL_MAP1_VERTEX_3,
GL_MAP1_VERTEX_4,
GL_MAP2_COLOR_4,
GL_MAP2_INDEX,
GL_MAP2_NORMAL,
GL_MAP2_TEXTURE_COORD_1,
GL_MAP2_TEXTURE_COORD_2,
GL_MAP2_TEXTURE_COORD_3,
GL_MAP2_TEXTURE_COORD_4,
GL_MAP2_VERTEX_3,
GL_MAP2_VERTEX_4,
GL_MAP1_GRID_DOMAIN,
GL_MAP1_GRID_SEGMENTS,
GL_MAP2_GRID_DOMAIN,
GL_MAP2_GRID_SEGMENTS,
GL_FEEDBACK_BUFFER_SIZE,
GL_FEEDBACK_BUFFER_TYPE,
GL_SELECTION_BUFFER_SIZE,
GL_VERTEX_ARRAY,
GL_NORMAL_ARRAY,
GL_COLOR_ARRAY,
GL_INDEX_ARRAY,
GL_TEXTURE_COORD_ARRAY,
GL_EDGE_FLAG_ARRAY,
GL_VERTEX_ARRAY_SIZE,
GL_VERTEX_ARRAY_TYPE,
GL_VERTEX_ARRAY_STRIDE,
GL_NORMAL_ARRAY_TYPE,
GL_NORMAL_ARRAY_STRIDE,
GL_COLOR_ARRAY_SIZE,
GL_COLOR_ARRAY_TYPE,
GL_COLOR_ARRAY_STRIDE,
GL_INDEX_ARRAY_TYPE,
GL_INDEX_ARRAY_STRIDE,
GL_TEXTURE_COORD_ARRAY_SIZE,
GL_TEXTURE_COORD_ARRAY_TYPE,
GL_TEXTURE_COORD_ARRAY_STRIDE,
GL_EDGE_FLAG_ARRAY_STRIDE,
GL_TEXTURE_COMPONENTS,
GL_TEXTURE_BORDER,
GL_TEXTURE_LUMINANCE_SIZE,
GL_TEXTURE_INTENSITY_SIZE,
GL_TEXTURE_PRIORITY,
GL_TEXTURE_RESIDENT,
GL_AMBIENT,
GL_DIFFUSE,
GL_SPECULAR,
GL_POSITION,
GL_SPOT_DIRECTION,
GL_SPOT_EXPONENT,
GL_SPOT_CUTOFF,
GL_CONSTANT_ATTENUATION,
GL_LINEAR_ATTENUATION,
GL_QUADRATIC_ATTENUATION,
GL_COMPILE,
GL_COMPILE_AND_EXECUTE,
GL_2_BYTES,
GL_3_BYTES,
GL_4_BYTES,
GL_EMISSION,
GL_SHININESS,
GL_AMBIENT_AND_DIFFUSE,
GL_COLOR_INDEXES,
GL_MODELVIEW,
GL_PROJECTION,
GL_COLOR_INDEX,
GL_LUMINANCE,
GL_LUMINANCE_ALPHA,
GL_BITMAP,
GL_RENDER,
GL_FEEDBACK,
GL_SELECT,
GL_FLAT,
GL_SMOOTH,
GL_S,
GL_T,
GL_R,
GL_Q,
GL_MODULATE,
GL_DECAL,
GL_TEXTURE_ENV_MODE,
GL_TEXTURE_ENV_COLOR,
GL_TEXTURE_ENV,
GL_EYE_LINEAR,
GL_OBJECT_LINEAR,
GL_SPHERE_MAP,
GL_TEXTURE_GEN_MODE,
GL_OBJECT_PLANE,
GL_EYE_PLANE,
GL_CLAMP,
GL_ALPHA4,
GL_ALPHA8,
GL_ALPHA12,
GL_ALPHA16,
GL_LUMINANCE4,
GL_LUMINANCE8,
GL_LUMINANCE12,
GL_LUMINANCE16,
GL_LUMINANCE4_ALPHA4,
GL_LUMINANCE6_ALPHA2,
GL_LUMINANCE8_ALPHA8,
GL_LUMINANCE12_ALPHA4,
GL_LUMINANCE12_ALPHA12,
GL_LUMINANCE16_ALPHA16,
GL_INTENSITY,
GL_INTENSITY4,
GL_INTENSITY8,
GL_INTENSITY12,
GL_INTENSITY16,
GL_V2F,
GL_V3F,
GL_C4UB_V2F,
GL_C4UB_V3F,
GL_C3F_V3F,
GL_N3F_V3F,
GL_C4F_N3F_V3F,
GL_T2F_V3F,
GL_T4F_V4F,
GL_T2F_C4UB_V3F,
GL_T2F_C3F_V3F,
GL_T2F_N3F_V3F,
GL_T2F_C4F_N3F_V3F,
GL_T4F_C4F_N3F_V4F,
GL_CLIP_PLANE0,
GL_CLIP_PLANE1,
GL_CLIP_PLANE2,
GL_CLIP_PLANE3,
GL_CLIP_PLANE4,
GL_CLIP_PLANE5,
GL_LIGHT0,
GL_LIGHT1,
GL_LIGHT2,
GL_LIGHT3,
GL_LIGHT4,
GL_LIGHT5,
GL_LIGHT6,
GL_LIGHT7,
GL_RESCALE_NORMAL,
GL_LIGHT_MODEL_COLOR_CONTROL,
GL_SINGLE_COLOR,
GL_SEPARATE_SPECULAR_COLOR,
GL_ALIASED_POINT_SIZE_RANGE,
GL_CLIENT_ACTIVE_TEXTURE,
GL_MAX_TEXTURE_UNITS,
GL_TRANSPOSE_MODELVIEW_MATRIX,
GL_TRANSPOSE_PROJECTION_MATRIX,
GL_TRANSPOSE_TEXTURE_MATRIX,
GL_TRANSPOSE_COLOR_MATRIX,
GL_MULTISAMPLE_BIT,
GL_NORMAL_MAP,
GL_REFLECTION_MAP,
GL_COMPRESSED_ALPHA,
GL_COMPRESSED_LUMINANCE,
GL_COMPRESSED_LUMINANCE_ALPHA,
GL_COMPRESSED_INTENSITY,
GL_COMBINE,
GL_COMBINE_RGB,
GL_COMBINE_ALPHA,
GL_SOURCE0_RGB,
GL_SOURCE1_RGB,
GL_SOURCE2_RGB,
GL_SOURCE0_ALPHA,
GL_SOURCE1_ALPHA,
GL_SOURCE2_ALPHA,
GL_OPERAND0_RGB,
GL_OPERAND1_RGB,
GL_OPERAND2_RGB,
GL_OPERAND0_ALPHA,
GL_OPERAND1_ALPHA,
GL_OPERAND2_ALPHA,
GL_RGB_SCALE,
GL_ADD_SIGNED,
GL_INTERPOLATE,
GL_SUBTRACT,
GL_CONSTANT,
GL_PRIMARY_COLOR,
GL_PREVIOUS,
GL_DOT3_RGB,
GL_DOT3_RGBA,
GL_POINT_SIZE_MIN,
GL_POINT_SIZE_MAX,
GL_POINT_DISTANCE_ATTENUATION,
GL_GENERATE_MIPMAP,
GL_GENERATE_MIPMAP_HINT,
GL_FOG_COORDINATE_SOURCE,
GL_FOG_COORDINATE,
GL_FRAGMENT_DEPTH,
GL_CURRENT_FOG_COORDINATE,
GL_FOG_COORDINATE_ARRAY_TYPE,
GL_FOG_COORDINATE_ARRAY_STRIDE,
GL_FOG_COORDINATE_ARRAY_POINTER,
GL_FOG_COORDINATE_ARRAY,
GL_COLOR_SUM,
GL_CURRENT_SECONDARY_COLOR,
GL_SECONDARY_COLOR_ARRAY_SIZE,
GL_SECONDARY_COLOR_ARRAY_TYPE,
GL_SECONDARY_COLOR_ARRAY_STRIDE,
GL_SECONDARY_COLOR_ARRAY_POINTER,
GL_SECONDARY_COLOR_ARRAY,
GL_TEXTURE_FILTER_CONTROL,
GL_DEPTH_TEXTURE_MODE,
GL_COMPARE_R_TO_TEXTURE,
GL_VERTEX_ARRAY_BUFFER_BINDING,
GL_NORMAL_ARRAY_BUFFER_BINDING,
GL_COLOR_ARRAY_BUFFER_BINDING,
GL_INDEX_ARRAY_BUFFER_BINDING,
GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING,
GL_EDGE_FLAG_ARRAY_BUFFER_BINDING,
GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING,
GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING,
GL_WEIGHT_ARRAY_BUFFER_BINDING,
GL_FOG_COORD_SRC,
GL_FOG_COORD,
GL_CURRENT_FOG_COORD,
GL_FOG_COORD_ARRAY_TYPE,
GL_FOG_COORD_ARRAY_STRIDE,
GL_FOG_COORD_ARRAY_POINTER,
GL_FOG_COORD_ARRAY,
GL_FOG_COORD_ARRAY_BUFFER_BINDING,
GL_SRC0_RGB,
GL_SRC1_RGB,
GL_SRC2_RGB,
GL_SRC0_ALPHA,
GL_SRC2_ALPHA,
GL_VERTEX_PROGRAM_TWO_SIDE,
GL_POINT_SPRITE,
GL_COORD_REPLACE,
GL_MAX_TEXTURE_COORDS,
GL_CURRENT_RASTER_SECONDARY_COLOR,
GL_SLUMINANCE_ALPHA,
GL_SLUMINANCE8_ALPHA8,
GL_SLUMINANCE,
GL_SLUMINANCE8,
GL_COMPRESSED_SLUMINANCE,
GL_COMPRESSED_SLUMINANCE_ALPHA,
GL_CLAMP_VERTEX_COLOR,
GL_CLAMP_FRAGMENT_COLOR,
GL_ALPHA_INTEGER,
GL_TEXTURE_LUMINANCE_TYPE,
GL_TEXTURE_INTENSITY_TYPE
};