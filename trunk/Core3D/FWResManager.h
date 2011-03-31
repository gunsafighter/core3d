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
	class FWApplication;
	class FWResManager;

	typedef UINT32 HRESOURCE;
	typedef void* (*PFN_LOADFUNCTION)(FWResManager*, tstring);
	typedef void  (*PFN_UNLOADFUNCTION)(FWResManager*, void*);
	class FWResManager
	{
	protected:
		friend class FWApplication;
	#	ifdef WIN32
		friend class FWApplicationWin32;
	#	endif

		FWResManager(FWApplication* pkApp);
		~FWResManager();

		bool			Initialize();
	public:
		void			RegisterResourceExtension(tstring strExtension, PFN_LOADFUNCTION pfnLoadFunction, PFN_UNLOADFUNCTION pfnUnloadFunction);
		HRESOURCE		LoadResource(tstring strFileName);
		void			ReleaseResource(HRESOURCE hRes);
		void*			GetResource(HRESOURCE hRes);
		FWApplication*	GetApplication();
	private:
		struct ManagedResource
		{
			HRESOURCE	hResource;
			UINT32		uiReferences;
			tstring		strFileName;
			tstring		strExtension;
			void*		pvResource;
		};
		std::vector<ManagedResource>::iterator GetManagedResourceIterator(HRESOURCE hRes);
	private:
		FWApplication*							m_pkApplication;
		std::map<tstring, PFN_LOADFUNCTION>		m_mapRegEntityExtensionsLoad;
		std::map<tstring, PFN_UNLOADFUNCTION>	m_mapRegEntityExtensionsUnload;
		std::vector<ManagedResource>			m_vecManagedResources;
		UINT32									m_uiNumLoadResources;
	};
}