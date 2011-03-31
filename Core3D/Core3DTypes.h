#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DBase.h"
#include "Core3DMath.h"

//------------------------------------------------------------------------
// COMMENT : WIN32 Version
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define CORE3D_ERROR(desc)		{::OutputDebugString(desc);}
#define CORE3D_NOTIFY(desc)		{::OutputDebugString(desc);}
#endif
//------------------------------------------------------------------------

#ifndef WIN32
#	ifdef _UNICODE
#		define __T(x)	L ## x
#	else
#		define __T(x)	x
#	endif
#	define _T(x)       __T(x)
#endif

#define CORE3D_SUCCESSFUL(res)	(Core3D::OK == res)
#define CORE3D_FAILED(res)		(Core3D::OK != res)
#define CORE3D_VERTEXFORMAT_DECL(iStream, iType, iRegister) \
	{iStream, iType, iRegister}

namespace Core3D
{
//------------------------------------------------------------------------
// COMMENT : WIN32 Version
	#ifdef WIN32
	typedef HWND	WinHandle;
	#endif
//------------------------------------------------------------------------
	typedef Vector4 ShaderReg;

	const UINT32 VERTEX_CACH_SIZE			= 32;
	const UINT32 VERTEX_SHADER_REGISTERS	= 8;
	const UINT32 PIXEL_SHADER_REGISTERS		= 8;
	const UINT32 NUM_SHADER_CONSTANTS		= 32;
	const UINT32 MAX_VERTEX_STREAMS			= 8;
	const UINT32 MAX_TEXTURE_SAMPLERS		= 16;

	enum RenderState
	{
		RS_ZENABLE = 0,
		RS_ZWRITEENABLE,
		RS_ZFUNC,

		RS_COLORWRITEENABLE,
		RS_FILLMODE,
		RS_CULLMODE,

		RS_SUBDIVISIONMODE,
		RS_SUBDIVISIONLEVELS,
		RS_SUBDIVISIONPOSITIONREGISTER,
		RS_SUBDIVISIONNORMALREGISTER,
		RS_SUBDIVISIONMAXSCREENAREA,
		RS_SUBDIVISIONMAXINNERLEVELS,

		RS_SCISSORTESTENABLE,
		RS_LINETHICKNESS,

		RS_NUMRENDERSTATES
	};

	enum CmpFunc
	{
		CMP_NEVER = 0,
		CMP_EQUAL,
		CMP_NOTEQUAL,
		CMP_LESS,
		CMP_LESSEQUAL,
		CMP_GREATEREQUAL,
		CMP_GREATER,
		CMP_ALWAYS
	};

	enum Cull
	{
		CULL_NONE = 0,
		CULL_CW,
		CULL_CCW
	};

	enum Fill
	{
		FILL_SOLID = 0,
		FILL_WIREFRAME
	};

	enum TextureSamplerState
	{
		TSS_ADDRESSU = 0,
		TSS_ADDRESSV,
		TSS_ADDRESSW,
		TSS_MINFILTER,
		TSS_MAGFILTER,
		TSS_MIPFILTER,
		TSS_MIPLODBIAS,
		TSS_MAXMIPLEVEL,

		TSS_NUMTEXTURESAMPLERSTATES
	};

	enum TextureAddress
	{
		TA_WRAP = 0,
		TA_CLAMP
	};

	enum TextureFilter
	{
		TF_POINT = 0,
		TF_LINEAR
	};

	enum SubDiv
	{
		SUBDIV_NONE = 0,
		SUBDIV_SIMPLE,
		SUBDIV_SMOOTH,
		SUBDIV_ADAPTIVE
	};

	enum Format
	{
		FMT_R32F = 0,
		FMT_R32G32F,
		FMT_R32G32B32F,
		FMT_R32G32B32A32F,

		FMT_INDEX16,
		FMT_INDEX32
	};

	enum PrimitiveType
	{
		PT_TRIANGLEFAN = 0,
		PT_TRIANGLESTRIP,
		PT_TRIANGLELIST
	};

	enum VertexElementType
	{
		VET_FLOAT32 = 0,
		VET_VECTOR2,
		VET_VECTOR3,
		VET_VECTOR4
	};

	enum ShaderConstant
	{
		SC_WORLDMATRIX = 0,
		SC_VIEWMATRIX,
		SC_PROJECTIONMATRIX,
		SC_WVPMATRIX
	};

	enum TextureSampleInput
	{
		TSI_2COORDS = 0,
		TSI_3COORDS,
		TSI_VECTOR
	};

	enum CubeFaces
	{
		CF_POSITIVE_X = 0,
		CF_NEGATIVE_X,
		CF_POSITIVE_Y,
		CF_NEGATIVE_Y,
		CF_POSITIVE_Z,
		CF_NEGATIVE_Z
	};

