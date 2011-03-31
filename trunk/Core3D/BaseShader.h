#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DTypes.h"

namespace Core3D
{
	class Device;
	class BaseShader : public RefObject
	{
	public:
		void	SetFloat(UINT32 uiIndex, FLOAT32 fValue);
		FLOAT32	GetFloat(UINT32 uiIndex);

		void	SetVector(UINT32 uiIndex, const Vector4& rkVector);
		const Vector4&	GetVector(UINT32 uiIndex);

		void	SetMatrix(UINT32 uiIndex, const Matrix4x4& rkMatrix);
		const Matrix4x4& GetMatrix(UINT32 uiIndex);
	protected:
		friend class Device;

		void	SetDevice(Device* pkDevice);
		Result	SampleTexture(Vector4& rkColor, UINT32 uiSamplerNumber, 
			FLOAT32 fU, FLOAT32 fV, FLOAT32 fW = 0.0f, 
			const Vector4* pkXGradient = NULL, const Vector4* pkYGradient = NULL);
	private:
		FLOAT32		m_afConstants[NUM_SHADER_CONSTANTS];
		Vector4		m_akConstants[NUM_SHADER_CONSTANTS];
		Matrix4x4	m_amatConstants[NUM_SHADER_CONSTANTS];
		Device*		m_pkDevice;
	};
}