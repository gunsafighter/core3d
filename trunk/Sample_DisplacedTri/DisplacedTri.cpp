#include "../Core3D/FWApplication.h"
#include "../Core3D/FWScene.h"
#include "../Core3D/FWLight.h"
#include "FreeCamera.h"
#include "DisplacedTri.h"

namespace Core3D
{
	FWEntity* CreateDisplacedTri(FWScene* pkScene)
	{
		return new DisplacedTri(pkScene);
	}
}

class TriangleVS : public CORE3DVERTEXSHADER
{
public:
	void Execute(const Core3D::ShaderReg* pkIput, C3DVECTOR4& rkPosition, Core3D::ShaderReg* pkOutput)
	{
		// COMMENT : Offset position
		C3DVECTOR4 kTexNormal;
		SampleTexture(kTexNormal, 1, pkIput[3].x, pkIput[3].y);
		const C3DFLOAT32 fHeight = 0.4f * kTexNormal.a;
		C3DVECTOR3 kNormal = pkIput[1];
		kNormal.Normalize(); // Renormalize normal - length changed due to interpolation of vertices during subdivision

		// COMMENT : Transform position
		rkPosition = (pkIput[0] + kNormal * fHeight) * GetMatrix(Core3D::SC_WVPMATRIX);

		// COMMENT : Pass texcoord to pixelshader
		pkOutput[0] = pkIput[3];

		// COMMENT : Build transformation matrix to tangent space
		C3DVECTOR3 kTangent = pkIput[2];
		kTangent.Normalize();
		Core3D::Vec3TransformNormal(kNormal, kNormal, GetMatrix(Core3D::SC_WORLDMATRIX));
		Core3D::Vec3TransformNormal(kTangent, kTangent, GetMatrix(Core3D::SC_WORLDMATRIX));
		C3DVECTOR3 kBinormal;
		Core3D::Vec3Cross(kBinormal, kNormal, kTangent);

		const C3DMATRIX kMatWorldToTangentSpace(
			kTangent.x, kBinormal.x, kNormal.x, 0.0f, 
			kTangent.y, kBinormal.y, kNormal.y, 0.0f, 
			kTangent.z, kBinormal.z, kNormal.z, 0.0f, 
			0.0f,	    0.0f,        0.0f,      1.0f);

		// COMMENT : Transform light direction to tangent space
		const C3DVECTOR3 kWorldPosition = pkIput[0] * GetMatrix(Core3D::SC_WORLDMATRIX);
		C3DVECTOR3 kLightDir = (C3DVECTOR3)GetVector(1) - kWorldPosition;
		C3DVECTOR3 kLightDirTangentSpace;
		Core3D::Vec3TransformNormal(kLightDirTangentSpace, kLightDir, kMatWorldToTangentSpace);
		pkOutput[1] = kLightDirTangentSpace;

		// COMMENT : Compute half vector and transform to tangent space
		C3DVECTOR3 kViewDir = (C3DVECTOR3)GetVector(0) - kWorldPosition;
		const C3DVECTOR3 kHalf = (kViewDir.Normalize() + kLightDir.Normalize()) * 0.5f;
		C3DVECTOR3 kHalfTangentSpace;
		Core3D::Vec3TransformNormal(kHalfTangentSpace, kHalf, kMatWorldToTangentSpace);
		pkOutput[2] = kHalfTangentSpace;
	}

	Core3D::ShaderRegType GetOutputRegisters(C3DUINT32 uiRegister)
	{
		switch(uiRegister)
		{
		case 0: return Core3D::SRT_VECTOR3;
		case 1: return Core3D::SRT_VECTOR3;
		case 2: return Core3D::SRT_VECTOR3;
		default: return Core3D::SRT_UNUSED;
		}
	}
};

