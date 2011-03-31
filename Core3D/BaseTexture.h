#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DTypes.h"

namespace Core3D
{
	class Device;
	class BaseTexture : public RefObject
	{
	public:
		Device* GetDevice();
	protected:
		friend class Device;

		BaseTexture(Device* pkDevice);
		virtual ~BaseTexture();

		virtual TextureSampleInput GetTextureSampleInput() = 0;
		virtual Result SampleTexture(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV, FLOAT32 fW, 
			const Vector4* pkXGradient, const Vector4* pkYGradient, const UINT32* puiSamplerStates) = 0;
	protected:
		Device* m_pkDevice;
	};
}