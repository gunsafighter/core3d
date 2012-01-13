#pragma once
#include "../Core3D/FWEntity.h"
#include "../Core3D/FWGraphics.h"
#include "../Core3D/FWTexture.h"
#include "../Core3D/FWResManager.h"

namespace Core3D
{
	FWEntity* CreateCrystal(FWScene* pkScene);
}

class CrystalVS;
class CrystalPS;
class Crystal : public Core3D::FWEntity
{
public:
	friend Core3D::FWEntity* Core3D::CreateCrystal(Core3D::FWScene* pkScene);
private:
	Crystal(Core3D::FWScene* pkScene);
public:
	~Crystal();

	bool Initialize(tstring strModel, tstring strTexture, tstring strNormalMap);
	bool FrameMove();
	void Render(UINT32 uiPass);
private:
	CrystalVS* m_pkVertexShader;
	CrystalPS* m_pkPixelShader;
	Core3D::HRESOURCE  m_hModel, m_hTexture, m_hNormalMap, m_hEnvironment;
};