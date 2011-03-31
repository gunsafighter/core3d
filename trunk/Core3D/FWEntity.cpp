#include "FWEntity.h"
#include "FWScene.h"

namespace Core3D
{
	FWScene* FWEntity::GetScene()
	{
		return m_pkScene;
	}
}