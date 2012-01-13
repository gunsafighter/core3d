#include "App.h"
#include "../Core3D/FWInput.h"
#include "../Core3D/FWGraphics.h"
#include "../Core3D/FWResManager.h"
#include "../Core3D/FWLight.h"

#include "FreeCamera.h"
#include "EnvSphere.h"

#ifdef _DEBUG
#	pragma comment(lib, "../debug/Core3D_D.lib")
#	pragma comment(lib, "../Core3D/libpng/libpngd.lib")
#	pragma comment(lib, "../Core3D/zlib/zlibd.lib")
#else
#	pragma comment(lib, "../release/Core3D.lib")
#	pragma comment(lib, "../Core3D/libpng/libpng.lib")
#	pragma comment(lib, "../Core3D/zlib/zlib.lib")
#endif

bool App::CreateWorld()
{
	m_pkCamera	= NULL;
	m_hSphere	= NULL;
	m_hLight	= NULL;

	// COMMENT : Create and setup camera
	m_pkCamera	= new FreeCamera(GetGraphics());
	if(false == m_pkCamera->CreateRenderCamera(GetWindowWidth(), GetWindowHeight())) {return false;}
	m_pkCamera->CalculateProjection(CORE3D_PI * 0.5f, 10.0f, 0.1f);
	m_pkCamera->SetPosition(C3DVECTOR3(0.0f, 0.0f, -2.0f));
	m_pkCamera->SetLookAt(C3DVECTOR3(0.0f, 0.0f, 0.0f), C3DVECTOR3(0.0f, 1.0f, 0.0f));
	m_pkCamera->CalculateView();

	// COMMENT : Register board entity and create an instance
	GetScene()->RegisterEntityType(_T("EnvSphere"), &Core3D::CreateEnvSphere);
	m_hSphere	= GetScene()->CreateEntity(_T("EnvSphere"));
	if(NULL == m_hSphere) {return false;}
	
	EnvSphere* pkSphere = static_cast<EnvSphere*>(GetScene()->GetEntity(m_hSphere));
	if(false == pkSphere->Initialize(1.0f, 16, 16, _T("majestic.cube"))) {return false;}

	// COMMENT : Create the light
	m_hLight = GetScene()->CreateLight();
	if(!m_hLight) {return false;}
	Core3D::FWLight* pkLight = GetScene()->GetLight(m_hLight);
	pkLight->SetPosition(C3DVECTOR3(1.5f, 0.25f, 0.0f));
	pkLight->SetColor(C3DVECTOR4(1.0f, 1.0f, 0.0f, 1.0f));

	// COMMENT : Demonstrating smooth-subdivision to get a round sphere
	// disable this and increase sphere-tesselation for better preformance (memory-usage will be highter!)
	GetGraphics()->SetRenderState(Core3D::RS_SUBDIVISIONMODE, Core3D::SUBDIV_SMOOTH);
	GetGraphics()->SetRenderState(Core3D::RS_SUBDIVISIONLEVELS, 1);
	GetGraphics()->SetRenderState(Core3D::RS_SUBDIVISIONPOSITIONREGISTER, 0);
	GetGraphics()->SetRenderState(Core3D::RS_SUBDIVISIONNORMALREGISTER, 0); // Using vertex-positions as normals (unit-sphere at origin)

	return true;
}

void App::DestroyWorld()
{
	GetScene()->ReleaseLight(m_hLight);
	GetScene()->ReleaseEntity(m_hSphere);
	CORE3D_SAFE_DELETE(m_pkCamera);
}

void App::FrameMove()
{
	if(GetInput()->KeyDown(DIK_ESCAPE)) {::PostQuitMessage(0);}


	if(0 == GetFrameIdent() % 5)
	{
		tchar szCaption[256] = _T("");
		_stprintf_s(szCaption, _T("EnvSphere, FPS: %3.1f"), GetFPS());
		::SetWindowText(GetWindowHandle(), szCaption);
	}

	// COMMENT: Rotate camera around sphere
	static C3DFLOAT32 fRotX = 0.5f, fRotY = 0.0f;
	C3DINT32 iDeltaX, iDeltaY;
	GetInput()->GetMovement(&iDeltaX, &iDeltaY);
	fRotX += -0.1f * iDeltaX * GetInvFPS();
	fRotY +=  0.1f * iDeltaY * GetInvFPS();

	const C3DFLOAT32 fCamPosX = -2.0f * sinf(fRotX);
	const C3DFLOAT32 fCamPosY = -1.0f * sinf(fRotY);
	const C3DFLOAT32 fCamPosZ = -2.0f * cosf(fRotX);
	m_pkCamera->SetPosition(C3DVECTOR3(fCamPosX, fCamPosY, fCamPosZ));
	m_pkCamera->SetLookAt(C3DVECTOR3(0.0f, 0.0f, 0.0f), C3DVECTOR3(0.0f, 1.0f, 0.0f));
	m_pkCamera->CalculateView();
}

void App::RenderWorld()
{
	if(NULL != m_pkCamera)
	{
		m_pkCamera->BeginRender();
		m_pkCamera->ClearToSceneColor();
		m_pkCamera->RenderPass(-1);
		m_pkCamera->EndRender(true);
	}
}