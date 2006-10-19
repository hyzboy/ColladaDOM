/*
 * Copyright 2006 Sony Computer Entertainment Inc.
 *
 * Licensed under the SCEA Shared Source License, Version 1.0 (the "License"); you may not use this 
 * file except in compliance with the License. You may obtain a copy of the License at:
 * http://research.scea.com/scea_shared_source_license.html
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License 
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or 
 * implied. See the License for the specific language governing permissions and limitations under the 
 * License. 
 */

#ifndef __DOM_TYPES_H__
#define __DOM_TYPES_H__

#include <dae/daeDomTypes.h>

// Register all types
extern void registerDomTypes();

typedef xsBoolean		domBool;
typedef xsDateTime		domDateTime;
typedef xsDouble		domFloat;
typedef xsLong		domInt;
typedef xsName		domName;
typedef xsString		domString;
typedef xsToken		domToken;
typedef xsUnsignedLong		domUint;
typedef daeTArray<domBool>		domListOfBools;
typedef daeTArray<domFloat>		domListOfFloats;
typedef xsHexBinaryArray		domListOfHexBinary;
typedef daeTArray<domInt>		domListOfInts;
typedef daeTArray<domName>		domListOfNames;
typedef daeTArray<domToken>		domListOfTokens;
typedef daeTArray<domUint>		domListOfUInts;
typedef domListOfBools		domBool2;
typedef domListOfBools		domBool3;
typedef domListOfBools		domBool4;
typedef domListOfFloats		domFloat2;
typedef domListOfFloats		domFloat3;
typedef domListOfFloats		domFloat4;
typedef domListOfFloats		domFloat7;
typedef domListOfFloats		domFloat2x2;
typedef domListOfFloats		domFloat3x3;
typedef domListOfFloats		domFloat4x4;
typedef domListOfFloats		domFloat2x3;
typedef domListOfFloats		domFloat2x4;
typedef domListOfFloats		domFloat3x2;
typedef domListOfFloats		domFloat3x4;
typedef domListOfFloats		domFloat4x2;
typedef domListOfFloats		domFloat4x3;
typedef domListOfInts		domInt2;
typedef domListOfInts		domInt3;
typedef domListOfInts		domInt4;
typedef domListOfInts		domInt2x2;
typedef domListOfInts		domInt3x3;
typedef domListOfInts		domInt4x4;
/**
 * This type is used for URI reference which can only reference a resource
 * declared within it's same document.
 */
typedef xsAnyURI		domURIFragmentType;
typedef domFloat4		domFx_color_common;
typedef xsString		domFx_draw_common;
typedef xsNonNegativeInteger		domGL_MAX_LIGHTS_index;
typedef xsNonNegativeInteger		domGL_MAX_CLIP_PLANES_index;
typedef xsNonNegativeInteger		domGL_MAX_TEXTURE_IMAGE_UNITS_index;
typedef xsFloat		domGl_alpha_value_type;
typedef xsFloat		domGlsl_float;
typedef xsInt		domGlsl_int;
typedef xsBoolean		domGlsl_bool;
typedef daeTArray<domGlsl_bool>		domGlsl_ListOfBool;
typedef daeTArray<domGlsl_float>		domGlsl_ListOfFloat;
typedef daeTArray<domGlsl_int>		domGlsl_ListOfInt;
typedef domGlsl_ListOfBool		domGlsl_bool2;
typedef domGlsl_ListOfBool		domGlsl_bool3;
typedef domGlsl_ListOfBool		domGlsl_bool4;
typedef domGlsl_ListOfFloat		domGlsl_float2;
typedef domGlsl_ListOfFloat		domGlsl_float3;
typedef domGlsl_ListOfFloat		domGlsl_float4;
typedef domGlsl_ListOfFloat		domGlsl_float2x2;
typedef domGlsl_ListOfFloat		domGlsl_float3x3;
typedef domGlsl_ListOfFloat		domGlsl_float4x4;
typedef domGlsl_ListOfInt		domGlsl_int2;
typedef domGlsl_ListOfInt		domGlsl_int3;
typedef domGlsl_ListOfInt		domGlsl_int4;
typedef xsToken		domGlsl_identifier;
typedef xsBoolean		domCg_bool;
typedef xsFloat		domCg_float;
typedef xsInt		domCg_int;
typedef xsFloat		domCg_half;
typedef xsFloat		domCg_fixed;
typedef xsBoolean		domCg_bool1;
typedef xsFloat		domCg_float1;
typedef xsInt		domCg_int1;
typedef xsFloat		domCg_half1;
typedef xsFloat		domCg_fixed1;
typedef daeTArray<domCg_bool>		domCg_ListOfBool;
typedef daeTArray<domCg_float>		domCg_ListOfFloat;
typedef daeTArray<domCg_int>		domCg_ListOfInt;
typedef daeTArray<domCg_half>		domCg_ListOfHalf;
typedef daeTArray<domCg_fixed>		domCg_ListOfFixed;
typedef domCg_ListOfBool		domCg_bool2;
typedef domCg_ListOfBool		domCg_bool3;
typedef domCg_ListOfBool		domCg_bool4;
typedef domCg_ListOfBool		domCg_bool1x1;
typedef domCg_ListOfBool		domCg_bool1x2;
typedef domCg_ListOfBool		domCg_bool1x3;
typedef domCg_ListOfBool		domCg_bool1x4;
typedef domCg_ListOfBool		domCg_bool2x1;
typedef domCg_ListOfBool		domCg_bool2x2;
typedef domCg_ListOfBool		domCg_bool2x3;
typedef domCg_ListOfBool		domCg_bool2x4;
typedef domCg_ListOfBool		domCg_bool3x1;
typedef domCg_ListOfBool		domCg_bool3x2;
typedef domCg_ListOfBool		domCg_bool3x3;
typedef domCg_ListOfBool		domCg_bool3x4;
typedef domCg_ListOfBool		domCg_bool4x1;
typedef domCg_ListOfBool		domCg_bool4x2;
typedef domCg_ListOfBool		domCg_bool4x3;
typedef domCg_ListOfBool		domCg_bool4x4;
typedef domCg_ListOfFloat		domCg_float2;
typedef domCg_ListOfFloat		domCg_float3;
typedef domCg_ListOfFloat		domCg_float4;
typedef domCg_ListOfFloat		domCg_float1x1;
typedef domCg_ListOfFloat		domCg_float1x2;
typedef domCg_ListOfFloat		domCg_float1x3;
typedef domCg_ListOfFloat		domCg_float1x4;
typedef domCg_ListOfFloat		domCg_float2x1;
typedef domCg_ListOfFloat		domCg_float2x2;
typedef domCg_ListOfFloat		domCg_float2x3;
typedef domCg_ListOfFloat		domCg_float2x4;
typedef domCg_ListOfFloat		domCg_float3x1;
typedef domCg_ListOfFloat		domCg_float3x2;
typedef domCg_ListOfFloat		domCg_float3x3;
typedef domCg_ListOfFloat		domCg_float3x4;
typedef domCg_ListOfFloat		domCg_float4x1;
typedef domCg_ListOfFloat		domCg_float4x2;
typedef domCg_ListOfFloat		domCg_float4x3;
typedef domCg_ListOfFloat		domCg_float4x4;
typedef domCg_ListOfInt		domCg_int2;
typedef domCg_ListOfInt		domCg_int3;
typedef domCg_ListOfInt		domCg_int4;
typedef domCg_ListOfInt		domCg_int1x1;
typedef domCg_ListOfInt		domCg_int1x2;
typedef domCg_ListOfInt		domCg_int1x3;
typedef domCg_ListOfInt		domCg_int1x4;
typedef domCg_ListOfInt		domCg_int2x1;
typedef domCg_ListOfInt		domCg_int2x2;
typedef domCg_ListOfInt		domCg_int2x3;
typedef domCg_ListOfInt		domCg_int2x4;
typedef domCg_ListOfInt		domCg_int3x1;
typedef domCg_ListOfInt		domCg_int3x2;
typedef domCg_ListOfInt		domCg_int3x3;
typedef domCg_ListOfInt		domCg_int3x4;
typedef domCg_ListOfInt		domCg_int4x1;
typedef domCg_ListOfInt		domCg_int4x2;
typedef domCg_ListOfInt		domCg_int4x3;
typedef domCg_ListOfInt		domCg_int4x4;
typedef domCg_ListOfHalf		domCg_half2;
typedef domCg_ListOfHalf		domCg_half3;
typedef domCg_ListOfHalf		domCg_half4;
typedef domCg_ListOfHalf		domCg_half1x1;
typedef domCg_ListOfHalf		domCg_half1x2;
typedef domCg_ListOfHalf		domCg_half1x3;
typedef domCg_ListOfHalf		domCg_half1x4;
typedef domCg_ListOfHalf		domCg_half2x1;
typedef domCg_ListOfHalf		domCg_half2x2;
typedef domCg_ListOfHalf		domCg_half2x3;
typedef domCg_ListOfHalf		domCg_half2x4;
typedef domCg_ListOfHalf		domCg_half3x1;
typedef domCg_ListOfHalf		domCg_half3x2;
typedef domCg_ListOfHalf		domCg_half3x3;
typedef domCg_ListOfHalf		domCg_half3x4;
typedef domCg_ListOfHalf		domCg_half4x1;
typedef domCg_ListOfHalf		domCg_half4x2;
typedef domCg_ListOfHalf		domCg_half4x3;
typedef domCg_ListOfHalf		domCg_half4x4;
typedef domCg_ListOfFixed		domCg_fixed2;
typedef domCg_ListOfFixed		domCg_fixed3;
typedef domCg_ListOfFixed		domCg_fixed4;
typedef domCg_ListOfFixed		domCg_fixed1x1;
typedef domCg_ListOfFixed		domCg_fixed1x2;
typedef domCg_ListOfFixed		domCg_fixed1x3;
typedef domCg_ListOfFixed		domCg_fixed1x4;
typedef domCg_ListOfFixed		domCg_fixed2x1;
typedef domCg_ListOfFixed		domCg_fixed2x2;
typedef domCg_ListOfFixed		domCg_fixed2x3;
typedef domCg_ListOfFixed		domCg_fixed2x4;
typedef domCg_ListOfFixed		domCg_fixed3x1;
typedef domCg_ListOfFixed		domCg_fixed3x2;
typedef domCg_ListOfFixed		domCg_fixed3x3;
typedef domCg_ListOfFixed		domCg_fixed3x4;
typedef domCg_ListOfFixed		domCg_fixed4x1;
typedef domCg_ListOfFixed		domCg_fixed4x2;
typedef domCg_ListOfFixed		domCg_fixed4x3;
typedef domCg_ListOfFixed		domCg_fixed4x4;
typedef xsToken		domCg_identifier;
typedef xsNonNegativeInteger		domGLES_MAX_LIGHTS_index;
typedef xsNonNegativeInteger		domGLES_MAX_CLIP_PLANES_index;
typedef xsNonNegativeInteger		domGLES_MAX_TEXTURE_COORDS_index;
typedef xsNonNegativeInteger		domGLES_MAX_TEXTURE_IMAGE_UNITS_index;
typedef xsNonNegativeInteger		domGles_texcombiner_argument_index_type;
typedef xsNCName		domGles_rendertarget_common;