class TrianglePS : public CORE3DPIXELSHADER
{
public:
	bool MightKillPixels() {return false;}
	bool Execute(const Core3D::ShaderReg* pkInput, C3DVECTOR4& rkColor, C3DFLOAT32& rfDepth)
	{
		// COMMENT : Read normal from normalmap
		C3DVECTOR4 kTexNormal;
		SampleTexture(kTexNormal, 1, pkInput[0].x, pkInput[0].y, 0.0f);
		const C3DVECTOR3 kNormal(kTexNormal.x * 2.0f - 1.0f, kTexNormal.y * 2.0f - 1.0f, kTexNormal.y * 2.0f - 1.0f);
		
		// COMMENT : Sample texture
		C3DVECTOR4 kTex;
		SampleTexture(kTex, 0, pkInput[0].x, pkInput[0].y, 0.0f);

		// COMMENT : Renormalize interpolation light direction vector
		C3DVECTOR3 kLightDir = pkInput[1];
		kLightDir.Normalize();

		// COMMENT : Compute diffuse light
		C3DFLOAT32 fDiffuse = Core3D::Vec3Dot(kNormal, kLightDir);
		C3DFLOAT32 fSpecular = 0.0f;
		if(fDiffuse >= 0.0f)
		{
			// COMMENT : Compute specular light
			C3DVECTOR3 kHalf = pkInput[2];
			kHalf.Normalize();
			fSpecular = Core3D::Vec3Dot(kNormal, kHalf);
			if(fSpecular < 0.0f)	{fSpecular = 0.0f;}
			else					{fSpecular = powf(fSpecular, 256.0f);}
		}
		else
		{
			fDiffuse = 0.0f;
		}

		const C3DVECTOR4& rkLightColor = GetVector(0);
		rkColor = kTex * rkLightColor * fDiffuse + rkLightColor * fSpecular;

		return true;
	}
};

Core3D::VertexElement akVertexDeclaration[] = 
{
	CORE3D_VERTEXFORMAT_DECL(0, Core3D::VET_VECTOR3, 0),
	CORE3D_VERTEXFORMAT_DECL(0, Core3D::VET_VECTOR3, 1),
	CORE3D_VERTEXFORMAT_DECL(0, Core3D::VET_VECTOR3, 2),
	CORE3D_VERTEXFORMAT_DECL(0, Core3D::VET_VECTOR2, 3)
};

DisplacedTri::DisplacedTri(Core3D::FWScene* pkScene)
{
	m_pkScene			= pkScene;
	m_pkVertexFormat	= NULL;
	m_pkVertexBuffer	= NULL;
	m_pkVertexShader	= NULL;
	m_pkPixelShader		= NULL;
	
	m_hTexture			= NULL;
	m_hNormalMap		= NULL;
}

DisplacedTri::~DisplacedTri()
{
	m_pkScene->GetApplication()->GetResManager()->ReleaseResource(m_hNormalMap);
	m_pkScene->GetApplication()->GetResManager()->ReleaseResource(m_hTexture);

	CORE3D_SAFE_RELEASE(m_pkPixelShader);
	CORE3D_SAFE_RELEASE(m_pkVertexShader);
	CORE3D_SAFE_RELEASE(m_pkVertexBuffer);
	CORE3D_SAFE_RELEASE(m_pkVertexFormat);
}

bool DisplacedTri::Initialize(const DisplacedTri::VertexData* pkVertices, tstring strTexture, tstring strNormalMap)
{
	Core3D::FWGraphics* pkGraphics = m_pkScene->GetApplication()->GetGraphics();
	Core3D::Device* pkDevice = pkGraphics->GetDevice();

	if(CORE3D_FAILED(pkDevice->CreateVertexFormat(&m_pkVertexFormat, akVertexDeclaration, sizeof(akVertexDeclaration))))
	{
		return false;
	}
	if(CORE3D_FAILED(pkDevice->CreateVertexBuffer(&m_pkVertexBuffer, sizeof(DisplacedTri::VertexData) * 3)))
	{
		return false;
	}

	DisplacedTri::VertexData* pkDest = NULL;
	if(CORE3D_FAILED(m_pkVertexBuffer->GetPointer(0, (void**)&pkDest)))
	{
		return false;
	}

	memcpy(pkDest, pkVertices, sizeof(DisplacedTri::VertexData) * 3);

	// COMMENT : Calculate triangle normal.
	C3DVECTOR3 kV01 = pkDest[1].kPosition - pkDest[0].kPosition;
	C3DVECTOR3 kV02 = pkDest[2].kPosition - pkDest[0].kPosition;
	C3DVECTOR3 kNormal;
	Core3D::Vec3Cross(kNormal, kV01, kV02);
	pkDest[0].kNormal = pkDest[1].kNormal = pkDest[2].kNormal = kNormal.Normalize();

	// COMMENT : Calculate triangle tangent.
	C3DFLOAT32 fDeltaV[2] = 
	{
		pkDest[1].kTexCoord0.y - pkDest[0].kTexCoord0.y, 
		pkDest[2].kTexCoord0.y - pkDest[0].kTexCoord0.y
	};
	C3DVECTOR3 kTangent = (kV01 * fDeltaV[1]) - (kV02 * fDeltaV[0]);
	pkDest[0].kTangent = pkDest[1].kTangent = pkDest[2].kTangent = kTangent.Normalize();

	m_pkVertexShader	= new TriangleVS;
	m_pkPixelShader		= new TrianglePS;

	// COMMENT : Load texture
	Core3D::FWResManager* pkResManager = m_pkScene->GetApplication()->GetResManager();
	m_hTexture = pkResManager->LoadResource(strTexture);
	if(!m_hTexture) {return false;}

	// COMMENT : Load normalmap
	m_hNormalMap = pkResManager->LoadResource(strNormalMap);
	if(!m_hNormalMap) {return false;}

	return true;
}

