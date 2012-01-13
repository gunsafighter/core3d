#include "../Core3D/FWApplication.h"
#include "../Core3D/FWScene.h"
#include "FreeCamera.h"
#include "DisplacedSphere.h"

namespace Core3D
{
	FWEntity* CreateDisplacedSphere(FWScene* pkScene)
	{
		return new DisplacedSphere(pkScene);
	}
}

class SphereVS : public CORE3DVERTEXSHADER
{
public:
	void Execute(const Core3D::ShaderReg* pkIput, C3DVECTOR4& rkPosition, Core3D::ShaderReg* pkOutput)
	{
		C3DVECTOR4 kTexture; // Look up height from texture
		SampleTexture(kTexture, 0, pkIput[1].x, pkIput[1].y);
		const C3DFLOAT32 fHeight = 0.1f * kTexture.a; // Height stored in alpha channel

		// COMMENT : Transform position (offset along normal)
		C3DVECTOR3 kNormal = pkIput[0];
		kNormal.Normalize(); // Renormalize normal - length changed due to interpolation of vertices during subdivision
		rkPosition = (pkIput[0] + kNormal * fHeight) * GetMatrix(Core3D::SC_WVPMATRIX);

		// COMMENT : Pass texcoord to pixel shader
		pkOutput[0] = pkIput[1];
	}

	Core3D::ShaderRegType GetOutputRegisters(C3DUINT32 uiRegister)
	{
		switch(uiRegister)
		{
		case 0: return Core3D::SRT_VECTOR2;
		default: return Core3D::SRT_UNUSED;
		}
	}
};

class SpherePS : public CORE3DPIXELSHADER
{
public:
	bool MightKillPixels() {return false;}
	bool Execute(const Core3D::ShaderReg* pkInput, C3DVECTOR4& rkColor, C3DFLOAT32& rfDepth)
	{
		SampleTexture(rkColor, 0, pkInput[0].x, pkInput[0].y, 0.0f);
		return true;
	}
};

class SpherePrimitiveAssembler : public CORE3DPRIMITIVEASSEMBLER
{
public:
	Core3D::PrimitiveType Execute(std::vector<C3DUINT32>& rkVertexIndices, C3DUINT32 uiNumVertices)
	{
		C3DUINT32 uiCurVertex = 0;
		while(uiCurVertex < uiNumVertices)
		{
			rkVertexIndices.push_back(uiCurVertex);
			rkVertexIndices.push_back(uiCurVertex + 1);
			rkVertexIndices.push_back(uiCurVertex + 2);

			rkVertexIndices.push_back(uiCurVertex + 1);
			rkVertexIndices.push_back(uiCurVertex + 3);
			rkVertexIndices.push_back(uiCurVertex + 2);

			uiCurVertex += 4;
		}
		return Core3D::PT_TRIANGLELIST;
	}
};

Core3D::VertexElement akVertexDeclaration[] = 
{
	CORE3D_VERTEXFORMAT_DECL(0, Core3D::VET_VECTOR3, 0), 
	CORE3D_VERTEXFORMAT_DECL(0, Core3D::VET_VECTOR2, 1)
};

DisplacedSphere::DisplacedSphere(Core3D::FWScene* pkScene)
{
	m_pkScene			= pkScene;
	m_pkVertexFormat	= NULL;
	m_pkVertexBuffer	= NULL;
	m_pkPrimitiveAssembler = NULL;
	m_pkVertexShader	= NULL;
	m_pkPixelShader		= NULL;

	m_uiNumVertices		= 0;
	m_uiNumPrimitives	= 0;
	m_hTexture			= NULL;
}

DisplacedSphere::~DisplacedSphere()
{
	m_pkScene->GetApplication()->GetResManager()->ReleaseResource(m_hTexture);
	CORE3D_SAFE_RELEASE(m_pkPixelShader);
	CORE3D_SAFE_RELEASE(m_pkVertexShader);
	CORE3D_SAFE_RELEASE(m_pkPrimitiveAssembler);
	CORE3D_SAFE_RELEASE(m_pkVertexBuffer);
	CORE3D_SAFE_RELEASE(m_pkVertexFormat);
}