/**
 * An enumuerated type specifying the acceptable morph methods.
 */
enum domMorphMethodType {
	MORPHMETHODTYPE_NORMALIZED,
	MORPHMETHODTYPE_RELATIVE,
	MORPHMETHODTYPE_COUNT = 2
};

/**
 * An enumerated type specifying the acceptable node types.
 */
enum domNodeType {
	NODETYPE_JOINT,
	NODETYPE_NODE,
	NODETYPE_COUNT = 2
};

/**
 * An enumerated type specifying the acceptable up-axis values.
 */
enum domUpAxisType {
	UPAXISTYPE_X_UP,
	UPAXISTYPE_Y_UP,
	UPAXISTYPE_Z_UP,
	UPAXISTYPE_COUNT = 3
};

/**
 * An enumerated type specifying the acceptable document versions.
 */
enum domVersionType {
	VERSIONTYPE_1_4_0,
	VERSIONTYPE_1_4_1,
	VERSIONTYPE_COUNT = 2
};

enum domFx_opaque_enum {
	FX_OPAQUE_ENUM_A_ONE,		/**< When a transparent opaque attribute is set to A_ONE, it means the transparency information will be taken from the alpha channel of the color, texture, or parameter supplying the value. The value of 1.0 is opaque in this mode. */
	FX_OPAQUE_ENUM_RGB_ZERO,		/**< When a transparent opaque attribute is set to RGB_ZERO, it means the transparency information will be taken from the red, green, and blue channels of the color, texture, or parameter supplying the value. Each channel is modulated independently. The value of 0.0 is opaque in this mode. */
	FX_OPAQUE_ENUM_COUNT = 2
};

enum domFx_surface_type_enum {
	FX_SURFACE_TYPE_ENUM_UNTYPED,		/**< When a surface's type attribute is set to UNTYPED, its type is initially unknown and established later by the context in which it is used, such as by a texture sampler that references it. A surface of any other type may be changed into an UNTYPED surface at run-time, as if it were created by <newparam>, using <setparam>. If there is a type mismatch between a <setparam> operation and what the run-time decides the type should be, the result is profile- and platform-specific behavior. */
	FX_SURFACE_TYPE_ENUM_1D,
	FX_SURFACE_TYPE_ENUM_2D,
	FX_SURFACE_TYPE_ENUM_3D,
	FX_SURFACE_TYPE_ENUM_RECT,
	FX_SURFACE_TYPE_ENUM_CUBE,
	FX_SURFACE_TYPE_ENUM_DEPTH,
	FX_SURFACE_TYPE_ENUM_COUNT = 7
};

enum domFx_surface_face_enum {
	FX_SURFACE_FACE_ENUM_POSITIVE_X,
	FX_SURFACE_FACE_ENUM_NEGATIVE_X,
	FX_SURFACE_FACE_ENUM_POSITIVE_Y,
	FX_SURFACE_FACE_ENUM_NEGATIVE_Y,
	FX_SURFACE_FACE_ENUM_POSITIVE_Z,
	FX_SURFACE_FACE_ENUM_NEGATIVE_Z,
	FX_SURFACE_FACE_ENUM_COUNT = 6
};

/**
 * The per-texel layout of the format.  The length of the string indicate
 * how many channels there are and the letter respresents the name of the
 * channel.  There are typically 0 to 4 channels.
 */
