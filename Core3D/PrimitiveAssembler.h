#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DTypes.h"

namespace Core3D
{
	class PrimitiveAssembler : public RefObject
	{
	protected:
		friend class Device;
		virtual PrimitiveType Execute(std::vector<UINT32>& rkVertexIndices, UINT32 uiNumVertices) = 0;
	};
}