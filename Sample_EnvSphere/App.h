#pragma once
#include "../Core3D/FWApplication.h"
#include "../Core3D/FWScene.h"

class FreeCamera;
class App : public Core3D::FWApplicationWin32
{
public:
	bool CreateWorld();
	void DestroyWorld();
	void FrameMove();
	void RenderWorld();
private:
	FreeCamera*		m_pkCamera;
	Core3D::HENTITY m_hSphere;
	Core3D::HLIGHT	m_hLight;
};