enum domFx_surface_format_hint_channels_enum {
	FX_SURFACE_FORMAT_HINT_CHANNELS_ENUM_RGB,		/**< RGB color  map */
	FX_SURFACE_FORMAT_HINT_CHANNELS_ENUM_RGBA,		/**< RGB color + Alpha map often used for color + transparency or other things packed into channel A like specular power */
	FX_SURFACE_FORMAT_HINT_CHANNELS_ENUM_L,		/**< Luminance map often used for light mapping */
	FX_SURFACE_FORMAT_HINT_CHANNELS_ENUM_LA,		/**< Luminance+Alpha map often used for light mapping */
	FX_SURFACE_FORMAT_HINT_CHANNELS_ENUM_D,		/**< Depth map often used for displacement, parellax, relief, or shadow mapping */
	FX_SURFACE_FORMAT_HINT_CHANNELS_ENUM_XYZ,		/**< Typically used for normal maps or 3component displacement maps. */
	FX_SURFACE_FORMAT_HINT_CHANNELS_ENUM_XYZW,		/**< Typically used for normal maps where W is the depth for relief or parrallax mapping */
	FX_SURFACE_FORMAT_HINT_CHANNELS_ENUM_COUNT = 7
};

/**
 * Each channel of the texel has a precision.  Typically these are all linked
 * together.  An exact format lay lower the precision of an individual channel
 * but applying a higher precision by linking the channels together may still
 * convey the same information.
 */
enum domFx_surface_format_hint_precision_enum {
	FX_SURFACE_FORMAT_HINT_PRECISION_ENUM_LOW,		/**< For integers this typically represents 8 bits.  For floats typically 16 bits. */
	FX_SURFACE_FORMAT_HINT_PRECISION_ENUM_MID,		/**< For integers this typically represents 8 to 24 bits.  For floats typically 16 to 32 bits. */
	FX_SURFACE_FORMAT_HINT_PRECISION_ENUM_HIGH,		/**< For integers this typically represents 16 to 32 bits.  For floats typically 24 to 32 bits. */
	FX_SURFACE_FORMAT_HINT_PRECISION_ENUM_COUNT = 3
};

/**
 * Each channel represents a range of values. Some example ranges are signed
 * or unsigned integers, or between between a clamped range such as 0.0f to
 * 1.0f, or high dynamic range via floating point
 */
enum domFx_surface_format_hint_range_enum {
	FX_SURFACE_FORMAT_HINT_RANGE_ENUM_SNORM,		/**< Format is representing a decimal value that remains within the -1 to 1 range. Implimentation could be integer-fixedpoint or floats. */
	FX_SURFACE_FORMAT_HINT_RANGE_ENUM_UNORM,		/**< Format is representing a decimal value that remains within the 0 to 1 range. Implimentation could be integer-fixedpoint or floats. */
	FX_SURFACE_FORMAT_HINT_RANGE_ENUM_SINT,		/**< Format is representing signed integer numbers.  (ex. 8bits = -128 to 127) */
	FX_SURFACE_FORMAT_HINT_RANGE_ENUM_UINT,		/**< Format is representing unsigned integer numbers.  (ex. 8bits = 0 to 255) */
	FX_SURFACE_FORMAT_HINT_RANGE_ENUM_FLOAT,		/**< Format should support full floating point ranges.  High precision is expected to be 32bit. Mid precision may be 16 to 32 bit.  Low precision is expected to be 16 bit. */
	FX_SURFACE_FORMAT_HINT_RANGE_ENUM_COUNT = 5
};

/**
 * Additional hints about data relationships and other things to help the
 * application pick the best format.
 */
enum domFx_surface_format_hint_option_enum {
	FX_SURFACE_FORMAT_HINT_OPTION_ENUM_SRGB_GAMMA,		/**< colors are stored with respect to the sRGB 2.2 gamma curve rather than linear */
	FX_SURFACE_FORMAT_HINT_OPTION_ENUM_NORMALIZED3,		/**< the texel's XYZ/RGB should be normalized such as in a normal map. */
	FX_SURFACE_FORMAT_HINT_OPTION_ENUM_NORMALIZED4,		/**< the texel's XYZW/RGBA should be normalized such as in a normal map. */
	FX_SURFACE_FORMAT_HINT_OPTION_ENUM_COMPRESSABLE,		/**< The surface may use run-time compression.  Considering the best compression based on desired, channel, range, precision, and options */
	FX_SURFACE_FORMAT_HINT_OPTION_ENUM_COUNT = 4
};

enum domFx_sampler_wrap_common {
	FX_SAMPLER_WRAP_COMMON_NONE,
	FX_SAMPLER_WRAP_COMMON_WRAP,
	FX_SAMPLER_WRAP_COMMON_MIRROR,
	FX_SAMPLER_WRAP_COMMON_CLAMP,
	FX_SAMPLER_WRAP_COMMON_BORDER,
	FX_SAMPLER_WRAP_COMMON_COUNT = 5
};

enum domFx_sampler_filter_common {
	FX_SAMPLER_FILTER_COMMON_NONE,
	FX_SAMPLER_FILTER_COMMON_NEAREST,
	FX_SAMPLER_FILTER_COMMON_LINEAR,
	FX_SAMPLER_FILTER_COMMON_NEAREST_MIPMAP_NEAREST,
	FX_SAMPLER_FILTER_COMMON_LINEAR_MIPMAP_NEAREST,
	FX_SAMPLER_FILTER_COMMON_NEAREST_MIPMAP_LINEAR,
	FX_SAMPLER_FILTER_COMMON_LINEAR_MIPMAP_LINEAR,
	FX_SAMPLER_FILTER_COMMON_COUNT = 7
};

enum domFx_modifier_enum_common {
	FX_MODIFIER_ENUM_COMMON_CONST,
	FX_MODIFIER_ENUM_COMMON_UNIFORM,
	FX_MODIFIER_ENUM_COMMON_VARYING,
	FX_MODIFIER_ENUM_COMMON_STATIC,
	FX_MODIFIER_ENUM_COMMON_VOLATILE,
	FX_MODIFIER_ENUM_COMMON_EXTERN,
	FX_MODIFIER_ENUM_COMMON_SHARED,
	FX_MODIFIER_ENUM_COMMON_COUNT = 7
};

enum domFx_pipeline_stage_common {
	FX_PIPELINE_STAGE_COMMON_VERTEXPROGRAM,
	FX_PIPELINE_STAGE_COMMON_FRAGMENTPROGRAM,
	FX_PIPELINE_STAGE_COMMON_VERTEXSHADER,
	FX_PIPELINE_STAGE_COMMON_PIXELSHADER,
	FX_PIPELINE_STAGE_COMMON_COUNT = 4
};

enum domGl_blend_type {
	GL_BLEND_TYPE_ZERO = 0x0,
	GL_BLEND_TYPE_ONE = 0x1,
	GL_BLEND_TYPE_SRC_COLOR = 0x0300,
	GL_BLEND_TYPE_ONE_MINUS_SRC_COLOR = 0x0301,
	GL_BLEND_TYPE_DEST_COLOR = 0x0306,
	GL_BLEND_TYPE_ONE_MINUS_DEST_COLOR = 0x0307,
	GL_BLEND_TYPE_SRC_ALPHA = 0x0302,
	GL_BLEND_TYPE_ONE_MINUS_SRC_ALPHA = 0x0303,
	GL_BLEND_TYPE_DST_ALPHA = 0x0304,
	GL_BLEND_TYPE_ONE_MINUS_DST_ALPHA = 0x0305,
	GL_BLEND_TYPE_CONSTANT_COLOR = 0x8001,
	GL_BLEND_TYPE_ONE_MINUS_CONSTANT_COLOR = 0x8002,
	GL_BLEND_TYPE_CONSTANT_ALPHA = 0x8003,
	GL_BLEND_TYPE_ONE_MINUS_CONSTANT_ALPHA = 0x8004,
	GL_BLEND_TYPE_SRC_ALPHA_SATURATE = 0x0308,
	GL_BLEND_TYPE_COUNT = 15
};

