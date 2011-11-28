#include "App.h"
#include "../Core3D/FWInput.h"
#include "../Core3D/FWGraphics.h"
#include "../Core3D/FWResManager.h"

#include "FreeCamera.h"
#include "Checkboard.h"

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
	m_hBoard	= NULL;

	// COMMENT : Create and setup camera
	m_pkCamera	= new FreeCamera(GetGraphics());
	if(false == m_pkCamera->CreateRenderCamera(GetWindowWidth(), GetWindowHeight())) {return false;}
	m_pkCamera->CalculateProjection(CORE3D_PI * 0.5f, 2.0f, 0.001f);
	m_pkCamera->SetPosition(C3DVECTOR3(-0.01f, 0.025f, 0.0f));
	m_pkCamera->SetLookAt(C3DVECTOR3(0.0f, 0.0f, 0.05f), C3DVECTOR3(0.0f, 1.0f, 0.0f));
	m_pkCamera->CalculateView();

	// COMMENT : Register board entity and create an instance
	GetScene()->RegisterEntityType(_T("Board"), &Core3D::CreateCheckboard);
	m_hBoard	= GetScene()->CreateEntity(_T("Board"));
	if(NULL == m_hBoard) {return false;}

	Checkboard* pkCheckboard = static_cast<Checkboard*>(GetScene()->GetEntity(m_hBoard));
	if(false == pkCheckboard->Initialize()) {return false;}

	return true;
}

void App::DestroyWorld()
{
	GetScene()->ReleaseEntity(m_hBoard);
	CORE3D_SAFE_DELETE(m_pkCamera);
}

void App::FrameMove()
{
	if(GetInput()->KeyDown(DIK_ESCAPE)) {::PostQuitMessage(0);}

	if(GetInput()->KeyDown(DIK_W))
	{
		C3DVECTOR3 kDirection = m_pkCamera->GetDirection();
		kDirection *= -0.01f;
		m_pkCamera->SetPositionRel(kDirection);
		m_pkCamera->CalculateView();
	}

	if(GetInput()->KeyDown(DIK_S))
	{
		C3DVECTOR3 kDirection = m_pkCamera->GetDirection();
		kDirection *= 0.01f;
		m_pkCamera->SetPositionRel(kDirection);
		m_pkCamera->CalculateView();
	}

	if(0 == GetFrameIdent() % 5)
	{
		tchar szCaption[256] = _T("");
		_stprintf_s(szCaption, _T("Antialiased procedural checkboard, FPS: %3.1f"), GetFPS());
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