#include "CubeTexture.h"
#include "Texture.h"
#include "Device.h"

namespace Core3D
{
	CubeTexture::CubeTexture(Device* pkDevice)
		: BaseTexture(pkDevice)
	{
		for(UINT32 uiFace = (UINT32)CF_POSITIVE_X; uiFace <= (UINT32)CF_NEGATIVE_Z; ++uiFace)
		{
			m_apkCubeFaces[uiFace] = NULL;
		}
	}

	CubeTexture::~CubeTexture()
	{
		for(UINT32 uiFace = (UINT32)CF_POSITIVE_X; uiFace <= CF_NEGATIVE_Z; ++uiFace)
		{
			CORE3D_SAFE_RELEASE(m_apkCubeFaces[uiFace]);
		}
	}

	Result CubeTexture::Create(UINT32 uiEdgeLength, UINT32 uiMipLevels, Format eFormat)
	{
		if(0 == uiEdgeLength)
		{
			CORE3D_ERROR(_T("CubeTexture::Create() - Edge length is invalid.\n"));
			return INVALID_PARAMETERS;
		}

		if(eFormat < FMT_R32F || eFormat > FMT_R32G32B32A32F)
		{
			CORE3D_ERROR(_T("CubeTexture::Create() - Invalid format specified.\n"));
			return INVALID_PARAMETERS;
		}

		Result eResult;
		for(UINT32 uiFace = (UINT32)CF_POSITIVE_X; uiFace <= (UINT32)CF_NEGATIVE_Z; ++uiFace)
		{
			eResult = m_pkDevice->CreateTexture(&m_apkCubeFaces[uiFace], uiEdgeLength, uiEdgeLength, uiMipLevels, eFormat);
			if(CORE3D_FAILED(eResult)) {return eResult;}
		}
		return OK;
	}

	TextureSampleInput CubeTexture::GetTextureSampleInput()
	{
		return TSI_VECTOR;
	}

	Result CubeTexture::GenerateMipSubLevels(UINT32 uiSrcLevel)
	{
		for(UINT32 uiFace = (UINT32)CF_POSITIVE_X; uiFace <= CF_NEGATIVE_Z; ++uiFace)
		{
			Result eResult = m_apkCubeFaces[uiFace]->GenerateMipSubLevels(uiSrcLevel);
			if(CORE3D_FAILED(eResult)) {return eResult;}
		}
		return OK;
	}

	Result CubeTexture::LockRect(CubeFaces eFace, UINT32 uiMipLevel, void **ppvData, const Rect *pkRect)
	{
		if(eFace < 0 || eFace >= 6)
		{
			CORE3D_ERROR(_T("CubeTexture::LockRect() - Invalid cube face requested.\n"));
			return INVALID_PARAMETERS;
		}
		return m_apkCubeFaces[eFace]->LockRect(uiMipLevel, ppvData, pkRect);
	}

	Result CubeTexture::UnlockRect(CubeFaces eFace, UINT32 uiMipLevel)
	{
		if(eFace < 0 || eFace >= 6)
		{
			CORE3D_ERROR(_T("CubeTexture::UnlockRect() - Invalid cube face specified.\n"));
			return INVALID_PARAMETERS;
		}
		return m_apkCubeFaces[eFace]->UnlockRect(uiMipLevel);
	}

	Result CubeTexture::SampleTexture(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV, FLOAT32 fW, 
		const Vector4* pkXGradient, const Vector4* pkYGradient, const UINT32* puiSamplerStates)
	{
		// COMMENT : Determine face and local U/V coordinates
		// Source : http://developer.nvidia.com/object/cube_map_ogl_tutorial.html

		// Major Axis
		// Direction	Target								SC	TC	MA
		// ----------	----------------------------		--- --- ---
		// +RX			GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT	-RZ -RY +RX
		// -RX			GL_TEXTURE_CUBE_MAP_NEGATIVE_X_EXT	+RZ -RY +RX
		// +RY			GL_TEXTURE_CUBE_MAP_POSITIVE_Y_EXT	+RX +RZ +RY
		// -RY			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_EXT	+RX -RZ +RY
		// +RZ			GL_TEXTURE_CUBE_MAP_POSITIVE_Z_EXT	+RX -RY +RZ
		// -RZ			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_EXT	-RX	-RY +RZ

		FLOAT32 fCU, fCV, fInvMag;
		CubeFaces eFace;
		const FLOAT32 ABS_U = fabsf(fU);
		const FLOAT32 ABS_V = fabsf(fV);
		const FLOAT32 ABS_W = fabsf(fW);

		if(ABS_U >= ABS_V && ABS_U >= ABS_W)
		{
			if(fU >= 0.0f)
			{
				// COMMENT : Major axis direction - +RX
				eFace	= CF_POSITIVE_X;
				fCU		= -fW;
				fCV		= -fV;
				fInvMag = 1.0f / ABS_U;
			}
			else
			{
				// COMMENT : Major axis direction - -RX
				eFace	= CF_NEGATIVE_X;
				fCU		= fW;
				fCV		= -fV;
				fInvMag	= 1.0f / ABS_U;
			}
		}
		else if(ABS_V >= ABS_U && ABS_V >= ABS_W)
		{
			if(fV >= 0.0f)
			{
				// COMMENT : Major axis direction - +RY
				eFace	= CF_POSITIVE_Y;
				fCU		= fU;
				fCV		= fW;
				fInvMag = 1.0f / ABS_V;
			}
			else
			{
				// COMMENT : Major axis direction - -RY
				eFace	= CF_NEGATIVE_Y;
				fCU		= fU;
				fCV		= -fW;
				fInvMag = 1.0f / ABS_V;
			}
		}
		// if(ABS_W >= ABS_U && ABS_W >= ABS_V)
		else
		{
			if(fW >= 0.0f)
			{
				// COMMENT : Major axis direction - +RZ
				eFace	= CF_POSITIVE_Z;
				fCU		= fU;
				fCV		= -fV;
				fInvMag = 1.0f / ABS_W;
			}
			else
			{
				// COMMENT : Major axis direction - -RZ
				eFace	= CF_NEGATIVE_Z;
				fCU		= -fU;
				fCV		= -fV;
				fInvMag = 1.0f / ABS_W;
			}
		}

		fInvMag *= 0.5f;
		const FLOAT32 U = (fCU * fInvMag + 0.5f);
		const FLOAT32 V = (fCV * fInvMag + 0.5f);
		return m_apkCubeFaces[eFace]->SampleTexture(rkColor, U, V, 0.0f, pkXGradient, pkYGradient, puiSamplerStates);
	}

	Format CubeTexture::GetFormat()
	{
		return m_apkCubeFaces[0]->GetFormat();
	}

	UINT32 CubeTexture::GetFormatFloats()
	{
		return m_apkCubeFaces[0]->GetFormatFloats();
	}

	UINT32 CubeTexture::GetMipLevels()
	{
		return m_apkCubeFaces[0]->GetMipLevels();
	}

	UINT32 CubeTexture::GetEdgeLength(UINT32 uiMipLevel /* = 0 */)
	{
		return m_apkCubeFaces[uiMipLevel]->GetWidth(uiMipLevel);
	}

	Texture* CubeTexture::GetCubeFace(CubeFaces eFace)
	{
		if(eFace < 0 || eFace >= 6)
		{
			CORE3D_ERROR(_T("CubeTexture::GetCubeFace() - Invalid cube face requested.\n"));
			return NULL;
		}
		m_apkCubeFaces[eFace]->AddRef();
		return m_apkCubeFaces[eFace];
	}
}