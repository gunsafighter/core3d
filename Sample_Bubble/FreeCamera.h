#pragma once
#include "..\Core3D\FWCamera.h"

class FreeCamera : public Core3D::FWCamera
{
public:
	enum Pass
	{
		PASS_DEFUALT
	};
	FreeCamera(Core3D::FWGraphics* pkParent);
	void RenderPass(C3DINT32 iPass = -1);
};