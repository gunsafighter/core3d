#pragma once
#include "../Core3D/FWCamera.h"

class FreeCamera : public Core3D::FWCamera
{
public:
	enum Pass
	{
		PASS_LIGHTING // Per-light : Calculate per-pixel lighting
	};

	FreeCamera(Core3D::FWGraphics* pkGraphics);
	void RenderPass(C3DINT32 iPass = -1);
};