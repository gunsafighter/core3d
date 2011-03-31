#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Core3D
{
	struct Plane
	{
	public:
		Plane();
		Plane(const Plane& rhs);
		Plane(FLOAT32 fA, FLOAT32 fB, FLOAT32 fC, FLOAT32 fD);
		Plane(const Vector3& rkNormal, FLOAT32 fD);

		const Plane& operator=(const Plane& rhs);

		operator FLOAT32*();
		operator const FLOAT32*() const;

		Plane operator+() const;
		Plane operator-() const;

		FLOAT32 operator*(const Vector3& rkVal) const;
		FLOAT32 operator*(const Vector4& rkVal) const;
	public:
		Vector3 kNormal;
		FLOAT32 d;
	};
}
#include "Plane.inl"