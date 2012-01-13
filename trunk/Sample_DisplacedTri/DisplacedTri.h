#pragma once
#include "../Core3D/FWEntity.h"
#include "../Core3D/FWResManager.h"
#include "../Core3D/FWTexture.h"
#include "../Core3D/FWGraphics.h"

namespace Core3D
{
	FWEntity* CreateDisplacedTri(FWScene* pkScene);
}

class TriangleVS;
class TrianglePS;

class DisplacedTri : public Core3D::FWEntity
{
public:
	friend Core3D::FWEntity* Core3D::CreateDisplacedTri(Core3D::FWScene* pkScene);
	struct VertexData
	{
		C3DVECTOR3 kPosition;
		C3DVECTOR3 kNormal;
		C3DVECTOR3 kTangent;
		C3DVECTOR3 kTexCoord0;
	};
private:
	DisplacedTri(Core3D::FWScene* pkScene);
public:
	~DisplacedTri();
	bool Initialize(const DisplacedTri::VertexData* pkVertices, tstring strTexture, tstring strNormalMap);
	bool FrameMove();
	void Render(C3DUINT32 uiPass);
private:
	Core3D::FWScene*		m_pkScene;
	LPCORE3DVERTEXFORMAT	m_pkVertexFormat;
	LPCORE3DVERTEXBUFFER	m_pkVertexBuffer;
	TriangleVS*				m_pkVertexShader;
	TrianglePS*				m_pkPixelShader;
	Core3D::HRESOURCE		m_hTexture;
	Core3D::HRESOURCE		m_hNormalMap;
};