#include "VideoDevice.h"
#include <stdio.h>
#include <SDL_main.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

const Color Color::Black(0.0f, 0.0f, 0.0f, 1.0f);
const Color Color::Blue(0.0f, 0.0f, 0.5f, 1.0f);
const Color Color::Green(0.0f, 0.5f, 0.0f, 1.0f);
const Color Color::Cyan(0.0f, 0.5f, 0.5f, 1.0f);
const Color Color::Red(0.5f, 0.0f, 0.0f, 1.0f);
const Color Color::Mangenta(0.5f, 0.0f, 0.5f, 1.0f);
const Color Color::Brown(0.5f, 0.5f, 0.0f, 1.0f);
const Color Color::Grey(0.5f, 0.5f, 0.5f, 1.0f);
const Color Color::BrightBlue(0.0f, 0.0f, 1.0f, 1.0f);
const Color Color::BrightGreen(0.0f, 1.0f, 0.0f, 1.0f);
const Color Color::BrightCyan(0.0f, 1.0f, 1.0f, 1.0f);
const Color Color::BrightRed(1.0f, 0.0f, 0.0f, 1.0f);
const Color Color::BrightMangenta(1.0f, 0.0f, 1.0f, 1.0f);
const Color Color::Yellow(1.0f, 1.0f, 0.0f, 1.0f);
const Color Color::White(1.0f, 1.0f, 1.0f, 1.0f);

