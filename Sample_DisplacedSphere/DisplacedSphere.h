#pragma once
#include "../Core3D/FWEntity.h"
#include "../Core3D/FWGraphics.h"
#include "../Core3D/FWTexture.h"
#include "../Core3D/FWResManager.h"

namespace Core3D
{
	FWEntity* CreateDisplacedSphere(FWScene* pkScene);
}

class SphereVS;
class SpherePS;
class SpherePrimitiveAssembler;
class DisplacedSphere : public Core3D::FWEntity
{
public:
	friend Core3D::FWEntity* Core3D::CreateDisplacedSphere(Core3D::FWScene* pkScene);
	struct VertexData
	{
		C3DVECTOR3 kPosition;
		C3DVECTOR2 kTex;
	};
private:
	DisplacedSphere(Core3D::FWScene* pkScene);
public:
	~DisplacedSphere();
	bool Initialize(C3DFLOAT32 fRadius, C3DUINT32 uiStacks, C3DUINT32 uiSlices, tstring strTexture);
	bool FrameMove();
	void Render(C3DUINT32 uiPass);

private:
	Core3D::FWScene*			m_pkScene;
	LPCORE3DVERTEXFORMAT		m_pkVertexFormat;
	LPCORE3DVERTEXBUFFER		m_pkVertexBuffer;
	SpherePrimitiveAssembler*	m_pkPrimitiveAssembler;
	SphereVS*					m_pkVertexShader;
	SpherePS*					m_pkPixelShader;
	C3DUINT32					m_uiNumVertices, m_uiNumPrimitives;
	Core3D::HRESOURCE			m_hTexture;
};