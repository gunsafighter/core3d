#include "BaseShader.h"
#include "Device.h"

namespace Core3D
{
	void BaseShader::SetFloat(UINT32 uiIndex, FLOAT32 fValue)
	{
		m_afConstants[uiIndex] = fValue;
	}

	FLOAT32 BaseShader::GetFloat(UINT32 uiIndex)
	{
		return m_afConstants[uiIndex];
	}

	void BaseShader::SetVector(UINT32 uiIndex, const Vector4& rkVector)
	{
		m_akConstants[uiIndex] = rkVector;
	}

	const Vector4& BaseShader::GetVector(UINT32 uiIndex)
	{
		return m_akConstants[uiIndex];
	}

	void BaseShader::SetMatrix(UINT32 uiIndex, const Matrix4x4& rkMatrix)
	{
		m_amatConstants[uiIndex] = rkMatrix;
	}

	const Matrix4x4& BaseShader::GetMatrix(UINT32 uiIndex)
	{
		return m_amatConstants[uiIndex];
	}

	void BaseShader::SetDevice(Device* pkDevice)
	{
		m_pkDevice = pkDevice;
	}

	Result BaseShader::SampleTexture(Vector4& rkColor, UINT32 uiSamplerNumber, FLOAT32 fU, FLOAT32 fV, FLOAT32 fW /* = 0.0f */, 
		const Vector4* pkXGradient /* = NULL */, const Vector4* pkYGradient /* = NULL */)
	{
		return m_pkDevice->SampleTexture(rkColor, uiSamplerNumber, fU, fV, fW, 
			pkXGradient, pkYGradient);
	}
}