bool DisplacedSphere::Initialize(C3DFLOAT32 fRadius, C3DUINT32 uiStacks, C3DUINT32 uiSlices, tstring strTexture)
{
	Core3D::FWGraphics* pkGraphics = m_pkScene->GetApplication()->GetGraphics();
	Core3D::Device* pkDevice = pkGraphics->GetDevice();

	if(CORE3D_FAILED(pkDevice->CreateVertexFormat(&m_pkVertexFormat, akVertexDeclaration, sizeof(akVertexDeclaration))))
	{
		return false;
	}

	// COMMENT : Construct a sphere
	m_uiNumVertices		= uiStacks * uiSlices * 4;
	m_uiNumPrimitives	= uiStacks * uiSlices * 2;
	if(CORE3D_FAILED(pkDevice->CreateVertexBuffer(&m_pkVertexBuffer, sizeof(DisplacedSphere::VertexData) * m_uiNumVertices)))
	{
		return false;
	}
	VertexData* pkDestVertices = NULL;
	if(CORE3D_FAILED(m_pkVertexBuffer->GetPointer(0, (void**)&pkDestVertices)))
	{
		return false;
	}

	const C3DFLOAT32 fStepV = 1.0f / (C3DFLOAT32)uiStacks;
	const C3DFLOAT32 fStepU = 1.0f / (C3DFLOAT32)uiSlices;

	C3DFLOAT32 fV = 0.0f;
	for(C3DUINT32 i = 0; i < uiStacks; ++i, fV += fStepV)
	{
		C3DFLOAT32 fNextV = fV + fStepV;
		C3DFLOAT32 fU = 0.0f;
		for(C3DUINT32 j = 0; j < uiSlices; ++j, fU += fStepU)
		{
			C3DFLOAT32 fNextU = fU + fStepU;
			// COMMENT : Create a quad
			// 0 -- 1
			// |    |
			// 2 -- 3
			C3DFLOAT32 x[4], y[4], z[4];

			x[0] = fRadius * sinf(fV * CORE3D_PI) * cosf(fU * 2.0f * CORE3D_PI);
			z[0] = fRadius * sinf(fV * CORE3D_PI) * sinf(fU * 2.0f * CORE3D_PI);
			y[0] = fRadius * cosf(fV * CORE3D_PI);

			x[1] = fRadius * sinf(fV * CORE3D_PI) * cosf(fNextU * 2.0f * CORE3D_PI);
			z[1] = fRadius * sinf(fV * CORE3D_PI) * sinf(fNextU * 2.0f * CORE3D_PI);
			y[1] = fRadius * cosf(fV * CORE3D_PI);

			x[2] = fRadius * sinf(fNextV * CORE3D_PI) * cosf(fU * 2.0f * CORE3D_PI);
			z[2] = fRadius * sinf(fNextV * CORE3D_PI) * sinf(fU * 2.0f * CORE3D_PI);
			y[2] = fRadius * cosf(fNextV * CORE3D_PI);

			x[3] = fRadius * sinf(fNextV * CORE3D_PI) * cosf(fNextU * 2.0f * CORE3D_PI);
			z[3] = fRadius * sinf(fNextV * CORE3D_PI) * sinf(fNextU * 2.0f * CORE3D_PI);
			y[3] = fRadius * cosf(fNextV * CORE3D_PI);
			
			pkDestVertices->kPosition	= C3DVECTOR3(x[0], y[0], z[0]);
			pkDestVertices->kTex		= C3DVECTOR2(fU, fV);
			pkDestVertices++;

			pkDestVertices->kPosition	= C3DVECTOR3(x[1], y[1], z[1]);
			pkDestVertices->kTex		= C3DVECTOR2(fNextU, fV);
			pkDestVertices++;

			pkDestVertices->kPosition	= C3DVECTOR3(x[2], y[2], z[2]);
			pkDestVertices->kTex		= C3DVECTOR2(fU, fNextV);
			pkDestVertices++;

			pkDestVertices->kPosition	= C3DVECTOR3(x[3], y[3], z[3]);
			pkDestVertices->kTex		= C3DVECTOR2(fNextU, fNextV);
			pkDestVertices++;
		}
	}

	m_pkPrimitiveAssembler	= new SpherePrimitiveAssembler;
	m_pkVertexShader		= new SphereVS;
	m_pkPixelShader			= new SpherePS;

	// COMMENT : Load environment texture
	Core3D::FWResManager* pkResManager = m_pkScene->GetApplication()->GetResManager();
	m_hTexture = pkResManager->LoadResource(strTexture);
	if(!m_hTexture) {return false;}
	return true;
}

bool DisplacedSphere::FrameMove()
{
	return false;
}

void DisplacedSphere::Render(C3DUINT32 uiPass)
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

	m_pkVertexShader->SetMatrix(Core3D::SC_WORLDMATRIX, pkCurrentCamera->GetWorldMatrix());
	m_pkVertexShader->SetMatrix(Core3D::SC_VIEWMATRIX, pkCurrentCamera->GetViewMatrix());
	m_pkVertexShader->SetMatrix(Core3D::SC_PROJECTIONMATRIX, pkCurrentCamera->GetProjectionMatrix());
	m_pkVertexShader->SetMatrix(Core3D::SC_WVPMATRIX, pkCurrentCamera->GetWorldMatrix() * pkCurrentCamera->GetViewMatrix() * pkCurrentCamera->GetProjectionMatrix());

	Core3D::FWResManager* pkResManager = m_pkScene->GetApplication()->GetResManager();
	Core3D::FWTexture* pkTexture = (Core3D::FWTexture*)pkResManager->GetResource(m_hTexture);
	pkGraphics->SetTexture(0, pkTexture->GetTexture());

	pkGraphics->SetVertexFormat(m_pkVertexFormat);
	pkGraphics->SetVertexStream(0, m_pkVertexBuffer, 0, sizeof(DisplacedSphere::VertexData));
	pkGraphics->SetPrimitiveAssembler(m_pkPrimitiveAssembler);
	pkGraphics->SetVertexShader(m_pkVertexShader);
	pkGraphics->SetPixelShader(m_pkPixelShader);

	pkGraphics->GetDevice()->DrawDynamicPrimitive(0, m_uiNumVertices);
}