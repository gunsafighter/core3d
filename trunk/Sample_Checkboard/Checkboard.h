#pragma once
#include "../Core3D/FWEntity.h"
#include "../Core3D/FWGraphics.h"

namespace Core3D
{
	FWEntity* CreateCheckboard(FWScene* pkScene);
}

class CheckboardVS;
class CheckboardPS;
class Checkboard : public Core3D::FWEntity
{
public:
	friend Core3D::FWEntity* Core3D::CreateCheckboard(Core3D::FWScene* pkScene);
	struct VertexData
	{
		C3DVECTOR3 kPosition;
		C3DVECTOR2 kTexCoord0;
	};
private:
	Checkboard(Core3D::FWScene* pkScene);
public:
	~Checkboard();

	bool Initialize();
	bool FrameMove();
	void Render(UINT32 uiPass);
private:
	LPCORE3DVERTEXFORMAT	m_pkVertexFormat;
	LPCORE3DVERTEXBUFFER	m_pkVertexBuffer;
	CheckboardVS*			m_pkVertexShader;
	CheckboardPS*			m_pkPixelShader;
};