enum domGl_face_type {
	GL_FACE_TYPE_FRONT = 0x0404,
	GL_FACE_TYPE_BACK = 0x0405,
	GL_FACE_TYPE_FRONT_AND_BACK = 0x0408,
	GL_FACE_TYPE_COUNT = 3
};

enum domGl_blend_equation_type {
	GL_BLEND_EQUATION_TYPE_FUNC_ADD = 0x8006,
	GL_BLEND_EQUATION_TYPE_FUNC_SUBTRACT = 0x800A,
	GL_BLEND_EQUATION_TYPE_FUNC_REVERSE_SUBTRACT = 0x800B,
	GL_BLEND_EQUATION_TYPE_MIN = 0x8007,
	GL_BLEND_EQUATION_TYPE_MAX = 0x8008,
	GL_BLEND_EQUATION_TYPE_COUNT = 5
};

enum domGl_func_type {
	GL_FUNC_TYPE_NEVER = 0x0200,
	GL_FUNC_TYPE_LESS = 0x0201,
	GL_FUNC_TYPE_LEQUAL = 0x0203,
	GL_FUNC_TYPE_EQUAL = 0x0202,
	GL_FUNC_TYPE_GREATER = 0x0204,
	GL_FUNC_TYPE_NOTEQUAL = 0x0205,
	GL_FUNC_TYPE_GEQUAL = 0x0206,
	GL_FUNC_TYPE_ALWAYS = 0x0207,
	GL_FUNC_TYPE_COUNT = 8
};

enum domGl_stencil_op_type {
	GL_STENCIL_OP_TYPE_KEEP = 0x1E00,
	GL_STENCIL_OP_TYPE_ZERO = 0x0,
	GL_STENCIL_OP_TYPE_REPLACE = 0x1E01,
	GL_STENCIL_OP_TYPE_INCR = 0x1E02,
	GL_STENCIL_OP_TYPE_DECR = 0x1E03,
	GL_STENCIL_OP_TYPE_INVERT = 0x150A,
	GL_STENCIL_OP_TYPE_INCR_WRAP = 0x8507,
	GL_STENCIL_OP_TYPE_DECR_WRAP = 0x8508,
	GL_STENCIL_OP_TYPE_COUNT = 8
};

enum domGl_material_type {
	GL_MATERIAL_TYPE_EMISSION = 0x1600,
	GL_MATERIAL_TYPE_AMBIENT = 0x1200,
	GL_MATERIAL_TYPE_DIFFUSE = 0x1201,
	GL_MATERIAL_TYPE_SPECULAR = 0x1202,
	GL_MATERIAL_TYPE_AMBIENT_AND_DIFFUSE = 0x1602,
	GL_MATERIAL_TYPE_COUNT = 5
};

enum domGl_fog_type {
	GL_FOG_TYPE_LINEAR = 0x2601,
	GL_FOG_TYPE_EXP = 0x0800,
	GL_FOG_TYPE_EXP2 = 0x0801,
	GL_FOG_TYPE_COUNT = 3
};

enum domGl_fog_coord_src_type {
	GL_FOG_COORD_SRC_TYPE_FOG_COORDINATE = 0x8451,
	GL_FOG_COORD_SRC_TYPE_FRAGMENT_DEPTH = 0x8452,
	GL_FOG_COORD_SRC_TYPE_COUNT = 2
};

enum domGl_front_face_type {
	GL_FRONT_FACE_TYPE_CW = 0x0900,
	GL_FRONT_FACE_TYPE_CCW = 0x0901,
	GL_FRONT_FACE_TYPE_COUNT = 2
};

enum domGl_light_model_color_control_type {
	GL_LIGHT_MODEL_COLOR_CONTROL_TYPE_SINGLE_COLOR = 0x81F9,
	GL_LIGHT_MODEL_COLOR_CONTROL_TYPE_SEPARATE_SPECULAR_COLOR = 0x81FA,
	GL_LIGHT_MODEL_COLOR_CONTROL_TYPE_COUNT = 2
};

enum domGl_logic_op_type {
	GL_LOGIC_OP_TYPE_CLEAR = 0x1500,
	GL_LOGIC_OP_TYPE_AND = 0x1501,
	GL_LOGIC_OP_TYPE_AND_REVERSE = 0x1502,
	GL_LOGIC_OP_TYPE_COPY = 0x1503,
	GL_LOGIC_OP_TYPE_AND_INVERTED = 0x1504,
	GL_LOGIC_OP_TYPE_NOOP = 0x1505,
	GL_LOGIC_OP_TYPE_XOR = 0x1506,
	GL_LOGIC_OP_TYPE_OR = 0x1507,
	GL_LOGIC_OP_TYPE_NOR = 0x1508,
	GL_LOGIC_OP_TYPE_EQUIV = 0x1509,
	GL_LOGIC_OP_TYPE_INVERT = 0x150A,
	GL_LOGIC_OP_TYPE_OR_REVERSE = 0x150B,
	GL_LOGIC_OP_TYPE_COPY_INVERTED = 0x150C,
	GL_LOGIC_OP_TYPE_NAND = 0x150E,
	GL_LOGIC_OP_TYPE_SET = 0x150F,
	GL_LOGIC_OP_TYPE_COUNT = 15
};

enum domGl_polygon_mode_type {
	GL_POLYGON_MODE_TYPE_POINT = 0x1B00,
	GL_POLYGON_MODE_TYPE_LINE = 0x1B01,
	GL_POLYGON_MODE_TYPE_FILL = 0x1B02,
	GL_POLYGON_MODE_TYPE_COUNT = 3
};

enum domGl_shade_model_type {
	GL_SHADE_MODEL_TYPE_FLAT = 0x1D00,
	GL_SHADE_MODEL_TYPE_SMOOTH = 0x1D01,
	GL_SHADE_MODEL_TYPE_COUNT = 2
};

enum domGlsl_pipeline_stage {
	GLSL_PIPELINE_STAGE_VERTEXPROGRAM,
	GLSL_PIPELINE_STAGE_FRAGMENTPROGRAM,
	GLSL_PIPELINE_STAGE_COUNT = 2
};

enum domCg_pipeline_stage {
	CG_PIPELINE_STAGE_VERTEX,
	CG_PIPELINE_STAGE_FRAGMENT,
	CG_PIPELINE_STAGE_COUNT = 2
};

enum domGles_texenv_mode_enums {
	GLES_TEXENV_MODE_ENUMS_REPLACE = 0x1E01,
	GLES_TEXENV_MODE_ENUMS_MODULATE = 0x2100,
	GLES_TEXENV_MODE_ENUMS_DECAL = 0x2101,
	GLES_TEXENV_MODE_ENUMS_BLEND = 0x0BE2,
	GLES_TEXENV_MODE_ENUMS_ADD = 0x0104,
	GLES_TEXENV_MODE_ENUMS_COUNT = 5
};

enum domGles_texcombiner_operatorRGB_enums {
	GLES_TEXCOMBINER_OPERATORRGB_ENUMS_REPLACE = 0x1E01,
	GLES_TEXCOMBINER_OPERATORRGB_ENUMS_MODULATE = 0x2100,
	GLES_TEXCOMBINER_OPERATORRGB_ENUMS_ADD = 0x0104,
	GLES_TEXCOMBINER_OPERATORRGB_ENUMS_ADD_SIGNED = 0x8574,
	GLES_TEXCOMBINER_OPERATORRGB_ENUMS_INTERPOLATE = 0x8575,
	GLES_TEXCOMBINER_OPERATORRGB_ENUMS_SUBTRACT = 0x84E7,
	GLES_TEXCOMBINER_OPERATORRGB_ENUMS_DOT3_RGB = 0x86AE,
	GLES_TEXCOMBINER_OPERATORRGB_ENUMS_DOT3_RGBA = 0x86AF,
	GLES_TEXCOMBINER_OPERATORRGB_ENUMS_COUNT = 8
};

