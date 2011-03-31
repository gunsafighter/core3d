#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Framework Library for Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////
#include "FWType.h"

//---------------------------------------------------------------------
// COMMENT : WIN32 Version
#ifdef WIN32
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#endif
//---------------------------------------------------------------------

namespace Core3D
{
	class FWApplication;
	class FWInput
	{
	protected:
		friend class FWApplication;
	#	ifdef WIN32
		friend class FWApplicationWin32;
	#	endif

		FWInput(FWApplication* pkApp);
		virtual ~FWInput() {}

		virtual bool	Initialize()							= 0;
	public:
		virtual void	Update()								= 0;
		virtual bool	KeyDown(BYTE8 nKey)						= 0;
		virtual bool	KeyUp(BYTE8 nKey)						= 0;

		virtual bool	ButtonDown(INT32 iButton)				= 0;
		virtual bool	ButtonUp(INT32 iButton)					= 0;

		virtual void	GetMovement(INT32* piDX, INT32* piDY)	= 0;
		virtual INT32	GetWheelMovement()						= 0;

		FWApplication*	GetApplication();
	protected:
		FWApplication*	m_pkApplication;
	};

	//---------------------------------------------------------------------
	// COMMENT : WIN32 Version
	#ifdef WIN32
	class FWInputWin32 : public FWInput
	{
	protected:
		friend class FWApplication;
		friend class FWApplicationWin32;

		FWInputWin32(FWApplication* pkApp);
		~FWInputWin32();

		bool			Initialize();
	public:
		void			Update();
		inline bool		KeyDown(BYTE8 nKey)						{return (m_aKeys[nKey] & 0x80) ? true : false;}
		inline bool		KeyUp(BYTE8 nKey)						{return (m_aKeys[nKey] & 0x80) ? false : true;}

		inline bool		ButtonDown(INT32 iButton)				{return (m_kMouseState.rgbButtons[iButton] & 0x80) ? true : false;}
		inline bool		ButtonUp(INT32 iButton)					{return (m_kMouseState.rgbButtons[iButton] & 0x80) ? false : true;}

		inline void		GetMovement(INT32* piDX, INT32* piDY)	{*piDX = m_kMouseState.lX; *piDY = m_kMouseState.lY;}
		inline INT32	GetWheelMovement()						{return m_kMouseState.lZ;}
	private:
		void			AcquireDevices();
		void			UnacquireDevices();
	private:
		LPDIRECTINPUT8			m_pkDirectInput;
		LPDIRECTINPUTDEVICE8	m_pkDIDeviceKeyboard;
		LPDIRECTINPUTDEVICE8	m_pkDIDeviceMouse;

		BYTE8					m_aKeys[256];
		DIMOUSESTATE			m_kMouseState;
	};
	#endif
	//---------------------------------------------------------------------
}