#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DTypes.h"	// COMMENT : Include Core3DBase.h and Core3DMath.h.
#include "Core3DCore.h"		// COMMENT : Include CoreLib header files.

//------------------------------------------------------------------------
typedef Core3D::INT8				C3DINT8;
typedef Core3D::INT16				C3DINT16;
typedef Core3D::INT32				C3DINT32;

typedef Core3D::UINT8				C3DUINT8;
typedef Core3D::UINT16				C3DUINT16;
typedef Core3D::UINT32				C3DUINT32;

typedef Core3D::FLOAT32				C3DFLOAT32;
typedef Core3D::FLOAT64				C3DFLOAT64;

typedef Core3D::BYTE8				C3DBYTE8;
//------------------------------------------------------------------------

//------------------------------------------------------------------------
typedef Core3D::Vector2				C3DVECTOR2;
typedef Core3D::Vector3				C3DVECTOR3;
typedef Core3D::Vector4				C3DVECTOR4;
typedef Core3D::Matrix4x4			C3DMATRIX;
typedef Core3D::Quaternion			C3DQUATERNION;
typedef Core3D::Plane				C3DPLANE;
//------------------------------------------------------------------------

//------------------------------------------------------------------------
typedef Core3D::BaseShader			CORE3DBASESHADER;
typedef CORE3DBASESHADER*			LPCORE3DBASESHADER;

typedef Core3D::BaseTexture			CORE3DBASETEXTURE;
typedef CORE3DBASETEXTURE*			LPCORE3DBASETEXTURE;

typedef Core3D::CubeTexture			CORE3DCUBETEXTURE;
typedef CORE3DCUBETEXTURE*			LPCORE3DCUBETEXTURE;

typedef Core3D::Device				CORE3DDEVICE;
typedef CORE3DDEVICE*				LPCORE3DDEVICE;

typedef Core3D::Object				CORE3DOBJECT;
typedef CORE3DOBJECT*				LPCORE3DOBJECT;

typedef Core3D::IndexBuffer			CORE3DINDEXBUFFER;
typedef CORE3DINDEXBUFFER*			LPCORE3DINDEXBUFFER;

typedef Core3D::PresentTarget		CORE3DPRESENTTARGET;
typedef CORE3DPRESENTTARGET*		LPCORE3DPRESENTTARGET;

typedef Core3D::PrimitiveAssembler	CORE3DPRIMITIVEASSEMBLER;
typedef CORE3DPRIMITIVEASSEMBLER*	LPCORE3DPRIMITIVEASSEMBLER;

typedef Core3D::RenderTarget		CORE3DRENDERTARGET;
typedef CORE3DRENDERTARGET*			LPCORE3DRENDERTARGET;

typedef Core3D::VertexShader		CORE3DVERTEXSHADER;
typedef CORE3DVERTEXSHADER*			LPCORE3DVERTEXSHADER;

typedef Core3D::TriangleShader		CORE3DTRIANGLESHADER;
typedef CORE3DTRIANGLESHADER*		LPCORE3DTRIANGLESHADER;

typedef Core3D::PixelShader			CORE3DPIXELSHADER;
typedef CORE3DPIXELSHADER*			LPCORE3DPIXELSHADER;

typedef Core3D::Surface				CORE3DSURFACE;
typedef CORE3DSURFACE*				LPCORE3DSURFACE;

typedef Core3D::Texture				CORE3DTEXTURE;
typedef CORE3DTEXTURE*				LPCORE3DTEXTURE;

typedef Core3D::VertexBuffer		CORE3DVERTEXBUFFER;
typedef CORE3DVERTEXBUFFER*			LPCORE3DVERTEXBUFFER;

typedef Core3D::VertexFormat		CORE3DVERTEXFORMAT;
typedef CORE3DVERTEXFORMAT*			LPCORE3DVERTEXFORMAT;

typedef Core3D::Volume				CORE3DVOLUME;
typedef CORE3DVOLUME*				LPCORE3DVOLUME;

typedef Core3D::VolumeTexture		CORE3DVOLUMETEXTURE;
typedef CORE3DVOLUMETEXTURE*		LPCORE3DVOLUMETEXTURE;
//------------------------------------------------------------------------