GLenum GetGLEnum(VideoDevice::Constant constant)
{
	static const vector<GLenum> glEnums =
	{
		GL_TRUE,
		GL_FALSE,
		GL_STENCIL_BUFFER_BIT,
		GL_COLOR_BUFFER_BIT,

		GL_PROJECTION,
		GL_MODELVIEW,

		GL_POINTS,
		GL_LINES,
		GL_LINE_LOOP,
		GL_LINE_STRIP,
		GL_TRIANGLES,
		GL_TRIANGLE_STRIP,
		GL_TRIANGLE_FAN,
		GL_ZERO,
		GL_ONE,
		GL_SRC_COLOR,
		GL_ONE_MINUS_SRC_COLOR,
		GL_SRC_ALPHA,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_DST_ALPHA,
		GL_ONE_MINUS_DST_ALPHA,
		GL_DST_COLOR,
		GL_ONE_MINUS_DST_COLOR,
		GL_SRC_ALPHA_SATURATE,
		GL_FUNC_ADD,
		GL_BLEND_EQUATION,
		GL_BLEND_EQUATION_RGB,
		GL_BLEND_EQUATION_ALPHA,
		GL_FUNC_SUBTRACT,
		GL_FUNC_REVERSE_SUBTRACT,
		GL_BLEND_DST_RGB,
		GL_BLEND_SRC_RGB,
		GL_BLEND_DST_ALPHA,
		GL_BLEND_SRC_ALPHA,
		GL_CONSTANT_COLOR,
		GL_ONE_MINUS_CONSTANT_COLOR,
		GL_CONSTANT_ALPHA,
		GL_ONE_MINUS_CONSTANT_ALPHA,
		GL_BLEND_COLOR,
		GL_ARRAY_BUFFER,
		GL_ELEMENT_ARRAY_BUFFER,
		GL_ARRAY_BUFFER_BINDING,
		GL_ELEMENT_ARRAY_BUFFER_BINDING,
		GL_STREAM_DRAW,
		GL_STATIC_DRAW,
		GL_DYNAMIC_DRAW,
		GL_BUFFER_SIZE,
		GL_BUFFER_USAGE,
		GL_CURRENT_VERTEX_ATTRIB,
		GL_FRONT,
		GL_BACK,
		GL_FRONT_AND_BACK,
		GL_CULL_FACE,
		GL_BLEND,
		GL_DITHER,
		GL_STENCIL_TEST,
		GL_DEPTH_TEST,
		GL_SCISSOR_TEST,
		GL_POLYGON_OFFSET_FILL,
		GL_SAMPLE_ALPHA_TO_COVERAGE,
		GL_SAMPLE_COVERAGE,
		GL_NO_ERROR,
		GL_INVALID_ENUM,
		GL_INVALID_VALUE,
		GL_INVALID_OPERATION,
		GL_OUT_OF_MEMORY,
		GL_CW,
		GL_CCW,
		GL_LINE_WIDTH,
		GL_ALIASED_POINT_SIZE_RANGE,
		GL_ALIASED_LINE_WIDTH_RANGE,
		GL_CULL_FACE_MODE,
		GL_FRONT_FACE,
		GL_DEPTH_RANGE,
		GL_DEPTH_WRITEMASK,
		GL_DEPTH_CLEAR_VALUE,
		GL_DEPTH_FUNC,
		GL_STENCIL_CLEAR_VALUE,
		GL_STENCIL_FUNC,
		GL_STENCIL_FAIL,
		GL_STENCIL_PASS_DEPTH_FAIL,
		GL_STENCIL_PASS_DEPTH_PASS,
		GL_STENCIL_REF,
		GL_STENCIL_VALUE_MASK,
		GL_STENCIL_WRITEMASK,
		GL_STENCIL_BACK_FUNC,
		GL_STENCIL_BACK_FAIL,
		GL_STENCIL_BACK_PASS_DEPTH_FAIL,
		GL_STENCIL_BACK_PASS_DEPTH_PASS,
		GL_STENCIL_BACK_REF,
		GL_STENCIL_BACK_VALUE_MASK,
		GL_STENCIL_BACK_WRITEMASK,
		GL_VIEWPORT,
		GL_SCISSOR_BOX,
		GL_COLOR_CLEAR_VALUE,
		GL_COLOR_WRITEMASK,
		GL_UNPACK_ALIGNMENT,
		GL_PACK_ALIGNMENT,
		GL_MAX_TEXTURE_SIZE,
		GL_MAX_VIEWPORT_DIMS,
		GL_SUBPIXEL_BITS,
		GL_RED_BITS,
		GL_GREEN_BITS,
		GL_BLUE_BITS,
		GL_ALPHA_BITS,
		GL_DEPTH_BITS,
		GL_STENCIL_BITS,
		GL_POLYGON_OFFSET_UNITS,
		GL_POLYGON_OFFSET_FACTOR,
		GL_TEXTURE_BINDING_2D,
		GL_SAMPLE_BUFFERS,
		GL_SAMPLES,
		GL_SAMPLE_COVERAGE_VALUE,
		GL_SAMPLE_COVERAGE_INVERT,
		GL_COMPRESSED_TEXTURE_FORMATS,
		GL_DONT_CARE,
		GL_FASTEST,
		GL_NICEST,
		GL_GENERATE_MIPMAP_HINT,
		GL_BYTE,
		GL_UNSIGNED_BYTE,
		GL_SHORT,
		GL_UNSIGNED_SHORT,
		GL_INT,
		GL_UNSIGNED_INT,
		GL_FLOAT,
		GL_DEPTH_COMPONENT,
		GL_ALPHA,
		GL_RGB,
		GL_RGBA,
		GL_LUMINANCE,
		GL_LUMINANCE_ALPHA,
		GL_UNSIGNED_SHORT_4_4_4_4,
		GL_UNSIGNED_SHORT_5_5_5_1,
		GL_UNSIGNED_SHORT_5_6_5,
		GL_FRAGMENT_SHADER,
		GL_VERTEX_SHADER,
		GL_MAX_VERTEX_ATTRIBS,
		GL_MAX_VERTEX_UNIFORM_VECTORS,
		GL_MAX_VARYING_VECTORS,
		GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
		GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
		GL_MAX_TEXTURE_IMAGE_UNITS,
		GL_MAX_FRAGMENT_UNIFORM_VECTORS,
		GL_SHADER_TYPE,
		GL_DELETE_STATUS,
		GL_LINK_STATUS,
		GL_VALIDATE_STATUS,
		GL_ATTACHED_SHADERS,
		GL_ACTIVE_UNIFORMS,
		GL_ACTIVE_ATTRIBUTES,
		GL_SHADING_LANGUAGE_VERSION,
		GL_CURRENT_PROGRAM,
		GL_NEVER,
		GL_LESS,
		GL_EQUAL,
		GL_LEQUAL,
		GL_GREATER,
		GL_NOTEQUAL,
		GL_GEQUAL,
		GL_ALWAYS,
		GL_KEEP,
		GL_REPLACE,
		GL_INCR,
		GL_DECR,
		GL_INVERT,
		GL_INCR_WRAP,
		GL_DECR_WRAP,
		GL_VENDOR,
		GL_RENDERER,
		GL_VERSION,
		GL_NEAREST,
		GL_LINEAR,
		GL_NEAREST_MIPMAP_NEAREST,
		GL_LINEAR_MIPMAP_NEAREST,
		GL_NEAREST_MIPMAP_LINEAR,
		GL_LINEAR_MIPMAP_LINEAR,
		GL_TEXTURE_MAG_FILTER,
		GL_TEXTURE_MIN_FILTER,
		GL_TEXTURE_WRAP_S,
		GL_TEXTURE_WRAP_T,
		GL_TEXTURE_2D,
		GL_TEXTURE,
		GL_TEXTURE_CUBE_MAP,
		GL_TEXTURE_BINDING_CUBE_MAP,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		GL_MAX_CUBE_MAP_TEXTURE_SIZE,
		GL_TEXTURE0,
		GL_TEXTURE1,
		GL_TEXTURE2,
		GL_TEXTURE3,
		GL_TEXTURE4,
		GL_TEXTURE5,
		GL_TEXTURE6,
		GL_TEXTURE7,
		GL_TEXTURE8,
		GL_TEXTURE9,
		GL_TEXTURE10,
		GL_TEXTURE11,
		GL_TEXTURE12,
		GL_TEXTURE13,
		GL_TEXTURE14,
		GL_TEXTURE15,
		GL_TEXTURE16,
		GL_TEXTURE17,
		GL_TEXTURE18,
		GL_TEXTURE19,
		GL_TEXTURE20,
		GL_TEXTURE21,
		GL_TEXTURE22,
		GL_TEXTURE23,
		GL_TEXTURE24,
		GL_TEXTURE25,
		GL_TEXTURE26,
		GL_TEXTURE27,
		GL_TEXTURE28,
		GL_TEXTURE29,
		GL_TEXTURE30,
		GL_TEXTURE31,
		GL_ACTIVE_TEXTURE,
		GL_REPEAT,
		GL_CLAMP_TO_EDGE,
		GL_MIRRORED_REPEAT,
		GL_FLOAT_VEC2,
		GL_FLOAT_VEC3,
		GL_FLOAT_VEC4,
		GL_INT_VEC2,
		GL_INT_VEC3,
		GL_INT_VEC4,
		GL_BOOL,
		GL_BOOL_VEC2,
		GL_BOOL_VEC3,
		GL_BOOL_VEC4,
		GL_FLOAT_MAT2,
		GL_FLOAT_MAT3,
		GL_FLOAT_MAT4,
		GL_SAMPLER_2D,
		GL_SAMPLER_CUBE,
		GL_VERTEX_ATTRIB_ARRAY_ENABLED,
		GL_VERTEX_ATTRIB_ARRAY_SIZE,
		GL_VERTEX_ATTRIB_ARRAY_STRIDE,
		GL_VERTEX_ATTRIB_ARRAY_TYPE,
		GL_VERTEX_ATTRIB_ARRAY_NORMALIZED,
		GL_VERTEX_ATTRIB_ARRAY_POINTER,
		GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING,
		GL_IMPLEMENTATION_COLOR_READ_TYPE,
		GL_IMPLEMENTATION_COLOR_READ_FORMAT,
		GL_COMPILE_STATUS,
		GL_LOW_FLOAT,
		GL_MEDIUM_FLOAT,
		GL_HIGH_FLOAT,
		GL_LOW_INT,
		GL_MEDIUM_INT,
		GL_HIGH_INT,
		GL_FRAMEBUFFER,
		GL_RENDERBUFFER,
		GL_RGBA4,
		GL_RGB5_A1,
		GL_RGB565,
		GL_DEPTH_COMPONENT16,
		GL_STENCIL_INDEX,
		GL_STENCIL_INDEX8,
		GL_DEPTH_STENCIL,
		GL_RENDERBUFFER_WIDTH,
		GL_RENDERBUFFER_HEIGHT,
		GL_RENDERBUFFER_INTERNAL_FORMAT,
		GL_RENDERBUFFER_RED_SIZE,
		GL_RENDERBUFFER_GREEN_SIZE,
		GL_RENDERBUFFER_BLUE_SIZE,
		GL_RENDERBUFFER_ALPHA_SIZE,
		GL_RENDERBUFFER_DEPTH_SIZE,
		GL_RENDERBUFFER_STENCIL_SIZE,
		GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE,
		GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
		GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL,
		GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE,
		GL_COLOR_ATTACHMENT0,
		GL_DEPTH_ATTACHMENT,
		GL_STENCIL_ATTACHMENT,
		GL_DEPTH_STENCIL_ATTACHMENT,
		GL_NONE,
		GL_FRAMEBUFFER_COMPLETE,
		GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT,
		GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT,
		// GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS,
		GL_FRAMEBUFFER_UNSUPPORTED,
		GL_FRAMEBUFFER_BINDING,
		GL_RENDERBUFFER_BINDING,
		GL_MAX_RENDERBUFFER_SIZE,
		GL_INVALID_FRAMEBUFFER_OPERATION,
		// GL_UNPACK_FLIP_Y_WEBGL,
		// GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL,
		// GL_CONTEXT_LOST_WEBGL,
		// GL_UNPACK_COLORSPACE_CONVERSION_WEBGL,
		// GL_BROWSER_DEFAULT_WEBGL,
		GL_READ_BUFFER,
		GL_UNPACK_ROW_LENGTH,
		GL_UNPACK_SKIP_ROWS,
		GL_UNPACK_SKIP_PIXELS,
		GL_PACK_ROW_LENGTH,
		GL_PACK_SKIP_ROWS,
		GL_PACK_SKIP_PIXELS,
		GL_COLOR,
		GL_DEPTH,
		GL_STENCIL,
		GL_RED,
		GL_RGB8,
		GL_RGBA8,
		GL_RGB10_A2,
		GL_TEXTURE_BINDING_3D,
		GL_UNPACK_SKIP_IMAGES,
		GL_UNPACK_IMAGE_HEIGHT,
		GL_TEXTURE_3D,
		GL_TEXTURE_WRAP_R,
		GL_MAX_3D_TEXTURE_SIZE,
		GL_UNSIGNED_INT_2_10_10_10_REV,
		GL_MAX_ELEMENTS_VERTICES,
		GL_MAX_ELEMENTS_INDICES,
		GL_TEXTURE_MIN_LOD,
		GL_TEXTURE_MAX_LOD,
		GL_TEXTURE_BASE_LEVEL,
		GL_TEXTURE_MAX_LEVEL,
		GL_MIN,
		GL_MAX,
		GL_DEPTH_COMPONENT24,
		GL_MAX_TEXTURE_LOD_BIAS,
		GL_TEXTURE_COMPARE_MODE,
		GL_TEXTURE_COMPARE_FUNC,
		GL_CURRENT_QUERY,
		GL_QUERY_RESULT,
		GL_QUERY_RESULT_AVAILABLE,
		GL_STREAM_READ,
		GL_STREAM_COPY,
		GL_STATIC_READ,
		GL_STATIC_COPY,
		GL_DYNAMIC_READ,
		GL_DYNAMIC_COPY,
		GL_MAX_DRAW_BUFFERS,
		GL_DRAW_BUFFER0,
		GL_DRAW_BUFFER1,
		GL_DRAW_BUFFER2,
		GL_DRAW_BUFFER3,
		GL_DRAW_BUFFER4,
		GL_DRAW_BUFFER5,
		GL_DRAW_BUFFER6,
		GL_DRAW_BUFFER7,
		GL_DRAW_BUFFER8,
		GL_DRAW_BUFFER9,
		GL_DRAW_BUFFER10,
		GL_DRAW_BUFFER11,
		GL_DRAW_BUFFER12,
		GL_DRAW_BUFFER13,
		GL_DRAW_BUFFER14,
		GL_DRAW_BUFFER15,
		GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,
		GL_MAX_VERTEX_UNIFORM_COMPONENTS,
		GL_SAMPLER_3D,
		GL_SAMPLER_2D_SHADOW,
		GL_FRAGMENT_SHADER_DERIVATIVE_HINT,
		GL_PIXEL_PACK_BUFFER,
		GL_PIXEL_UNPACK_BUFFER,
		GL_PIXEL_PACK_BUFFER_BINDING,
		GL_PIXEL_UNPACK_BUFFER_BINDING,
		GL_FLOAT_MAT2x3,
		GL_FLOAT_MAT2x4,
		GL_FLOAT_MAT3x2,
		GL_FLOAT_MAT3x4,
		GL_FLOAT_MAT4x2,
		GL_FLOAT_MAT4x3,
		GL_SRGB,
		GL_SRGB8,
		GL_SRGB8_ALPHA8,
		GL_COMPARE_REF_TO_TEXTURE,
		GL_RGBA32F,
		GL_RGB32F,
		GL_RGBA16F,
		GL_RGB16F,
		GL_VERTEX_ATTRIB_ARRAY_INTEGER,
		GL_MAX_ARRAY_TEXTURE_LAYERS,
		GL_MIN_PROGRAM_TEXEL_OFFSET,
		GL_MAX_PROGRAM_TEXEL_OFFSET,
		GL_MAX_VARYING_COMPONENTS,
		GL_TEXTURE_2D_ARRAY,
		GL_TEXTURE_BINDING_2D_ARRAY,
		GL_R11F_G11F_B10F,
		GL_UNSIGNED_INT_10F_11F_11F_REV,
		GL_RGB9_E5,
		GL_UNSIGNED_INT_5_9_9_9_REV,
		GL_TRANSFORM_FEEDBACK_BUFFER_MODE,
		GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS,
		GL_TRANSFORM_FEEDBACK_VARYINGS,
		GL_TRANSFORM_FEEDBACK_BUFFER_START,
		GL_TRANSFORM_FEEDBACK_BUFFER_SIZE,
		GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,
		GL_RASTERIZER_DISCARD,
		GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS,
		GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS,
		GL_INTERLEAVED_ATTRIBS,
		GL_SEPARATE_ATTRIBS,
		GL_TRANSFORM_FEEDBACK_BUFFER,
		GL_TRANSFORM_FEEDBACK_BUFFER_BINDING,
		GL_RGBA32UI,
		GL_RGB32UI,
		GL_RGBA16UI,
		GL_RGB16UI,
		GL_RGBA8UI,
		GL_RGB8UI,
		GL_RGBA32I,
		GL_RGB32I,
		GL_RGBA16I,
		GL_RGB16I,
		GL_RGBA8I,
		GL_RGB8I,
		GL_RED_INTEGER,
		GL_RGB_INTEGER,
		GL_RGBA_INTEGER,
		GL_SAMPLER_2D_ARRAY,
		GL_SAMPLER_2D_ARRAY_SHADOW,
		GL_SAMPLER_CUBE_SHADOW,
		GL_UNSIGNED_INT_VEC2,
		GL_UNSIGNED_INT_VEC3,
		GL_UNSIGNED_INT_VEC4,
		GL_INT_SAMPLER_2D,
		GL_INT_SAMPLER_3D,
		GL_INT_SAMPLER_CUBE,
		GL_INT_SAMPLER_2D_ARRAY,
		GL_UNSIGNED_INT_SAMPLER_2D,
		GL_UNSIGNED_INT_SAMPLER_3D,
		GL_UNSIGNED_INT_SAMPLER_CUBE,
		GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,
		GL_DEPTH_COMPONENT32F,
		GL_DEPTH32F_STENCIL8,
		GL_FLOAT_32_UNSIGNED_INT_24_8_REV,
		GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING,
		GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE,
		GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE,
		GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE,
		GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE,
		GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE,
		GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE,
		GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE,
		GL_FRAMEBUFFER_DEFAULT,
		GL_UNSIGNED_INT_24_8,
		GL_DEPTH24_STENCIL8,
		GL_UNSIGNED_NORMALIZED,
		GL_DRAW_FRAMEBUFFER_BINDING,
		GL_READ_FRAMEBUFFER,
		GL_DRAW_FRAMEBUFFER,
		GL_READ_FRAMEBUFFER_BINDING,
		GL_RENDERBUFFER_SAMPLES,
		GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER,
		GL_MAX_COLOR_ATTACHMENTS,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3,
		GL_COLOR_ATTACHMENT4,
		GL_COLOR_ATTACHMENT5,
		GL_COLOR_ATTACHMENT6,
		GL_COLOR_ATTACHMENT7,
		GL_COLOR_ATTACHMENT8,
		GL_COLOR_ATTACHMENT9,
		GL_COLOR_ATTACHMENT10,
		GL_COLOR_ATTACHMENT11,
		GL_COLOR_ATTACHMENT12,
		GL_COLOR_ATTACHMENT13,
		GL_COLOR_ATTACHMENT14,
		GL_COLOR_ATTACHMENT15,
		GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE,
		GL_MAX_SAMPLES,
		GL_HALF_FLOAT,
		GL_RG,
		GL_RG_INTEGER,
		GL_R8,
		GL_RG8,
		GL_R16F,
		GL_R32F,
		GL_RG16F,
		GL_RG32F,
		GL_R8I,
		GL_R8UI,
		GL_R16I,
		GL_R16UI,
		GL_R32I,
		GL_R32UI,
		GL_RG8I,
		GL_RG8UI,
		GL_RG16I,
		GL_RG16UI,
		GL_RG32I,
		GL_RG32UI,
		GL_VERTEX_ARRAY_BINDING,
		GL_R8_SNORM,
		GL_RG8_SNORM,
		GL_RGB8_SNORM,
		GL_RGBA8_SNORM,
		GL_SIGNED_NORMALIZED,
		GL_COPY_READ_BUFFER,
		GL_COPY_WRITE_BUFFER,
		GL_COPY_READ_BUFFER_BINDING,
		GL_COPY_WRITE_BUFFER_BINDING,
		GL_UNIFORM_BUFFER,
		GL_UNIFORM_BUFFER_BINDING,
		GL_UNIFORM_BUFFER_START,
		GL_UNIFORM_BUFFER_SIZE,
		GL_MAX_VERTEX_UNIFORM_BLOCKS,
		GL_MAX_FRAGMENT_UNIFORM_BLOCKS,
		GL_MAX_COMBINED_UNIFORM_BLOCKS,
		GL_MAX_UNIFORM_BUFFER_BINDINGS,
		GL_MAX_UNIFORM_BLOCK_SIZE,
		GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,
		GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS,
		GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,
		GL_ACTIVE_UNIFORM_BLOCKS,
		GL_UNIFORM_TYPE,
		GL_UNIFORM_SIZE,
		GL_UNIFORM_BLOCK_INDEX,
		GL_UNIFORM_OFFSET,
		GL_UNIFORM_ARRAY_STRIDE,
		GL_UNIFORM_MATRIX_STRIDE,
		GL_UNIFORM_IS_ROW_MAJOR,
		GL_UNIFORM_BLOCK_BINDING,
		GL_UNIFORM_BLOCK_DATA_SIZE,
		GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,
		GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,
		GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER,
		GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER,
		GL_INVALID_INDEX,
		GL_MAX_VERTEX_OUTPUT_COMPONENTS,
		GL_MAX_FRAGMENT_INPUT_COMPONENTS,
		GL_MAX_SERVER_WAIT_TIMEOUT,
		GL_OBJECT_TYPE,
		GL_SYNC_CONDITION,
		GL_SYNC_STATUS,
		GL_SYNC_FLAGS,
		GL_SYNC_FENCE,
		GL_SYNC_GPU_COMMANDS_COMPLETE,
		GL_UNSIGNALED,
		GL_SIGNALED,
		GL_ALREADY_SIGNALED,
		GL_TIMEOUT_EXPIRED,
		GL_CONDITION_SATISFIED,
		GL_WAIT_FAILED,
		GL_SYNC_FLUSH_COMMANDS_BIT,
		GL_VERTEX_ATTRIB_ARRAY_DIVISOR,
		GL_ANY_SAMPLES_PASSED,
		GL_ANY_SAMPLES_PASSED_CONSERVATIVE,
		GL_SAMPLER_BINDING,
		GL_RGB10_A2UI,
		GL_INT_2_10_10_10_REV,
		GL_TRANSFORM_FEEDBACK,
		GL_TRANSFORM_FEEDBACK_PAUSED,
		GL_TRANSFORM_FEEDBACK_ACTIVE,
		GL_TRANSFORM_FEEDBACK_BINDING,
		GL_TEXTURE_IMMUTABLE_FORMAT,
		GL_MAX_ELEMENT_INDEX,
		GL_TEXTURE_IMMUTABLE_LEVELS,
		// GL_TIMEOUT_IGNORED,
		// GL_MAX_CLIENT_WAIT_TIMEOUT_WEBGL,

		// GL_QUERY_COUNTER_BITS_EXT,
		GL_TIME_ELAPSED_EXT,
		// GL_TIMESTAMP_EXT,
		// GL_GPU_DISJOINT_EXT,

		GL_COMPRESSED_RGB_S3TC_DXT1_EXT,
		GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
		GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,
		GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,

		GL_COMPRESSED_SRGB_S3TC_DXT1_EXT,
		GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,
		GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,
		GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,

		GL_COMPRESSED_R11_EAC,
		GL_COMPRESSED_SIGNED_R11_EAC,
		GL_COMPRESSED_RG11_EAC,
		GL_COMPRESSED_SIGNED_RG11_EAC,
		GL_COMPRESSED_RGB8_ETC2,
		GL_COMPRESSED_SRGB8_ETC2,
		GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
		GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
		GL_COMPRESSED_RGBA8_ETC2_EAC,
		GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,

		// GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,
		// GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,
		// GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,
		// GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,

		GL_COMPRESSED_RGBA_ASTC_4x4_KHR,
		GL_COMPRESSED_RGBA_ASTC_5x4_KHR,
		GL_COMPRESSED_RGBA_ASTC_5x5_KHR,
		GL_COMPRESSED_RGBA_ASTC_6x5_KHR,
		GL_COMPRESSED_RGBA_ASTC_6x6_KHR,
		GL_COMPRESSED_RGBA_ASTC_8x5_KHR,
		GL_COMPRESSED_RGBA_ASTC_8x6_KHR,
		GL_COMPRESSED_RGBA_ASTC_8x8_KHR,
		GL_COMPRESSED_RGBA_ASTC_10x5_KHR,
		GL_COMPRESSED_RGBA_ASTC_10x6_KHR,
		GL_COMPRESSED_RGBA_ASTC_10x8_KHR,
		GL_COMPRESSED_RGBA_ASTC_10x10_KHR,
		GL_COMPRESSED_RGBA_ASTC_12x10_KHR,
		GL_COMPRESSED_RGBA_ASTC_12x12_KHR,
		GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,
		GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,
		GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,
		GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,
		GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,
		GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,
		GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,
		GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,
		GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,
		GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,
		GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,
		GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,
		GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,
		GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,
		0,
		0,
		0
	};

	return glEnums[((int)constant) + 1];
}

