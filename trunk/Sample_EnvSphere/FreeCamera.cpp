#include "FreeCamera.h"
#include "../Core3D/FWScene.h"
#include "../Core3D/FWGraphics.h"
#include "../Core3D/FWApplication.h"

FreeCamera::FreeCamera(Core3D::FWGraphics* pkGraphics)
: FWCamera(pkGraphics)
{

}

void FreeCamera::RenderPass(C3DINT32 iPass /* = -1 */)
{
	Core3D::FWScene* pkScene = m_pkGraphics->GetApplication()->GetScene();
	if(-1 == iPass || PASS_LIGHTING == iPass)
	{
		C3DUINT32 uiNumLights = pkScene->GetNumLights();
		for(C3DUINT32 i = 0; i < uiNumLights; ++i)
		{
			pkScene->SetCurrentLight(i);
			pkScene->Render(PASS_LIGHTING);
		}
	}
}