#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "BaseTexture.h"

namespace Core3D
{
	class Device;
	class Texture;
	class CubeTexture : public BaseTexture
	{
	public:
		Result GenerateMipSubLevels(UINT32 uiSrcLevel);
		Result LockRect(CubeFaces eFace, UINT32 uiMipLevel, void** ppvData, const Rect* pkRect);
		Result UnlockRect(CubeFaces eFace, UINT32 uiMipLevel);
		Format GetFormat();
		UINT32 GetFormatFloats();
		UINT32 GetMipLevels();
		UINT32 GetEdgeLength(UINT32 uiMipLevel = 0);
		Texture* GetCubeFace(CubeFaces eFace);
	protected:
		friend class Device;

		CubeTexture(Device* pkDevice);
		~CubeTexture();

		Result Create(UINT32 uiEdgeLength, UINT32 uiMipLevels, Format eFormat);
		TextureSampleInput GetTextureSampleInput();
		Result SampleTexture(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV, FLOAT32 fW, 
			const Vector4* pkXGradient, const Vector4* pkYGradient, const UINT32* puiSamplerStates);
	private:
		Texture* m_apkCubeFaces[6];
	};
}