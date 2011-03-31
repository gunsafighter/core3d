#include "Surface.h"
#include "Device.h"

namespace Core3D
{
	Surface::Surface(Device* pkDevice)
		: m_pkDevice(pkDevice)
		, m_uiWidth(0)
		, m_uiHeight(0)
		, m_uiWidthMin(0)
		, m_uiHeightMin(0)
		, m_bLockedComplete(false)
		, m_pfPartialLockData(NULL)
		, m_pfData(NULL)
	{

	}

	Surface::~Surface()
	{
		CORE3D_SAFE_DELETEARRAY(m_pfPartialLockData);
		CORE3D_SAFE_DELETEARRAY(m_pfData);
	}

	Result Surface::Create(UINT32 uiWidth, UINT32 uiHeight, Format eFormat)
	{
		if((0 == uiWidth) || (0 == uiHeight))
		{
			CORE3D_ERROR(_T("Surface::Create() - Surface dimensions are invalid.\n"));
			return INVALID_PARAMETERS;
		}

		UINT32 uiFloats = 0;
		switch(eFormat)
		{
		case FMT_R32F:			uiFloats = 1; break;
		case FMT_R32G32F:		uiFloats = 2; break;
		case FMT_R32G32B32F:	uiFloats = 3; break;
		case FMT_R32G32B32A32F: uiFloats = 4; break;
		default: CORE3D_ERROR(_T("Surface::Create() - Invalid format specified.\n")); return INVALID_FORMAT;
		}

		m_eFormat		= eFormat;
		m_uiWidth		= uiWidth;
		m_uiHeight		= uiHeight;
		m_uiWidthMin	= uiWidth - 1;
		m_uiHeightMin	= uiHeight - 1;

		m_pfData = new FLOAT32[uiWidth * uiHeight * uiFloats];
		if(NULL == m_pfData)
		{
			CORE3D_ERROR(_T("Surface::Create() - Out of memory, cannot create surface.\n"));
			return OUT_OF_MEMORY;
		}
		return OK;
	}

	Result Surface::Clear(const Vector4& rkColor, const Rect* pkRect)
	{
		Rect rcClear;
		if(NULL != pkRect)
		{
			if((pkRect->uiRight > m_uiWidth) || (pkRect->uiBottom > m_uiHeight))
			{
				CORE3D_ERROR(_T("Surface::Clear() - Clear rectangle exceeds surface's dimensions.\n"));
				return INVALID_PARAMETERS;
			}

			if((pkRect->uiLeft >= pkRect->uiRight) || (pkRect->uiTop >= pkRect->uiBottom))
			{
				CORE3D_ERROR(_T("Surface::Clear() - Invalid rectangle specified.\n"));
				return INVALID_PARAMETERS;
			}
			rcClear = *pkRect;
		}
		else
		{
			rcClear.uiLeft		= 0;
			rcClear.uiTop		= 0;
			rcClear.uiRight		= m_uiWidth;
			rcClear.uiBottom	= m_uiHeight;
		}

		FLOAT32* pfData = NULL;
		Result eResult	= LockRect((void**)&pfData, NULL);
		if(CORE3D_FAILED(eResult)) {return eResult;}

		const UINT32 BRIDGE_STEP = (m_uiWidth - rcClear.uiRight) + rcClear.uiLeft;

		switch(m_eFormat)
		{
		case FMT_R32F:
			{
				FLOAT32* pfCurrentData = &pfData[rcClear.uiTop * m_uiWidth + rcClear.uiLeft];
				for(UINT32 uiY = rcClear.uiTop; uiY < rcClear.uiBottom; ++uiY, pfCurrentData += BRIDGE_STEP)
				{
					for(UINT32 uiX = rcClear.uiLeft; uiX < rcClear.uiRight; ++uiX, ++pfCurrentData)
					{
						*pfCurrentData = rkColor.r;
					}
				}
			}
			break;
		case FMT_R32G32F:
			{
				Vector2* pkCurrentData = &((Vector2*)pfData)[rcClear.uiTop * m_uiWidth + rcClear.uiLeft];
				for(UINT32 uiY = rcClear.uiTop; uiY < rcClear.uiBottom; ++uiY, pkCurrentData += BRIDGE_STEP)
				{
					for(UINT32 uiX = rcClear.uiLeft; uiX < rcClear.uiRight; ++uiX, ++pkCurrentData)
					{
						*pkCurrentData = rkColor;
					}
				}
			}
			break;
		case FMT_R32G32B32F:
			{
				Vector3* pkCurrentData = &((Vector3*)pfData)[rcClear.uiTop * m_uiWidth + rcClear.uiLeft];
				for(UINT32 uiY = rcClear.uiTop; uiY < rcClear.uiBottom; ++uiY, pkCurrentData += BRIDGE_STEP)
				{
					for(UINT32 uiX = rcClear.uiLeft; uiX < rcClear.uiRight; ++uiX, ++pkCurrentData)
					{
						*pkCurrentData = rkColor;
					}
				}
			}
			break;
		case FMT_R32G32B32A32F:
			{
				Vector4* pkCurrentData = &((Vector4*)pfData)[rcClear.uiTop * m_uiWidth + rcClear.uiLeft];
				for(UINT32 uiY = rcClear.uiTop; uiY < rcClear.uiBottom; ++uiY, pkCurrentData += BRIDGE_STEP)
				{
					for(UINT32 uiX = rcClear.uiLeft; uiX < rcClear.uiRight; ++uiX, ++pkCurrentData)
					{
						*pkCurrentData = rkColor;
					}
				}
			}
			break;
		default:
			CORE3D_ERROR(_T("Surface::Clear() - Invalid surface format.\n"));
			UnlockRect();
			return INVALID_FORMAT;
		}

		UnlockRect();
		return OK;
	}

