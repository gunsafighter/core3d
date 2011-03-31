#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Framework Library for Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////
#include "FWType.h"
#include <map>
#include <vector>

namespace Core3D
{
	class FWEntity;
	class FWLight;
	class FWScene;

	// COMMENT : Implement scene graph
	typedef UINT32 HENTITY;
	typedef UINT32 HLIGHT;
	typedef FWEntity* (*PFN_CREATEFUNCTION)(FWScene*);
	class FWScene
	{
	protected:
		friend class FWApplication;
	#	ifdef WIN32
		friend class FWApplicationWin32;
	#	endif

		FWScene(FWApplication* pkApp);
		~FWScene();

		bool			Initialize();
		void			FrameMove();
	public:
		void			RegisterEntityType(tstring strTypeName, PFN_CREATEFUNCTION pfnCreateFunction);
		
		HENTITY			CreateEntity(tstring strTypeName, bool bSceneProcess = true);
		FWEntity*		GetEntity(HENTITY hEntity);
		void			ReleaseEntity(HENTITY hEntity);

		HLIGHT			CreateLight();
		FWLight*		GetLight(HLIGHT hLight);
		void			ReleaseLight(HLIGHT hLight);

		void			Render(UINT32 uiPass);
		FWApplication*	GetApplication();

		inline void		SetClearColor(Vector4 kClearColor)				{m_kClearColor = kClearColor;}
		inline const Vector4& GetClearColor()					const	{return m_kClearColor;}

		inline void		SetAmbientLightColor(Vector4 kAmbientLightColor){m_kAmbientLightColor = kAmbientLightColor;}
		inline const Vector4& GetAmbientLightColor()			const	{return m_kAmbientLightColor;}

		inline UINT32	GetNumLights()							const	{return static_cast<UINT32>(m_vecSceneLights.size());}
		FWLight*		GetLightFromNum(UINT32 uiNum);

		inline void		SetCurrentLight(UINT32 uiNum)					{m_uiCurrentLight = uiNum;}
		FWLight*		GetCurrentLight();
	private:
		struct SceneEntity
		{
			HENTITY		hEntity;
			FWEntity*	pkEntity;
			bool		bSceneProcess;
		};

		struct SceneLight
		{
			HLIGHT		hLight;
			FWLight*	pkLight;
		};
		std::vector<SceneEntity>::iterator	GetSceneEntityIterator(HENTITY hEntity);
		std::vector<SceneLight>::iterator	GetSceneLightIterator(HLIGHT hLight);
	private:
		FWApplication*				m_pkApplication;
		Vector4						m_kClearColor;
		Vector4						m_kAmbientLightColor;
		std::map<tstring, PFN_CREATEFUNCTION> m_mapRegEntityTypes;
		std::vector<SceneEntity>	m_vecSceneEntities;
		UINT32						m_uiNumCreatedEntities;
		std::vector<SceneLight>		m_vecSceneLights;
		UINT32						m_uiNumCreatedLights;
		UINT32						m_uiCurrentLight;
	};
}