////////////////////////////////////////////////////////////////////
Texture2D::Texture2D()
: handle(0)
, width(0)
, height(0)
, nrComponents(0)
{
}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &handle);
}

int Texture2D::GetWidth() const
{
	return width;
}

int Texture2D::GetHeight() const
{
	return height;
}

void Texture2D::Load(const char* path_)
{
	int nrComponents;
	void *data = stbi_load(path_, &width, &height, &nrComponents, 0);
	stbi__vertical_flip(data, width, height, nrComponents * 1);

	if (data)
	{
		glGenTextures(1, &handle);

		glBindTexture(GL_TEXTURE_2D, handle);

		if(nrComponents==3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if(nrComponents==4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);
	
		stbi_image_free(data);
	}
}

void Texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

//////////////////////////////////////////////////////////////////////////////////
class VideoDeviceImpl
{
public:
	SDL_Window* window;
	SDL_GLContext glContext;
};

VideoDevice::VideoDevice()
{
	impl = new VideoDeviceImpl();
}

VideoDevice::~VideoDevice()
{
	if (impl)
	{
		delete impl;
		impl = 0;
	}
}

boolean VideoDevice::Open(const string& name_, u32 x_, u32 y_, u32 width_, u32 height_)
{
	// Create impl->window
	impl->window = SDL_CreateWindow(name_.c_str(), x_, y_, width_, height_, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (impl->window == NULL)
	{
		// Display error message
		//	printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return FALSE;
	}

	// Create OpenGL context
	impl->glContext = SDL_GL_CreateContext(impl->window);
	if (impl->glContext == NULL)
	{
		// Display error message
		//printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return FALSE;
	}

	SDL_GL_SetSwapInterval(1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	return TRUE;
}

void VideoDevice::Close()
{
	if (impl->window)
	{
		SDL_GL_DeleteContext(impl->glContext);
		impl->glContext = NULL;

		SDL_DestroyWindow(impl->window);
		impl->window = NULL;
	}
}

void VideoDevice::MakeCurrent()
{
	SDL_GL_MakeCurrent(impl->window, impl->glContext);
}

boolean VideoDevice::IsCurrent()
{
	SDL_GLContext currentGLContext = SDL_GL_GetCurrentContext();

	return currentGLContext == impl->glContext;
}

void VideoDevice::Flush()
{
	SDL_GL_SwapWindow(impl->window);
}

void VideoDevice::Enable(VideoDevice::Constant cap)
{
	glEnable(GetGLEnum(cap));
}

void VideoDevice::Disable(VideoDevice::Constant cap)
{
	glDisable(GetGLEnum(cap));
}

void VideoDevice::Clear(const Color& c)
{
	glClearColor(c.r, c.g, c.b, c.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void VideoDevice::BlendFunc(VideoDevice::Constant sfactor, VideoDevice::Constant dfactor)
{
	glBlendFunc(GetGLEnum(sfactor), GetGLEnum(dfactor));
}

void VideoDevice::MatrixMode(VideoDevice::Constant mode)
{
	glMatrixMode(GetGLEnum(mode));
}

void VideoDevice::LoadIdentity()
{
	glLoadIdentity();
}

void VideoDevice::Ortho2D(int left, int right, int bottom, int top)
{
	gluOrtho2D(left, right, bottom, top);
}

void VideoDevice::Viewport(int x_, int y_, int width_, int height_)
{
	glViewport(x_, y_, width_, height_);

	glEnable(GL_SCISSOR_TEST);
	glScissor(x_, y_, width_, height_);
}

void VideoDevice::DrawPoint(const Vector2& v, const Color& c)
{
	glDisable(GL_TEXTURE_2D);
	
	glBegin(GL_POINTS);

	glColor4f(c.r, c.g, c.b, c.a);
	glVertex2fv((f32*)&v);
	
	glEnd();
}

void VideoDevice::DrawLine(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1)
{
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);

	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glEnd();
}

void VideoDevice::DrawWireTriangle(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1, const Vector2& v2, const Color& c2)
{
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINE_LOOP);

	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glEnd();
}

void VideoDevice::DrawWireRectangle(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1, const Vector2& v2, const Color& c2, const Vector2& v3, const Color& c3)
{
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINE_LOOP);

	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glColor4fv((f32*)&c3);
	glVertex2fv((f32*)&v3);

	glEnd();
}

void VideoDevice::DrawWireCircle(const Vector2& center, f32 radius, const Color& c)
{
}

void VideoDevice::DrawSolidTriangle(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1, const Vector2& v2, const Color& c2)
{
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_TRIANGLES);

	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glEnd();
}

void VideoDevice::DrawSolidRectangle(const Vector2& v0, const Color& c0, const Vector2& v1, const Color& c1, const Vector2& v2, const Color& c2, const Vector2& v3, const Color& c3)
{
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);

	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glColor4fv((f32*)&c3);
	glVertex2fv((f32*)&v3);

	glEnd();
}

