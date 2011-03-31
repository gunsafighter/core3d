#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Common.h"

namespace Core3D
{
	struct Matrix4x4
	{
	public:
		Matrix4x4();
		Matrix4x4(const Matrix4x4& rhs);
		Matrix4x4(
			FLOAT32 f11, FLOAT32 f12, FLOAT32 f13, FLOAT32 f14, 
			FLOAT32 f21, FLOAT32 f22, FLOAT32 f23, FLOAT32 f24, 
			FLOAT32 f31, FLOAT32 f32, FLOAT32 f33, FLOAT32 f34, 
			FLOAT32 f41, FLOAT32 f42, FLOAT32 f43, FLOAT32 f44);
		Matrix4x4(
			const struct Vector4& rkRow0, 
			const struct Vector4& rkRow1, 
			const struct Vector4& rkRow2, 
			const struct Vector4& rkRow3);

		const Matrix4x4& operator=(const Matrix4x4& rhs);

		operator FLOAT32*();
		operator const FLOAT32*() const;
		FLOAT32 operator()(UINT32 uiRow, UINT32 uiCol) const;

		Matrix4x4 operator+() const;
		Matrix4x4 operator-() const;

		const Matrix4x4& operator+=(const Matrix4x4& rhs);
		const Matrix4x4& operator-=(const Matrix4x4& rhs);
		const Matrix4x4& operator*=(const Matrix4x4& rhs);
		const Matrix4x4& operator*=(FLOAT32 fVal);
		const Matrix4x4& operator/=(FLOAT32 fVal);

		Matrix4x4 operator+(const Matrix4x4& rkMat) const;
		Matrix4x4 operator-(const Matrix4x4& rkMat) const;
		Matrix4x4 operator*(const Matrix4x4& rkMat) const;
		Matrix4x4 operator*(FLOAT32 fVal) const;
		Matrix4x4 operator/(FLOAT32 fVal) const;

		FLOAT32 Determinant() const;
	public:
		union
		{
			struct
			{
				FLOAT32 _11, _12, _13, _14;
				FLOAT32 _21, _22, _23, _24;
				FLOAT32 _31, _32, _33, _34;
				FLOAT32 _41, _42, _43, _44;
			};
			struct
			{
				FLOAT32 m[4][4];
			};
		};
	};
}
#include "Matrix4x4.inl"