#include "PresentTarget.h"
#include "Device.h"

namespace Core3D
{
	PresentTarget::PresentTarget(Device* pkDevice)
		: m_pkDevice(pkDevice)
	{
		// COMMENT: Cannot add a reference to device or the present-target will never be free
	}

	PresentTarget::~PresentTarget()
	{
		// COMMENT: See comment in constructor
	}

	Device* PresentTarget::GetDevice()
	{
		if(NULL != m_pkDevice) {m_pkDevice->AddRef();}
		return m_pkDevice;
	}

	//---------------------------------------------------------------------------------------
	// COMMENT : WIN32 Version
	#ifdef WIN32
	PresentTargetWin32::PresentTargetWin32(Device* pkDevice)
		: PresentTarget(pkDevice)
		, m_pkDirectDraw(NULL)
		, m_pkDirectDrawClipper(NULL)
		, m_bDDSurfaceLost(false)
	{
		m_apkDirectDrawSurfaces[0] = m_apkDirectDrawSurfaces[1] = NULL;
	}

	PresentTargetWin32::~PresentTargetWin32()
	{
		DeviceParameters kDeviceParameters = m_pkDevice->GetDeviceParameters();

		if(NULL != m_apkDirectDrawSurfaces[1])
		{
			if(true == kDeviceParameters.bWindowed) {m_apkDirectDrawSurfaces[1]->Release();}
			m_apkDirectDrawSurfaces[1] = NULL;
		}
		CORE3D_SAFE_RELEASE(m_apkDirectDrawSurfaces[0]);
		CORE3D_SAFE_RELEASE(m_pkDirectDrawClipper);

		if(NULL != m_pkDirectDraw)
		{
			if(true == kDeviceParameters.bWindowed)
			{
				m_pkDirectDraw->RestoreDisplayMode();
				m_pkDirectDraw->SetCooperativeLevel(kDeviceParameters.hDeviceWnd, DDSCL_NORMAL);
			}
			m_pkDirectDraw->Release();
			m_pkDirectDraw = NULL;
		}
	}

