#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Common.h"

namespace Core3D
{
	struct Vector2
	{
	public:
		Vector2();
		Vector2(const Vector2& rhs);
		Vector2(FLOAT32 fX, FLOAT32 fY);
		Vector2(const struct Vector3& rkVal);
		Vector2(const struct Vector4& rkVal);

		const Vector2& operator=(const Vector2& rhs);
		const Vector2& operator=(FLOAT32 fX);
		const Vector2& operator=(const struct Vector3& rkVal);
		const Vector2& operator=(const struct Vector4& rkVal);

		operator FLOAT32*();
		operator const FLOAT32*() const;

		Vector2 operator+() const;
		Vector2 operator-() const;

		const Vector2& operator+=(const Vector2& rhs);
		const Vector2& operator-=(const Vector2& rhs);
		const Vector2& operator*=(const Vector2& rhs);
		const Vector2& operator*=(FLOAT32 fVal);
		const Vector2& operator/=(FLOAT32 fVal);

		Vector2 operator+(const Vector2& rkVal) const;
		Vector2 operator-(const Vector2& rkVal) const;
		Vector2 operator*(const Vector2& rkVal) const;
		Vector2 operator*(FLOAT32 fVal) const;
		Vector2 operator/(FLOAT32 fVal) const;

		FLOAT32		Length() const;
		FLOAT32		LengthSq() const;
		Vector2&	Normalize();
	public:
		FLOAT32 x, y;
	};
}
#include "Vector2.inl"