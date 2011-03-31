#include "Texture.h"
#include "Device.h"
#include "Surface.h"

namespace Core3D
{
	Texture::Texture(Device* pkDevice)
		: BaseTexture(pkDevice)
		, m_uiMipLevels(0)
		, m_ppkMipLevels(NULL)
	{

	}

	Texture::~Texture()
	{
		for(UINT32 uiLevel = 0; uiLevel < m_uiMipLevels; ++uiLevel)
		{
			CORE3D_SAFE_RELEASE(m_ppkMipLevels[uiLevel]);
		}
		CORE3D_SAFE_DELETEARRAY(m_ppkMipLevels);
	}

	Result Texture::Create(UINT32 uiWidth, UINT32 uiHeight, UINT32 uiMipLevels, Format eFormat)
	{
		if(0 == uiWidth || 0 == uiHeight)
		{
			CORE3D_ERROR(_T("Texture::Create() - Texture dimensions are invalid.\n"));
			return INVALID_PARAMETERS;
		}

		if(FMT_R32F > eFormat || FMT_R32G32B32A32F < eFormat)
		{
			CORE3D_ERROR(_T("Texture::Create() - Invalid format specified.\n"));
			return INVALID_FORMAT;
		}

		m_fSquaredWidth		= static_cast<FLOAT32>(uiWidth * uiWidth);
		m_fSquaredHeight	= static_cast<FLOAT32>(uiHeight * uiHeight);

		if(0 == uiMipLevels)
		{
			UINT32 uiTempWidth	= uiWidth;
			UINT32 uiTempHeight = uiHeight;
			do 
			{
				++uiMipLevels;
				uiTempWidth		>>= 1;
				uiTempHeight	>>= 1;
			} while((0 != uiTempWidth) && (0 != uiTempHeight));
		}

		m_ppkMipLevels = new Surface*[uiMipLevels];
		if(NULL == m_ppkMipLevels)
		{
			CORE3D_ERROR(_T("Texture::Create() - Out of memory, cannot create mip-levels.\n"));
			return OUT_OF_MEMORY;
		}
		memset(m_ppkMipLevels, 0, sizeof(Surface*) * uiMipLevels);

		Surface** ppkCurrentMipLevel = m_ppkMipLevels;
		do 
		{
			Result eResult = m_pkDevice->CreateSurface(ppkCurrentMipLevel, uiWidth, uiHeight, eFormat);
			if(CORE3D_FAILED(eResult))
			{
				CORE3D_ERROR(_T("Texture::Create() - Creation of mip-level failed.\n"));
				return eResult;
			}

			++m_uiMipLevels;
			++ppkCurrentMipLevel;
			
			--uiMipLevels;
			if(0 == uiMipLevels) {break;}

			uiWidth		>>= 1;
			uiHeight	>>= 1;
		} while((0 != uiWidth) && (0 == uiHeight));
		
		return OK;
	}

	TextureSampleInput Texture::GetTextureSampleInput()
	{
		return TSI_2COORDS;
	}

