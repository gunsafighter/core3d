#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DTypes.h"

//---------------------------------------------------------------------------------------
// COMMENT : WIN32 Version
#ifdef WIN32
#include <ddraw.h>
#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")
#endif
//---------------------------------------------------------------------------------------

namespace Core3D
{
	class Device;
	class PresentTarget : public RefObject
	{
	public:
		virtual Result Create() = 0;
		virtual Result Present(const FLOAT32* pfSource, UINT32 uiFloats) = 0;
		Device* GetDevice();
	protected:
		PresentTarget(Device* pkDevice);
		virtual ~PresentTarget();
	protected:
		Device* m_pkDevice;
	};

	//-----------------------------------------------------------------------------------
	// COMMENT : WIN32 Version
	#ifdef WIN32
	class PresentTargetWin32 : public PresentTarget
	{
	public:
		Result	Create();
		Result	Present(const FLOAT32* pfSource, UINT32 uiFloats);
	protected:
		friend class Device;

		PresentTargetWin32(Device* pkDevice);
		virtual ~PresentTargetWin32();
	private:
		void	ProcessBits(UINT32 uiMask, UINT16& ruiLowBit, UINT16& ruiNumBits);
	private:
		LPDIRECTDRAW7			m_pkDirectDraw;
		LPDIRECTDRAWCLIPPER		m_pkDirectDrawClipper;
		LPDIRECTDRAWSURFACE7	m_apkDirectDrawSurfaces[2];
		bool					m_bDDSurfaceLost;

		UINT16					m_aui16BitMaxVal[3];
		UINT16					m_aui16BitShift[3];
	};
	#endif
	//---------------------------------------------------------------------------------
}