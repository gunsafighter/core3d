#pragma once
#include "../Core3D/FWEntity.h"
#include "../Core3D/FWGraphics.h"
#include "../Core3D/FWTexture.h"
#include "../Core3D/FWResManager.h"

namespace Core3D
{
	FWEntity* CreateEnvSphere(FWScene* pkScene);
}

class SphereVS;
class SpherePS;
class EnvSphere : public Core3D::FWEntity
{
public:
	friend Core3D::FWEntity* Core3D::CreateEnvSphere(Core3D::FWScene* pkScene);
	struct VertexData
	{
		C3DVECTOR3 kPosition;
	};
private:
	EnvSphere(Core3D::FWScene* pkScene);
public:
	~EnvSphere();
	bool Initialize(C3DFLOAT32 fRadius, C3DUINT32 uiStacks, C3DUINT32 uiSlices, tstring strEnvironment);
	bool FrameMove();
	void Render(C3DUINT32 uiPass);
public:
	inline void SetColor(const C3DVECTOR4& rkColor) {m_kColor = rkColor;}
	inline const C3DVECTOR4 GetColor()				{return m_kColor;}
private:
	Core3D::FWScene*		m_pkScene;
	LPCORE3DVERTEXFORMAT	m_pkVertexFormat;
	LPCORE3DVERTEXBUFFER	m_pkVertexBuffer;
	LPCORE3DINDEXBUFFER		m_pkIndexBuffer;
	SphereVS*				m_pkVertexShader;
	SpherePS*				m_pkPixelShader;
	C3DVECTOR4				m_kColor;
	C3DUINT32				m_uiNumVertices, m_uiNumPrimitives;
	Core3D::HRESOURCE		m_hEnvironment;
};