	enum PixelShaderOutput
	{
		PSO_COLORONLY = 0,
		PSO_COLORDEPTH
	};

	enum ShaderRegType
	{
		SRT_UNUSED = 0,
		SRT_FLOAT32,
		SRT_VECTOR2,
		SRT_VECTOR3,
		SRT_VECTOR4
	};

	enum ClippingPlanes
	{
		CP_LEFT = 0,
		CP_RIGHT,
		CP_TOP,
		CP_BOTTOM,
		CP_NEAR,
		CP_FAR,

		CP_USER0,
		CP_USER1,
		CP_USER2,
		CP_USER3,

		CP_NUMPLANES
	};

	//////////////////////////////////////////////////////////////////////////
	// Structures
	//////////////////////////////////////////////////////////////////////////
	// COMMENT : Defines a rectangle.
	struct Rect
	{
		UINT32		uiLeft, uiTop;
		UINT32		uiRight, uiBottom;
	};

	// COMMENT : Defines a box. Added for volume texture support.
	struct Box
	{
		UINT32		uiLeft, uiTop, uiFront;
		UINT32		uiRight, uiBottom, uiBack;
	};

	// COMMENT : This structure defines the device parameters.
	struct DeviceParameters
	{
		WinHandle	hDeviceWnd;								// Handle to the output window
		bool		bWindowed;								// True if the application runs windowed, false if it runs in full screen
		UINT32		uiFullScreenColorBits;					// Bit depth of back-buffer in full screen mode(ignored in windowed mode). Valid values : 32, 24, 16.
		UINT32		uiBackBufferWidth, uiBackBufferHeight;	// Dimension of the back-buffer in pixels.
	};

	// COMMENT : Describes a vertex element.
	struct VertexElement
	{
		UINT32				uiStream;		// Index of the stream this element is loaded from.
		VertexElementType	eType;			// Type of this vertex element. Set this field to a member of the enumeration type.
		UINT32				uiRegister;		// The register of the vertex shader the vertex element's value will be passed to.
	};

	//////////////////////////////////////////////////////////////////////////
	// Internal structures
	//////////////////////////////////////////////////////////////////////////
	// COMMENT : Describes the vertex shader input.
	// this structure is used internally by devices.
	struct VertexShaderInput
	{
		ShaderReg	kShaderInputs[VERTEX_SHADER_REGISTERS];
	};

	// COMMENT : Describes the vertex shader output.
	// this structure is used internally by devices.
	struct VertexShaderOutput
	{
		ShaderReg			kShaderOutputs[PIXEL_SHADER_REGISTERS];	// Vertex shader output registers, which are in turn used as pixel
																	// shader input registers.
		Vector4				kPosition;								// Position of this vertex.
		VertexShaderInput	kSourceInput;							// Original vertex shader input fetched from vertex streams: 
																	// added for triangle subdivision.
	};

	// COMMENT : Describes a structure that is used for triangle gradient storage.
	// this structure is used internally by devices.
	struct TriangleInfo
	{
		FLOAT32		fCommonGradient;								// Gradient constant.
		const VertexShaderOutput* pkBaseVertex;						// Base vertex for gradient computations.
		FLOAT32		fZDdx, fZDdy;									// Z partial derivatives with respect to the screen space 
																	// X and Y coordinates.
		FLOAT32		fWDdx, fWDdy;									// W partial derivatives with respect to the screen space
																	// X and Y coordinates.
		ShaderReg	kShaderOutputsDdx[PIXEL_SHADER_REGISTERS];		// Shader register partial derivatives with respect to the
																	// screen space X coordinates.
		ShaderReg	kShaderOutputsDdy[PIXEL_SHADER_REGISTERS];		// Shader register partial derivatives with respect to the
																	// screen space Y coordinates.
		UINT32		uiCurrentPixelX, uiCurrentPixelY;				// Integer coordinates of current pixel:
																	// needed by pixel shader for computation of partial derivatives.
		FLOAT32		fCurrentPixelInvW;								// 1.0f / W of the current pixel:
																	// needed by pixel shader for computation of partial derivatives.
	};

	// COMMENT : Describes a structure that is used for vertex caching.
	// this structure is used internally by devices.
	struct VertexCacheEntry
	{
		UINT32				uiVertexIndex;	// Index of the contained vertex in the vertex buffer.
		VertexShaderOutput	kVertexOutput;	// Vertex shader output, vertex data.
		UINT32				uiFetchTime;	// Whenever a vertex cache entry is reserved for drawing(updated or simply 'touched
											// and returned') it's fetch-time is set to m_uiFetchedVertices.
	};
}