#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "BaseShader.h"

namespace Core3D
{
	class VertexShader : public BaseShader
	{
	protected:
		friend class Device;
		virtual void Execute(const ShaderReg* pkIput, Vector4& rkPosition, ShaderReg* pkOutput) = 0;
		virtual ShaderRegType GetOutputRegisters(UINT32 uiRegister) = 0;
	};

	class TriangleShader : public BaseShader
	{
	protected:
		friend class Device;
		virtual bool Execute(ShaderReg* pkShaderRegs0, ShaderReg* pkShaderRegs1, ShaderReg* pkShaderRegs2) = 0;
	};

	class PixelShader : public BaseShader
	{
	protected:
		friend class Device;
		virtual PixelShaderOutput GetShaderOutput();
		virtual bool MightKillPixels();
		virtual bool Execute(const ShaderReg* pkInput, Vector4& rkColor, FLOAT32& rfDepth) = 0;

		void SetInfo(const ShaderRegType* peVSOutputs, const TriangleInfo* pkTriangleInfo);
		void GetDerivatives(UINT32 uiRegister, Vector4& rkDdx, Vector4& rkDdy) const;
	private:
		const ShaderRegType*	m_peVSOutputs;
		const TriangleInfo*		m_pkTriangleInfo;
	};
}