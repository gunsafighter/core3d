#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Common.h"

namespace Core3D
{
	struct Vector4
	{
	public:
		Vector4();
		Vector4(const Vector4& rhs);
		Vector4(FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ, FLOAT32 fW);
		Vector4(const struct Vector2& rkVal);
		Vector4(const struct Vector3& rkVal);

		const Vector4& operator=(const Vector4& rhs);
		const Vector4& operator=(FLOAT32 fVal);
		const Vector4& operator=(const struct Vector2& rkVal);
		const Vector4& operator=(const struct Vector3& rkVal);

		operator FLOAT32*();
		operator const FLOAT32*() const;

		Vector4 operator+() const;
		Vector4 operator-() const;

		const Vector4& operator+=(const Vector4& rhs);
		const Vector4& operator-=(const Vector4& rhs);
		const Vector4& operator*=(const Vector4& rhs);
		const Vector4& operator*=(const struct Matrix4x4& rkMat);
		const Vector4& operator*=(FLOAT32 fVal);
		const Vector4& operator/=(FLOAT32 fVal);

		Vector4 operator+(const Vector4& rkVal) const;
		Vector4 operator-(const Vector4& rkVal) const;
		Vector4 operator*(const Vector4& rkVal) const;
		Vector4 operator*(const struct Matrix4x4& rkMat) const;
		Vector4 operator*(FLOAT32 fVal) const;
		Vector4 operator/(FLOAT32 fVal) const;

		FLOAT32		Length() const;
		FLOAT32		LengthSq() const;
		Vector4&	Normailze();
		Vector4&	Homogenize(const FLOAT32 fInvW = 0.0f); // Divides the vector by it's W-Coordinate
		Vector4&	Clamp(FLOAT32 fLower, FLOAT32 fUpper);
		Vector4&	Saturate();
	public:
		union
		{
			struct {FLOAT32 x, y, z, w;};
			struct {FLOAT32 r, g, b, a;};
		};
	};
}
#include "Vector4.inl"