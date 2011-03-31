#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DTypes.h"

namespace Core3D
{
	class Device;
	class VertexFormat : public RefObject
	{
	public:
		Device* GetDevice();
	protected:
		friend class Device;

		VertexFormat(Device* pkDevice);
		~VertexFormat();
		Result Create(const VertexElement* pkVertexDeclaration, UINT32 uiVertexDeclSize);
		UINT32 GetNumVertexElements();
		UINT32 GetHighestStream();
		VertexElement* GetElements();
	private:
		Device*			m_pkDevice;
		UINT32			m_uiNumVertexElements;
		UINT32			m_uiHighestStream;
		VertexElement*	m_pkElements;
	};
}