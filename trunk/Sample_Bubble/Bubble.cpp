#include "Bubble.h"
#include "FreeCamera.h"
#include "..\Core3D\FWApplication.h"
#include "..\Core3D\FWScene.h"

namespace Core3D
{
	FWEntity* CreateBubble(FWScene* pkScene)
	{
		return new Bubble(pkScene);
	}
}

class BubbleVS : public CORE3DVERTEXSHADER
{
public:
	void Execute(const Core3D::ShaderReg* pkIput, C3DVECTOR4& rkPosition, Core3D::ShaderReg* pkOutput)
	{
		const C3DVECTOR4 kWaveSpeed(0.6f, 0.7f, 1.2f, 1.4f);
		const C3DVECTOR4 kWaveDirX(0.0f, 2.0f, 0.0f, 4.0f);
		const C3DVECTOR4 kWaveDirY(2.0f, 0.0f, 4.0f, 0.0f);
		const C3DVECTOR4 kWaveHeight(0.25f, 0.25f, 0.125f, 0.125f);
		const C3DFLOAT32 fNormalWarpScale = 0.01f;

		C3DVECTOR4 kWaves = kWaveDirX * pkIput[1].x + kWaveDirY * pkIput[1].y;
		kWaves += kWaveSpeed * GetFloat(0);

		// COMMENT : Calculate fractions of warp-vector components and scale to radians.
		kWaves.x -= (C3DINT32)kWaves.x + 0.5f; kWaves.y -= (C3DINT32)kWaves.y + 0.5f;
		kWaves.z -= (C3DINT32)kWaves.z + 0.5f; kWaves.w -= (C3DINT32)kWaves.w + 0.5f;

		const C3DVECTOR4 kSinWaves(sinf(kWaves.x), sinf(kWaves.y), sinf(kWaves.z), sinf(kWaves.w));
		const C3DVECTOR4 kCosWaves(cosf(kWaves.x), cosf(kWaves.y), cosf(kWaves.z), sinf(kWaves.w));

		// COMMENT : Apply deformation in direction of normal using sine-waves
		C3DVECTOR3 kNormal = pkIput[0]; kNormal.Normalize();
		const C3DVECTOR4 kPosition = pkIput[0] + kNormal * Core3D::Vec4Dot(kSinWaves, kWaveHeight);
		rkPosition = kPosition * GetMatrix(Core3D::SC_WVPMATRIX);

		// COMMENT : Pass texcoord to pixel shader
		pkOutput[0] = pkIput[1];

		const C3DVECTOR3 kTangent(-kNormal.z, 0.0f, kNormal.x); // COMMENT : tangent.x = normal % vector3(0, 0, -1); tangent.y = 0; tangent.z = normal % vector3(1, 0, 0);
		C3DVECTOR3 kBinormal;
		Core3D::Vec3Cross(kBinormal, kNormal, kTangent);

		const C3DFLOAT32 fNormalWarpBinormal= Core3D::Vec4Dot(-kCosWaves, kWaveDirX);
		const C3DFLOAT32 fNormalWarpTangent	= Core3D::Vec4Dot(-kCosWaves, kWaveDirY);
		C3DVECTOR3 kWarpedNormal			= kBinormal * fNormalWarpBinormal + kTangent * fNormalWarpTangent;
		kWarpedNormal						= kWarpedNormal * fNormalWarpScale + kNormal;
		kWarpedNormal.Normalize();
		Core3D::Vec3TransformNormal(kWarpedNormal, kWarpedNormal, GetMatrix(Core3D::SC_WORLDMATRIX));

		const C3DVECTOR3 kWorldPos		= kPosition * GetMatrix(Core3D::SC_WORLDMATRIX);
		const C3DVECTOR3 kViewDir		= ((C3DVECTOR3)GetVector(0) - kWorldPos).Normalize();
		const C3DFLOAT32 fViewDotNormal	= Core3D::Vec3Dot(kWarpedNormal, kViewDir);

		pkOutput[1]		= kWarpedNormal * (2.0f * fViewDotNormal) - kViewDir;
		pkOutput[2].x	= fViewDotNormal;
	}