	Result PresentTargetWin32::Create()
	{
		DeviceParameters kDeviceParameters = m_pkDevice->GetDeviceParameters();
		if((0 == kDeviceParameters.uiBackBufferWidth) || (0 == kDeviceParameters.uiBackBufferHeight))
		{
			CORE3D_ERROR(_T("PresentTargetWin32::Create() - Invalid backbuffer dimensions have been supplied.\n"));
			return INVALID_PARAMETERS;
		}

		if(FAILED(DirectDrawCreateEx(NULL, (void**)&m_pkDirectDraw, IID_IDirectDraw7, NULL)))
		{
			CORE3D_ERROR(_T("PresentTargetWin32::Create() - Couldn't create DirectDraw7 instance.\n"));
			return UNKNOWN;
		}

		UINT32 uiDDFlags = kDeviceParameters.bWindowed ? DDSCL_NORMAL : (DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT);
		if(FAILED(m_pkDirectDraw->SetCooperativeLevel(kDeviceParameters.hDeviceWnd, uiDDFlags)))
		{
			CORE3D_ERROR(_T("PresentTargetWin32::Create() - Couldn't set cooperative level.\n"));
			return UNKNOWN;
		}

		if(false == kDeviceParameters.bWindowed)
		{
			switch(kDeviceParameters.uiFullScreenColorBits)
			{
			case 16:
			case 24:
			case 32:
				break;
			default:
				CORE3D_ERROR(_T("PresentTargetWin32::Create() - Invalid back-buffer bit-depth specified.\n"));
				return UNKNOWN;
			}

			// COMMENT : Go to full-screen mode
			if(FAILED(m_pkDirectDraw->SetDisplayMode(kDeviceParameters.uiBackBufferWidth, kDeviceParameters.uiBackBufferHeight, kDeviceParameters.uiFullScreenColorBits, 0, 0)))
			{
				CORE3D_ERROR(_T("PresentTargetWin32::Create() - Couldn't set display mode.\n"));
				return UNKNOWN;
			}
		}
		else
		{
			// COMMENT : Determine display mode
			DDSURFACEDESC2 kDescSurface;
			kDescSurface.dwSize = sizeof(kDescSurface);
			if(FAILED(m_pkDirectDraw->GetDisplayMode(&kDescSurface)))
			{
				CORE3D_ERROR(_T("PresentTargetWin32::Create() - Couldn't get display mode.\n"));
				return UNKNOWN;
			}

			switch(kDescSurface.ddpfPixelFormat.dwRGBBitCount)
			{
			case 16:
				{
					UINT16 uiNumRedBits, uiNumGreenBits, uiNumBlueBits;
					ProcessBits(kDescSurface.ddpfPixelFormat.dwRBitMask, m_aui16BitShift[0], uiNumRedBits);
					ProcessBits(kDescSurface.ddpfPixelFormat.dwGBitMask, m_aui16BitShift[1], uiNumGreenBits);
					ProcessBits(kDescSurface.ddpfPixelFormat.dwBBitMask, m_aui16BitShift[2], uiNumBlueBits);
					m_aui16BitMaxVal[0] = (UINT16)((1 << uiNumRedBits) - 1);
					m_aui16BitMaxVal[1] = (UINT16)((1 << uiNumGreenBits) - 1);
					m_aui16BitMaxVal[2] = (UINT16)((1 << uiNumBlueBits) - 1);
				}
				break;
			case 24:
			case 32:
				break;
			default:
				CORE3D_ERROR(_T(""));
				return UNKNOWN;
			}
		}

		if(true == kDeviceParameters.bWindowed)
		{
			// COMMENT : Create a primary surface without a back-buffer (a secondary surface will be used as back-buffer).
			DDSURFACEDESC2 kDescSurface;
			ZeroMemory(&kDescSurface, sizeof(kDescSurface));
			kDescSurface.dwSize			= sizeof(kDescSurface);
			kDescSurface.dwFlags		= DDSD_CAPS;
			kDescSurface.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
			if(FAILED(m_pkDirectDraw->CreateSurface(&kDescSurface, &m_apkDirectDrawSurfaces[0], NULL)))
			{
				CORE3D_ERROR(_T("PresentTargetWin32::Create() - Couldn't create primary surface.\n"));
				return UNKNOWN;
			}

			// COMMENT : Create the secondary surface
			kDescSurface.dwFlags		= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
			kDescSurface.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
			kDescSurface.dwWidth		= kDeviceParameters.uiBackBufferWidth;
			kDescSurface.dwHeight		= kDeviceParameters.uiBackBufferHeight;
			if(FAILED(m_pkDirectDraw->CreateSurface(&kDescSurface, &m_apkDirectDrawSurfaces[1], NULL)))
			{
				CORE3D_ERROR(_T("PresentTargetWin32::Create() - Couldn't create secondary surface.\n"));
				return UNKNOWN;
			}

			// COMMENT : Create clipper
			if(FAILED(m_pkDirectDraw->CreateClipper(0, &m_pkDirectDrawClipper, NULL)))
			{
				CORE3D_ERROR(_T("PresentTargetWin32::Create() - Couldn't create clipper.\n"));
				return UNKNOWN;
			}

			// COMMENT : Set clipper to window
			if(FAILED(m_pkDirectDrawClipper->SetHWnd(0, kDeviceParameters.hDeviceWnd)))
			{
				CORE3D_ERROR(_T("PresentTargetWin32::Create() - Couldn't set clipper window handle.\n"));
				return UNKNOWN;
			}

			// COMMENT : Attach clipper object to primary surface
			if(FAILED(m_apkDirectDrawSurfaces[0]->SetClipper(m_pkDirectDrawClipper)))
			{
				CORE3D_ERROR(_T("PresentTargetWin32::Create() - Couldn't attach clipper to primary surface.\n"));
				return UNKNOWN;
			}
		}
		else
		{
			// COMMENT : Create a primary surface with one back-buffer if in full-screen mode
			DDSURFACEDESC2 kDescSurface;
			ZeroMemory(&kDescSurface, sizeof(kDescSurface));
			kDescSurface.dwSize				= sizeof(kDescSurface);
			kDescSurface.dwFlags			= DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
			kDescSurface.dwBackBufferCount	= 1;
			kDescSurface.ddsCaps.dwCaps		= DDSCAPS_PRIMARYSURFACE | DDSCAPS_COMPLEX | DDSCAPS_FLIP;
			if(FAILED(m_pkDirectDraw->CreateSurface(&kDescSurface, &m_apkDirectDrawSurfaces[0], NULL)))
			{
				CORE3D_ERROR(_T("PresentTargetWin32::Create() - Couldn't create primary surface.\n"));
				return UNKNOWN;
			}

			// COMMENT : Get DirectDraw back-buffer
			DDSCAPS2 kCapsDirectDraw = {DDSCAPS_BACKBUFFER};
			if(FAILED(m_apkDirectDrawSurfaces[0]->GetAttachedSurface(&kCapsDirectDraw, &m_apkDirectDrawSurfaces[1])))
			{
				CORE3D_ERROR(_T("PresentTargetWin32::Create() - Couldn't access secondary surface.\n"));
				return UNKNOWN;
			}
		}
		return OK;
	}