bool DisplacedTri::FrameMove()
{
	return false;
}

void DisplacedTri::Render(C3DUINT32 uiPass)
{
	switch(uiPass)
	{
	case FreeCamera::PASS_LIGHTING: break;
	}

	Core3D::FWGraphics* pkGraphics = m_pkScene->GetApplication()->GetGraphics();
	Core3D::FWCamera* pkCurrentCamera = pkGraphics->GetCurrentCamera();
	C3DMATRIX kMatWorld;
	Core3D::MatrixIdentity(kMatWorld);
	pkCurrentCamera->SetWorldMatrix(kMatWorld);

	m_pkVertexShader->SetMatrix(Core3D::SC_WORLDMATRIX, pkCurrentCamera->GetWorldMatrix());
	m_pkVertexShader->SetMatrix(Core3D::SC_VIEWMATRIX, pkCurrentCamera->GetViewMatrix());
	m_pkVertexShader->SetMatrix(Core3D::SC_PROJECTIONMATRIX, pkCurrentCamera->GetProjectionMatrix());
	m_pkVertexShader->SetMatrix(Core3D::SC_WVPMATRIX, pkCurrentCamera->GetWorldMatrix() * pkCurrentCamera->GetViewMatrix() * pkCurrentCamera->GetProjectionMatrix());

	C3DVECTOR3 kCamPos = pkCurrentCamera->GetPosition();
	m_pkVertexShader->SetVector(0, C3DVECTOR4(kCamPos.x, kCamPos.y, kCamPos.z, 0.0f));

	Core3D::FWLight* pkLight = m_pkScene->GetCurrentLight();

	C3DVECTOR3 kLightPos = pkLight->GetPosition();
	m_pkVertexShader->SetVector(1, C3DVECTOR4(kLightPos.x, kLightPos.y, kLightPos.z, 0.0f));

	m_pkPixelShader->SetVector(0, pkLight->GetColor());

	pkGraphics->SetVertexFormat(m_pkVertexFormat);
	pkGraphics->SetVertexStream(0, m_pkVertexBuffer, 0, sizeof(DisplacedTri::VertexData));
	pkGraphics->SetVertexShader(m_pkVertexShader);
	pkGraphics->SetPixelShader(m_pkPixelShader);

	Core3D::FWResManager* pkResManager = m_pkScene->GetApplication()->GetResManager();
	Core3D::FWTexture* pkTexture = (Core3D::FWTexture*)pkResManager->GetResource(m_hTexture);
	pkGraphics->SetTexture(0, pkTexture->GetTexture());

	Core3D::FWTexture* pkNormalMap = (Core3D::FWTexture*)pkResManager->GetResource(m_hNormalMap);
	pkGraphics->SetTexture(1, pkNormalMap->GetTexture());

	for(C3DUINT32 i = 0; i < 2; ++i)
	{
		pkGraphics->SetTextureSamplerState(i, Core3D::TSS_ADDRESSU, Core3D::TA_CLAMP);
		pkGraphics->SetTextureSamplerState(i, Core3D::TSS_ADDRESSV, Core3D::TA_CLAMP);
	}
	pkGraphics->GetDevice()->DrawPrimitive(Core3D::PT_TRIANGLELIST, 0, 1);
}