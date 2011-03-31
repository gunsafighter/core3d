#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DTypes.h"

namespace Core3D
{
	class Device;
	class IndexBuffer : public RefObject
	{
	public:
		Device* GetDevice();
		Result	GetPointer(UINT32 uiOffset, void** ppvData);
		UINT32	GetLength();
		Format	GetFormat();
	protected:
		friend class Device;

		IndexBuffer(Device* pkDevice);
		~IndexBuffer();

		Result	Create(UINT32 uiLength, Format eFormat);
		Result	GetVertexIndex(UINT32 uiArrayIndex, UINT32& ruiValue);
	private:
		Device* m_pkDevice;
		UINT32	m_uiLength;
		Format	m_eFormat;
		BYTE8*	m_pData;
	};
}