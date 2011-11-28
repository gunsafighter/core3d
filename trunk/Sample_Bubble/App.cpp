#include "App.h"
#include "..\Core3D\FWInput.h"
#include "..\Core3D\FWGraphics.h"
#include "..\Core3D\FWResManager.h"

#include "FreeCamera.h"
#include "Bubble.h"

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
	m_hBubble	= NULL;

	// COMMENT : Create and setup camera
	m_pkCamera	= new FreeCamera(GetGraphics());
	if(false == m_pkCamera->CreateRenderCamera(GetWindowWidth(), GetWindowHeight())) {return false;}
	m_pkCamera->CalculateProjection(CORE3D_PI / 6.0f, 1000.0f, 1.0f);
	m_pkCamera->SetPosition(C3DVECTOR3(0.0f, 0.0f, -150.0f));
	m_pkCamera->SetLookAt(C3DVECTOR3(0.0f, 0.0f, 0.0f), C3DVECTOR3(0.0f, 1.0f, 0.0f));
	m_pkCamera->CalculateView();

	GetScene()->SetClearColor(C3DVECTOR4(0.0f, 0.0f, 0.5f, 0.0f));

	// COMMENT : Register bubble entity and create an instance
	GetScene()->RegisterEntityType(_T("Bubble"), &Core3D::CreateBubble);
	m_hBubble = GetScene()->CreateEntity(_T("Bubble"));
	if(NULL == m_hBubble) {return false;}

	Bubble* pkBubble = static_cast<Bubble*>(GetScene()->GetEntity(m_hBubble));
	if(false == pkBubble->Initialize(20.0f, 16, 16)) {return false;}

	return true;
}

void App::DestroyWorld()
{
	GetScene()->ReleaseEntity(m_hBubble);
	CORE3D_SAFE_DELETE(m_pkCamera);
}

void App::FrameMove()
{
	if(GetInput()->KeyDown(DIK_ESCAPE)) {::PostQuitMessage(0);}

	if(GetInput()->ButtonDown(0))
	{
		// COMMENT : Rotate camera around the bubble
		static C3DFLOAT32 fRotX = 0.0f, fRotY = 0.0f;
		INT32 iDeltaX, iDeltaY;
		GetInput()->GetMovement(&iDeltaX, &iDeltaY);
		fRotX += 0.1f * iDeltaX * GetInvFPS();
		fRotY += 0.1f * iDeltaY * GetInvFPS();

		const C3DFLOAT32 fCamPosX = -150.0f * sinf(fRotX);
		const C3DFLOAT32 fCamPosY = -100.0f * sinf(fRotY);
		const C3DFLOAT32 fCamPosZ = -150.0f * cosf(fRotX);
		m_pkCamera->SetPosition(C3DVECTOR3(fCamPosX, fCamPosY, fCamPosZ));
		m_pkCamera->SetLookAt(C3DVECTOR3(0.0f, 0.0f, 0.0f), C3DVECTOR3(0.0f, 1.0f, 0.0f));
		m_pkCamera->CalculateView();
	}

	if(0 == GetFrameIdent() % 5)
	{
		tchar szCaption[256] = _T("");
		_stprintf_s(szCaption, _T("Bubble, FPS: %3.1f"), GetFPS());
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