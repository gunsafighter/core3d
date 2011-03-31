#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Framework Library for Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////
#include "FWType.h"

namespace Core3D
{
	class FWScene;
	class FWLight
	{
	private:
		friend class FWScene;
		
		FWLight(FWScene* pkScene);
	public:
		virtual ~FWLight() {}

		FWScene*				GetScene();
		inline void				SetPosition(const Vector3& rkPosition)	{m_kPosition = rkPosition;}
		inline const Vector3&	GetPosition()					const	{return m_kPosition;}

		inline void				SetRange(FLOAT32 fRange)				{m_fRange = fRange;}
		inline FLOAT32			GetRange()						const	{return m_fRange;}

		inline void				SetColor(const Vector4& rkColor)		{m_kColor = rkColor;}
		inline const Vector4&	GetColor()						const	{return m_kColor;}
	private:
		FWScene*	m_pkScene;
		Vector3		m_kPosition;
		Vector4		m_kColor;
		FLOAT32		m_fRange;
	};
}