	Core3D::ShaderRegType GetOutputRegisters(C3DUINT32 uiRegister)
	{
		switch(uiRegister)
		{
		case 0: return Core3D::SRT_VECTOR2; // COMMENT : Texcoord
		case 1: return Core3D::SRT_VECTOR3; // COMMENT : Reflection
		case 2: return Core3D::SRT_FLOAT32; // COMMENT : N * V
		default: return Core3D::SRT_UNUSED;
		}
	}
};

class BubblePS : public CORE3DPIXELSHADER
{
public:
	bool MightKillPixels()
	{
		return false;
	}

	bool Execute(const Core3D::ShaderReg* pkInput, C3DVECTOR4& rkColor, C3DFLOAT32& rfDepth)
	{
		C3DVECTOR4 kRainbowFilm;
		SampleTexture(kRainbowFilm, 0, pkInput[0].x, pkInput[0].y, 0.0f);

		const C3DFLOAT32 fFresnel = 1.0f - fabsf(pkInput[2].x);

		C3DVECTOR4 kReflectionEnv;
		SampleTexture(kReflectionEnv, 1, pkInput[1].x, pkInput[1].y, pkInput[1].z);

		C3DFLOAT32 fAlpha = Core3D::Saturate(4.0f * (kReflectionEnv.a * kReflectionEnv.a - 0.75f));
		const C3DVECTOR4 kBaseEnvColor = (kRainbowFilm * kReflectionEnv * 2.0f).Saturate();

		C3DVECTOR4 kColor;
		Core3D::Vec4Lerp(kColor, kBaseEnvColor, kReflectionEnv, fAlpha);

		fAlpha += 0.6f * fFresnel + 0.1f;

		Core3D::Vec4Lerp(rkColor, rkColor, kColor, Core3D::Saturate(fAlpha));
		return true;
	}
};

Core3D::VertexElement akVertexDeclaration[] = 
{
	CORE3D_VERTEXFORMAT_DECL(0, Core3D::VET_VECTOR3, 0),
	CORE3D_VERTEXFORMAT_DECL(0, Core3D::VET_VECTOR2, 1)
};

Bubble::Bubble(Core3D::FWScene* pkScene)
{
	m_pkScene = pkScene;

	m_pkVertexFormat	= NULL;
	m_pkVertexBuffer	= NULL;
	m_pkIndexBuffer		= NULL;
	m_pkVertexShader	= NULL;
	m_pkPixelShader		= NULL;

	m_uiNumVertices		= 0;
	m_uiNumPrimitives	= 0;

	m_hRainbowFilm		= 0;
	m_hEnvironment		= 0;
}

Bubble::~Bubble()
{
	m_pkScene->GetApplication()->GetResManager()->ReleaseResource(m_hEnvironment);
	m_pkScene->GetApplication()->GetResManager()->ReleaseResource(m_hRainbowFilm);

	CORE3D_SAFE_RELEASE(m_pkPixelShader);
	CORE3D_SAFE_RELEASE(m_pkVertexShader);
	CORE3D_SAFE_RELEASE(m_pkIndexBuffer);
	CORE3D_SAFE_RELEASE(m_pkVertexBuffer);
	CORE3D_SAFE_RELEASE(m_pkVertexFormat);
}

