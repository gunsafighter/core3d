#include "../Core3D/FWApplication.h"
#include "../Core3D/FWScene.h"
#include "../Core3D/FWModel.h"
#include "Crystal.h"
#include "FreeCamera.h"

namespace Core3D
{
	FWEntity* CreateCrystal(FWScene* pkScene)
	{
		return new Crystal(pkScene);
	}
}

class CrystalVS : public CORE3DVERTEXSHADER
{
public:
	void Execute(const Core3D::ShaderReg* pkIput, C3DVECTOR4& rkPosition, Core3D::ShaderReg* pkOutput)
	{
		// COMMENT : Transform position
		rkPosition	= pkIput[0] * GetMatrix(Core3D::SC_WVPMATRIX);
		// COMMENT : Pass texcoord to pixel shader
		pkOutput[0] = pkIput[2];
		// COMMENT : Pass view vector in world space to the pixel shader
		const C3DVECTOR3 kWorldPosition = pkIput[0] * GetMatrix(Core3D::SC_WORLDMATRIX);
		pkOutput[1] = static_cast<C3DVECTOR3>(GetVector(0)) - kWorldPosition;

		// COMMENT : Build transformation matrix to tangent space
		C3DVECTOR3 kNormal;
		C3DVECTOR3 kTangent;
		C3DVECTOR3 kBinormal;
		Core3D::Vec3TransformNormal(kNormal, pkIput[1], GetMatrix(Core3D::SC_WORLDMATRIX));
		Core3D::Vec3TransformNormal(kTangent, pkIput[3], GetMatrix(Core3D::SC_WORLDMATRIX));
		Core3D::Vec3Cross(kBinormal, kNormal, kTangent);

		// COMMENT : Pass transformed normal, tangent and binormal vector to the pixel shader.
		pkOutput[2] = kTangent; pkOutput[3] = kBinormal; pkOutput[4] = kNormal;
	}

	Core3D::ShaderRegType GetOutputRegisters(Core3D::UINT32 uiRegister)
	{
		switch(uiRegister)
		{
		case 0: return Core3D::SRT_VECTOR2; // Base texture coordinates
		case 1: return Core3D::SRT_VECTOR3; // World space view vector
		case 2: return Core3D::SRT_VECTOR3; // Tangent
		case 3: return Core3D::SRT_VECTOR3; // Binormal
		case 4: return Core3D::SRT_VECTOR3; // Normal
		default: return Core3D::SRT_UNUSED;
		}
	}
};

class CrystalPS : public CORE3DPIXELSHADER
{
public:
	bool MightKillPixels() {return false;}
	bool Execute(const Core3D::ShaderReg* pkInput, C3DVECTOR4& rkColor, C3DFLOAT32& rfDepth)
	{
		// COMMENT : Sample texture and normalmap
		C3DVECTOR4 kTexture;
		C3DVECTOR4 kNomralMap;
		SampleTexture(kTexture, 0, pkInput[0].x, pkInput[0].y, 0.0f);
		SampleTexture(kNomralMap, 1, pkInput[0].x, pkInput[0].y, 0.0f);

		// COMMENT : Weaken bump map
		Core3D::Vec4Lerp(kNomralMap, kNomralMap, C3DVECTOR4(0.0f, 0.0f, 1.0f, 0.0f), 0.4f);

		// COMMENT : Transform bump-normal to object space...
		const C3DMATRIX kMatTangentToWorldSpace(pkInput[2], pkInput[3], pkInput[4], C3DVECTOR4(0.0f, 0.0f, 0.0f, 1.0f));
		const C3DVECTOR3 kBumpNormal = C3DVECTOR3(kNomralMap.x * 2.0f - 1.0f, kNomralMap.y * 2.0f - 1.0f, kNomralMap.z * 2.0f - 1.0f);
		C3DVECTOR3 kNormal;
		Core3D::Vec3TransformNormal(kNormal, kBumpNormal, kMatTangentToWorldSpace);
		kNormal.Normalize();

		// COMMENT : Compute fresnel term and reflection vector
		C3DVECTOR3 kViewDir = pkInput[1];
		kViewDir.Normalize();
		const C3DFLOAT32 fViewDotNormal = Core3D::Saturate(Core3D::Vec3Dot(kNormal, kViewDir));
		C3DVECTOR3 kReflection		= kNormal * (2.0f * fViewDotNormal) - kViewDir;
		const C3DFLOAT32 fFresnel	= 1.0f - fViewDotNormal;

		C3DVECTOR4 kEnvironment;
		SampleTexture(kEnvironment, 2, kReflection.x, kReflection.y, kReflection.z);

		const C3DVECTOR4 kCrystalColor = kEnvironment * fFresnel + kTexture * GetVector(0);
		const C3DFLOAT32 fAlpha = fFresnel + 0.5f;
		Core3D::Vec4Lerp(rkColor, rkColor, kCrystalColor, Core3D::Saturate(fAlpha));
		return true;
	}
};

