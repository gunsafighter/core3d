#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Common.h"

namespace Core3D
{
	struct Vector3
	{
	public:
		Vector3();
		Vector3(const Vector3& rhs);
		Vector3(FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ);
		Vector3(const struct Vector2& rkVal);
		Vector3(const struct Vector4& rkVal);
		
		const Vector3& operator=(const Vector3& rhs);
		const Vector3& operator=(FLOAT32 fX);
		const Vector3& operator=(const struct Vector2& rkVal);
		const Vector3& operator=(const struct Vector4& rkVal);

		operator FLOAT32*();
		operator const FLOAT32*() const;

		Vector3 operator+() const;
		Vector3 operator-() const;

		const Vector3& operator+=(const Vector3& rhs);
		const Vector3& operator-=(const Vector3& rhs);
		const Vector3& operator*=(const Vector3& rhs);
		const Vector3& operator*=(const struct Matrix4x4& rkMat);
		const Vector3& operator*=(FLOAT32 fVal);
		const Vector3& operator/=(FLOAT32 fVal);

		Vector3 operator+(const Vector3& rkVal) const;
		Vector3 operator-(const Vector3& rkVal) const;
		Vector3 operator*(const Vector3& rkVal) const;
		Vector3 operator*(const struct Matrix4x4& rkMat) const;
		Vector3 operator*(FLOAT32 fVal) const;
		Vector3 operator/(FLOAT32 fVal) const;

		FLOAT32		Length() const;
		FLOAT32		LengthSq() const;
		Vector3&	Normalize();
		Vector3&	Clamp(FLOAT32 fLower, FLOAT32 fUpper);
		Vector3&	Saturate();
	public:
		union
		{
			struct {FLOAT32 x, y, z;};
			struct {FLOAT32 r, g, b;};
		};
	};
}
#include "Vector3.inl"