enum domGles_texcombiner_operatorAlpha_enums {
	GLES_TEXCOMBINER_OPERATORALPHA_ENUMS_REPLACE = 0x1E01,
	GLES_TEXCOMBINER_OPERATORALPHA_ENUMS_MODULATE = 0x2100,
	GLES_TEXCOMBINER_OPERATORALPHA_ENUMS_ADD = 0x0104,
	GLES_TEXCOMBINER_OPERATORALPHA_ENUMS_ADD_SIGNED = 0x8574,
	GLES_TEXCOMBINER_OPERATORALPHA_ENUMS_INTERPOLATE = 0x8575,
	GLES_TEXCOMBINER_OPERATORALPHA_ENUMS_SUBTRACT = 0x84E7,
	GLES_TEXCOMBINER_OPERATORALPHA_ENUMS_COUNT = 6
};

enum domGles_texcombiner_source_enums {
	GLES_TEXCOMBINER_SOURCE_ENUMS_TEXTURE = 0x1702,
	GLES_TEXCOMBINER_SOURCE_ENUMS_CONSTANT = 0x8576,
	GLES_TEXCOMBINER_SOURCE_ENUMS_PRIMARY = 0x8577,
	GLES_TEXCOMBINER_SOURCE_ENUMS_PREVIOUS = 0x8578,
	GLES_TEXCOMBINER_SOURCE_ENUMS_COUNT = 4
};

enum domGles_texcombiner_operandRGB_enums {
	GLES_TEXCOMBINER_OPERANDRGB_ENUMS_SRC_COLOR = 0x0300,
	GLES_TEXCOMBINER_OPERANDRGB_ENUMS_ONE_MINUS_SRC_COLOR = 0x0301,
	GLES_TEXCOMBINER_OPERANDRGB_ENUMS_SRC_ALPHA = 0x0302,
	GLES_TEXCOMBINER_OPERANDRGB_ENUMS_ONE_MINUS_SRC_ALPHA = 0x0303,
	GLES_TEXCOMBINER_OPERANDRGB_ENUMS_COUNT = 4
};

enum domGles_texcombiner_operandAlpha_enums {
	GLES_TEXCOMBINER_OPERANDALPHA_ENUMS_SRC_ALPHA = 0x0302,
	GLES_TEXCOMBINER_OPERANDALPHA_ENUMS_ONE_MINUS_SRC_ALPHA = 0x0303,
	GLES_TEXCOMBINER_OPERANDALPHA_ENUMS_COUNT = 2
};

enum domGles_sampler_wrap {
	GLES_SAMPLER_WRAP_REPEAT,
	GLES_SAMPLER_WRAP_CLAMP,
	GLES_SAMPLER_WRAP_CLAMP_TO_EDGE,
	GLES_SAMPLER_WRAP_MIRRORED_REPEAT,		/**< supported by GLES 1.1 only */
	GLES_SAMPLER_WRAP_COUNT = 4
};

enum domGles_stencil_op_type {
	GLES_STENCIL_OP_TYPE_KEEP = 0x1E00,
	GLES_STENCIL_OP_TYPE_ZERO = 0x0,
	GLES_STENCIL_OP_TYPE_REPLACE = 0x1E01,
	GLES_STENCIL_OP_TYPE_INCR = 0x1E02,
	GLES_STENCIL_OP_TYPE_DECR = 0x1E03,
	GLES_STENCIL_OP_TYPE_INVERT = 0x150A,
	GLES_STENCIL_OP_TYPE_COUNT = 6
};

enum domSpringType {
	SPRINGTYPE_LINEAR,
	SPRINGTYPE_ANGULAR,
	SPRINGTYPE_COUNT = 2
};

enum domGl_enumeration {
	GL_ENUMERATION_ZERO = 0x0,
	GL_ENUMERATION_ONE = 0x1,
	GL_ENUMERATION_SRC_COLOR = 0x0300,
	GL_ENUMERATION_ONE_MINUS_SRC_COLOR = 0x0301,
	GL_ENUMERATION_DEST_COLOR = 0x0306,
	GL_ENUMERATION_ONE_MINUS_DEST_COLOR = 0x0307,
	GL_ENUMERATION_SRC_ALPHA = 0x0302,
	GL_ENUMERATION_ONE_MINUS_SRC_ALPHA = 0x0303,
	GL_ENUMERATION_DST_ALPHA = 0x0304,
	GL_ENUMERATION_ONE_MINUS_DST_ALPHA = 0x0305,
	GL_ENUMERATION_CONSTANT_COLOR = 0x8001,
	GL_ENUMERATION_ONE_MINUS_CONSTANT_COLOR = 0x8002,
	GL_ENUMERATION_CONSTANT_ALPHA = 0x8003,
	GL_ENUMERATION_ONE_MINUS_CONSTANT_ALPHA = 0x8004,
	GL_ENUMERATION_SRC_ALPHA_SATURATE = 0x0308,
	GL_ENUMERATION_FRONT = 0x0404,
	GL_ENUMERATION_BACK = 0x0405,
	GL_ENUMERATION_FRONT_AND_BACK = 0x0408,
	GL_ENUMERATION_FUNC_ADD = 0x8006,
	GL_ENUMERATION_FUNC_SUBTRACT = 0x800A,
	GL_ENUMERATION_FUNC_REVERSE_SUBTRACT = 0x800B,
	GL_ENUMERATION_MIN = 0x8007,
	GL_ENUMERATION_MAX = 0x8008,
	GL_ENUMERATION_NEVER = 0x0200,
	GL_ENUMERATION_LESS = 0x0201,
	GL_ENUMERATION_LEQUAL = 0x0203,
	GL_ENUMERATION_EQUAL = 0x0202,
	GL_ENUMERATION_GREATER = 0x0204,
	GL_ENUMERATION_NOTEQUAL = 0x0205,
	GL_ENUMERATION_GEQUAL = 0x0206,
	GL_ENUMERATION_ALWAYS = 0x0207,
	GL_ENUMERATION_KEEP = 0x1E00,
	GL_ENUMERATION_REPLACE = 0x1E01,
	GL_ENUMERATION_INCR = 0x1E02,
	GL_ENUMERATION_DECR = 0x1E03,
	GL_ENUMERATION_INVERT = 0x150A,
	GL_ENUMERATION_INCR_WRAP = 0x8507,
	GL_ENUMERATION_DECR_WRAP = 0x8508,
	GL_ENUMERATION_EMISSION = 0x1600,
	GL_ENUMERATION_AMBIENT = 0x1200,
	GL_ENUMERATION_DIFFUSE = 0x1201,
	GL_ENUMERATION_SPECULAR = 0x1202,
	GL_ENUMERATION_AMBIENT_AND_DIFFUSE = 0x1602,
	GL_ENUMERATION_LINEAR = 0x2601,
	GL_ENUMERATION_EXP = 0x0800,
	GL_ENUMERATION_EXP2 = 0x0801,
	GL_ENUMERATION_FOG_COORDINATE = 0x8451,
	GL_ENUMERATION_FRAGMENT_DEPTH = 0x8452,
	GL_ENUMERATION_CW = 0x0900,
	GL_ENUMERATION_CCW = 0x0901,
	GL_ENUMERATION_SINGLE_COLOR = 0x81F9,
	GL_ENUMERATION_SEPARATE_SPECULAR_COLOR = 0x81FA,
	GL_ENUMERATION_CLEAR = 0x1500,
	GL_ENUMERATION_AND = 0x1501,
	GL_ENUMERATION_AND_REVERSE = 0x1502,
	GL_ENUMERATION_COPY = 0x1503,
	GL_ENUMERATION_AND_INVERTED = 0x1504,
	GL_ENUMERATION_NOOP = 0x1505,
	GL_ENUMERATION_XOR = 0x1506,
	GL_ENUMERATION_OR = 0x1507,
	GL_ENUMERATION_NOR = 0x1508,
	GL_ENUMERATION_EQUIV = 0x1509,
	GL_ENUMERATION_OR_REVERSE = 0x150B,
	GL_ENUMERATION_COPY_INVERTED = 0x150C,
	GL_ENUMERATION_NAND = 0x150E,
	GL_ENUMERATION_SET = 0x150F,
	GL_ENUMERATION_POINT = 0x1B00,
	GL_ENUMERATION_LINE = 0x1B01,
	GL_ENUMERATION_FILL = 0x1B02,
	GL_ENUMERATION_FLAT = 0x1D00,
	GL_ENUMERATION_SMOOTH = 0x1D01,
	GL_ENUMERATION_COUNT = 72
};

