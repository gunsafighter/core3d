#include "Checkboard.h"
#include "FreeCamera.h"

#include "../Core3D/FWApplication.h"
#include "../Core3D/FWScene.h"

namespace Core3D
{
	FWEntity* CreateCheckboard(FWScene* pkScene)
	{
		return new Checkboard(pkScene);
	}
}

class CheckboardVS : public CORE3DVERTEXSHADER
{
public:
	void Execute(const Core3D::ShaderReg* pkIput, C3DVECTOR4& rkPosition, Core3D::ShaderReg* pkOutput)
	{
		// COMMENT : Transform position
		rkPosition	= pkIput[0] * GetMatrix(Core3D::SC_WVPMATRIX);
		// COMMENT : Pass texture coordinate to pixel shader
		pkOutput[0] = pkIput[1];
	}

	Core3D::ShaderRegType GetOutputRegisters(UINT32 uiRegister)
	{
		switch(uiRegister)
		{
		case 0: return Core3D::SRT_VECTOR2;
		}
		return Core3D::SRT_UNUSED;
	}
};

#define ANTIALIAS_BOARD
class CheckboardPS : public CORE3DPIXELSHADER
{
private:
	inline const C3DFLOAT32 MaxF(const C3DFLOAT32 fValA, const C3DFLOAT32 fValB) const
	{
		return fValA > fValB ? fValA : fValB;
	}

	inline const C3DFLOAT32 FloorF(const C3DFLOAT32 fVal) const
	{
		return (C3DFLOAT32)Core3D::FtoL((C3DFLOAT32)fVal);
	}

	inline const C3DFLOAT32 FilterWidth() const
	{
		C3DVECTOR4 kDdx, kDdy;
		GetDerivatives(0, kDdx, kDdy);
		C3DFLOAT32 fChangeX = (*(C3DVECTOR2*)&kDdx).Length();
		C3DFLOAT32 fChangeY = (*(C3DVECTOR2*)&kDdy).Length();
		return MaxF(fChangeX, fChangeY);
	}

	inline const C3DVECTOR2 BumpInt(const C3DVECTOR2& rkIn) const
	{
		C3DVECTOR2 kFloorVec(FloorF(rkIn.x * 0.5f), FloorF(rkIn.y * 0.5f));
		return C3DVECTOR2(kFloorVec.x + MaxF(rkIn.x - 2.0f * kFloorVec.x - 1.0f, 0.0f), 
						  kFloorVec.y + MaxF(rkIn.y - 2.0f * kFloorVec.y - 1.0f, 0.0f));
	}
public:
	bool MightKillPixels()
	{
		return false;
	}
	
	bool Execute(const Core3D::ShaderReg* pkInput, C3DVECTOR4& rkColor, C3DFLOAT32& rfDepth)
	{
	#	ifdef ANTIALIAS_BOARD
		const C3DFLOAT32 WIDTH = FilterWidth();
		C3DVECTOR2 kStep	= C3DVECTOR2(0.5f * WIDTH, 0.5f * WIDTH);
		C3DVECTOR2 kP0		= *(C3DVECTOR2*)&pkInput[0] - kStep;
		C3DVECTOR2 kP1		= *(C3DVECTOR2*)&pkInput[0] + kStep;
		C3DVECTOR2 kInt		= (BumpInt(kP1) - BumpInt(kP0)) / WIDTH;
		C3DFLOAT32 fScale	= kInt.x * kInt.y + (1.0f - kInt.x) * (1.0f - kInt.y);
	#	else
		C3DFLOAT32 fScale	= (fmodf(FloorF(pkInput[0].x) + FloorF(pkInput[0].y), 2.0f) < 1.0f) ? 0.0f : 1.0f;
	#	endif
		rkColor				= C3DVECTOR4(fScale, fScale, fScale, 1.0f);
		return true;
	}
};

Core3D::VertexElement akVertexDeclaration[] = 
{
	CORE3D_VERTEXFORMAT_DECL(0, Core3D::VET_VECTOR3, 0),
	CORE3D_VERTEXFORMAT_DECL(0, Core3D::VET_VECTOR2, 1)
};

Checkboard::Checkboard(Core3D::FWScene* pkScene)
{
	m_pkScene			= pkScene;

	m_pkVertexFormat	= NULL;
	m_pkVertexBuffer	= NULL;
	m_pkVertexShader	= NULL;
	m_pkPixelShader		= NULL;
}

