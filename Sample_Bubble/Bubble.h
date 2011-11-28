#pragma once
#include "..\Core3D\FWEntity.h"
#include "..\Core3D\FWGraphics.h"
#include "..\Core3D\FWTexture.h"
#include "..\Core3D\FWResManager.h"

namespace Core3D
{
	FWEntity* CreateBubble(FWScene* pkScene);
}

class BubbleVS;
class BubblePS;
class Bubble : Core3D::FWEntity
{
public:
	friend Core3D::FWEntity* Core3D::CreateBubble(Core3D::FWScene* pkScene);
	struct VertexData
	{
		C3DVECTOR3 kPosition;
		C3DVECTOR2 kTexCoord0;
	};
private:
	Bubble(Core3D::FWScene* pkScene);
public:
	~Bubble();

	bool Initialize(C3DFLOAT32 fRadius, C3DUINT32 uiStacks, C3DUINT32 uiSlices);
	bool FrameMove();
	void Render(C3DUINT32 uiPass);

private:
	LPCORE3DVERTEXFORMAT m_pkVertexFormat;
	LPCORE3DVERTEXBUFFER m_pkVertexBuffer;
	LPCORE3DINDEXBUFFER  m_pkIndexBuffer;
	BubbleVS*			 m_pkVertexShader;
	BubblePS*			 m_pkPixelShader;
	C3DUINT32			 m_uiNumVertices, m_uiNumPrimitives;
	Core3D::HRESOURCE	 m_hRainbowFilm, m_hEnvironment;
};