enum domGles_enumeration {
	GLES_ENUMERATION_ZERO = 0x0,
	GLES_ENUMERATION_ONE = 0x1,
	GLES_ENUMERATION_SRC_COLOR = 0x0300,
	GLES_ENUMERATION_ONE_MINUS_SRC_COLOR = 0x0301,
	GLES_ENUMERATION_DEST_COLOR = 0x0306,
	GLES_ENUMERATION_ONE_MINUS_DEST_COLOR = 0x0307,
	GLES_ENUMERATION_SRC_ALPHA = 0x0302,
	GLES_ENUMERATION_ONE_MINUS_SRC_ALPHA = 0x0303,
	GLES_ENUMERATION_DST_ALPHA = 0x0304,
	GLES_ENUMERATION_ONE_MINUS_DST_ALPHA = 0x0305,
	GLES_ENUMERATION_CONSTANT_COLOR = 0x8001,
	GLES_ENUMERATION_ONE_MINUS_CONSTANT_COLOR = 0x8002,
	GLES_ENUMERATION_CONSTANT_ALPHA = 0x8003,
	GLES_ENUMERATION_ONE_MINUS_CONSTANT_ALPHA = 0x8004,
	GLES_ENUMERATION_SRC_ALPHA_SATURATE = 0x0308,
	GLES_ENUMERATION_FRONT = 0x0404,
	GLES_ENUMERATION_BACK = 0x0405,
	GLES_ENUMERATION_FRONT_AND_BACK = 0x0408,
	GLES_ENUMERATION_NEVER = 0x0200,
	GLES_ENUMERATION_LESS = 0x0201,
	GLES_ENUMERATION_LEQUAL = 0x0203,
	GLES_ENUMERATION_EQUAL = 0x0202,
	GLES_ENUMERATION_GREATER = 0x0204,
	GLES_ENUMERATION_NOTEQUAL = 0x0205,
	GLES_ENUMERATION_GEQUAL = 0x0206,
	GLES_ENUMERATION_ALWAYS = 0x0207,
	GLES_ENUMERATION_KEEP = 0x1E00,
	GLES_ENUMERATION_REPLACE = 0x1E01,
	GLES_ENUMERATION_INCR = 0x1E02,
	GLES_ENUMERATION_DECR = 0x1E03,
	GLES_ENUMERATION_INVERT = 0x150A,
	GLES_ENUMERATION_INCR_WRAP = 0x8507,
	GLES_ENUMERATION_DECR_WRAP = 0x8508,
	GLES_ENUMERATION_EMISSION = 0x1600,
	GLES_ENUMERATION_AMBIENT = 0x1200,
	GLES_ENUMERATION_DIFFUSE = 0x1201,
	GLES_ENUMERATION_SPECULAR = 0x1202,
	GLES_ENUMERATION_AMBIENT_AND_DIFFUSE = 0x1602,
	GLES_ENUMERATION_LINEAR = 0x2601,
	GLES_ENUMERATION_EXP = 0x0800,
	GLES_ENUMERATION_EXP2 = 0x0801,
	GLES_ENUMERATION_CW = 0x0900,
	GLES_ENUMERATION_CCW = 0x0901,
	GLES_ENUMERATION_SINGLE_COLOR = 0x81F9,
	GLES_ENUMERATION_SEPARATE_SPECULAR_COLOR = 0x81FA,
	GLES_ENUMERATION_CLEAR = 0x1500,
	GLES_ENUMERATION_AND = 0x1501,
	GLES_ENUMERATION_AND_REVERSE = 0x1502,
	GLES_ENUMERATION_COPY = 0x1503,
	GLES_ENUMERATION_AND_INVERTED = 0x1504,
	GLES_ENUMERATION_NOOP = 0x1505,
	GLES_ENUMERATION_XOR = 0x1506,
	GLES_ENUMERATION_OR = 0x1507,
	GLES_ENUMERATION_NOR = 0x1508,
	GLES_ENUMERATION_EQUIV = 0x1509,
	GLES_ENUMERATION_OR_REVERSE = 0x150B,
	GLES_ENUMERATION_COPY_INVERTED = 0x150C,
	GLES_ENUMERATION_NAND = 0x150E,
	GLES_ENUMERATION_SET = 0x150F,
	GLES_ENUMERATION_POINT = 0x1B00,
	GLES_ENUMERATION_LINE = 0x1B01,
	GLES_ENUMERATION_FILL = 0x1B02,
	GLES_ENUMERATION_FLAT = 0x1D00,
	GLES_ENUMERATION_SMOOTH = 0x1D01,
	GLES_ENUMERATION_COUNT = 65
};

