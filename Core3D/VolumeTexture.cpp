#include "VolumeTexture.h"
#include "Device.h"
#include "Volume.h"

namespace Core3D
{
	VolumeTexture::VolumeTexture(Device* pkDevice)
		: BaseTexture(pkDevice)
		, m_uiMipLevels(0)
		, m_ppkMipLevels(NULL)
	{

	}

	VolumeTexture::~VolumeTexture()
	{
		for(UINT32 uiLevel = 0; uiLevel < m_uiMipLevels; ++uiLevel)
		{
			CORE3D_SAFE_RELEASE(m_ppkMipLevels[uiLevel]);
		}
		CORE3D_SAFE_DELETEARRAY(m_ppkMipLevels);
	}

	Result VolumeTexture::Create(UINT32 uiWidth, UINT32 uiHeight, UINT32 uiDepth, UINT32 uiMipLevels, Format eFormat)
	{
		if((0 == uiWidth) || (0 == uiHeight) || (0 == uiDepth))
		{
			CORE3D_ERROR(_T("VolumeTexture::Create() - Texture dimensions are invalid.\n"));
			return INVALID_PARAMETERS;
		}

		if((eFormat < FMT_R32F) || (eFormat > FMT_R32G32B32A32F))
		{
			CORE3D_ERROR(_T("VolumeTexture::Create() - Invalid format specified.\n"));
			return INVALID_FORMAT;
		}

		m_fSquaredWidth		= static_cast<FLOAT32>(uiWidth * uiWidth);
		m_fSquaredHeight	= static_cast<FLOAT32>(uiHeight * uiHeight);
		m_fSquaredDepth		= static_cast<FLOAT32>(uiDepth * uiDepth);
		if(0 == uiMipLevels)
		{
			UINT32 uiTempWidth = uiWidth, uiTempHeight = uiHeight, uiTempDepth = uiDepth;
			do 
			{
				++uiMipLevels;
				uiTempWidth		>>= 1;
				uiTempHeight	>>= 1;
				uiTempDepth		>>= 1;
			} while(uiTempWidth && uiTempHeight && uiTempDepth);
		}

		m_ppkMipLevels = new Volume*[uiMipLevels];
		if(NULL == m_ppkMipLevels)
		{
			CORE3D_ERROR(_T("VolumeTexture::Create() - Out of memory, cannot create mip levels.\n"));
			return OUT_OF_MEMORY;
		}
		memset(m_ppkMipLevels, 0, sizeof(Volume*) * uiMipLevels);

		Volume** ppkCurrentMipLevel = m_ppkMipLevels;
		do 
		{
			Result eResult = m_pkDevice->CreateVolume(ppkCurrentMipLevel, uiWidth, uiHeight, uiDepth, eFormat);
			if(CORE3D_FAILED(eResult))
			{
				CORE3D_ERROR(_T("VolumeTexture::Create() - Creation of mip level failed.\n"));
				return eResult;
			}

			++m_uiMipLevels;
			++ppkCurrentMipLevel;
			
			if(0 == (--uiMipLevels)) {break;}
			uiWidth		>>= 1;
			uiHeight	>>= 1;
			uiDepth		>>= 1;
		} while(uiWidth && uiHeight && uiDepth);

		return OK;
	}

	TextureSampleInput VolumeTexture::GetTextureSampleInput()
	{
		return TSI_3COORDS;
	}