	Result Surface::LockRect(void** ppvData, const Rect* pkRect)
	{
		if(NULL == ppvData)
		{
			CORE3D_ERROR(_T("Surface::LockRect() - Data pointer pointer to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		if((false != m_bLockedComplete) || (NULL != m_pfPartialLockData))
		{
			CORE3D_ERROR(_T("Surface::LockRect() - Mip level is already locked!\n"));
			return INVALID_STATE;
		}

		if(NULL == pkRect)
		{
			*ppvData			= m_pfData;
			m_bLockedComplete	= true;
			return OK;
		}

		if((pkRect->uiRight > m_uiWidth) || (pkRect->uiBottom > m_uiHeight))
		{
			CORE3D_ERROR(_T("Surface::LockRect() - Rectangle exceeds surface dimensions!\n"));
			return INVALID_PARAMETERS;
		}

		if((pkRect->uiLeft >= pkRect->uiRight) || (pkRect->uiTop >= pkRect->uiBottom))
		{
			CORE3D_ERROR(_T("Surface::LockRect() - Invalid rectangle specified!\n"));
			return INVALID_PARAMETERS;
		}

		m_kPartialLockRect = *pkRect;
		// COMMENT : Create lock buffer
		const UINT32 LOCK_WIDTH		= m_kPartialLockRect.uiRight - m_kPartialLockRect.uiLeft;
		const UINT32 LOCK_HEIGHT	= m_kPartialLockRect.uiBottom - m_kPartialLockRect.uiTop;
		const UINT32 SURFACE_FLOATS = GetFormatFloats();
		m_pfPartialLockData			= new FLOAT32[LOCK_WIDTH * LOCK_HEIGHT * SURFACE_FLOATS];
		if(NULL == m_pfPartialLockData)
		{
			CORE3D_ERROR(_T("Surface::LockRect() - Memory allocation failed!\n"));
			return OUT_OF_MEMORY;
		}

		FLOAT32* pfCurrentLockData	= m_pfPartialLockData;
		for(UINT32 uiY = m_kPartialLockRect.uiTop; uiY < m_kPartialLockRect.uiBottom; ++uiY)
		{
			const FLOAT32* pfCurrentSurfaceData = &m_pfData[(uiY * m_uiWidth + m_kPartialLockRect.uiLeft) * SURFACE_FLOATS];
			memcpy(pfCurrentLockData, pfCurrentSurfaceData, sizeof(FLOAT32) * SURFACE_FLOATS * LOCK_WIDTH);
			pfCurrentLockData += (SURFACE_FLOATS * LOCK_WIDTH);
		}

		*ppvData = m_pfPartialLockData;
		return OK;
	}

	Result Surface::UnlockRect()
	{
		if((false == m_bLockedComplete) && (NULL == m_pfPartialLockData))
		{
			CORE3D_ERROR(_T("Surface::UnlockRect() - Cannot unlock mip level because it isn't locked.\n"));
			return INVALID_STATE;
		}

		if(true == m_bLockedComplete)
		{
			m_bLockedComplete = false;
			return OK;
		}

		const UINT32 LOCK_WIDTH		= m_kPartialLockRect.uiRight - m_kPartialLockRect.uiLeft;
		const UINT32 SURFACE_FLOATS = GetFormatFloats();
		const FLOAT32* pfCurrentLockData = m_pfPartialLockData;
		for(UINT32 uiY = m_kPartialLockRect.uiTop; uiY < m_kPartialLockRect.uiBottom; ++uiY)
		{
			FLOAT32* pfCurrentSurfaceData = &m_pfData[(uiY * m_uiWidth + m_kPartialLockRect.uiLeft) * SURFACE_FLOATS];
			memcpy(pfCurrentSurfaceData, pfCurrentLockData, sizeof(FLOAT32) * SURFACE_FLOATS * LOCK_WIDTH);
			pfCurrentLockData += (SURFACE_FLOATS * LOCK_WIDTH);
		}
		CORE3D_SAFE_DELETEARRAY(m_pfPartialLockData);
		return OK;
	}

	UINT32 Surface::GetFormatFloats()
	{
		switch(m_eFormat)
		{
		case FMT_R32F:			return 1;
		case FMT_R32G32F:		return 2;
		case FMT_R32G32B32F:	return 3;
		case FMT_R32G32B32A32F: return 4;
		}
		return 0;
	}

	void Surface::SamplePoint(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV)
	{
		const INT32 PIXEL_X = Core3D::FtoL(fU * (FLOAT32)m_uiWidthMin);
		const INT32 PIXEL_Y = Core3D::FtoL(fV * (FLOAT32)m_uiHeightMin);

		switch(m_eFormat)
		{
		case FMT_R32F:
			{
				FLOAT32* pfPixel = &m_pfData[PIXEL_Y * m_uiWidth + PIXEL_X];
				rkColor = Vector4(pfPixel[0], 0.0f, 0.0f, 1.0f);
			}
			break;
		case FMT_R32G32F:
			{
				Vector2* pkPixel = &((Vector2*)m_pfData)[PIXEL_Y * m_uiWidth + PIXEL_X];
				rkColor = Vector4(pkPixel->x, pkPixel->y, 0.0f, 1.0f);
			}
			break;
		case FMT_R32G32B32F:
			{
				Vector3* pkPixel = &((Vector3*)m_pfData)[PIXEL_Y * m_uiWidth + PIXEL_X];
				rkColor = Vector4(pkPixel->x, pkPixel->y, pkPixel->z, 1.0f);
			}
			break;
		case FMT_R32G32B32A32F:
			{
				Vector4* pkPixel = &((Vector4*)m_pfData)[PIXEL_Y * m_uiWidth + PIXEL_X];
				rkColor = *pkPixel;
			}
			break;
		}
	}

	void Surface::SampleLinear(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV)
	{
		FLOAT32 X			= fU * (FLOAT32)m_uiWidthMin;
		FLOAT32 Y			= fV * (FLOAT32)m_uiHeightMin;
		const INT32 PIXEL_X = Core3D::FtoL(X);
		const INT32 PIXEL_Y = Core3D::FtoL(Y);

		INT32 PIXEL_X2		= PIXEL_X + 1;
		INT32 PIXEL_Y2		= PIXEL_Y + 1;
		if(PIXEL_X2 >= (INT32)m_uiWidth)	{PIXEL_X2 = m_uiWidthMin;}
		if(PIXEL_Y2 >= (INT32)m_uiHeight)	{PIXEL_Y2 = m_uiHeightMin;}

		const INT32 INDEX_ROWS[2]		= {PIXEL_Y * m_uiWidth, PIXEL_Y2 * m_uiWidth};
		const FLOAT32 INTERPOLATIONS[2] = {X - (const FLOAT32)PIXEL_X, Y - (const FLOAT32)PIXEL_Y};

		switch(m_eFormat)
		{
		case FMT_R32F:
			{
				FLOAT32 afColorRows[2];
				afColorRows[0]		= Core3D::Lerp(m_pfData[INDEX_ROWS[0] + PIXEL_X], m_pfData[INDEX_ROWS[0] + PIXEL_X2], INTERPOLATIONS[0]);
				afColorRows[1]		= Core3D::Lerp(m_pfData[INDEX_ROWS[1] + PIXEL_X], m_pfData[INDEX_ROWS[1] + PIXEL_X2], INTERPOLATIONS[0]);
				FLOAT32 fFinalColor = Core3D::Lerp(afColorRows[0], afColorRows[1], INTERPOLATIONS[1]);
				rkColor = Vector4(fFinalColor, 0.0f, 0.0f, 1.0f);
			}
			break;
		case FMT_R32G32F:
			{
				Vector2* pkPixelData = (Vector2*)m_pfData;
				
				static Vector2 akColorRows[2];
				Core3D::Vec2Lerp(akColorRows[0], pkPixelData[INDEX_ROWS[0] + PIXEL_X], pkPixelData[INDEX_ROWS[0] + PIXEL_X2], INTERPOLATIONS[0]);
				Core3D::Vec2Lerp(akColorRows[1], pkPixelData[INDEX_ROWS[1] + PIXEL_X], pkPixelData[INDEX_ROWS[1] + PIXEL_X2], INTERPOLATIONS[0]);
				
				static Vector2 kFinalColor;
				Core3D::Vec2Lerp(kFinalColor, akColorRows[0], akColorRows[1], INTERPOLATIONS[1]);
				rkColor = Vector4(kFinalColor.x, kFinalColor.y, 0.0f, 1.0f);
			}
			break;
		case FMT_R32G32B32F:
			{
				Vector3* pkPixelData = (Vector3*)m_pfData;
				
				static Vector3 akColorRows[2];
				Core3D::Vec3Lerp(akColorRows[0], pkPixelData[INDEX_ROWS[0] + PIXEL_X], pkPixelData[INDEX_ROWS[0] + PIXEL_X2], INTERPOLATIONS[0]);
				Core3D::Vec3Lerp(akColorRows[1], pkPixelData[INDEX_ROWS[1] + PIXEL_X], pkPixelData[INDEX_ROWS[1] + PIXEL_X2], INTERPOLATIONS[0]);

				static Vector3 kFinalColor;
				Core3D::Vec3Lerp(kFinalColor, akColorRows[0], akColorRows[1], INTERPOLATIONS[1]);
				rkColor = Vector4(kFinalColor.x, kFinalColor.y, kFinalColor.z, 1.0f);
			}
			break;
		case FMT_R32G32B32A32F:
			{
				Vector4* pkPixelData = (Vector4*)m_pfData;

				static Vector4 akColorRows[2];
				Core3D::Vec4Lerp(akColorRows[0], pkPixelData[INDEX_ROWS[0] + PIXEL_X], pkPixelData[INDEX_ROWS[0] + PIXEL_X2], INTERPOLATIONS[0]);
				Core3D::Vec4Lerp(akColorRows[1], pkPixelData[INDEX_ROWS[1] + PIXEL_X], pkPixelData[INDEX_ROWS[1] + PIXEL_X2], INTERPOLATIONS[0]);
				Core3D::Vec4Lerp(rkColor, akColorRows[0], akColorRows[1], INTERPOLATIONS[1]);
			}
			break;
		}
	}

	Format Surface::GetFormat()
	{
		return m_eFormat;
	}

	UINT32 Surface::GetWidth()
	{
		return m_uiWidth;
	}

	UINT32 Surface::GetHeight()
	{
		return m_uiHeight;
	}

	Result Surface::CopyToSurface(const Rect* pkSrcRect, Surface* pkDestSurface, const Rect* pkDestRect, TextureFilter eFilter)
	{
		if(NULL == pkDestSurface)
		{
			CORE3D_ERROR(_T("Surface::CopyToSurface() - No destination surface specified.\n"));
			return INVALID_PARAMETERS;
		}

		if((TF_POINT != eFilter) || (TF_LINEAR != eFilter))
		{
			CORE3D_ERROR(_T("Surface::CopyToSurface() - Invalid filter specified.\n"));
			return INVALID_PARAMETERS;
		}

		Rect rcSrc;
		if(NULL != pkSrcRect)
		{
			if((pkSrcRect->uiRight > m_uiWidth) || (pkSrcRect->uiBottom > m_uiHeight))
			{
				CORE3D_ERROR(_T("Surface::CopyToSurface() - Source rectangle exceeds surface dimensions.\n"));
				return INVALID_PARAMETERS;
			}

			if((pkSrcRect->uiLeft >= pkSrcRect->uiRight) || (pkSrcRect->uiTop >= pkSrcRect->uiBottom))
			{
				CORE3D_ERROR(_T("Surface::CopyToSurface() - Invalid source rectangle specified.\n"));
				return INVALID_PARAMETERS;
			}
			rcSrc = *pkSrcRect;
		}
		else
		{
			rcSrc.uiLeft	= 0;
			rcSrc.uiTop		= 0;
			rcSrc.uiRight	= m_uiWidth;
			rcSrc.uiBottom	= m_uiHeight;
		}

		Rect rcDest;
		if(NULL != pkDestRect)
		{
			if((pkDestRect->uiRight > pkDestSurface->GetWidth()) || (pkDestRect->uiBottom > pkDestSurface->GetHeight()))
			{
				CORE3D_ERROR(_T("Surface::CopyToSurface() - Destination rectangle exceeds surface dimensions.\n"));
				return INVALID_PARAMETERS;
			}

			if((pkDestRect->uiLeft >= pkDestRect->uiRight) || (pkDestRect->uiTop >= pkDestRect->uiBottom))
			{
				CORE3D_ERROR(_T("Surface::CopyToSurface() - Invalid destination rectangle specified.\n"));
				return INVALID_PARAMETERS;
			}
			rcDest = *pkDestRect;
		}
		else
		{
			rcDest.uiLeft	= 0;
			rcDest.uiTop	= 0;
			rcDest.uiRight	= pkDestSurface->GetWidth();
			rcDest.uiBottom = pkDestSurface->GetHeight();
		}

		FLOAT32* pfDestData = NULL;
		Result eResult = pkDestSurface->LockRect((void**)&pfDestData, pkDestRect);
		if(CORE3D_FAILED(eResult))
		{
			CORE3D_ERROR(_T("Surface::CopyToSurface() - Couldn't lock destination surface.\n"));
			return eResult;
		}

		const UINT32 DEST_FLOATS	= pkDestSurface->GetFormatFloats();
		const UINT32 DEST_WIDTH		= rcDest.uiRight - rcDest.uiLeft;
		const UINT32 DEST_HEIGHT	= rcDest.uiBottom - rcDest.uiTop;
		if((NULL == pkSrcRect) && (NULL == pkDestRect) && (DEST_FLOATS == GetFormatFloats()))
		{
			memcpy(pfDestData, m_pfData, sizeof(FLOAT32) * DEST_FLOATS * DEST_WIDTH * DEST_HEIGHT);
			pkDestSurface->UnlockRect();
			return OK;
		}

		FLOAT32 STEP_U	= 1.0f / m_uiWidthMin;
		FLOAT32 STEP_V	= 1.0f / m_uiHeightMin;
		FLOAT32 fSrcV	= rcSrc.uiTop * STEP_V;
		for(UINT32 uiY = 0; uiY < DEST_HEIGHT; ++uiY, fSrcV += STEP_V)
		{
			FLOAT32 fSrcU = rcSrc.uiLeft * STEP_U;
			for(UINT32 uiX = 0; uiX < DEST_WIDTH; ++uiX, fSrcU += STEP_U, pfDestData += DEST_FLOATS)
			{
				Vector4 kSrcColor;
				if(TF_LINEAR == eFilter)	{SampleLinear(kSrcColor, fSrcU, fSrcV);}
				else						{SamplePoint(kSrcColor, fSrcU, fSrcV);}

				switch(DEST_FLOATS)
				{
				case 4: pfDestData[3] = kSrcColor.a;
				case 3: pfDestData[2] = kSrcColor.b;
				case 2: pfDestData[1] = kSrcColor.g;
				case 1: pfDestData[0] = kSrcColor.r;
				}
			}
		}
		pkDestSurface->UnlockRect();
		return OK;
	}
}