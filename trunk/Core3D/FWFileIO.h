#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Framework Library for Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////
#include "FWType.h"

#ifdef WIN32
#	define DATA_PATH _T("./data")
#endif

namespace Core3D
{
	class FWApplication;
	class FWFileIO
	{
	protected:
		friend class FWApplication;
	#	ifdef WIN32
		friend class FWApplicationWin32;
	#	endif

		FWFileIO(FWApplication* pkApp);
		~FWFileIO();
		
		bool	Initialize();
	public:
		UINT32	ReadFile(tstring strFileName, BYTE8** ppData, bool bText = false);
		UINT32	ReadFile(tstring strFileName, BYTE8* pData, UINT32 uiSize, bool bText = false);

		UINT32	WriteFile(tstring strFileName, BYTE8* pData, UINT32 uiSize, bool bText = false);
		bool	FileExists(tstring strFileName);

		FWApplication*			GetApplication();
		const tchar*			DiskFilePath(tstring strFileName);
	private:
		FWApplication* m_pkApplication;
	};
}