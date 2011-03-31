#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DTypes.h"

namespace Core3D
{
	class Device;
	class Volume : public RefObject
	{
	public:
		void	SamplePoint(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV, FLOAT32 fW);
		void	SampleLinear(Vector4& rkColor, FLOAT32 fU, FLOAT32 fV, FLOAT32 fW);
		Result	Clear(const Vector4& rkColor, const Box* pkBox);
		Result	CopyToVolume(const Box* pkSrcBox, Volume* pkDestVolume, const Box* pkDestBox, TextureFilter eFilter);
		Result	LockBox(void** ppvData, const Box* pkBox);
		Result	UnlockBox();
		Format	GetFormat();
		UINT32	GetFormatFloats();
		UINT32	GetWidth();
		UINT32	GetHeight();
		UINT32	GetDepth();
		Device* GetDevice();
	protected:
		friend class Device;

		Volume(Device* pkDevice);
		~Volume();
		Result Create(UINT32 uiWidth, UINT32 uiHeight, UINT32 uiDepth, Format eFormat);
	private:
		Device*		m_pkDevice;
		Format		m_eFormat;
		UINT32		m_uiWidth;
		UINT32		m_uiHeight;
		UINT32		m_uiDepth;
		UINT32		m_uiWidthMin;
		UINT32		m_uiHeightMin;
		UINT32		m_uiDepthMin;
		bool		m_bLockedComplete;
		Box			m_kPartialLockBox;
		FLOAT32*	m_pfPartialLockData;
		FLOAT32*	m_pfData;
	};
}