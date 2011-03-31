#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Framework Library for Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////
#include "FWType.h"

namespace Core3D
{

	struct CreationFlags
	{
		tstring strWindowTitle;
	#	ifdef WIN32
		HICON	hIcon;
	#	endif
		UINT16	uiWindowWidth, uiWindowHeight;
		bool	bWindowed;
	};

	class FWInput;
	class FWFileIO;
	class FWGraphics;
	class FWResManager;
	class FWScene;
	class FWApplication
	{
	public:
		FWApplication();
		virtual ~FWApplication();

		virtual bool	Initialize(const CreationFlags& rkCreationFlags) = 0;
		virtual INT32	Run()			= 0;

		virtual bool	CreateWorld()	= 0;
		virtual void	DestroyWorld()	= 0;

		virtual void	FrameMove()		= 0;
		virtual void	RenderWorld()	= 0;
	protected:
		bool			CreateSubSystems(const CreationFlags& rkCreationFlags);
	public:
		FLOAT32			GetFPS()			const {return m_fFPS;}
		FLOAT32			GetInvFPS()			const {return m_fInvFPS;}
		FLOAT32			GetElapsedTime()	const {return m_fElapsedTime;}
		WinHandle		GetWindowHandle()	const {return m_hWindowHandle;}
		bool			GetWindowed()		const {return m_bWindowed;}
		bool			GetActive()			const {return m_bActive;}
		tstring			GetWindowTitle()	const {return m_strWindowTitle;}
		UINT16			GetWindowWidth()	const {return m_uiWindowWidth;}
		UINT16			GetWindowHeight()	const {return m_uiWindowHeight;}
		UINT32			GetFrameIdent()		const {return m_uiFrameIdent;}
		void*			GetAppData()			  {return m_pAppData;}
		
		void			SetAppData(void* pvData, UINT32 uiLength);
		//---------------------------------------------------------------------
		// COMMENT : Sub-Systems
		FWInput*		GetInput();
		FWFileIO*		GetFileIO();
		FWGraphics*		GetGraphics();
		FWResManager*	GetResManager();
		FWScene*		GetScene();
		//---------------------------------------------------------------------
	protected:
		FLOAT32			m_fFPS, m_fInvFPS, m_fElapsedTime;
		WinHandle		m_hWindowHandle;
		bool			m_bWindowed;
		bool			m_bActive;
		tstring			m_strWindowTitle;
		UINT16			m_uiWindowWidth, m_uiWindowHeight;
		UINT32			m_uiFrameIdent;
		BYTE8*			m_pAppData;
		//---------------------------------------------------------------------
		// COMMENT : Sub-Systems
		FWInput*		m_pkInput;
		FWFileIO*		m_pkFileIO;
		FWGraphics*		m_pkGraphics;
		FWResManager*	m_pkResManager;
		FWScene*		m_pkScene;
		//---------------------------------------------------------------------
	};

	//-------------------------------------------------------------------------
	// COMMENT : WIN32 Version
	#ifdef WIN32
	class FWApplicationWin32 : public FWApplication
	{
	public:
		FWApplicationWin32();
		virtual ~FWApplicationWin32();
		
		bool	Initialize(const CreationFlags& rkCreationFlags);
		INT32	Run();

		virtual bool CreateWorld()	= 0;
		virtual void DestroyWorld() = 0;

		virtual void FrameMove()	= 0;
		virtual void RenderWorld()	= 0;
	private:
		void	BeginFrame();
		void	EndFrame();
		bool	CheckMessages();

		LRESULT MsgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK WinProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);
	private:
		LARGE_INTEGER m_nTicksPerSecond;
		LARGE_INTEGER m_nStartTime;
		LARGE_INTEGER m_nLastTime;
	};
	#endif
	//-------------------------------------------------------------------------
}