Checkboard::~Checkboard()
{
	CORE3D_SAFE_RELEASE(m_pkPixelShader);
	CORE3D_SAFE_RELEASE(m_pkVertexShader);
	CORE3D_SAFE_RELEASE(m_pkVertexBuffer);
	CORE3D_SAFE_RELEASE(m_pkVertexFormat);
}

bool Checkboard::Initialize()
{
	Core3D::FWGraphics* pkGraphics	= m_pkScene->GetApplication()->GetGraphics();
	LPCORE3DDEVICE pkDevice			= pkGraphics->GetDevice();

	if(CORE3D_FAILED(pkDevice->CreateVertexFormat(&m_pkVertexFormat, akVertexDeclaration, sizeof(akVertexDeclaration))))
	{
		return false;
	}

	if(CORE3D_FAILED(pkDevice->CreateVertexBuffer(&m_pkVertexBuffer, sizeof(Checkboard::VertexData) * 4)))
	{
		return false;
	}

	Checkboard::VertexData* pkDest = NULL;
	if(CORE3D_FAILED(m_pkVertexBuffer->GetPointer(0, (void**)&pkDest)))
	{
		return false;
	}

	C3DUINT32 uiCheckers	= 80;
	pkDest->kPosition		= C3DVECTOR3(-0.5f, 0.0f, 0.0f);
	pkDest->kTexCoord0		= C3DVECTOR2(0.0f, static_cast<C3DFLOAT32>(uiCheckers));
	pkDest++;

	pkDest->kPosition		= C3DVECTOR3(-0.5f, 0.0f, 1.0f);
	pkDest->kTexCoord0		= C3DVECTOR2(0.0f, 0.0f);
	pkDest++;

	pkDest->kPosition		= C3DVECTOR3(0.5f, 0.0f, 0.0f);
	pkDest->kTexCoord0		= C3DVECTOR2(static_cast<C3DFLOAT32>(uiCheckers), static_cast<C3DFLOAT32>(uiCheckers));
	pkDest++;

	pkDest->kPosition		= C3DVECTOR3(0.5f, 0.0f, 1.0f);
	pkDest->kTexCoord0		= C3DVECTOR2(static_cast<C3DFLOAT32>(uiCheckers), 0.0f);
	pkDest++;

	m_pkVertexShader		= new CheckboardVS;
	m_pkPixelShader			= new CheckboardPS;

	return true;
}

bool Checkboard::FrameMove()
{
	return false;
}

void Checkboard::Render(UINT32 uiPass)
{
	switch(uiPass)
	{
	case FreeCamera::PASS_DEFUALT: break;
	}

	Core3D::FWGraphics* pkGraphics		= m_pkScene->GetApplication()->GetGraphics();
	Core3D::FWCamera* pkCurrentCamera	= pkGraphics->GetCurrentCamera();

	C3DMATRIX kMatWorld;
	Core3D::MatrixIdentity(kMatWorld);
	pkCurrentCamera->SetWorldMatrix(kMatWorld);

	m_pkVertexShader->SetMatrix(Core3D::SC_WORLDMATRIX,			pkCurrentCamera->GetWorldMatrix());
	m_pkVertexShader->SetMatrix(Core3D::SC_VIEWMATRIX,			pkCurrentCamera->GetViewMatrix());
	m_pkVertexShader->SetMatrix(Core3D::SC_PROJECTIONMATRIX,	pkCurrentCamera->GetProjectionMatrix());
	m_pkVertexShader->SetMatrix(Core3D::SC_WVPMATRIX, pkCurrentCamera->GetWorldMatrix() * pkCurrentCamera->GetViewMatrix() * pkCurrentCamera->GetProjectionMatrix());

	pkGraphics->SetVertexFormat(m_pkVertexFormat);
	pkGraphics->SetVertexStream(0, m_pkVertexBuffer, 0, sizeof(Checkboard::VertexData));
	pkGraphics->SetVertexShader(m_pkVertexShader);
	pkGraphics->SetPixelShader(m_pkPixelShader);

	pkGraphics->GetDevice()->DrawPrimitive(Core3D::PT_TRIANGLESTRIP, 0, 2);
}