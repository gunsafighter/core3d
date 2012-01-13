#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Framework Library for Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////
#include "FWType.h"

namespace Core3D
{
	class FWResManager;
	class FWObjModel
	{
	public:
		struct VertexData
		{
			Vector3 kPosition;
			Vector3 kNormal;
			Vector2 kTexCoord0;
			Vector3 kTangent;
		};
	protected:
		friend void*	LoadObjModel(FWResManager*, tstring);
		friend void		UnloadObjModel(FWResManager*, void*);

		FWObjModel(FWResManager* pkResMgr);
		~FWObjModel();

	public:
		bool			LoadModel(const char* pData);
		FWResManager*	GetResManager();

		inline UINT32			GetNumFaces()		const	{return m_uiNumFaces;}
		inline UINT32			GetNumVertices()	const	{return 3 * m_uiNumFaces;}
		inline UINT32			GetStride()			const	{return static_cast<UINT32>(sizeof(FWObjModel::VertexData));}
		inline VertexFormat*	GetVertexFormat()			{return m_pkVertexFormat;}
		inline VertexBuffer*	GetVertexBuffer()			{return m_pkVertexBuffer;}
	private:
		FWResManager*	m_pkResManager;
		UINT32			m_uiNumFaces;
		VertexFormat*	m_pkVertexFormat;
		VertexBuffer*	m_pkVertexBuffer;
	};
}