	void PresentTargetWin32::ProcessBits(UINT32 uiMask, UINT16& ruiLowBit, UINT16& ruiNumBits)
	{
		// COMMENT : Small utility function to find the low-bit and number of bits in a supplied mask
		// article - Nathan Davies(http://www.gamedev.net/reference/articles/article588.asp)
		UINT32 uiTestMask = 1;
		for(ruiLowBit = 0; ruiLowBit < 32; ++ruiLowBit)
		{
			if(uiMask & uiTestMask) {break;}
			uiTestMask <<= 1;
		}

		uiTestMask <<= 1;
		for(ruiNumBits = 1; ruiNumBits < 32; ++ruiNumBits)
		{
			if(0 == (uiMask & uiTestMask)) {break;}
			uiTestMask <<= 1;
		}
	}

	Result PresentTargetWin32::Present(const FLOAT32* pfSource, UINT32 uiFloats)
	{
		DeviceParameters kDeviceParameters = m_pkDevice->GetDeviceParameters();

		// COMMENT : Check for lost DirectDraw surface
		if(true == m_bDDSurfaceLost)
		{
			m_apkDirectDrawSurfaces[0]->Restore();
			if(true == kDeviceParameters.bWindowed) {m_apkDirectDrawSurfaces[1]->Restore();}
			m_bDDSurfaceLost = false;
		}

		// COMMENT : Lock back-buffer surface
		DDSURFACEDESC2 kDescSurface;
		kDescSurface.dwSize = sizeof(kDescSurface);
		if(FAILED(m_apkDirectDrawSurfaces[1]->Lock(NULL, &kDescSurface, DDLOCK_WAIT | DDLOCK_NOSYSLOCK | DDLOCK_WRITEONLY, NULL)))
		{
			CORE3D_ERROR(_T("PresentTargetWin32::Present() - Couldn't lock secondary surface.\n"));
			return UNKNOWN;
		}

		// COMMENT : Copy pixels to the back-buffer surface
		const UINT32 DEST_BYTES		= kDescSurface.ddpfPixelFormat.dwRGBBitCount / 8;
		const UINT32 DEST_ROWJUMP	= kDescSurface.lPitch - DEST_BYTES * kDeviceParameters.uiBackBufferWidth;
		UINT8* pDestination			= reinterpret_cast<UINT8*>(kDescSurface.lpSurface);

		Core3D::FpuTruncate();

		if(2 == DEST_BYTES)
		{
			// COMMENT : 16 Bit
			UINT32 uiHeight = kDeviceParameters.uiBackBufferHeight;
			while(uiHeight--)
			{
				UINT32 uiWidth = kDeviceParameters.uiBackBufferWidth;
				while(uiWidth--)
				{
					const INT32 R = Core3D::Clamp<INT32>(Core3D::FtoL(pfSource[0] * (FLOAT32)m_aui16BitMaxVal[0]), 0, m_aui16BitMaxVal[0]);
					const INT32 G = Core3D::Clamp<INT32>(Core3D::FtoL(pfSource[1] * (FLOAT32)m_aui16BitMaxVal[1]), 0, m_aui16BitMaxVal[1]);
					const INT32 B = Core3D::Clamp<INT32>(Core3D::FtoL(pfSource[2] * (FLOAT32)m_aui16BitMaxVal[2]), 0, m_aui16BitMaxVal[2]);
					pfSource += uiFloats;

					*((UINT16*)pDestination) = (R << m_aui16BitShift[0]) | (G << m_aui16BitShift[1]) | (B << m_aui16BitShift[2]);
					pDestination += 2;
				}
				pDestination += DEST_ROWJUMP;
			}
		}
		else
		{
			// COMMENT : 24 or 32 Bit
			UINT32 uiHeight = kDeviceParameters.uiBackBufferHeight;
			while(uiHeight--)
			{
				UINT32 uiWidth = kDeviceParameters.uiBackBufferWidth;
				while(uiWidth--)
				{
					pDestination[0] = Core3D::Clamp<INT32>(Core3D::FtoL(pfSource[2] * 255.0f), 0, 255); // B
					pDestination[1] = Core3D::Clamp<INT32>(Core3D::FtoL(pfSource[1] * 255.0f), 0, 255); // G
					pDestination[2] = Core3D::Clamp<INT32>(Core3D::FtoL(pfSource[0] * 255.0f), 0, 255); // R
					pfSource		+= uiFloats;
					pDestination	+= DEST_BYTES;
				}
				pDestination += DEST_ROWJUMP;
			}
		}

		Core3D::FpuReset();

		// COMMENT : Unlock back-buffer surface and surface
		m_apkDirectDrawSurfaces[1]->Unlock(NULL);

		// COMMENT : Present the image to the screen
		if(true == kDeviceParameters.bWindowed)
		{
			POINT ptTopLeft = {0, 0};
			::ClientToScreen(kDeviceParameters.hDeviceWnd, &ptTopLeft);

			RECT rcDestination;
			::GetClientRect(kDeviceParameters.hDeviceWnd, &rcDestination);
			::OffsetRect(&rcDestination, ptTopLeft.x, ptTopLeft.y);

			RECT rcSource;
			::SetRect(&rcSource, 0, 0, kDeviceParameters.uiBackBufferWidth, kDeviceParameters.uiBackBufferHeight);

			// COMMENT : Blt secondary to primary surface
			if(FAILED(m_apkDirectDrawSurfaces[0]->Blt(&rcDestination, m_apkDirectDrawSurfaces[1], &rcSource, DDBLT_WAIT, NULL)))
			{
				m_bDDSurfaceLost = true;
				CORE3D_ERROR(_T("PresentTargetWin32::Present() - Primary surface have been lost!\n"));
				return UNKNOWN;
			}
		}
		else
		{
			if(FAILED(m_apkDirectDrawSurfaces[0]->Flip(NULL, DDFLIP_WAIT)))
			{
				m_bDDSurfaceLost = true;
				CORE3D_ERROR(_T("PresentTargetWin32::Present() - Surface have been lost!\n"));
				return UNKNOWN;
			}
		}
		return OK;
	}
	#endif
	//---------------------------------------------------------------------------------------
}