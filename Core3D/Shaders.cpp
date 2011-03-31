#include "Shaders.h"

namespace Core3D
{
	PixelShaderOutput PixelShader::GetShaderOutput()
	{
		return PSO_COLORONLY;
	}

	bool PixelShader::MightKillPixels()
	{
		return true;
	}

	void PixelShader::SetInfo(const ShaderRegType* peVSOutputs, const TriangleInfo* pkTriangleInfo)
	{
		m_peVSOutputs		= peVSOutputs;
		m_pkTriangleInfo	= pkTriangleInfo;
	}

	// COMMENT : Partial derivative equations taken from
	// "MIP-Map Level Selection for Texture Mapping"
	// Jon P. Ewins, Member, IEEE, Marcus D. Waller,
	// Martin White, and Paul F. Lister, Member, IEEE
	void PixelShader::GetDerivatives(UINT32 uiRegister, Vector4& rkDdx, Vector4& rkDdy) const
	{
		rkDdx = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		rkDdy = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		if((uiRegister < 0) || (uiRegister >= PIXEL_SHADER_REGISTERS)) {return;}

		const ShaderReg& A = m_pkTriangleInfo->kShaderOutputsDdx[uiRegister];
		const ShaderReg& B = m_pkTriangleInfo->kShaderOutputsDdy[uiRegister];
		const ShaderReg& C = m_pkTriangleInfo->pkBaseVertex->kShaderOutputs[uiRegister];

		const FLOAT32	 D = m_pkTriangleInfo->fWDdx;
		const FLOAT32	 E = m_pkTriangleInfo->fWDdy;
		const FLOAT32	 F = m_pkTriangleInfo->pkBaseVertex->kPosition.w;

		const FLOAT32 REL_PIXEL_X	= m_pkTriangleInfo->uiCurrentPixelX - m_pkTriangleInfo->pkBaseVertex->kPosition.x;
		const FLOAT32 REL_PIXEL_Y	= m_pkTriangleInfo->uiCurrentPixelY - m_pkTriangleInfo->pkBaseVertex->kPosition.y;
		const FLOAT32 INV_W_SQUARE	= m_pkTriangleInfo->fCurrentPixelInvW * m_pkTriangleInfo->fCurrentPixelInvW;

		// COMMENT : 
		// Compute partial derivative with respect to the x-screen space coordinate
		// Compute partial derivative with respect to the y-screen space coordinate
		switch(m_peVSOutputs[uiRegister])
		{
		case SRT_VECTOR4:
			rkDdx.w = ((A.w * F - C.w * D) + (A.w * E - B.w * D) * REL_PIXEL_Y) * INV_W_SQUARE;
			rkDdy.w = ((B.w * F - C.w * E) + (B.w * D - A.w * E) * REL_PIXEL_X) * INV_W_SQUARE;
		case SRT_VECTOR3:
			rkDdx.z = ((A.z * F - C.z * D) + (A.z * E - B.z * D) * REL_PIXEL_Y) * INV_W_SQUARE;
			rkDdy.z = ((B.z * F - C.z * E) + (B.z * D - A.z * E) * REL_PIXEL_X) * INV_W_SQUARE;
		case SRT_VECTOR2:
			rkDdx.y = ((A.y * F - C.y * D) + (A.y * E - B.y * D) * REL_PIXEL_Y) * INV_W_SQUARE;
			rkDdy.y = ((B.y * F - C.y * E) + (B.y * D - A.y * E) * REL_PIXEL_X) * INV_W_SQUARE;
		case SRT_FLOAT32:
			rkDdx.x = ((A.x * F - C.x * D) + (A.x * E - B.x * D) * REL_PIXEL_Y) * INV_W_SQUARE;
			rkDdy.x = ((B.x * F - C.x * E) + (B.x * D - A.x * E) * REL_PIXEL_X) * INV_W_SQUARE;
		case SRT_UNUSED:
		default:
			break;
		}
	}
}