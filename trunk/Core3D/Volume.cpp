#include "Volume.h"
#include "Device.h"

namespace Core3D
{
	Volume::Volume(Device* pkDevice)
		: m_pkDevice(pkDevice)
		, m_uiWidth(0)
		, m_uiHeight(0)
		, m_uiDepth(0)
		, m_uiWidthMin(0)
		, m_uiHeightMin(0)
		, m_uiDepthMin(0)
		, m_bLockedComplete(false)
		, m_pfPartialLockData(NULL)
		, m_pfData(NULL)
	{

	}

	Volume::~Volume()
	{
		CORE3D_SAFE_DELETEARRAY(m_pfPartialLockData);
		CORE3D_SAFE_DELETEARRAY(m_pfData);
	}

	Result Volume::Create(UINT32 uiWidth, UINT32 uiHeight, UINT32 uiDepth, Format eFormat)
	{
		if((0 == uiWidth) || (0 == uiHeight) || (0 == uiDepth))
		{
			CORE3D_ERROR(_T("Volume::Create() - Volume dimensions are invalid.\n"));
			return INVALID_PARAMETERS;
		}

		UINT32 uiFloats;
		switch(eFormat)
		{
		case FMT_R32F:			uiFloats = 1; break;
		case FMT_R32G32F:		uiFloats = 2; break;
		case FMT_R32G32B32F:	uiFloats = 3; break;
		case FMT_R32G32B32A32F: uiFloats = 4; break;
		default: CORE3D_ERROR(_T("Volume::Create() - Invalid format specified.\n")); return INVALID_FORMAT;
		}

		m_eFormat		= eFormat;
		m_uiWidth		= uiWidth;
		m_uiHeight		= uiHeight;
		m_uiDepth		= uiDepth;
		m_uiWidthMin	= uiWidth - 1;
		m_uiHeightMin	= uiHeight - 1;
		m_uiDepthMin	= uiDepth - 1;
		m_pfData		= new FLOAT32[uiWidth * uiHeight * uiDepth * uiFloats];
		if(NULL == m_pfData)
		{
			CORE3D_ERROR(_T("Volume::Create() - Out of memory, cannot create volume.\n"));
			return OUT_OF_MEMORY;
		}
		return OK;
	}

