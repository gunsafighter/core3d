#include "FWApplication.h"
#include "FWInput.h"
#include "FWFileIO.h"
#include "FWGraphics.h"
#include "FWScene.h"
#include "FWResManager.h"

#ifdef WIN32
static Core3D::FWApplicationWin32* gs_pkApplication = NULL;
#endif
namespace Core3D
{
	FWApplication::FWApplication()
	{
		m_fFPS				= 0.0f;
		m_fInvFPS			= 0.0f;
		m_fElapsedTime		= 0.0f;

		m_hWindowHandle		= NULL;
		m_bWindowed			= true;
		m_bActive			= false;
		m_strWindowTitle	= _T("<Default>");
		m_uiWindowWidth		= 640;
		m_uiWindowHeight	= 480;

		m_uiFrameIdent		= 0;

		m_pAppData			= NULL;
		m_pkInput			= NULL;
		m_pkFileIO			= NULL;
		m_pkGraphics		= NULL;
		m_pkScene			= NULL;
		m_pkResManager		= NULL;
	}

	FWApplication::~FWApplication()
	{
		CORE3D_SAFE_DELETEARRAY(m_pAppData);

		CORE3D_SAFE_DELETE(m_pkScene);
		CORE3D_SAFE_DELETE(m_pkResManager);
		CORE3D_SAFE_DELETE(m_pkGraphics);
		CORE3D_SAFE_DELETE(m_pkFileIO);
		CORE3D_SAFE_DELETE(m_pkInput);
	}

	bool FWApplication::CreateSubSystems(const CreationFlags& rkCreationFlags)
	{
	#	ifdef WIN32
		m_pkInput		= new FWInputWin32(this);
	#	endif
		if(false == m_pkInput->Initialize())					{return false;}

		m_pkFileIO		= new FWFileIO(this);
		if(false == m_pkFileIO->Initialize())					{return false;}

		m_pkGraphics	= new FWGraphics(this);
		if(false == m_pkGraphics->Initialize(rkCreationFlags))	{return false;}

		m_pkResManager	= new FWResManager(this);
		if(false == m_pkResManager->Initialize())				{return false;}

		m_pkScene		= new FWScene(this);
		if(false == m_pkScene->Initialize())					{return false;}

		if(false == CreateWorld())
		{
			DestroyWorld();
			return false;
		}
		return true;
	}

	void FWApplication::SetAppData(void* pvData, UINT32 uiLength)
	{
		CORE3D_SAFE_DELETEARRAY(m_pAppData);
		m_pAppData = new BYTE8[uiLength];
		memcpy(m_pAppData, pvData, uiLength);
	}

	FWInput* FWApplication::GetInput()
	{
		return m_pkInput;
	}

	FWFileIO* FWApplication::GetFileIO()
	{
		return m_pkFileIO;
	}

	FWGraphics* FWApplication::GetGraphics()
	{
		return m_pkGraphics;
	}

	FWResManager* FWApplication::GetResManager()
	{
		return m_pkResManager;
	}

	FWScene* FWApplication::GetScene()
	{
		return m_pkScene;
	}

	//-------------------------------------------------------------------------
	// COMMENT : WIN32 Version
	#ifdef WIN32

	FWApplicationWin32::FWApplicationWin32()
	{
		gs_pkApplication			= this;

		m_nTicksPerSecond.QuadPart	= 0;
		m_nStartTime.QuadPart		= 0;
		m_nLastTime.QuadPart		= 0;
	}

	FWApplicationWin32::~FWApplicationWin32()
	{
		::UnregisterClass(_T("<Application Window>"), ::GetModuleHandle(NULL));
	}