	Result VolumeTexture::Clear(UINT32 uiMipLevel, const Vector4& rkColor, const Box* pkBox)
	{
		if(uiMipLevel >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("VolumeTexture::Clear() - Invalid mip level specified.\n"));
			return INVALID_PARAMETERS;
		}
		return m_ppkMipLevels[uiMipLevel]->Clear(rkColor, pkBox);
	}

	Result VolumeTexture::GenerateMipSubLevels(UINT32 uiSrcLevel)
	{
		if((uiSrcLevel + 1) >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("VolumeTexture::GenerateMipSubLevels() - Source level refers either to last mip level of is larger than the number of mip levels.\n"));
			return INVALID_PARAMETERS;
		}

		for(UINT32 uiLevel = uiSrcLevel + 1; uiLevel < m_uiMipLevels; ++uiLevel)
		{
			const FLOAT32* pfSrcData = NULL;
			Result eResult = LockBox(uiLevel - 1, (void**)&pfSrcData, NULL);
			if(CORE3D_FAILED(eResult)) {return eResult;}

			FLOAT32* pfDestData = NULL;
			eResult = LockBox(uiLevel, (void**)&pfDestData, NULL);
			if(CORE3D_FAILED(eResult))
			{
				UnlockBox(uiLevel - 1);
				return eResult;
			}

			const UINT32 SRC_DEPTH	= GetDepth(uiLevel - 1);
			const UINT32 SRC_HEIGHT = GetHeight(uiLevel - 1);
			const UINT32 SRC_WIDTH	= GetWidth(uiLevel - 1);
			switch(GetFormat())
			{
			case FMT_R32F:
				{
					for(UINT32 uiZ = 0; uiZ < SRC_DEPTH; uiZ += 2)
					{
						const UINT32 INDEX_SLICES[2] = {uiZ * SRC_WIDTH * SRC_HEIGHT, (uiZ + 1) * SRC_WIDTH * SRC_HEIGHT};
						for(UINT32 uiY = 0; uiY < SRC_HEIGHT; uiY += 2)
						{
							const UINT32 INDEX_ROWS[2] = {uiY * SRC_WIDTH, (uiY + 1) * SRC_WIDTH};
							for(UINT32 uiX = 0; uiX < SRC_WIDTH; uiX += 2, ++pfDestData)
							{
								const FLOAT32 SRC_PIXELS[8] = 
								{
									pfSrcData[INDEX_SLICES[0] + INDEX_ROWS[0] + uiX],
									pfSrcData[INDEX_SLICES[0] + INDEX_ROWS[0] + uiX + 1],
									pfSrcData[INDEX_SLICES[0] + INDEX_ROWS[1] + uiX],
									pfSrcData[INDEX_SLICES[0] + INDEX_ROWS[1] + uiX + 1],

									pfSrcData[INDEX_SLICES[1] + INDEX_ROWS[0] + uiX],
									pfSrcData[INDEX_SLICES[1] + INDEX_ROWS[0] + uiX + 1],
									pfSrcData[INDEX_SLICES[1] + INDEX_ROWS[1] + uiX],
									pfSrcData[INDEX_SLICES[1] + INDEX_ROWS[1] + uiX + 1]
								};
								*pfDestData = (SRC_PIXELS[0] + SRC_PIXELS[1] + SRC_PIXELS[2] + SRC_PIXELS[3] + 
									SRC_PIXELS[4] + SRC_PIXELS[5] + SRC_PIXELS[6] + SRC_PIXELS[7]) * 0.125f;
							}
						}
					}
				}
				break;
			case FMT_R32G32F:
				{
					for(UINT32 uiZ = 0; uiZ < SRC_DEPTH; uiZ += 2)
					{
						const UINT32 INDEX_SLICES[2] = {uiZ * SRC_WIDTH * SRC_HEIGHT, (uiZ + 1) * SRC_WIDTH * SRC_HEIGHT};
						for(UINT32 uiY = 0; uiY < SRC_HEIGHT; uiY += 2)
						{
							const UINT32 INDEX_ROWS[2] = {uiY * SRC_WIDTH, (uiY + 1) * SRC_WIDTH};
							for(UINT32 uiX = 0; uiX < SRC_WIDTH; uiX += 2, pfDestData += 2)
							{
								const Vector2* SRC_PIXELS[8] = 
								{
									&((Vector2*)pfSrcData)[INDEX_SLICES[0] + INDEX_ROWS[0] + uiX],
									&((Vector2*)pfSrcData)[INDEX_SLICES[0] + INDEX_ROWS[0] + uiX + 1],
									&((Vector2*)pfSrcData)[INDEX_SLICES[0] + INDEX_ROWS[1] + uiX],
									&((Vector2*)pfSrcData)[INDEX_SLICES[0] + INDEX_ROWS[1] + uiX + 1],

									&((Vector2*)pfSrcData)[INDEX_SLICES[1] + INDEX_ROWS[0] + uiX],
									&((Vector2*)pfSrcData)[INDEX_SLICES[1] + INDEX_ROWS[0] + uiX + 1],
									&((Vector2*)pfSrcData)[INDEX_SLICES[1] + INDEX_ROWS[1] + uiX],
									&((Vector2*)pfSrcData)[INDEX_SLICES[1] + INDEX_ROWS[1] + uiX + 1]
								};
								*(Vector2*)pfDestData = (*SRC_PIXELS[0] + *SRC_PIXELS[1] + *SRC_PIXELS[2] + *SRC_PIXELS[3] + 
									*SRC_PIXELS[4] + *SRC_PIXELS[5] + *SRC_PIXELS[6] + *SRC_PIXELS[7]) * 0.125f;
							}
						}
					}
				}
				break;
			case FMT_R32G32B32F:
				{
					for(UINT32 uiZ = 0; uiZ < SRC_DEPTH; uiZ += 2)
					{
						const UINT32 INDEX_SLICES[2] = {uiZ * SRC_WIDTH * SRC_HEIGHT, (uiZ + 1) * SRC_WIDTH * SRC_HEIGHT};
						for(UINT32 uiY = 0; uiY < SRC_HEIGHT; uiY += 2)
						{
							const UINT32 INDEX_ROWS[2] = {uiY * SRC_WIDTH, (uiY + 1) * SRC_WIDTH};
							for(UINT32 uiX = 0; uiX < SRC_WIDTH; uiX += 2, pfDestData += 3)
							{
								const Vector3* SRC_PIXELS[8] = 
								{
									&((Vector3*)pfSrcData)[INDEX_SLICES[0] + INDEX_ROWS[0] + uiX],
									&((Vector3*)pfSrcData)[INDEX_SLICES[0] + INDEX_ROWS[0] + uiX + 1],
									&((Vector3*)pfSrcData)[INDEX_SLICES[0] + INDEX_ROWS[1] + uiX],
									&((Vector3*)pfSrcData)[INDEX_SLICES[0] + INDEX_ROWS[1] + uiX + 1],

									&((Vector3*)pfSrcData)[INDEX_SLICES[1] + INDEX_ROWS[0] + uiX],
									&((Vector3*)pfSrcData)[INDEX_SLICES[1] + INDEX_ROWS[0] + uiX + 1],
									&((Vector3*)pfSrcData)[INDEX_SLICES[1] + INDEX_ROWS[1] + uiX],
									&((Vector3*)pfSrcData)[INDEX_SLICES[1] + INDEX_ROWS[1] + uiX + 1]
								};
								*(Vector3*)pfDestData = (*SRC_PIXELS[0] + *SRC_PIXELS[1] + *SRC_PIXELS[2] + *SRC_PIXELS[3] + 
									*SRC_PIXELS[4] + *SRC_PIXELS[5] + *SRC_PIXELS[6] + *SRC_PIXELS[7]) * 0.125f;
							}
						}
					}
				}
				break;
			case FMT_R32G32B32A32F:
				{
					for(UINT32 uiZ = 0; uiZ < SRC_DEPTH; uiZ += 2)
					{
						const UINT32 INDEX_SLICES[2] = {uiZ * SRC_WIDTH * SRC_HEIGHT, (uiZ + 1) * SRC_WIDTH * SRC_HEIGHT};
						for(UINT32 uiY = 0; uiY < SRC_HEIGHT; uiY += 2)
						{
							const UINT32 INDEX_ROWS[2] = {uiY * SRC_WIDTH, (uiY + 1) * SRC_WIDTH};
							for(UINT32 uiX = 0; uiX < SRC_WIDTH; uiX += 2, pfDestData += 4)
							{
								const Vector4* SRC_PIXELS[8] = 
								{
									&((Vector4*)pfSrcData)[INDEX_SLICES[0] + INDEX_ROWS[0] + uiX],
									&((Vector4*)pfSrcData)[INDEX_SLICES[0] + INDEX_ROWS[0] + uiX + 1],
									&((Vector4*)pfSrcData)[INDEX_SLICES[0] + INDEX_ROWS[1] + uiX],
									&((Vector4*)pfSrcData)[INDEX_SLICES[0] + INDEX_ROWS[1] + uiX + 1],

									&((Vector4*)pfSrcData)[INDEX_SLICES[1] + INDEX_ROWS[0] + uiX],
									&((Vector4*)pfSrcData)[INDEX_SLICES[1] + INDEX_ROWS[0] + uiX + 1],
									&((Vector4*)pfSrcData)[INDEX_SLICES[1] + INDEX_ROWS[1] + uiX],
									&((Vector4*)pfSrcData)[INDEX_SLICES[1] + INDEX_ROWS[1] + uiX + 1]
								};
								*(Vector4*)pfDestData = (*SRC_PIXELS[0] + *SRC_PIXELS[1] + *SRC_PIXELS[2] + *SRC_PIXELS[3] + 
									*SRC_PIXELS[4] + *SRC_PIXELS[5] + *SRC_PIXELS[6] + *SRC_PIXELS[7]) * 0.125f;
							}
						}
					}
				}
				break;
			}

			UnlockBox(uiLevel);
			UnlockBox(uiLevel - 1);
		}
		return OK;
	}

	Result VolumeTexture::LockBox(UINT32 uiMipLevel, void** ppvData, const Box* pkBox)
	{
		if(uiMipLevel >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("VolumeTexture::LockBox() - Invalid mip level specified.\n"));
			return INVALID_PARAMETERS;
		}
		return m_ppkMipLevels[uiMipLevel]->LockBox(ppvData, pkBox);
	}

	Result VolumeTexture::UnlockBox(UINT32 uiMipLevel)
	{
		if(uiMipLevel >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("VolumeTexture::UnlockBox() - Invalid mip level specified.\n"));
			return INVALID_PARAMETERS;
		}
		return m_ppkMipLevels[uiMipLevel]->UnlockBox();
	}

	Volume* VolumeTexture::GetMipLevel(UINT32 uiMipLevel)
	{
		if(uiMipLevel >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("VolumeTexture::GetMipLevel() - Invalid mip level specified.\n"));
			return NULL;
		}
		m_ppkMipLevels[uiMipLevel]->AddRef();
		return m_ppkMipLevels[uiMipLevel];
	}

	Result VolumeTexture::SampleTexture(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV, FLOAT32 fW, const Vector4* pkXGradient, const Vector4* pkYGradient, const UINT32* puiSamplerStates)
	{
		UINT32 uiTextureFilter		= puiSamplerStates[TSS_MINFILTER];
		FLOAT32 fTextureMipLevel	= 0.0f;

		if((NULL != pkXGradient) && (NULL != pkYGradient))
		{
			// COMMENT : Compute the mip-level and determine the texture filter type.
			const FLOAT32 LEN_X_GRAD = pkXGradient->x * pkXGradient->x * m_fSquaredWidth + pkXGradient->y * pkXGradient->y * m_fSquaredHeight + 
									   pkXGradient->z * pkXGradient->z * m_fSquaredDepth;
			const FLOAT32 LEN_Y_GRAD = pkYGradient->x * pkYGradient->x * m_fSquaredWidth + pkYGradient->y * pkYGradient->y * m_fSquaredHeight + 
									   pkYGradient->z * pkYGradient->z * m_fSquaredDepth;
			const FLOAT32 TEXELS_PER_SCREENPIXEL = sqrtf(LEN_X_GRAD > LEN_Y_GRAD ? LEN_X_GRAD : LEN_Y_GRAD);

			if(1.0f >= TEXELS_PER_SCREENPIXEL)
			{
				fTextureMipLevel	= 0.0f;
				uiTextureFilter		= puiSamplerStates[TSS_MAGFILTER];
			}
			else
			{
				static const FLOAT32 INV_LOG2 = 1.0f / logf(2.0f);
				fTextureMipLevel	= logf(TEXELS_PER_SCREENPIXEL) * INV_LOG2;
				uiTextureFilter		= puiSamplerStates[TSS_MINFILTER];
			}
		}

		if(TF_LINEAR == puiSamplerStates[TSS_MIPFILTER])
		{
			UINT32 uiMipLevelA = Core3D::FtoL(fTextureMipLevel);
			UINT32 uiMipLevelB = uiMipLevelA + 1;
			if(uiMipLevelA >= m_uiMipLevels) {uiMipLevelA = m_uiMipLevels - 1;}
			if(uiMipLevelB >= m_uiMipLevels) {uiMipLevelB = m_uiMipLevels - 1;}

			Vector4 kColorA, kColorB;
			if(TF_LINEAR == uiTextureFilter)
			{
				m_ppkMipLevels[uiMipLevelA]->SampleLinear(kColorA, fU, fV, fW);
				m_ppkMipLevels[uiMipLevelB]->SampleLinear(kColorB, fU, fV, fW);
			}
			else
			{
				m_ppkMipLevels[uiMipLevelA]->SamplePoint(kColorA, fU, fV, fW);
				m_ppkMipLevels[uiMipLevelB]->SamplePoint(kColorB, fU, fV, fW);
			}

			FLOAT32 fInterpolation = fTextureMipLevel - static_cast<FLOAT32>(uiMipLevelA);
			Core3D::Vec4Lerp(rkColor, kColorA, kColorB, fInterpolation);
		}
		else
		{
			UINT32 uiMipLevel = Core3D::FtoL(fTextureMipLevel);
			if(uiMipLevel >= m_uiMipLevels)		{uiMipLevel = m_uiMipLevels - 1;}
			if(TF_LINEAR == uiTextureFilter)	{m_ppkMipLevels[uiMipLevel]->SampleLinear(rkColor, fU, fV, fW);}
			else								{m_ppkMipLevels[uiMipLevel]->SamplePoint(rkColor, fU, fV, fW);}
		}

		return OK;
	}

	Format VolumeTexture::GetFormat()
	{
		return m_ppkMipLevels[0]->GetFormat();
	}

	UINT32 VolumeTexture::GetFormatFloats()
	{
		return m_ppkMipLevels[0]->GetFormatFloats();
	}

	UINT32 VolumeTexture::GetMipLevels()
	{
		return m_uiMipLevels;
	}

	UINT32 VolumeTexture::GetWidth(UINT32 uiMipLevel /* = 0 */)
	{
		if(uiMipLevel >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("VolumeTexture::GetWidth() - Invalid mip-level specified.\n"));
			return 0;
		}
		return m_ppkMipLevels[uiMipLevel]->GetWidth();
	}

	UINT32 VolumeTexture::GetHeight(UINT32 uiMipLevel /* = 0 */)
	{
		if(uiMipLevel >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("VolumeTexture::GetHeight() - Invalid mip-level specified.\n"));
			return 0;
		}
		return m_ppkMipLevels[uiMipLevel]->GetHeight();
	}

	UINT32 VolumeTexture::GetDepth(UINT32 uiMipLevel /* = 0 */)
	{
		if(uiMipLevel >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("VolumeTexture::GetDepth() - Invalid mip-level specified.\n"));
			return 0;
		}
		return m_ppkMipLevels[uiMipLevel]->GetDepth();
	}
}