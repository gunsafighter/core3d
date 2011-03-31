#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DTypes.h"

namespace Core3D
{
	class Device;
	class VertexBuffer : public RefObject
	{
	public:
		Device* GetDevice();
		Result	GetPointer(UINT32 uiOffset, void** ppvData);
		UINT32	GetLength();
	protected:
		friend class Device;

		VertexBuffer(Device* pkDevice);
		~VertexBuffer();
		Result Create(UINT32 uiLength);
	private:
		Device* m_pkDevice;
		UINT32	m_uiLength;
		BYTE8*	m_pData;
	};
}