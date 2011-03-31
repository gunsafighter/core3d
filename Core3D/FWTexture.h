#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Framework Library for Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////
#include "FWType.h"

namespace Core3D
{
	class FWResManager;
	class FWTexture
	{
	protected:
		friend void*	LoadTexture(FWResManager*, tstring);
		friend void*	LoadCubeTexture(FWResManager*, tstring);
		friend void*	LoadAnimatedTexture(FWResManager*, tstring);
		friend void		UnloadTexture(FWResManager*, void*);

		FWTexture(FWResManager* pkResMgr, Texture* pkTexture);
		FWTexture(FWResManager* pkResMgr, CubeTexture* pkTexture);
		FWTexture(FWResManager* pkResMgr, UINT32 uiNumTextures, FLOAT32 fFPS, Texture** ppkTexture);
		~FWTexture();
	public:
		enum TextureType
		{
			TEXTURETYPE_DEFAULT,
			TEXTURETYPE_CUBE
		};
	public:
		FWResManager*	GetResManager();
		BaseTexture*	GetTexture();
		TextureType		GetTextureType() const;
	private:
		FWResManager*	m_pkResManager;
		UINT32			m_uiNumTextures;
		Texture**		m_ppkTextures;
		CubeTexture*	m_pkCubeTexture;
		FLOAT32			m_fFPS;
		FLOAT32			m_fCurrentTexture;
		TextureType		m_eTextureType;
	};
}