//Element Type Enum
namespace COLLADA_TYPE
{
	enum TypeEnum
	{
		NO_TYPE = 0,
		INPUTGLOBAL,
		INPUTLOCAL,
		INPUTLOCALOFFSET,
		INSTANCEWITHEXTRA,
		TARGETABLEFLOAT,
		TARGETABLEFLOAT3,
		FX_SURFACE_FORMAT_HINT_COMMON,
		CHANNELS,
		RANGE,
		PRECISION,
		OPTION,
		FX_SURFACE_INIT_PLANAR_COMMON,
		ALL,
		FX_SURFACE_INIT_VOLUME_COMMON,
		PRIMARY,
		FX_SURFACE_INIT_CUBE_COMMON,
		ORDER,
		FACE,
		FX_SURFACE_INIT_FROM_COMMON,
		FX_SURFACE_COMMON,
		FORMAT,
		SIZE,
		VIEWPORT_RATIO,
		MIP_LEVELS,
		MIPMAP_GENERATE,
		FX_SAMPLER1D_COMMON,
		SOURCE,
		WRAP_S,
		MINFILTER,
		MAGFILTER,
		MIPFILTER,
		BORDER_COLOR,
		MIPMAP_MAXLEVEL,
		MIPMAP_BIAS,
		FX_SAMPLER2D_COMMON,
		WRAP_T,
		FX_SAMPLER3D_COMMON,
		WRAP_P,
		FX_SAMPLERCUBE_COMMON,
		FX_SAMPLERRECT_COMMON,
		FX_SAMPLERDEPTH_COMMON,
		FX_COLORTARGET_COMMON,
		FX_DEPTHTARGET_COMMON,
		FX_STENCILTARGET_COMMON,
		FX_CLEARCOLOR_COMMON,
		FX_CLEARDEPTH_COMMON,
		FX_CLEARSTENCIL_COMMON,
		FX_ANNOTATE_COMMON,
		FX_INCLUDE_COMMON,
		FX_NEWPARAM_COMMON,
		SEMANTIC,
		MODIFIER,
		FX_CODE_PROFILE,
		GL_SAMPLER1D,
		GL_SAMPLER2D,
		GL_SAMPLER3D,
		GL_SAMPLERCUBE,
		GL_SAMPLERRECT,
		GL_SAMPLERDEPTH,
		GLSL_NEWARRAY_TYPE,
		GLSL_SETARRAY_TYPE,
		GLSL_SURFACE_TYPE,
		GENERATOR,
		NAME,
		GLSL_NEWPARAM,
		GLSL_SETPARAM_SIMPLE,
		GLSL_SETPARAM,
		COMMON_FLOAT_OR_PARAM_TYPE,
		FLOAT,
		PARAM,
		COMMON_COLOR_OR_TEXTURE_TYPE,
		COLOR,
		TEXTURE,
		COMMON_TRANSPARENT_TYPE,
		COMMON_NEWPARAM_TYPE,
		FLOAT2,
		FLOAT3,
		FLOAT4,
		CG_SAMPLER1D,
		CG_SAMPLER2D,
		CG_SAMPLER3D,
		CG_SAMPLERCUBE,
		CG_SAMPLERRECT,
		CG_SAMPLERDEPTH,
		CG_CONNECT_PARAM,
		CG_NEWARRAY_TYPE,
		CG_SETARRAY_TYPE,
		CG_SETUSER_TYPE,
		CG_SURFACE_TYPE,
		CG_NEWPARAM,
		CG_SETPARAM_SIMPLE,
		CG_SETPARAM,
		GLES_TEXTURE_CONSTANT_TYPE,
		GLES_TEXENV_COMMAND_TYPE,
		GLES_TEXCOMBINER_ARGUMENTRGB_TYPE,
		GLES_TEXCOMBINER_ARGUMENTALPHA_TYPE,
		GLES_TEXCOMBINER_COMMANDRGB_TYPE,
		GLES_TEXCOMBINER_COMMANDALPHA_TYPE,
		GLES_TEXCOMBINER_COMMAND_TYPE,
		GLES_TEXTURE_PIPELINE,
		GLES_TEXTURE_UNIT,
		SURFACE,
		SAMPLER_STATE,
		TEXCOORD,
		GLES_SAMPLER_STATE,
		GLES_NEWPARAM,
		FX_SURFACE_INIT_COMMON,
		INIT_AS_NULL,
		INIT_AS_TARGET,
		FX_ANNOTATE_TYPE_COMMON,
		BOOL,
		BOOL2,
		BOOL3,
		BOOL4,
		INT,
		INT2,
		INT3,
		INT4,
		FLOAT2X2,
		FLOAT3X3,
		FLOAT4X4,
		STRING,
		FX_BASIC_TYPE_COMMON,
		FLOAT1X1,
		FLOAT1X2,
		FLOAT1X3,
		FLOAT1X4,
		FLOAT2X1,
		FLOAT2X3,
		FLOAT2X4,
		FLOAT3X1,
		FLOAT3X2,
		FLOAT3X4,
		FLOAT4X1,
		FLOAT4X2,
		FLOAT4X3,
		ENUM,
		GL_PIPELINE_SETTINGS,
		ALPHA_FUNC,
		FUNC,
		VALUE,
		BLEND_FUNC,
		SRC,
		DEST,
		BLEND_FUNC_SEPARATE,
		SRC_RGB,
		DEST_RGB,
		SRC_ALPHA,
		DEST_ALPHA,
		BLEND_EQUATION,
		BLEND_EQUATION_SEPARATE,
		RGB,
		ALPHA,
		COLOR_MATERIAL,
		MODE,
		CULL_FACE,
		DEPTH_FUNC,
		FOG_MODE,
		FOG_COORD_SRC,
		FRONT_FACE,
		LIGHT_MODEL_COLOR_CONTROL,
		LOGIC_OP,
		POLYGON_MODE,
		SHADE_MODEL,
		STENCIL_FUNC,
		REF,
		MASK,
		STENCIL_OP,
		FAIL,
		ZFAIL,
		ZPASS,
		STENCIL_FUNC_SEPARATE,
		FRONT,
		BACK,
		STENCIL_OP_SEPARATE,
		STENCIL_MASK_SEPARATE,
		LIGHT_ENABLE,
		LIGHT_AMBIENT,
		LIGHT_DIFFUSE,
		LIGHT_SPECULAR,
		LIGHT_POSITION,
		LIGHT_CONSTANT_ATTENUATION,
		LIGHT_LINEAR_ATTENUATION,
		LIGHT_QUADRATIC_ATTENUATION,
		LIGHT_SPOT_CUTOFF,
		LIGHT_SPOT_DIRECTION,
		LIGHT_SPOT_EXPONENT,
		TEXTURE1D,
		TEXTURE2D,
		TEXTURE3D,
		TEXTURECUBE,
		TEXTURERECT,
		TEXTUREDEPTH,
		TEXTURE1D_ENABLE,
		TEXTURE2D_ENABLE,
		TEXTURE3D_ENABLE,
		TEXTURECUBE_ENABLE,
		TEXTURERECT_ENABLE,
		TEXTUREDEPTH_ENABLE,
		TEXTURE_ENV_COLOR,
		TEXTURE_ENV_MODE,
		CLIP_PLANE,
		CLIP_PLANE_ENABLE,
		BLEND_COLOR,
		CLEAR_COLOR,
		CLEAR_STENCIL,
		CLEAR_DEPTH,
		COLOR_MASK,
		DEPTH_BOUNDS,
		DEPTH_MASK,
		DEPTH_RANGE,
		FOG_DENSITY,
		FOG_START,
		FOG_END,
		FOG_COLOR,
		LIGHT_MODEL_AMBIENT,
		LIGHTING_ENABLE,
		LINE_STIPPLE,
		LINE_WIDTH,
		MATERIAL_AMBIENT,
		MATERIAL_DIFFUSE,
		MATERIAL_EMISSION,
		MATERIAL_SHININESS,
		MATERIAL_SPECULAR,
		MODEL_VIEW_MATRIX,
		POINT_DISTANCE_ATTENUATION,
		POINT_FADE_THRESHOLD_SIZE,
		POINT_SIZE,
		POINT_SIZE_MIN,
		POINT_SIZE_MAX,
		POLYGON_OFFSET,
		PROJECTION_MATRIX,
		SCISSOR,
		STENCIL_MASK,
		ALPHA_TEST_ENABLE,
		AUTO_NORMAL_ENABLE,
		BLEND_ENABLE,
		COLOR_LOGIC_OP_ENABLE,
		COLOR_MATERIAL_ENABLE,
		CULL_FACE_ENABLE,
		DEPTH_BOUNDS_ENABLE,
		DEPTH_CLAMP_ENABLE,
		DEPTH_TEST_ENABLE,
		DITHER_ENABLE,
		FOG_ENABLE,
		LIGHT_MODEL_LOCAL_VIEWER_ENABLE,
		LIGHT_MODEL_TWO_SIDE_ENABLE,
		LINE_SMOOTH_ENABLE,
		LINE_STIPPLE_ENABLE,
		LOGIC_OP_ENABLE,
		MULTISAMPLE_ENABLE,
		NORMALIZE_ENABLE,
		POINT_SMOOTH_ENABLE,
		POLYGON_OFFSET_FILL_ENABLE,
		POLYGON_OFFSET_LINE_ENABLE,
		POLYGON_OFFSET_POINT_ENABLE,
		POLYGON_SMOOTH_ENABLE,
		POLYGON_STIPPLE_ENABLE,
		RESCALE_NORMAL_ENABLE,
		SAMPLE_ALPHA_TO_COVERAGE_ENABLE,
		SAMPLE_ALPHA_TO_ONE_ENABLE,
		SAMPLE_COVERAGE_ENABLE,
		SCISSOR_TEST_ENABLE,
		STENCIL_TEST_ENABLE,
		GLSL_PARAM_TYPE,
		CG_PARAM_TYPE,
		BOOL1,
		BOOL1X1,
		BOOL1X2,
		BOOL1X3,
		BOOL1X4,
		BOOL2X1,
		BOOL2X2,
		BOOL2X3,
		BOOL2X4,
		BOOL3X1,
		BOOL3X2,
		BOOL3X3,
		BOOL3X4,
		BOOL4X1,
		BOOL4X2,
		BOOL4X3,
		BOOL4X4,
		FLOAT1,
		INT1,
		INT1X1,
		INT1X2,
		INT1X3,
		INT1X4,
		INT2X1,
		INT2X2,
		INT2X3,
		INT2X4,
		INT3X1,
		INT3X2,
		INT3X3,
		INT3X4,
		INT4X1,
		INT4X2,
		INT4X3,
		INT4X4,
		HALF,
		HALF1,
		HALF2,
		HALF3,
		HALF4,
		HALF1X1,
		HALF1X2,
		HALF1X3,
		HALF1X4,
		HALF2X1,
		HALF2X2,
		HALF2X3,
		HALF2X4,
		HALF3X1,
		HALF3X2,
		HALF3X3,
		HALF3X4,
		HALF4X1,
		HALF4X2,
		HALF4X3,
		HALF4X4,
		FIXED,
		FIXED1,
		FIXED2,
		FIXED3,
		FIXED4,
		FIXED1X1,
		FIXED1X2,
		FIXED1X3,
		FIXED1X4,
		FIXED2X1,
		FIXED2X2,
		FIXED2X3,
		FIXED2X4,
		FIXED3X1,
		FIXED3X2,
		FIXED3X3,
		FIXED3X4,
		FIXED4X1,
		FIXED4X2,
		FIXED4X3,
		FIXED4X4,
		GLES_PIPELINE_SETTINGS,
		TEXTURE_PIPELINE,
		LIGHT_LINEAR_ATTENUTATION,
		TEXTURE_PIPELINE_ENABLE,
		GLES_BASIC_TYPE_COMMON,
		COLLADA,
		SCENE,
		IDREF_ARRAY,
		NAME_ARRAY,
		BOOL_ARRAY,
		FLOAT_ARRAY,
		INT_ARRAY,
		ACCESSOR,
		TECHNIQUE_COMMON,
		GEOMETRY,
		MESH,
		SPLINE,
		CONTROL_VERTICES,
		P,
		LINES,
		LINESTRIPS,
		POLYGONS,
		PH,
		H,
		POLYLIST,
		VCOUNT,
		TRIANGLES,
		TRIFANS,
		TRISTRIPS,
		VERTICES,
		LOOKAT,
		MATRIX,
		ROTATE,
		SCALE,
		SKEW,
		TRANSLATE,
		IMAGE,
		DATA,
		INIT_FROM,
		LIGHT,
		AMBIENT,
		DIRECTIONAL,
		POINT,
		SPOT,
		MATERIAL,
		CAMERA,
		OPTICS,
		ORTHOGRAPHIC,
		PERSPECTIVE,
		IMAGER,
		ANIMATION,
		ANIMATION_CLIP,
		CHANNEL,
		SAMPLER,
		CONTROLLER,
		SKIN,
		BIND_SHAPE_MATRIX,
		JOINTS,
		VERTEX_WEIGHTS,
		V,
		MORPH,
		TARGETS,
		ASSET,
		CONTRIBUTOR,
		AUTHOR,
		AUTHORING_TOOL,
		COMMENTS,
		COPYRIGHT,
		SOURCE_DATA,
		CREATED,
		KEYWORDS,
		MODIFIED,
		REVISION,
		SUBJECT,
		TITLE,
		UNIT,
		UP_AXIS,
		EXTRA,
		TECHNIQUE,
		NODE,
		VISUAL_SCENE,
		EVALUATE_SCENE,
		RENDER,
		LAYER,
		BIND_MATERIAL,
		INSTANCE_CAMERA,
		INSTANCE_CONTROLLER,
		SKELETON,
		INSTANCE_EFFECT,
		TECHNIQUE_HINT,
		SETPARAM,
		INSTANCE_FORCE_FIELD,
		INSTANCE_GEOMETRY,
		INSTANCE_LIGHT,
		INSTANCE_MATERIAL,
		BIND,
		BIND_VERTEX_INPUT,
		INSTANCE_NODE,
		INSTANCE_PHYSICS_MATERIAL,
		INSTANCE_PHYSICS_MODEL,
		INSTANCE_RIGID_BODY,
		ANGULAR_VELOCITY,
		VELOCITY,
		DYNAMIC,
		MASS_FRAME,
		SHAPE,
		HOLLOW,
		INSTANCE_RIGID_CONSTRAINT,
		LIBRARY_ANIMATIONS,
		LIBRARY_ANIMATION_CLIPS,
		LIBRARY_CAMERAS,
		LIBRARY_CONTROLLERS,
		LIBRARY_GEOMETRIES,
		LIBRARY_EFFECTS,
		LIBRARY_FORCE_FIELDS,
		LIBRARY_IMAGES,
		LIBRARY_LIGHTS,
		LIBRARY_MATERIALS,
		LIBRARY_NODES,
		LIBRARY_PHYSICS_MATERIALS,
		LIBRARY_PHYSICS_MODELS,
		LIBRARY_PHYSICS_SCENES,
		LIBRARY_VISUAL_SCENES,
		FX_PROFILE_ABSTRACT,
		EFFECT,
		GL_HOOK_ABSTRACT,
		PROFILE_GLSL,
		PASS,
		DRAW,
		SHADER,
		COMPILER_TARGET,
		COMPILER_OPTIONS,
		PROFILE_COMMON,
		CONSTANT,
		LAMBERT,
		PHONG,
		BLINN,
		PROFILE_CG,
		PROFILE_GLES,
		COLOR_TARGET,
		DEPTH_TARGET,
		STENCIL_TARGET,
		COLOR_CLEAR,
		DEPTH_CLEAR,
		STENCIL_CLEAR,
		BOX,
		HALF_EXTENTS,
		PLANE,
		EQUATION,
		SPHERE,
		RADIUS,
		ELLIPSOID,
		CYLINDER,
		HEIGHT,
		TAPERED_CYLINDER,
		RADIUS1,
		RADIUS2,
		CAPSULE,
		TAPERED_CAPSULE,
		CONVEX_MESH,
		FORCE_FIELD,
		PHYSICS_MATERIAL,
		PHYSICS_SCENE,
		RIGID_BODY,
		RIGID_CONSTRAINT,
		REF_ATTACHMENT,
		ATTACHMENT,
		ENABLED,
		INTERPENETRATE,
		LIMITS,
		SWING_CONE_AND_TWIST,
		LINEAR,
		SPRING,
		ANGULAR,
		PHYSICS_MODEL
	};
};

#endif