void VideoDevice::DrawSolidCircle(const Vector2& center, f32 radius, const Color& c)
{
}

void VideoDevice::DrawPrimitive(u32 primitive, const Vector2* vertices, const Color* colors, u32 count)
{
	glDisable(GL_TEXTURE_2D);

	glVertexPointer(2, GL_FLOAT, 0, vertices);
	glColorPointer(4, GL_FLOAT, 0, colors);

	glDrawArrays(primitive, 0, count);
}

/////////////////////////////////////////////////////////////////////////////////////////////
void VideoDevice::DrawTexPoint(const Texture2D& texture, const Vector2& v, const Color& c, const Vector2& t)
{
	glEnable(GL_TEXTURE_2D);
	texture.Bind();

	glTexCoord2fv((f32*)&t);
	glColor4fv((f32*)&c);
	glBegin(GL_POINTS);
	glVertex2fv((f32*)&v);
	glEnd();
}

void VideoDevice::DrawTexLine(const Texture2D& texture, const Vector2& v0, const Color& c0, const Vector2& t0, const Vector2& v1, const Color& c1, const Vector2& t1)
{
	glEnable(GL_TEXTURE_2D);
	texture.Bind();

	glBegin(GL_LINES);

	glTexCoord2fv((f32*)&t0);
	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glTexCoord2fv((f32*)&t1);
	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glEnd();
}

