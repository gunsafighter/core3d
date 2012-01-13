#include "App.h"
#include "resource.h"

INT APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, INT nCmdShow)
{
	C3DUINT16 uiWidth	= 400;
	C3DUINT16 uiHeight	= 300;

	Core3D::CreationFlags kCreateFlags;
	kCreateFlags.strWindowTitle = _T("DisplacedTri");
	kCreateFlags.hIcon			= ::LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	kCreateFlags.uiWindowWidth	= uiWidth;
	kCreateFlags.uiWindowHeight = uiHeight;
	kCreateFlags.bWindowed		= true;

	App kApp;
	if(false == kApp.Initialize(kCreateFlags)) {return 1;}
	return kApp.Run();
}