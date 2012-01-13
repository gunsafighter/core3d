#include "../Core3D/FWApplication.h"
#include "../Core3D/FWScene.h"
#include "../Core3D/FWLight.h"
#include "EnvSphere.h"
#include "FreeCamera.h"

namespace Core3D
{
	FWEntity* CreateEnvSphere(FWScene* pkScene)
	{
		return new EnvSphere(pkScene);
	}
}

class SphereVS : public CORE3DVERTEXSHADER
{
public:
	void Execute(const Core3D::ShaderReg* pkIput, C3DVECTOR4& rkPosition, Core3D::ShaderReg* pkOutput)
	{
		// COMMENT: Transform position
		rkPosition = pkIput[0] * GetMatrix(Core3D::SC_WVPMATRIX);
		// COMMENT: Transform normal (normal = position when sphere's origin = (0,0,0))
		Core3D::Vec3TransformNormal(*(C3DVECTOR3*)&pkOutput[0], pkIput[0], GetMatrix(Core3D::SC_WORLDMATRIX));
		// COMMENT: Calculate per pixel light direction
		const C3DVECTOR3 kWorldPosition = pkIput[0] * GetMatrix(Core3D::SC_WORLDMATRIX);
		C3DVECTOR3 kLightDir = (C3DVECTOR3)GetVector(1) - kWorldPosition;
		pkOutput[1] = kLightDir;
		// COMMENT: Calculate per pixel light view direction
		C3DVECTOR3 kViewDir = (C3DVECTOR3)GetVector(0) - kWorldPosition;
		pkOutput[2] = kViewDir;
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

class SpherePS : public CORE3DPIXELSHADER
{
public:
	bool Execute(const Core3D::ShaderReg* pkInput, C3DVECTOR4& rkColor, C3DFLOAT32& rfDepth)
	{
		C3DVECTOR3 kNormal = pkInput[0]; kNormal.Normalize();
		C3DVECTOR3 kLightDir = pkInput[1]; kLightDir.Normalize();

		// COMMENT: Compute fresnel term and reflection vector
		const C3DVECTOR3 kViewDir = pkInput[2];
		const C3DFLOAT32 fViewDotNormal = Core3D::Saturate(Core3D::Vec3Dot(kNormal, kViewDir));
		C3DVECTOR3 kReflection = kNormal * (2.0f * fViewDotNormal) - kViewDir;
		kReflection.Normalize();
		const C3DFLOAT32 fFresnel = (1.0f - fViewDotNormal);

		// COMMENT: Compute diffuse and specular light
		C3DFLOAT32 fDiffuse = Core3D::Vec3Dot(kNormal, kLightDir);
		C3DFLOAT32 fSpecular = 0.0f;
		if(fDiffuse >= 0.0f)
		{
			fSpecular = Core3D::Vec3Dot(kLightDir, kReflection);
			if(fSpecular >= 0.0f)	{fSpecular = powf(fSpecular, 10.0f);}
			else					{fSpecular = 0.0f;}
		}
		else
		{
			fDiffuse = 0.0f;
		}

		C3DVECTOR4 kReflectionEnv;
		SampleTexture(kReflectionEnv, 0, kReflection.x, kReflection.y, kReflection.z);
		Core3D::Vec4Lerp(rkColor, rkColor, kReflectionEnv, 1.0f - fFresnel); // Use inverse fresnel
		const C3DVECTOR4& rkLightColor = GetVector(1);
		rkColor += GetVector(0) * rkLightColor * fDiffuse + rkLightColor * fSpecular;

		return true;
	}
};

Core3D::VertexElement akVertexDeclaration[] = 
{
	CORE3D_VERTEXFORMAT_DECL(0, Core3D::VET_VECTOR3, 0)
};

EnvSphere::EnvSphere(Core3D::FWScene* pkScene)
{
	m_pkScene			= pkScene;
	m_pkVertexFormat	= NULL;
	m_pkVertexBuffer	= NULL;
	m_pkIndexBuffer		= NULL;
	m_pkVertexShader	= NULL;
	m_pkPixelShader		= NULL;
	
	SetColor(C3DVECTOR4(0.5f, 0.5f, 0.5f, 1.0f));

	m_uiNumVertices		= 0;
	m_uiNumPrimitives	= 0;
	m_hEnvironment		= NULL;
}

EnvSphere::~EnvSphere()
{
	m_pkScene->GetApplication()->GetResManager()->ReleaseResource(m_hEnvironment);
	CORE3D_SAFE_RELEASE(m_pkPixelShader);
	CORE3D_SAFE_RELEASE(m_pkVertexShader);
	CORE3D_SAFE_RELEASE(m_pkIndexBuffer);
	CORE3D_SAFE_RELEASE(m_pkVertexBuffer);
	CORE3D_SAFE_RELEASE(m_pkVertexFormat);
}

bool EnvSphere::Initialize(C3DFLOAT32 fRadius, C3DUINT32 uiStacks, C3DUINT32 uiSlices, tstring strEnvironment)
{
	Core3D::FWGraphics* pkGraphics = m_pkScene->GetApplication()->GetGraphics();
	Core3D::Device* pkDevice = pkGraphics->GetDevice();

	if(CORE3D_FAILED(pkDevice->CreateVertexFormat(&m_pkVertexFormat, akVertexDeclaration, sizeof(akVertexDeclaration))))
	{
		return false;
	}

	// COMMENT: Construct a sphere
	m_uiNumVertices		= uiStacks * uiSlices * 4;
	m_uiNumPrimitives	= uiStacks * uiSlices * 2;

	if(CORE3D_FAILED(pkDevice->CreateVertexBuffer(&m_pkVertexBuffer, sizeof(EnvSphere::VertexData) * m_uiNumVertices)))
	{
		return false;
	}
	if(CORE3D_FAILED(pkDevice->CreateIndexBuffer(&m_pkIndexBuffer, sizeof(C3DUINT16) * m_uiNumPrimitives * 3, Core3D::FMT_INDEX16)))
	{
		return false;
	}

	EnvSphere::VertexData* pkDestVertices = NULL;
	if(CORE3D_FAILED(m_pkVertexBuffer->GetPointer(0, (void**)&pkDestVertices))) {return false;}
	C3DUINT16* pDestIndices = NULL;
	if(CORE3D_FAILED(m_pkIndexBuffer->GetPointer(0, (void**)&pDestIndices))) {return false;}

	const C3DFLOAT32 fStepV = 1.0f / (C3DFLOAT32)uiStacks;
	const C3DFLOAT32 fStepU = 1.0f / (C3DFLOAT32)uiSlices;
	C3DUINT32 uiCurrentVertex = 0;
	C3DFLOAT32 fV = 0.0f;
	for(C3DUINT32 i = 0; i < uiStacks; ++i, fV += fStepV)
	{
		C3DFLOAT32 fNextV = fV + fStepV;
		C3DFLOAT32 fU = 0.0f;
		for(C3DUINT32 j = 0; j < uiSlices; ++j, fU += fStepU)
		{
			C3DFLOAT32 fNextU = fU + fStepU;
			// Create quad
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

			pkDestVertices->kPosition = C3DVECTOR3(x[0], y[0], z[0]);
			pkDestVertices++;

			pkDestVertices->kPosition = C3DVECTOR3(x[1], y[1], z[1]);
			pkDestVertices++;

			pkDestVertices->kPosition = C3DVECTOR3(x[2], y[2], z[2]);
			pkDestVertices++;

			pkDestVertices->kPosition = C3DVECTOR3(x[3], y[3], z[3]);
			pkDestVertices++;

			*pDestIndices++ = uiCurrentVertex;
			*pDestIndices++ = uiCurrentVertex + 1;
			*pDestIndices++ = uiCurrentVertex + 2;

			*pDestIndices++ = uiCurrentVertex + 1;
			*pDestIndices++ = uiCurrentVertex + 3;
			*pDestIndices++ = uiCurrentVertex + 2;

			uiCurrentVertex += 4;
		}
	}

	m_pkVertexShader	= new SphereVS;
	m_pkPixelShader		= new SpherePS;

	// COMMENT: Load environment texture.
	Core3D::FWResManager* pkResManager = m_pkScene->GetApplication()->GetResManager();
	m_hEnvironment = pkResManager->LoadResource(strEnvironment);
	if(!m_hEnvironment)
	{
		return false;
	}

	// COMMENT: Make sure it is a cube map!
	Core3D::FWTexture* pkTexture = (Core3D::FWTexture*)pkResManager->GetResource(m_hEnvironment);
	if(pkTexture->GetTextureType() != Core3D::FWTexture::TEXTURETYPE_CUBE)
	{
		return false;
	}
	return true;
}

bool EnvSphere::FrameMove()
{
	return false;
}

void EnvSphere::Render(C3DUINT32 uiPass)
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

	m_pkPixelShader->SetVector(0, m_kColor);

	C3DVECTOR3 kCampPos = pkCurrentCamera->GetPosition();
	m_pkVertexShader->SetVector(0, C3DVECTOR4(kCampPos.x, kCampPos.y, kCampPos.z, 0.0f));

	Core3D::FWLight* pkLight = m_pkScene->GetCurrentLight();
	C3DVECTOR3 kLightPos = pkLight->GetPosition();
	m_pkVertexShader->SetVector(1, C3DVECTOR4(kLightPos.x, kLightPos.y, kLightPos.z, 0.0f));

	m_pkPixelShader->SetVector(1, pkLight->GetColor());

	Core3D::FWResManager* pkResManager = m_pkScene->GetApplication()->GetResManager();
	Core3D::FWTexture* pkTexture = (Core3D::FWTexture*)pkResManager->GetResource(m_hEnvironment);
	pkGraphics->SetTexture(0, pkTexture->GetTexture());

	pkGraphics->SetVertexFormat(m_pkVertexFormat);
	pkGraphics->SetVertexStream(0, m_pkVertexBuffer, 0, sizeof(EnvSphere::VertexData));
	pkGraphics->SetIndexBuffer(m_pkIndexBuffer);
	pkGraphics->SetVertexShader(m_pkVertexShader);
	pkGraphics->SetPixelShader(m_pkPixelShader);

	pkGraphics->GetDevice()->DrawIndexedPrimitive(Core3D::PT_TRIANGLELIST, 0, 0, 
		m_uiNumVertices, 0, m_uiNumPrimitives);
}