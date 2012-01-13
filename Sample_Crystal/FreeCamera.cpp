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
	if(-1 == iPass || PASS_DEFUALT == iPass) {pkScene->Render(PASS_DEFUALT);}
}