Crystal::Crystal(Core3D::FWScene* pkScene)
{
	m_pkScene			= pkScene;
	m_pkVertexShader	= NULL;
	m_pkPixelShader		= NULL;

	m_hModel			= NULL;
	m_hTexture			= NULL;
	m_hNormalMap		= NULL;
	m_hEnvironment		= NULL;
}

Crystal::~Crystal()
{
	m_pkScene->GetApplication()->GetResManager()->ReleaseResource(m_hEnvironment);
	m_pkScene->GetApplication()->GetResManager()->ReleaseResource(m_hNormalMap);
	m_pkScene->GetApplication()->GetResManager()->ReleaseResource(m_hTexture);
	m_pkScene->GetApplication()->GetResManager()->ReleaseResource(m_hModel);
	CORE3D_SAFE_RELEASE(m_pkPixelShader);
	CORE3D_SAFE_RELEASE(m_pkVertexShader);
}

bool Crystal::Initialize(tstring strModel, tstring strTexture, tstring strNormalMap)
{
	Core3D::FWGraphics* pkGraphics	= m_pkScene->GetApplication()->GetGraphics();
	Core3D::Device* pkDevice		= pkGraphics->GetDevice();

	m_pkVertexShader	= new CrystalVS;
	m_pkPixelShader		= new CrystalPS;

	// COMMENT : Load the model
	Core3D::FWResManager* pkResManager = m_pkScene->GetApplication()->GetResManager();
	m_hModel		= pkResManager->LoadResource(strModel);
	if(!m_hModel)		{return false;}
	// COMMENT : Load texture
	m_hTexture		= pkResManager->LoadResource(strTexture);
	if(!m_hTexture)		{return false;}
	m_hNormalMap	= pkResManager->LoadResource(strNormalMap);
	if(!m_hNormalMap)	{return false;}
	m_hEnvironment	= pkResManager->LoadResource(_T("crystal.cube"));
	if(!m_hEnvironment) {return false;}
	return true;
}

bool Crystal::FrameMove()
{
	return false;
}

void Crystal::Render(UINT32 uiPass)
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
	Core3D::FWTexture* pkTexture		= (Core3D::FWTexture*)pkResManager->GetResource(m_hTexture);
	pkGraphics->SetTexture(0, pkTexture->GetTexture());
	Core3D::FWTexture* pkNormalMap		= (Core3D::FWTexture*)pkResManager->GetResource(m_hNormalMap);
	pkGraphics->SetTexture(1, pkNormalMap->GetTexture());
	Core3D::FWTexture* pkEnvironment	= (Core3D::FWTexture*)pkResManager->GetResource(m_hEnvironment);
	pkGraphics->SetTexture(2, pkEnvironment->GetTexture());

	m_pkVertexShader->SetVector(0, pkCurrentCamera->GetPosition());		// Camera position
	m_pkPixelShader->SetVector(0, C3DVECTOR4(0.5f, 0.0f, 0.5f, 1.0f));	// Base color

	Core3D::FWObjModel* pkModel = (Core3D::FWObjModel*)pkResManager->GetResource(m_hModel);
	pkGraphics->SetVertexFormat(pkModel->GetVertexFormat());
	pkGraphics->SetVertexStream(0, pkModel->GetVertexBuffer(), 0, pkModel->GetStride());

	pkGraphics->SetVertexShader(m_pkVertexShader);
	pkGraphics->SetPixelShader(m_pkPixelShader);

	pkGraphics->SetRenderState(Core3D::RS_CULLMODE, Core3D::CULL_CW);
	pkGraphics->GetDevice()->DrawPrimitive(Core3D::PT_TRIANGLELIST, 0, pkModel->GetNumFaces());
	
	pkGraphics->SetRenderState(Core3D::RS_CULLMODE, Core3D::CULL_CCW);
	pkGraphics->GetDevice()->DrawPrimitive(Core3D::PT_TRIANGLELIST, 0, pkModel->GetNumFaces());
}