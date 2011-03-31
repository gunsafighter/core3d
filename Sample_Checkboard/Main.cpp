#include "App.h"
#include "resource.h"

INT APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, INT nCmdShow)
{
	C3DUINT16 uiWidth	= 640;
	C3DUINT16 uiHeight	= 480;

	Core3D::CreationFlags kCreationFlags;
	kCreationFlags.strWindowTitle	= _T("Antialiased procedural checkboard");
	kCreationFlags.hIcon			= ::LoadIcon(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	kCreationFlags.uiWindowWidth	= uiWidth;
	kCreationFlags.uiWindowHeight	= uiHeight;
	kCreationFlags.bWindowed		= true;

	App kApp;
	if(false == kApp.Initialize(kCreationFlags)) {return 1;}
	return kApp.Run();
}