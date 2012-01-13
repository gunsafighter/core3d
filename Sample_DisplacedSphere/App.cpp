#include "App.h"
#include "../Core3D/FWInput.h"
#include "../Core3D/FWGraphics.h"
#include "../Core3D/FWResManager.h"

#include "FreeCamera.h"
#include "DisplacedSphere.h"

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

	// COMMENT : Create and setup camera
	m_pkCamera	= new FreeCamera(GetGraphics());
	if(false == m_pkCamera->CreateRenderCamera(GetWindowWidth(), GetWindowHeight())) {return false;}
	m_pkCamera->CalculateProjection(CORE3D_PI * 0.5f, 10.0f, 0.1f);
	m_pkCamera->SetPosition(C3DVECTOR3(0.0f, 0.0f, -2.0f));
	m_pkCamera->SetLookAt(C3DVECTOR3(0.0f, 0.0f, 0.0f), C3DVECTOR3(0.0f, 1.0f, 0.0f));
	m_pkCamera->CalculateView();

	// COMMENT : Register board entity and create an instance
	GetScene()->RegisterEntityType(_T("DisplacedSphere"), &Core3D::CreateDisplacedSphere);
	m_hSphere	= GetScene()->CreateEntity(_T("DisplacedSphere"));
	if(NULL == m_hSphere) {return false;}

	DisplacedSphere* pkSphere = static_cast<DisplacedSphere*>(GetScene()->GetEntity(m_hSphere));
	if(false == pkSphere->Initialize(1.0f, 12, 12, _T("earth.png"))) {return false;}

	// COMMENT : Enable Core3D's subdivision stage, which will be the base for displacement mapping.
	GetGraphics()->SetRenderState(Core3D::RS_SUBDIVISIONMODE, Core3D::SUBDIV_ADAPTIVE);
	GetGraphics()->SetRenderState(Core3D::RS_SUBDIVISIONLEVELS, 1);

	const C3DFLOAT32 fSubdivisionMaxScreenArea = 14.0f * 14.0f; // Triangles larger than 14x14 square-pixels will be subdivided.
	GetGraphics()->SetRenderState(Core3D::RS_SUBDIVISIONMAXSCREENAREA, *(C3DUINT32*)&fSubdivisionMaxScreenArea);
	GetGraphics()->SetRenderState(Core3D::RS_SUBDIVISIONMAXINNERLEVELS, 2);
	
	GetGraphics()->SetRenderState(Core3D::RS_FILLMODE, Core3D::FILL_WIREFRAME);

	return true;
}

void App::DestroyWorld()
{
	GetScene()->ReleaseEntity(m_hSphere);
	CORE3D_SAFE_DELETE(m_pkCamera);
}

void App::FrameMove()
{
	if(GetInput()->KeyDown(DIK_ESCAPE)) {::PostQuitMessage(0);}

	if(GetInput()->ButtonDown(0))
	{
		static C3DFLOAT32 fRotX = 0.0f, fRotY = 0.0f;
		C3DINT32 iDeltaX, iDeltaY;

		GetInput()->GetMovement(&iDeltaX, &iDeltaY);
		fRotX += -2.0f * (C3DFLOAT32)iDeltaX * GetInvFPS();
		fRotY +=  2.0f * (C3DFLOAT32)iDeltaY * GetInvFPS();

		const C3DFLOAT32 fCamPosX = -2.0f * sinf(Core3D::DegToRad(fRotX));
		const C3DFLOAT32 fCamPosY = -2.0f * sinf(Core3D::DegToRad(fRotY));
		const C3DFLOAT32 fCamPosZ = -2.0f * cosf(Core3D::DegToRad(fRotX));
		m_pkCamera->SetPosition(C3DVECTOR3(fCamPosX, fCamPosY, fCamPosZ));
		m_pkCamera->SetLookAt(C3DVECTOR3(0.0f, 0.0f, 0.0f), C3DVECTOR3(0.0f, 1.0f, 0.0f));
		m_pkCamera->CalculateView();
	}

	if(0 == GetFrameIdent() % 5)
	{
		tchar szCaption[256] = _T("");
		_stprintf_s(szCaption, _T("DisplacedSphere, FPS: %3.1f"), GetFPS());
		::SetWindowText(GetWindowHandle(), szCaption);
	}
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