void VideoDevice::DrawTexWireTriangle(const Texture2D& texture, const Vector2& v0, const Color& c0, const Vector2& t0, const Vector2& v1, const Color& c1, const Vector2& t1, const Vector2& v2, const Color& c2, const Vector2& t2)
{
	glEnable(GL_TEXTURE_2D);
	texture.Bind();

	glBegin(GL_LINE_LOOP);

	glTexCoord2fv((f32*)&t0);
	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glTexCoord2fv((f32*)&t1);
	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glTexCoord2fv((f32*)&t2);
	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glEnd();
}

void VideoDevice::DrawTexWireRectangle(const Texture2D& texture, 
										const Vector2& v0, const Color& c0, const Vector2& t0, 
										const Vector2& v1, const Color& c1, const Vector2& t1,
										const Vector2& v2, const Color& c2, const Vector2& t2,
										const Vector2& v3, const Color& c3, const Vector2& t3)
{
	glEnable(GL_TEXTURE_2D);
	texture.Bind();

	glBegin(GL_LINES);

	glTexCoord2fv((f32*)&t0);
	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glTexCoord2fv((f32*)&t1);
	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glTexCoord2fv((f32*)&t2);
	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glTexCoord2fv((f32*)&t3);
	glColor4fv((f32*)&c3);
	glVertex2fv((f32*)&v3);

	glEnd();
}

