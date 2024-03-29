#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "BaseTexture.h"

namespace Core3D
{
	class Device;
	class Surface;
	class Texture : public BaseTexture
	{
	public:
		Result GenerateMipSubLevels(UINT32 uiSrcLevel);
		Result Clear(UINT32 uiMipLevel, const Vector4& rkColor, const Rect* pkRect);
		Result LockRect(UINT32 uiMipLevel, void** ppvData, const Rect* pkRect);
		Result UnlockRect(UINT32 uiMipLevel);
		Surface* GetMipLevel(UINT32 uiMipLevel);
		Format GetFormat();
		UINT32 GetFormatFloats();
		UINT32 GetMipLevels();
		UINT32 GetWidth(UINT32 uiMipLevel = 0);
		UINT32 GetHeight(UINT32 uiMipLevel = 0);
	protected:
		friend class Device;
		friend class CubeTexture;

		Texture(Device* pkDevice);
		~Texture();
		Result Create(UINT32 uiWidth, UINT32 uiHeight, UINT32 uiMipLevels, Format eFormat);
		Result SampleTexture(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV, FLOAT32 fW, 
			const Vector4* pkXGradient, const Vector4* pkYGradient, const UINT32* puiSamplerStates);
		TextureSampleInput GetTextureSampleInput();
	private:
		UINT32		m_uiMipLevels;
		FLOAT32		m_fSquaredWidth;
		FLOAT32		m_fSquaredHeight;
		Surface**	m_ppkMipLevels;
	};
}