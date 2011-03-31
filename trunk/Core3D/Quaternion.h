#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Common.h"

namespace Core3D
{
	struct Quaternion
	{
	public:
		Quaternion();
		Quaternion(const Quaternion& rhs);
		Quaternion(FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ, FLOAT32 fW);

		const Quaternion& operator=(const Quaternion& rhs);

		operator FLOAT32*();
		operator const FLOAT32*() const;

		Quaternion operator+() const;
		Quaternion operator-() const;

		const Quaternion& operator+=(const Quaternion& rhs);
		const Quaternion& operator-=(const Quaternion& rhs);
		const Quaternion& operator*=(const Quaternion& rhs);
		const Quaternion& operator*=(FLOAT32 fVal);
		const Quaternion& operator/=(FLOAT32 fVal);

		Quaternion operator+(const Quaternion& rkQuat) const;
		Quaternion operator-(const Quaternion& rkQuat) const;
		Quaternion operator*(const Quaternion& rkQuat) const;
		Quaternion operator*(FLOAT32 fVal) const;
		Quaternion operator/(FLOAT32 fVal) const;

		FLOAT32		Length() const;
		FLOAT32		LengthSq() const;
		Quaternion& Normalize();
	public:
		FLOAT32 x, y, z, w;
	};
}
#include "Quaternion.inl"