	Result Volume::Clear(const Vector4& rkColor, const Box* pkBox)
	{
		Box kClearBox;
		if(NULL != pkBox)
		{
			if( (pkBox->uiRight > m_uiWidth)	|| 
				(pkBox->uiBottom > m_uiHeight)	|| 
				(pkBox->uiBack > m_uiDepth)		)
			{
				CORE3D_ERROR(_T("Volume::Clear() - Clear box exceeds volume's dimensions.\n"));
				return INVALID_PARAMETERS;
			}

			if( (pkBox->uiLeft >= pkBox->uiRight)	|| 
				(pkBox->uiTop >= pkBox->uiBottom)	|| 
				(pkBox->uiFront >= pkBox->uiBack)	)
			{
				CORE3D_ERROR(_T("Volume::Clear() - Invalid box specified.\n"));
				return INVALID_PARAMETERS;
			}
			kClearBox = *pkBox;
		}
		else
		{
			kClearBox.uiLeft	= 0;
			kClearBox.uiTop		= 0;
			kClearBox.uiFront	= 0;
			kClearBox.uiRight	= m_uiWidth;
			kClearBox.uiBottom	= m_uiHeight;
			kClearBox.uiBack	= m_uiDepth;
		}

		FLOAT32* pfData = NULL;
		Result eResult	= LockBox((void**)&pfData, NULL);
		if(CORE3D_FAILED(eResult)) {return eResult;}

		switch(m_eFormat)
		{
		case FMT_R32F:
			{
				for(UINT32 uiZ = kClearBox.uiFront; uiZ < kClearBox.uiBack; ++uiZ)
				{
					FLOAT32* pfCurrentData2 = &pfData[uiZ * m_uiWidth * m_uiHeight];
					for(UINT32 uiY = kClearBox.uiTop; uiY < kClearBox.uiBottom; ++uiY)
					{
						FLOAT32* pfCurrentData = &pfCurrentData2[uiY * m_uiWidth + kClearBox.uiLeft];
						for(UINT32 uiX = kClearBox.uiLeft; uiX < kClearBox.uiRight; ++uiX, ++pfCurrentData)
						{
							*pfCurrentData = rkColor.r;
						}
					}
				}
			}
			break;
		case FMT_R32G32F:
			{
				for(UINT32 uiZ = kClearBox.uiFront; uiZ < kClearBox.uiBack; ++uiZ)
				{
					Vector2* pkCurrentData2 = &((Vector2*)pfData)[uiZ * m_uiWidth * m_uiHeight];
					for(UINT32 uiY = kClearBox.uiTop; uiY < kClearBox.uiBottom; ++uiY)
					{
						Vector2* pkCurrentData = &pkCurrentData2[uiY * m_uiWidth + kClearBox.uiLeft];
						for(UINT32 uiX = kClearBox.uiLeft; uiX < kClearBox.uiRight; ++uiX, ++pkCurrentData)
						{
							*pkCurrentData = rkColor;
						}
					}
				}
			}
			break;
		case FMT_R32G32B32F:
			{
				for(UINT32 uiZ = kClearBox.uiFront; uiZ < kClearBox.uiBack; ++uiZ)
				{
					Vector3* pkCurrentData2 = &((Vector3*)pfData)[uiZ * m_uiWidth * m_uiHeight];
					for(UINT32 uiY = kClearBox.uiTop; uiY < kClearBox.uiBottom; ++uiY)
					{
						Vector3* pkCurrentData = &pkCurrentData2[uiY * m_uiWidth + kClearBox.uiLeft];
						for(UINT32 uiX = kClearBox.uiLeft; uiX < kClearBox.uiRight; ++uiX, ++pkCurrentData)
						{
							*pkCurrentData = rkColor;
						}
					}
				}
			}
			break;
		case FMT_R32G32B32A32F:
			{
				for(UINT32 uiZ = kClearBox.uiFront; uiZ < kClearBox.uiBack; ++uiZ)
				{
					Vector4* pkCurrentData2 = &((Vector4*)pfData)[uiZ * m_uiWidth * m_uiHeight];
					for(UINT32 uiY = kClearBox.uiTop; uiY < kClearBox.uiBottom; ++uiY)
					{
						Vector4* pkCurrentData = &pkCurrentData2[uiY * m_uiWidth + kClearBox.uiLeft];
						for(UINT32 uiX = kClearBox.uiLeft; uiX < kClearBox.uiRight; ++uiX, ++pkCurrentData)
						{
							*pkCurrentData = rkColor;
						}
					}
				}
			}
			break;
		default:
			CORE3D_ERROR(_T("Volume::Clear() - Invalid volume format.\n"));
			UnlockBox();
			return INVALID_FORMAT;
		}
		UnlockBox();
		return OK;
	}

