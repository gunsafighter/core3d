#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DTypes.h"

namespace Core3D
{
	class Device;
	class Surface;
	class RenderTarget : public RefObject
	{
	public:
		Device*		GetDevice();
		Result		ClearColorBuffer(const Vector4& rkColor, const Rect* pkRect);
		Result		ClearDepthBuffer(FLOAT32 fDepth, const Rect* pkRect);
		Result		SetColorBuffer(Surface* pkColorBuffer);
		Result		SetDepthBuffer(Surface* pkDepthBuffer);
		Surface*	GetColorBuffer();
		Surface*	GetDepthBuffer();
		void		SetViewportMatrix(const Matrix4x4& rkMatViewport);
		const Matrix4x4& GetViewportMatrix();
	protected:
		friend class Device;

		RenderTarget(Device* pkDevice);
		~RenderTarget();
	private:
		Device*		m_pkDevice;
		Surface*	m_pkColorBuffer;
		Surface*	m_pkDepthBuffer;
		Matrix4x4	m_kMatViewport;
	};
}