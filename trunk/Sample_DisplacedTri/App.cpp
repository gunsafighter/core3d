#include "App.h"
#include "../Core3D/FWInput.h"
#include "../Core3D/FWGraphics.h"
#include "../Core3D/FWResManager.h"
#include "../Core3D/FWLight.h"

#include "FreeCamera.h"
#include "DisplacedTri.h"

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
	m_hTriangle	= NULL;
	m_hLight	= NULL;

	// COMMENT : Create and setup camera
	m_pkCamera	= new FreeCamera(GetGraphics());
	if(false == m_pkCamera->CreateRenderCamera(GetWindowWidth(), GetWindowHeight())) {return false;}
	m_pkCamera->CalculateProjection(CORE3D_PI * 0.5f, 10.0f, 0.1f);
	m_pkCamera->SetPosition(C3DVECTOR3(0.15f, -0.2f, -0.8f));
	m_pkCamera->SetLookAt(C3DVECTOR3(0.0f, 0.0f, 0.0f), C3DVECTOR3(0.0f, 1.0f, 0.0f));
	m_pkCamera->CalculateView();

	// COMMENT : Register board entity and create an instance
	GetScene()->RegisterEntityType(_T("DisplacedTri"), &Core3D::CreateDisplacedTri);
	m_hTriangle	= GetScene()->CreateEntity(_T("DisplacedTri"));
	if(NULL == m_hTriangle) {return false;}

	DisplacedTri::VertexData akVertices[3];
	akVertices[0].kPosition		= C3DVECTOR3(-1.0f, -0.5f, 0.0f);
	akVertices[0].kTexCoord0	= C3DVECTOR2(0.0, 1.0f);
	akVertices[1].kPosition		= C3DVECTOR3(0.0f, 1.0f, 0.5f);
	akVertices[1].kTexCoord0	= C3DVECTOR2(0.5f, 0.0f);
	akVertices[2].kPosition		= C3DVECTOR3(1.0f, 0.0f, 0.25f);
	akVertices[2].kTexCoord0	= C3DVECTOR2(1.0f, 0.0f);

	DisplacedTri* pkTriangle = static_cast<DisplacedTri*>(GetScene()->GetEntity(m_hTriangle));
	if(false == pkTriangle->Initialize(akVertices, _T("triangle.png"), _T("triangle_normals.png"))) {return false;}

	// COMMENT : Create the light
	m_hLight = GetScene()->CreateLight();
	if(!m_hLight) {return false;}
	Core3D::FWLight* pkLight = GetScene()->GetLight(m_hLight);
	pkLight->SetPosition(C3DVECTOR3(0.0f, 1.0f, -1.0f));
	pkLight->SetColor(C3DVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));

	// COMMENT : Enable Core3D's subdivision stage, which will be the base for displacement mapping.
	GetGraphics()->SetRenderState(Core3D::RS_SUBDIVISIONMODE, Core3D::SUBDIV_SIMPLE);
	GetGraphics()->SetRenderState(Core3D::RS_SUBDIVISIONLEVELS, 5);

	return true;
}

void App::DestroyWorld()
{
	GetScene()->ReleaseLight(m_hLight);
	GetScene()->ReleaseEntity(m_hTriangle);
	CORE3D_SAFE_DELETE(m_pkCamera);
}

void App::FrameMove()
{
	if(GetInput()->KeyDown(DIK_ESCAPE)) {::PostQuitMessage(0);}

	if(m_hLight && ::GetFocus())
	{
		POINT pt;
		::GetCursorPos(&pt);
		::ScreenToClient(GetWindowHandle(), &pt);

		C3DVECTOR3 kLightPos;
		kLightPos.x = (C3DFLOAT32)pt.x / (C3DFLOAT32)GetWindowWidth() * 2.0f - 1.0f;
		kLightPos.y = 0.5f;
		kLightPos.z = (C3DFLOAT32)pt.y / (C3DFLOAT32)GetWindowHeight() * 2.0f - 1.0f;

		Core3D::FWLight* pkLight = GetScene()->GetLight(m_hLight);
		pkLight->SetPosition(kLightPos);
	}

	if(0 == GetFrameIdent() % 5)
	{
		tchar szCaption[256] = _T("");
		_stprintf_s(szCaption, _T("DisplacedTri, FPS: %3.1f"), GetFPS());
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