	Result Volume::LockBox(void** ppvData, const Box* pkBox)
	{
		if(NULL == ppvData)
		{
			CORE3D_ERROR(_T("Volume::LockBox() - Data pointer pointer to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		if((false != m_bLockedComplete) || (NULL != m_pfPartialLockData))
		{
			CORE3D_ERROR(_T("Volume::LockBox() - Mip level is already locked.\n"));
			return INVALID_STATE;
		}

		if(NULL == pkBox)
		{
			*ppvData			= m_pfData;
			m_bLockedComplete	= true;
			return OK;
		}

		if( (pkBox->uiRight > m_uiWidth)	|| 
			(pkBox->uiBottom > m_uiHeight)	|| 
			(pkBox->uiBack > m_uiDepth)		)
		{
			CORE3D_ERROR(_T("Volume::LockBox() - Box exceeds volume dimensions.\n"));
			return INVALID_PARAMETERS;
		}

		if( (pkBox->uiLeft >= pkBox->uiRight) || 
			(pkBox->uiTop >= pkBox->uiBottom) || 
			(pkBox->uiFront >= pkBox->uiBack) )
		{
			CORE3D_ERROR(_T("Volume::LockBox() - Invalid box specified.\n"));
			return INVALID_PARAMETERS;
		}

		m_kPartialLockBox			= *pkBox;
		const UINT32 LOCK_WIDTH		= m_kPartialLockBox.uiRight - m_kPartialLockBox.uiLeft;
		const UINT32 LOCK_HEIGHT	= m_kPartialLockBox.uiBottom - m_kPartialLockBox.uiTop;
		const UINT32 LOCK_DEPTH		= m_kPartialLockBox.uiBack - m_kPartialLockBox.uiFront;
		const UINT32 VOLUME_FLOATS	= GetFormatFloats();

		m_pfPartialLockData = new FLOAT32[LOCK_WIDTH * LOCK_HEIGHT * LOCK_DEPTH * VOLUME_FLOATS];
		if(NULL == m_pfPartialLockData)
		{
			CORE3D_ERROR(_T("Volume::LockBox() - memory allocation failed.\n"));
			return OUT_OF_MEMORY;
		}

		FLOAT32* pfCurrentLockData = m_pfPartialLockData;
		for(UINT32 uiZ = m_kPartialLockBox.uiFront; uiZ < m_kPartialLockBox.uiBack; ++uiZ)
		{
			const FLOAT32* pfCurrentVolumeData2 = &m_pfData[(uiZ * m_uiWidth * m_uiHeight) * VOLUME_FLOATS];
			for(UINT32 uiY = m_kPartialLockBox.uiTop; uiY < m_kPartialLockBox.uiBottom; ++uiY)
			{
				const FLOAT32* pfCurrentVolumeData = &pfCurrentVolumeData2[(uiY * m_uiWidth + m_kPartialLockBox.uiLeft) * VOLUME_FLOATS];
				memcpy(pfCurrentLockData, pfCurrentVolumeData, sizeof(FLOAT32) * VOLUME_FLOATS * LOCK_WIDTH);
				pfCurrentLockData += (VOLUME_FLOATS * LOCK_WIDTH);
			}
		}

		*ppvData = m_pfPartialLockData;
		return OK;
	}

	Result Volume::UnlockBox()
	{
		if((false == m_bLockedComplete) && (NULL == m_pfPartialLockData))
		{
			CORE3D_ERROR(_T("Volume::UnlockBox() - Cannot unlock mip level decause it isn't locked.\n"));
			return INVALID_STATE;
		}

		if(true == m_bLockedComplete)
		{
			m_bLockedComplete = false;
			return OK;
		}

		const UINT32 LOCK_WIDTH		= m_kPartialLockBox.uiRight - m_kPartialLockBox.uiLeft;
		const UINT32 VOLUME_FLOATS	= GetFormatFloats();

		const FLOAT32* pfCurrentLockData = m_pfPartialLockData;
		for(UINT32 uiZ = m_kPartialLockBox.uiFront; uiZ < m_kPartialLockBox.uiBack; ++uiZ)
		{
			FLOAT32* pfCurrentVolumeData2 = &m_pfData[(uiZ * m_uiWidth * m_uiHeight) * VOLUME_FLOATS];
			for(UINT32 uiY = m_kPartialLockBox.uiTop; uiY < m_kPartialLockBox.uiBottom; ++uiY)
			{
				FLOAT32* pfCurrentVolumeData = &pfCurrentVolumeData2[(uiY * m_uiWidth + m_kPartialLockBox.uiLeft) * VOLUME_FLOATS];
				memcpy(pfCurrentVolumeData, pfCurrentLockData, sizeof(FLOAT32) * VOLUME_FLOATS * LOCK_WIDTH);
				pfCurrentLockData += (VOLUME_FLOATS * LOCK_WIDTH);
			}
		}

		CORE3D_SAFE_DELETEARRAY(m_pfPartialLockData);
		return OK;
	}

	UINT32 Volume::GetFormatFloats()
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

	void Volume::SamplePoint(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV, FLOAT32 fW)
	{
		FLOAT32 fX = fU * m_uiWidthMin;
		FLOAT32 fY = fV * m_uiHeightMin;
		FLOAT32 fZ = fW * m_uiDepthMin;
		const INT32 PIXEL_X = Core3D::FtoL(fX);
		const INT32 PIXEL_Y = Core3D::FtoL(fY);
		const INT32 PIXEL_Z = Core3D::FtoL(fZ);

		switch(m_eFormat)
		{
		case FMT_R32F:
			{
				FLOAT32* pfPixel = &m_pfData[PIXEL_Z * m_uiWidth * m_uiHeight + PIXEL_Y * m_uiWidth + PIXEL_X];
				rkColor = Vector4(pfPixel[0], 0.0f, 0.0f, 1.0f);
			}
			break;
		case FMT_R32G32F:
			{
				Vector2* pkPixel = &((Vector2*)m_pfData)[PIXEL_Z * m_uiWidth * m_uiHeight + PIXEL_Y * m_uiWidth + PIXEL_X];
				rkColor = Vector4(pkPixel->x, pkPixel->y, 0.0f, 1.0f);
			}
			break;
		case FMT_R32G32B32F:
			{
				Vector3* pkPixel = &((Vector3*)m_pfData)[PIXEL_Z * m_uiWidth * m_uiHeight + PIXEL_Y * m_uiWidth + PIXEL_X];
				rkColor = Vector4(pkPixel->x, pkPixel->y, pkPixel->z, 1.0f);
			}
			break;
		case FMT_R32G32B32A32F:
			{
				Vector4* pkPixel = &((Vector4*)m_pfData)[PIXEL_Z * m_uiWidth * m_uiHeight + PIXEL_Y * m_uiWidth + PIXEL_X];
				rkColor = *pkPixel;
			}
			break;
		}
	}

	void Volume::SampleLinear(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV, FLOAT32 fW)
	{
		FLOAT32 fX = fU * m_uiWidthMin;
		FLOAT32 fY = fV * m_uiHeightMin;
		FLOAT32 fZ = fW * m_uiDepthMin;
		const INT32 PIXEL_X = Core3D::FtoL(fX);
		const INT32 PIXEL_Y = Core3D::FtoL(fY);
		const INT32 PIXEL_Z = Core3D::FtoL(fZ);

		INT32 PIXEL_X2		= PIXEL_X + 1;
		INT32 PIXEL_Y2		= PIXEL_Y + 1;
		INT32 PIXEL_Z2		= PIXEL_Z + 1;
		if(PIXEL_X2 >= (INT32)m_uiWidth)	{PIXEL_X2 = m_uiWidthMin;}
		if(PIXEL_Y2 >= (INT32)m_uiHeight)	{PIXEL_Y2 = m_uiHeightMin;}
		if(PIXEL_Z2 >= (INT32)m_uiDepth)	{PIXEL_Z2 = m_uiDepthMin;}

		const INT32 INDEX_ROWS[2]		= {PIXEL_Y * m_uiWidth, PIXEL_Y2 * m_uiWidth};
		const INT32 INDEX_SLICES[2]		= {PIXEL_Z * m_uiWidth * m_uiHeight, PIXEL_Z2 * m_uiWidth * m_uiHeight};
		const FLOAT32 INTERPOLATION[3]	= {fX - (FLOAT32)PIXEL_X, fY - (FLOAT32)PIXEL_Y, fZ - (FLOAT32)PIXEL_Z};

		switch(m_eFormat)
		{
		case FMT_R32F:
			{
				FLOAT32 afColorSlices[2], afColorRows[2];

				afColorRows[0]		= Core3D::Lerp(m_pfData[INDEX_SLICES[0] + INDEX_ROWS[0] + PIXEL_X], m_pfData[INDEX_SLICES[0] + INDEX_ROWS[0] + PIXEL_X2], INTERPOLATION[0]);
				afColorRows[1]		= Core3D::Lerp(m_pfData[INDEX_SLICES[0] + INDEX_ROWS[1] + PIXEL_X], m_pfData[INDEX_SLICES[0] + INDEX_ROWS[1] + PIXEL_X2], INTERPOLATION[0]);
				afColorSlices[0]	= Core3D::Lerp(afColorRows[0], afColorRows[1], INTERPOLATION[1]);

				afColorRows[0]		= Core3D::Lerp(m_pfData[INDEX_SLICES[1] + INDEX_ROWS[0] + PIXEL_X], m_pfData[INDEX_SLICES[1] + INDEX_ROWS[0] + PIXEL_X2], INTERPOLATION[0]);
				afColorRows[1]		= Core3D::Lerp(m_pfData[INDEX_SLICES[1] + INDEX_ROWS[1] + PIXEL_X], m_pfData[INDEX_SLICES[1] + INDEX_ROWS[1] + PIXEL_X2], INTERPOLATION[0]);
				afColorSlices[1]	= Core3D::Lerp(afColorRows[0], afColorRows[1], INTERPOLATION[1]);

				FLOAT32 fFinalColor	= Core3D::Lerp(afColorSlices[0], afColorSlices[1], INTERPOLATION[2]);
				rkColor				= Vector4(fFinalColor, 0.0f, 0.0f, 1.0f);
			}
			break;
		case FMT_R32G32F:
			{
				Vector2* pkPixelData = (Vector2*)m_pfData;
				Vector2 akColorSlices[2], akColorRows[2];

				Core3D::Vec2Lerp(akColorRows[0], pkPixelData[INDEX_SLICES[0] + INDEX_ROWS[0] + PIXEL_X], pkPixelData[INDEX_SLICES[0] + INDEX_ROWS[0] + PIXEL_X2], INTERPOLATION[0]);
				Core3D::Vec2Lerp(akColorRows[1], pkPixelData[INDEX_SLICES[0] + INDEX_ROWS[1] + PIXEL_X], pkPixelData[INDEX_SLICES[0] + INDEX_ROWS[1] + PIXEL_X2], INTERPOLATION[0]);
				Core3D::Vec2Lerp(akColorSlices[0], akColorRows[0], akColorRows[1], INTERPOLATION[1]);

				Core3D::Vec2Lerp(akColorRows[0], pkPixelData[INDEX_SLICES[1] + INDEX_ROWS[0] + PIXEL_X], pkPixelData[INDEX_SLICES[1] + INDEX_ROWS[0] + PIXEL_X2], INTERPOLATION[0]);
				Core3D::Vec2Lerp(akColorRows[1], pkPixelData[INDEX_SLICES[1] + INDEX_ROWS[1] + PIXEL_X], pkPixelData[INDEX_SLICES[1] + INDEX_ROWS[1] + PIXEL_X2], INTERPOLATION[0]);
				Core3D::Vec2Lerp(akColorSlices[1], akColorRows[0], akColorRows[1], INTERPOLATION[1]);

				Vector2 kFinalColor;
				Core3D::Vec2Lerp(kFinalColor, akColorSlices[0], akColorSlices[1], INTERPOLATION[2]);
				rkColor = Vector4(kFinalColor.x, kFinalColor.y, 0.0f, 1.0f);
			}
			break;
		case FMT_R32G32B32F:
			{
				Vector3* pkPixelData = (Vector3*)m_pfData;
				Vector3 akColorSlices[2], akColorRows[2];

				Core3D::Vec3Lerp(akColorRows[0], pkPixelData[INDEX_SLICES[0] + INDEX_ROWS[0] + PIXEL_X], pkPixelData[INDEX_SLICES[0] + INDEX_ROWS[0] + PIXEL_X2], INTERPOLATION[0]);
				Core3D::Vec3Lerp(akColorRows[1], pkPixelData[INDEX_SLICES[0] + INDEX_ROWS[1] + PIXEL_X], pkPixelData[INDEX_SLICES[0] + INDEX_ROWS[1] + PIXEL_X2], INTERPOLATION[0]);
				Core3D::Vec3Lerp(akColorSlices[0], akColorRows[0], akColorRows[1], INTERPOLATION[1]);

				Core3D::Vec3Lerp(akColorRows[0], pkPixelData[INDEX_SLICES[1] + INDEX_ROWS[0] + PIXEL_X], pkPixelData[INDEX_SLICES[1] + INDEX_ROWS[0] + PIXEL_X2], INTERPOLATION[0]);
				Core3D::Vec3Lerp(akColorRows[1], pkPixelData[INDEX_SLICES[1] + INDEX_ROWS[1] + PIXEL_X], pkPixelData[INDEX_SLICES[1] + INDEX_ROWS[1] + PIXEL_X2], INTERPOLATION[0]);
				Core3D::Vec3Lerp(akColorSlices[1], akColorRows[0], akColorRows[1], INTERPOLATION[1]);

				Vector3 kFinalColor;
				Core3D::Vec3Lerp(kFinalColor, akColorSlices[0], akColorSlices[1], INTERPOLATION[2]);
				rkColor = Vector4(kFinalColor.x, kFinalColor.y, kFinalColor.z, 1.0f);
			}
			break;
		case FMT_R32G32B32A32F:
			{
				Vector4* pkPixelData = (Vector4*)m_pfData;
				Vector4 akColorSlices[2], akColorRows[2];

				Core3D::Vec4Lerp(akColorRows[0], pkPixelData[INDEX_SLICES[0] + INDEX_ROWS[0] + PIXEL_X], pkPixelData[INDEX_SLICES[0] + INDEX_ROWS[0] + PIXEL_X2], INTERPOLATION[0]);
				Core3D::Vec4Lerp(akColorRows[1], pkPixelData[INDEX_SLICES[0] + INDEX_ROWS[1] + PIXEL_X], pkPixelData[INDEX_SLICES[0] + INDEX_ROWS[1] + PIXEL_X2], INTERPOLATION[0]);
				Core3D::Vec4Lerp(akColorSlices[0], akColorRows[0], akColorRows[1], INTERPOLATION[1]);

				Core3D::Vec4Lerp(akColorRows[0], pkPixelData[INDEX_SLICES[1] + INDEX_ROWS[0] + PIXEL_X], pkPixelData[INDEX_SLICES[1] + INDEX_ROWS[0] + PIXEL_X2], INTERPOLATION[0]);
				Core3D::Vec4Lerp(akColorRows[1], pkPixelData[INDEX_SLICES[1] + INDEX_ROWS[1] + PIXEL_X], pkPixelData[INDEX_SLICES[1] + INDEX_ROWS[1] + PIXEL_X2], INTERPOLATION[0]);
				Core3D::Vec4Lerp(akColorSlices[1], akColorRows[0], akColorRows[1], INTERPOLATION[1]);

				Core3D::Vec4Lerp(rkColor, akColorSlices[0], akColorSlices[1], INTERPOLATION[2]);
			}
			break;
		}
	}

	Format Volume::GetFormat()
	{
		return m_eFormat;
	}

	UINT32 Volume::GetWidth()
	{
		return m_uiWidth;
	}

	UINT32 Volume::GetHeight()
	{
		return m_uiHeight;
	}

	UINT32 Volume::GetDepth()
	{
		return m_uiDepth;
	}

	Result Volume::CopyToVolume(const Box* pkSrcBox, Volume* pkDestVolume, const Box* pkDestBox, TextureFilter eFilter)
	{
		if(NULL == pkDestVolume)
		{
			CORE3D_ERROR(_T("Volume::CopyToVolume() - No destination volume specified.\n"));
			return INVALID_PARAMETERS;
		}

		if((TF_POINT != eFilter) || (TF_LINEAR != eFilter))
		{
			CORE3D_ERROR(_T("Volume::CopyToVolume() - Invalid filter specified.\n"));
			return INVALID_PARAMETERS;
		}

		Box kSrcBox;
		if(NULL != pkSrcBox)
		{
			if( (pkSrcBox->uiRight > m_uiWidth)		|| 
				(pkSrcBox->uiBottom > m_uiHeight)	|| 
				(pkSrcBox->uiBack > m_uiDepth)		)
			{
				CORE3D_ERROR(_T("Volume::CopyToVolume() - Source box exceeds volume dimensions.\n"));
				return INVALID_PARAMETERS;
			}

			if( (pkSrcBox->uiLeft >= pkSrcBox->uiRight) || 
				(pkSrcBox->uiTop >= pkSrcBox->uiBottom) || 
				(pkSrcBox->uiFront >= pkSrcBox->uiBack)	)
			{
				CORE3D_ERROR(_T("Volume::CopyToVolume() - Invalid source box specified.\n"));
				return INVALID_PARAMETERS;
			}
			kSrcBox = *pkSrcBox;
		}
		else
		{
			kSrcBox.uiLeft		= 0;
			kSrcBox.uiTop		= 0;
			kSrcBox.uiFront		= 0;
			kSrcBox.uiRight		= m_uiWidth;
			kSrcBox.uiBottom	= m_uiHeight;
			kSrcBox.uiBack		= m_uiDepth;
		}

		Box kDestBox;
		if(NULL != pkDestBox)
		{
			if( (pkDestBox->uiRight > pkDestVolume->GetWidth()) || 
				(pkDestBox->uiTop > pkDestVolume->GetHeight())	|| 
				(pkDestBox->uiBack > pkDestVolume->GetDepth())	)
			{
				CORE3D_ERROR(_T("Volume::CopyToVolume() - Destination box exceeds volume dimensions.\n"));
				return INVALID_PARAMETERS;
			}

			if( (pkDestBox->uiLeft >= pkDestBox->uiRight)	|| 
				(pkDestBox->uiTop >= pkDestBox->uiBottom)	|| 
				(pkDestBox->uiFront >= pkDestBox->uiBack)	)
			{
				CORE3D_ERROR(_T("Volume::CopyToVolume() - Invalid destination box specified.\n"));
				return INVALID_PARAMETERS;
			}
			kDestBox = *pkDestBox;
		}
		else
		{
			kDestBox.uiLeft		= 0;
			kDestBox.uiTop		= 0;
			kDestBox.uiFront	= 0;
			kDestBox.uiRight	= pkDestVolume->GetWidth();
			kDestBox.uiBottom	= pkDestVolume->GetHeight();
			kDestBox.uiBack		= pkDestVolume->GetDepth();
		}

		FLOAT32* pfDestData = NULL;
		Result eResult = pkDestVolume->LockBox((void**)&pfDestData, pkDestBox);
		if(CORE3D_FAILED(eResult))
		{
			CORE3D_ERROR(_T("Volume::CopyToVolume() - Couldn't lock destination volume.\n"));
			return eResult;
		}

		const UINT32 DEST_FLOATS	= pkDestVolume->GetFormatFloats();
		const UINT32 DEST_WIDTH		= kDestBox.uiRight - kDestBox.uiLeft;
		const UINT32 DEST_HEIGHT	= kDestBox.uiBottom - kDestBox.uiTop;
		const UINT32 DEST_DEPTH		= kDestBox.uiBack - kDestBox.uiFront;
		if( (NULL == pkSrcBox) && (NULL == pkDestBox) && (DEST_FLOATS == GetFormatFloats())		&& 
			(DEST_WIDTH == m_uiWidth) && (DEST_HEIGHT == m_uiHeight) && (DEST_DEPTH == m_uiDepth))
		{
			memcpy(pfDestData, m_pfData, sizeof(FLOAT32) * DEST_FLOATS * DEST_WIDTH * DEST_HEIGHT * DEST_DEPTH);
			pkDestVolume->UnlockBox();
			return OK;
		}

		const FLOAT32 STEP_U = 1.0f / m_uiWidthMin;
		const FLOAT32 STEP_V = 1.0f / m_uiHeightMin;
		const FLOAT32 STEP_W = 1.0f / m_uiDepthMin;
		FLOAT32 fSrcW = (FLOAT32)kSrcBox.uiFront * STEP_W;
		for(UINT32 uiZ = 0; uiZ < DEST_DEPTH; ++uiZ, fSrcW += STEP_W)
		{
			FLOAT32 fSrcV = (FLOAT32)kSrcBox.uiTop * STEP_V;
			for(UINT32 uiY = 0; uiY < DEST_HEIGHT; ++uiY, fSrcV += STEP_V)
			{
				FLOAT32 fSrcU = (FLOAT32)kSrcBox.uiLeft * STEP_U;
				for(UINT32 uiX = 0; uiX < DEST_WIDTH; ++uiX, fSrcU += STEP_U, pfDestData += DEST_FLOATS)
				{
					Vector4 kSrcColor;
					if(TF_LINEAR == eFilter)	{SampleLinear(kSrcColor, fSrcU, fSrcV, fSrcW);}
					else						{SamplePoint(kSrcColor, fSrcU, fSrcV, fSrcW);}

					switch(DEST_FLOATS)
					{
					case 4: pfDestData[3] = kSrcColor.a;
					case 3: pfDestData[2] = kSrcColor.b;
					case 2: pfDestData[1] = kSrcColor.g;
					case 1: pfDestData[0] = kSrcColor.r;
					}
				}
			}
		}

		pkDestVolume->UnlockBox();
		return OK;
	}
}