bool Bubble::Initialize(C3DFLOAT32 fRadius, C3DUINT32 uiStacks, C3DUINT32 uiSlices)
{
	Core3D::FWGraphics* pkGraphics	= m_pkScene->GetApplication()->GetGraphics();
	LPCORE3DDEVICE pkDevice			= pkGraphics->GetDevice();

	// COMMENT : Construct a sphere
	m_uiNumVertices		= uiStacks * uiSlices * 4;
	m_uiNumPrimitives	= uiStacks * uiSlices * 2;

	if(CORE3D_FAILED(pkDevice->CreateVertexFormat(&m_pkVertexFormat, akVertexDeclaration, sizeof(akVertexDeclaration))))
	{
		return false;
	}

	if(CORE3D_FAILED(pkDevice->CreateVertexBuffer(&m_pkVertexBuffer, sizeof(VertexData) * m_uiNumVertices)))
	{
		return false;
	}

	if(CORE3D_FAILED(pkDevice->CreateIndexBuffer(&m_pkIndexBuffer, sizeof(C3DUINT16) * m_uiNumPrimitives * 3, Core3D::FMT_INDEX16)))
	{
		return false;
	}

	VertexData* pkDestVertices = NULL;
	if(CORE3D_FAILED(m_pkVertexBuffer->GetPointer(0, (void**)&pkDestVertices)))
	{
		return false;
	}

	C3DUINT16* pDestIndices = NULL;
	if(CORE3D_FAILED(m_pkIndexBuffer->GetPointer(0, (void**)&pDestIndices)))
	{
		return false;
	}

	const C3DFLOAT32 fStepV = 1.0f / (C3DFLOAT32)uiStacks;
	const C3DFLOAT32 fStepU = 1.0f / (C3DFLOAT32)uiSlices;

	C3DUINT32 uiCurVertex = 0;
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
			pkDestVertices->kTexCoord0	= C3DVECTOR2(fU, fV);
			pkDestVertices++;

			pkDestVertices->kPosition	= C3DVECTOR3(x[1], y[1], z[1]);
			pkDestVertices->kTexCoord0	= C3DVECTOR2(fNextU, fV);
			pkDestVertices++;

			pkDestVertices->kPosition	= C3DVECTOR3(x[2], y[2], z[2]);
			pkDestVertices->kTexCoord0	= C3DVECTOR2(fU, fNextV);
			pkDestVertices++;

			pkDestVertices->kPosition	= C3DVECTOR3(x[3], y[3], z[3]);
			pkDestVertices->kTexCoord0	= C3DVECTOR2(fNextU, fNextV);
			pkDestVertices++;

			*pDestIndices++ = uiCurVertex;
			*pDestIndices++ = uiCurVertex + 1;
			*pDestIndices++ = uiCurVertex + 2;

			*pDestIndices++ = uiCurVertex + 1;
			*pDestIndices++ = uiCurVertex + 3;
			*pDestIndices++ = uiCurVertex + 2;

			uiCurVertex += 4;
		}
	}

	m_pkVertexShader	= new BubbleVS;
	m_pkPixelShader		= new BubblePS;

	// COMMENT : Load textures...
	Core3D::FWResManager* pkResManager = m_pkScene->GetApplication()->GetResManager();
	m_hRainbowFilm = pkResManager->LoadResource(_T("rainbowfilm_smooth.png"));
	if(!m_hRainbowFilm) {return false;}

	m_hEnvironment = pkResManager->LoadResource(_T("room.cube"));
	if(!m_hEnvironment) {return false;}

	return true;
}

bool Bubble::FrameMove()
{
	return false;
}

void Bubble::Render(C3DUINT32 uiPass)
{
	switch(uiPass)
	{
	case FreeCamera::PASS_DEFUALT: break;
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

	Core3D::FWResManager* pkResManager = m_pkScene->GetApplication()->GetResManager();
	Core3D::FWTexture* pkRainboeFilm = (Core3D::FWTexture*)pkResManager->GetResource(m_hRainbowFilm);
	pkGraphics->SetTexture(0, pkRainboeFilm->GetTexture());

	Core3D::FWTexture* pkEnvironment = (Core3D::FWTexture*)pkResManager->GetResource(m_hEnvironment);
	pkGraphics->SetTexture(1, pkEnvironment->GetTexture());

	m_pkVertexShader->SetFloat(0, pkGraphics->GetApplication()->GetElapsedTime());
	m_pkVertexShader->SetVector(0, pkCurrentCamera->GetPosition());

	pkGraphics->SetVertexFormat(m_pkVertexFormat);
	pkGraphics->SetVertexStream(0, m_pkVertexBuffer, 0, sizeof(VertexData));
	pkGraphics->SetIndexBuffer(m_pkIndexBuffer);
	pkGraphics->SetVertexShader(m_pkVertexShader);
	pkGraphics->SetPixelShader(m_pkPixelShader);

	pkGraphics->SetRenderState(Core3D::RS_CULLMODE, Core3D::CULL_CW);
	pkGraphics->GetDevice()->DrawIndexedPrimitive(Core3D::PT_TRIANGLELIST, 0, 0, 
		m_uiNumVertices, 0, m_uiNumPrimitives);

	pkGraphics->SetRenderState(Core3D::RS_CULLMODE, Core3D::CULL_CCW);
	pkGraphics->GetDevice()->DrawIndexedPrimitive(Core3D::PT_TRIANGLELIST, 0, 0, 
		m_uiNumVertices, 0, m_uiNumPrimitives);
}