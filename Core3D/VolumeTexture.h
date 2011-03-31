#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "BaseTexture.h"

namespace Core3D
{
	class Device;
	class Volume;
	class VolumeTexture : public BaseTexture
	{
	public:
		Result GenerateMipSubLevels(UINT32 uiSrcLevel);
		Result Clear(UINT32 uiMipLevel, const Vector4& rkColor, const Box* pkBox);
		Result LockBox(UINT32 uiMipLevel, void** ppvData, const Box* pkBox);
		Result UnlockBox(UINT32 uiMipLevel);
		Volume* GetMipLevel(UINT32 uiMipLevel);
		Format GetFormat();
		UINT32 GetFormatFloats();
		UINT32 GetMipLevels();
		UINT32 GetWidth(UINT32 uiMipLevel = 0);
		UINT32 GetHeight(UINT32 uiMipLevel = 0);
		UINT32 GetDepth(UINT32 uiMipLevel = 0);
	protected:
		friend class Device;

		VolumeTexture(Device* pkDevice);
		~VolumeTexture();
		Result Create(UINT32 uiWidth, UINT32 uiHeight, UINT32 uiDepth, UINT32 uiMipLevels, Format eFormat);
		Result SampleTexture(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV, FLOAT32 fW, 
			const Vector4* pkXGradient, const Vector4* pkYGradient, const UINT32* puiSamplerStates);
		TextureSampleInput GetTextureSampleInput();
	private:
		UINT32		m_uiMipLevels;
		FLOAT32		m_fSquaredWidth;
		FLOAT32		m_fSquaredHeight;
		FLOAT32		m_fSquaredDepth;
		Volume**	m_ppkMipLevels;
	};
}