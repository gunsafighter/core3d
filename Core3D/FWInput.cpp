#include "FWInput.h"
#include "FWApplication.h"

namespace Core3D
{
	FWInput::FWInput(FWApplication* pkApp)
	{
		m_pkApplication = pkApp;
	}

	FWApplication* FWInput::GetApplication()
	{
		return m_pkApplication;
	}

	//---------------------------------------------------------------------------------------
	// COMMENT : WIN32 Version
	#ifdef WIN32
	FWInputWin32::FWInputWin32(FWApplication* pkApp)
		: FWInput(pkApp)
	{
		m_pkDirectInput			= NULL;
		m_pkDIDeviceKeyboard	= NULL;
		m_pkDIDeviceMouse		= NULL;

		ZeroMemory(m_aKeys, sizeof(m_aKeys));
		ZeroMemory(&m_kMouseState, sizeof(m_kMouseState));
	}

	FWInputWin32::~FWInputWin32()
	{
		UnacquireDevices();
		CORE3D_SAFE_RELEASE(m_pkDIDeviceMouse);
		CORE3D_SAFE_RELEASE(m_pkDIDeviceKeyboard);
		CORE3D_SAFE_RELEASE(m_pkDirectInput);
	}

	bool FWInputWin32::Initialize()
	{
		// COMMENT : Initialize DirectInput and create DI devices
		if(FAILED(DirectInput8Create(::GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
			IID_IDirectInput8, (void**)&m_pkDirectInput, NULL)))
		{
			return false;
		}

		// COMMENT : Create keyboard device
		if(FAILED(m_pkDirectInput->CreateDevice(GUID_SysKeyboard, &m_pkDIDeviceKeyboard, NULL)))
		{
			return false;
		}
		m_pkDIDeviceKeyboard->SetDataFormat(&c_dfDIKeyboard);
		m_pkDIDeviceKeyboard->SetCooperativeLevel(m_pkApplication->GetWindowHandle(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

		// COMMENT : Create mouse device
		if(FAILED(m_pkDirectInput->CreateDevice(GUID_SysMouse, &m_pkDIDeviceMouse, NULL)))
		{
			return false;
		}
		m_pkDIDeviceMouse->SetDataFormat(&c_dfDIMouse);
		m_pkDIDeviceMouse->SetCooperativeLevel(m_pkApplication->GetWindowHandle(), DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

		AcquireDevices();
		return true;
	}

	void FWInputWin32::Update()
	{
		if(FAILED(m_pkDIDeviceKeyboard->GetDeviceState(sizeof(m_aKeys), (void*)m_aKeys)))
		{
			// COMMENT : Not acquired? acquire and try again.
			m_pkDIDeviceKeyboard->Acquire();
			m_pkDIDeviceKeyboard->GetDeviceState(sizeof(m_aKeys), (void*)m_aKeys);
		}

		if(FAILED(m_pkDIDeviceMouse->GetDeviceState(sizeof(DIMOUSESTATE), (void*)&m_kMouseState)))
		{
			// COMMENT : Not acquired? acquire and try again.
			m_pkDIDeviceMouse->Acquire();
			m_pkDIDeviceMouse->GetDeviceState(sizeof(DIMOUSESTATE), (void*)&m_kMouseState);
		}
	}

	void FWInputWin32::AcquireDevices()
	{
		m_pkDIDeviceKeyboard->Acquire();
		m_pkDIDeviceMouse->Acquire();
	}

	void FWInputWin32::UnacquireDevices()
	{
		m_pkDIDeviceKeyboard->Unacquire();
		m_pkDIDeviceMouse->Unacquire();
	}
	#endif
	//---------------------------------------------------------------------------------------
}