	bool FWApplicationWin32::Initialize(const CreationFlags& rkCreationFlags)
	{
		m_strWindowTitle	= rkCreationFlags.strWindowTitle;
		m_uiWindowWidth		= rkCreationFlags.uiWindowWidth;
		m_uiWindowHeight	= rkCreationFlags.uiWindowHeight;
		m_bWindowed			= rkCreationFlags.bWindowed;

		// COMMENT : Initialize the timer
		if(FALSE == ::QueryPerformanceFrequency(&m_nTicksPerSecond)) {return false;}

		// COMMENT : Create the render window
		WNDCLASS kWindowClass = 
		{
			NULL, &WinProc, 0, 0, ::GetModuleHandle(NULL), 
			rkCreationFlags.hIcon, 0, 0, 0, _T("<Application Window>")
		};
		::RegisterClass(&kWindowClass);

		UINT32 uiWindowStyle = WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION | WS_SYSMENU;
		RECT rc;
		::SetRect(&rc, 0, 0, m_uiWindowWidth, m_uiWindowHeight);
		::AdjustWindowRect(&rc, uiWindowStyle, FALSE);

		m_hWindowHandle = ::CreateWindow(_T("<Application Window>"), m_strWindowTitle.c_str(), 
			uiWindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, (rc.right - rc.left), (rc.bottom - rc.top), 
			NULL, NULL, ::GetModuleHandle(NULL), NULL);
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));

		return CreateSubSystems(rkCreationFlags);
	}

	INT32 FWApplicationWin32::Run()
	{
		::ShowWindow(m_hWindowHandle, SW_SHOW);
		::SetFocus(m_hWindowHandle);
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		m_bActive = true;

		// COMMENT : Begin application loop
		while(CheckMessages())
		{
			BeginFrame();
			RenderWorld();
			EndFrame();

			::Sleep(1);
		}

		// COMMENT : End of application loop(Termination of program)
		DestroyWorld();
		return 0;
	}

	void FWApplicationWin32::BeginFrame()
	{
		if(0 == m_nStartTime.QuadPart)
		{
			::QueryPerformanceCounter(&m_nStartTime);
			m_nLastTime.QuadPart = m_nStartTime.QuadPart;
		}
		++m_uiFrameIdent;

		m_pkInput->Update();
		FrameMove();
		m_pkScene->FrameMove();
	}

	bool FWApplicationWin32::CheckMessages()
	{
		while(true)
		{
			MSG kMessage;
			if(true == m_bActive)
			{
				if(FALSE == ::PeekMessage(&kMessage, 0, 0, 0, PM_REMOVE))	{break;}
			}
			else
			{
				if(FALSE == ::GetMessage(&kMessage, 0, 0, 0))				{break;}
			}
			
			::TranslateMessage(&kMessage);
			::DispatchMessage(&kMessage);

			if(WM_QUIT == kMessage.message) {return false;}
		}
		return true;
	}

	void FWApplicationWin32::EndFrame()
	{
		LARGE_INTEGER nCurrentTime;
		::QueryPerformanceCounter(&nCurrentTime);

		FLOAT32 fTimeDifference = static_cast<FLOAT32>(nCurrentTime.QuadPart - m_nLastTime.QuadPart);
		m_fElapsedTime			+= fTimeDifference / static_cast<FLOAT32>(m_nTicksPerSecond.QuadPart);
		m_fFPS					= static_cast<FLOAT32>(m_nTicksPerSecond.QuadPart) / fTimeDifference;
		m_fInvFPS				= 1.0f / m_fFPS;
		m_nLastTime.QuadPart	= nCurrentTime.QuadPart;
	}

	LRESULT CALLBACK FWApplicationWin32::WinProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
	{
		return gs_pkApplication->MsgProc(hWnd, uiMsg, wParam, lParam);
	}

	LRESULT FWApplicationWin32::MsgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
	{
		switch(uiMsg)
		{
		case WM_ACTIVATE:		m_bActive = (LOWORD(wParam) != WA_INACTIVE);	break;
		case WM_ENTERSIZEMOVE:	m_bActive = false;								break;
		case WM_EXITSIZEMOVE:	m_bActive = true;								break;
		// Turn off windows cursor in full-screen mode
		case WM_SETCURSOR:		if(m_bActive && !m_bWindowed) {::SetCursor(NULL); return true;} break;
		// Don't allow suspend / stand-by
		case WM_POWERBROADCAST: return false;
		case WM_CLOSE:			::PostQuitMessage(0);							return 0;
		case WM_SYSCOMMAND:
		// Prevent moving / sizing and power loss in full-screen mode
			switch(wParam)
			{
			case SC_MOVE:
			case SC_SIZE:
			case SC_MAXIMIZE:
			case SC_KEYMENU:
			case SC_MONITORPOWER:
				if(!m_bWindowed) {return 1;}
				break;
			}
			break;
		}
		return ::DefWindowProc(hWnd, uiMsg, wParam, lParam);
	}

	#endif
}
//-------------------------------------------------------------------------