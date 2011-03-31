#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DTypes.h"

namespace Core3D
{
	class Device;
	class Surface : public RefObject
	{
	public:
		void	SamplePoint(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV);
		void	SampleLinear(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV);
		Result	Clear(const Vector4& rkColor, const Rect* pkRect);
		Result	CopyToSurface(const Rect* pkSrcRect, Surface* pkDestSurface, const Rect* pkDestRect, TextureFilter eFilter);
		Result	LockRect(void** ppvData, const Rect* pkRect);
		Result	UnlockRect();
		Format	GetFormat();
		UINT32	GetFormatFloats();
		UINT32	GetWidth();
		UINT32	GetHeight();
		Device*	GetDevice();
	protected:
		friend class Device;

		Surface(Device* pkDevice);
		~Surface();
		Result Create(UINT32 uiWidth, UINT32 uiHeight, Format eFormat);
	private:
		Device*		m_pkDevice;
		Format		m_eFormat;
		UINT32		m_uiWidth;
		UINT32		m_uiHeight;
		UINT32		m_uiWidthMin;
		UINT32		m_uiHeightMin;
		bool		m_bLockedComplete;
		Rect		m_kPartialLockRect;
		FLOAT32*	m_pfPartialLockData;
		FLOAT32*	m_pfData;
	};
}