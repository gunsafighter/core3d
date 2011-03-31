#include "FWLight.h"
#include "FWScene.h"

namespace Core3D
{
	FWLight::FWLight(FWScene* pkScene)
	{
		m_pkScene = pkScene;
	}

	FWScene* FWLight::GetScene()
	{
		return m_pkScene;
	}
}