	Result Texture::Clear(UINT32 uiMipLevel, const Vector4& rkColor, const Rect* pkRect)
	{
		if(uiMipLevel >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("Texture::Clear() - Invalid mip-level specified.\n"));
			return INVALID_PARAMETERS;
		}
		return m_ppkMipLevels[uiMipLevel]->Clear(rkColor, pkRect);
	}

	Result Texture::GenerateMipSubLevels(UINT32 uiSrcLevel)
	{
		if((uiSrcLevel + 1) >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("Texture::GenerateMipSubLevels() - Source level refers either to last mip-level or is larger than the number of mip-levels.\n"));
			return INVALID_PARAMETERS;
		}

		for(UINT32 uiLevel = uiSrcLevel + 1; uiLevel < m_uiMipLevels; ++uiLevel)
		{
			const FLOAT32* pfSrcData	= NULL;
			Result eResult				= LockRect(uiLevel - 1, (void**)&pfSrcData, NULL);
			if(CORE3D_FAILED(eResult)) {return eResult;}

			FLOAT32* pfDestData = NULL;
			eResult				= LockRect(uiLevel, (void**)&pfDestData, NULL);
			if(CORE3D_FAILED(eResult))
			{
				UnlockRect(uiLevel - 1);
				return eResult;
			}

			const UINT32 SRC_WIDTH	= GetWidth(uiLevel - 1);
			const UINT32 SRC_HEIGHT = GetHeight(uiLevel - 1);

			switch(GetFormat())
			{
			case FMT_R32F:
				{
					for(UINT32 uiY = 0; uiY < SRC_HEIGHT; uiY += 2)
					{
						const UINT32 INDEX_ROWS[2] = {uiY * SRC_WIDTH, (uiY + 1) * SRC_WIDTH};
						for(UINT32 uiX = 0; uiX < SRC_WIDTH; uiX += 2, ++pfDestData)
						{
							const FLOAT32 SRC_PIXELS[4] = 
							{
								pfSrcData[INDEX_ROWS[0] + uiX],
								pfSrcData[INDEX_ROWS[0] + uiX + 1],
								pfSrcData[INDEX_ROWS[1] + uiX],
								pfSrcData[INDEX_ROWS[1] + uiX + 1]
							};
							*pfDestData = (SRC_PIXELS[0] + SRC_PIXELS[1] + SRC_PIXELS[2] + SRC_PIXELS[3]) * 0.25f;
						}
					}
				}
				break;
			case FMT_R32G32F:
				{
					for(UINT32 uiY = 0; uiY < SRC_HEIGHT; uiY += 2)
					{
						const UINT32 INDEX_ROWS[2] = {uiY * SRC_WIDTH, (uiY + 1) * SRC_WIDTH};
						for(UINT32 uiX = 0; uiX < SRC_WIDTH; uiX += 2, pfDestData += 2)
						{
							const Vector2* SRC_PIXELS[4] = 
							{
								&((Vector2*)pfSrcData)[INDEX_ROWS[0] + uiX],
								&((Vector2*)pfSrcData)[INDEX_ROWS[0] + uiX + 1],
								&((Vector2*)pfSrcData)[INDEX_ROWS[1] + uiX],
								&((Vector2*)pfSrcData)[INDEX_ROWS[1] + uiX + 1]
							};
							*((Vector2*)pfDestData) = (*SRC_PIXELS[0] + *SRC_PIXELS[1] + *SRC_PIXELS[2] + *SRC_PIXELS[3]) * 0.25f;
						}
					}
				}
				break;
			case FMT_R32G32B32F:
				{
					for(UINT32 uiY = 0; uiY < SRC_HEIGHT; uiY += 2)
					{
						const UINT32 INDEX_ROWS[2] = {uiY * SRC_WIDTH, (uiY + 1) * SRC_WIDTH};
						for(UINT32 uiX = 0; uiX < SRC_WIDTH; uiX += 2, pfDestData += 3)
						{
							const Vector3* SRC_PIXELS[4] = 
							{
								&((Vector3*)pfSrcData)[INDEX_ROWS[0] + uiX],
								&((Vector3*)pfSrcData)[INDEX_ROWS[0] + uiX + 1],
								&((Vector3*)pfSrcData)[INDEX_ROWS[1] + uiX],
								&((Vector3*)pfSrcData)[INDEX_ROWS[1] + uiX + 1]
							};
							*((Vector3*)pfDestData) = (*SRC_PIXELS[0] + *SRC_PIXELS[1] + *SRC_PIXELS[2] + *SRC_PIXELS[3]) * 0.25f;
						}
					}
				}
				break;
			case FMT_R32G32B32A32F:
				{
					for(UINT32 uiY = 0; uiY < SRC_HEIGHT; uiY += 2)
					{
						const UINT32 INDEX_ROWS[2] = {uiY * SRC_WIDTH, (uiY + 1) * SRC_WIDTH};
						for(UINT32 uiX = 0; uiX < SRC_WIDTH; uiX += 2, pfDestData += 4)
						{
							const Vector4* SRC_PIXELS[4] = 
							{
								&((Vector4*)pfSrcData)[INDEX_ROWS[0] + uiX],
								&((Vector4*)pfSrcData)[INDEX_ROWS[0] + uiX + 1],
								&((Vector4*)pfSrcData)[INDEX_ROWS[1] + uiX],
								&((Vector4*)pfSrcData)[INDEX_ROWS[1] + uiX + 1]
							};
							*((Vector4*)pfDestData) = (*SRC_PIXELS[0] + *SRC_PIXELS[1] + *SRC_PIXELS[2] + *SRC_PIXELS[3]) * 0.25f;
						}
					}
				}
				break;
			}

			UnlockRect(uiLevel);
			UnlockRect(uiLevel - 1);
		}
		return OK;
	}

	Result Texture::LockRect(UINT32 uiMipLevel, void** ppvData, const Rect* pkRect)
	{
		if(uiMipLevel >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("Texture::LockRect() - Invalid mip-level specified.\n"));
			return INVALID_PARAMETERS;
		}
		return m_ppkMipLevels[uiMipLevel]->LockRect(ppvData, pkRect);
	}

	Result Texture::UnlockRect(UINT32 uiMipLevel)
	{
		if(uiMipLevel >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("Texture::UnlockRect() - Invalid mip-level specified.\n"));
			return INVALID_PARAMETERS;
		}
		return m_ppkMipLevels[uiMipLevel]->UnlockRect();
	}

	Surface* Texture::GetMipLevel(UINT32 uiMipLevel)
	{
		if(uiMipLevel >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("Texture::GetMipLevel() - Invalid mip-level specified.\n"));
			return NULL;
		}
		m_ppkMipLevels[uiMipLevel]->AddRef();
		return m_ppkMipLevels[uiMipLevel];
	}

	Result Texture::SampleTexture(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV, FLOAT32 fW, const Vector4* pkXGradient, const Vector4* pkYGradient, const UINT32* puiSamplerStates)
	{
		UINT32 uiTextureFilter		= puiSamplerStates[TSS_MINFILTER];
		FLOAT32 fTextureMipLevel	= 0.0f;
		if((NULL != pkXGradient) && (NULL != pkYGradient))
		{
			// COMMENT : Compute the mip level and determine the texture filter type
			const FLOAT32 LEN_X_GRAD = pkXGradient->x * pkXGradient->x * m_fSquaredWidth + 
									   pkXGradient->y * pkXGradient->y * m_fSquaredHeight;
			const FLOAT32 LEN_Y_GRAD = pkYGradient->x * pkYGradient->x * m_fSquaredWidth + 
									   pkYGradient->y * pkYGradient->y * m_fSquaredHeight;
			const FLOAT32 TEXELS_PER_SCREENPIXEL = sqrtf(LEN_X_GRAD > LEN_Y_GRAD ? LEN_X_GRAD : LEN_Y_GRAD);

			if(TEXELS_PER_SCREENPIXEL <= 1.0f)
			{
				// COMMENT : if texels per screen-pixel < 1.0f --> magnification, no mip-mapping needed
				fTextureMipLevel	= 0.0f;
				uiTextureFilter		= puiSamplerStates[TSS_MAGFILTER];
			}
			else
			{
				// COMMENT : Minification, need mip-mapping
				static const FLOAT32 INV_LOG2 = 1.0f / logf(2.0f);
				fTextureMipLevel	= logf(TEXELS_PER_SCREENPIXEL) * INV_LOG2;
				uiTextureFilter		= puiSamplerStates[TSS_MINFILTER];
			}
		}

		const FLOAT32 MIP_LOD_BIAS = *((FLOAT32*)&puiSamplerStates[TSS_MIPLODBIAS]);
		const FLOAT32 MAX_MIPLEVEL = *((FLOAT32*)&puiSamplerStates[TSS_MAXMIPLEVEL]);
		fTextureMipLevel = Core3D::Clamp<FLOAT32>(fTextureMipLevel + MIP_LOD_BIAS, 0.0f, MAX_MIPLEVEL);

		if(TF_LINEAR == puiSamplerStates[TSS_MIPFILTER])
		{
			UINT32 uiMipLevelA = Core3D::FtoL(fTextureMipLevel);
			UINT32 uiMipLevelB = uiMipLevelA + 1;
			if(uiMipLevelA >= m_uiMipLevels) {uiMipLevelA = m_uiMipLevels - 1;}
			if(uiMipLevelB >= m_uiMipLevels) {uiMipLevelB = m_uiMipLevels - 1;}

			Vector4 kColorA, kColorB;
			if(TF_LINEAR == uiTextureFilter)
			{
				m_ppkMipLevels[uiMipLevelA]->SampleLinear(kColorA, fU, fV);
				m_ppkMipLevels[uiMipLevelB]->SampleLinear(kColorB, fU, fV);
			}
			else
			{
				m_ppkMipLevels[uiMipLevelA]->SamplePoint(kColorA, fU, fV);
				m_ppkMipLevels[uiMipLevelB]->SamplePoint(kColorA, fU, fV);
			}
			const FLOAT32 INTERPOLATION = fTextureMipLevel - static_cast<FLOAT32>(uiMipLevelA);
			Core3D::Vec4Lerp(rkColor, kColorA, kColorB, INTERPOLATION);
		}
		else
		{
			UINT32 uiMipLevel = Core3D::FtoL(fTextureMipLevel);
			if(uiMipLevel >= m_uiMipLevels) {uiMipLevel = m_uiMipLevels - 1;}

			if(TF_LINEAR == uiTextureFilter){m_ppkMipLevels[uiMipLevel]->SampleLinear(rkColor, fU, fV);}
			else							{m_ppkMipLevels[uiMipLevel]->SamplePoint(rkColor, fU, fV);}
		}
		return OK;
	}

	Format Texture::GetFormat()
	{
		return m_ppkMipLevels[0]->GetFormat();
	}

	UINT32 Texture::GetFormatFloats()
	{
		return m_ppkMipLevels[0]->GetFormatFloats();
	}

	UINT32 Texture::GetMipLevels()
	{
		return m_uiMipLevels;
	}

	UINT32 Texture::GetWidth(UINT32 uiMipLevel /* = 0 */)
	{
		if(uiMipLevel >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("Texture::GetWidth() - Invalid mip-level specified.\n"));
			return 0;
		}
		return m_ppkMipLevels[uiMipLevel]->GetWidth();
	}

	UINT32 Texture::GetHeight(UINT32 uiMipLevel /* = 0 */)
	{
		if(uiMipLevel >= m_uiMipLevels)
		{
			CORE3D_ERROR(_T("Texture::GetHeight() - Invalid mip-level specified.\n"));
			return 0;
		}
		return m_ppkMipLevels[uiMipLevel]->GetHeight();
	}
}