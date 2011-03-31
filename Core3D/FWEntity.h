#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Framework Library for Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////
#include "FWType.h"

namespace Core3D
{
	class FWScene;
	class FWEntity
	{
	protected:
		friend class FWScene;
	public:
		virtual ~FWEntity() {}

		virtual bool	FrameMove()				= 0;
		virtual void	Render(UINT32 uiPass)	= 0;
		
		FWScene*		GetScene();
	protected:
		FWScene* m_pkScene;
	};
}