void VideoDevice::DrawTexSolidTriangle(const Texture2D& texture,
										const Vector2& v0, const Color& c0, const Vector2& t0,
										const Vector2& v1, const Color& c1, const Vector2& t1,
										const Vector2& v2, const Color& c2, const Vector2& t2)
{
	glEnable(GL_TEXTURE_2D);
	texture.Bind();

	glBegin(GL_TRIANGLES);

	glTexCoord2fv((f32*)&t0);
	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glTexCoord2fv((f32*)&t1);
	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glTexCoord2fv((f32*)&t2);
	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glEnd();
}


void VideoDevice::DrawTexSolidRectangle(const Texture2D& texture,
	const Vector2& v0, const Color& c0, const Vector2& t0,
	const Vector2& v1, const Color& c1, const Vector2& t1,
	const Vector2& v2, const Color& c2, const Vector2& t2,
	const Vector2& v3, const Color& c3, const Vector2& t3)
{
	glEnable(GL_TEXTURE_2D);
	texture.Bind();

	glBegin(GL_QUADS);

	glTexCoord2fv((f32*)&t0);
	glColor4fv((f32*)&c0);
	glVertex2fv((f32*)&v0);

	glTexCoord2fv((f32*)&t1);
	glColor4fv((f32*)&c1);
	glVertex2fv((f32*)&v1);

	glTexCoord2fv((f32*)&t2);
	glColor4fv((f32*)&c2);
	glVertex2fv((f32*)&v2);

	glTexCoord2fv((f32*)&t3);
	glColor4fv((f32*)&c3);
	glVertex2fv((f32*)&v3);

	glEnd();
}

void VideoDevice::DrawTexPrimitive(const Texture2D& texture, u32 primitive, const Vector2* v, const Color* c, const Vector2* t, u32 count)
{
	glEnable(GL_TEXTURE_2D);
	texture.Bind();

	glVertexPointer(2, GL_FLOAT, 0, v);
	glColorPointer(4, GL_FLOAT, 0, c);
	glTexCoordPointer(2, GL_FLOAT, 0, t);

	glDrawArrays(primitive, 0, count);
}