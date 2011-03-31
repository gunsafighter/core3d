#include "Matrix4x4.h"
#include "Vector4.h"

namespace Core3D
{
	static FLOAT32 Determinant2x2(FLOAT32 fA, FLOAT32 fB, FLOAT32 fC, FLOAT32 fD)
	{
		return fA * fD - fB * fC;
	}

	static FLOAT32 Determinant3x3(FLOAT32 fA1, FLOAT32 fA2, FLOAT32 fA3, 
		FLOAT32 fB1, FLOAT32 fB2, FLOAT32 fB3, 
		FLOAT32 fC1, FLOAT32 fC2, FLOAT32 fC3)
	{
		// COMMENT : http://www.acm.org/pubs/tog/GraphicsGems/gems/MatrixInvert.c
		return  fA1 * Determinant2x2(fB2, fB3, fC2, fC3) - 
				fB1 * Determinant2x2(fA2, fA3, fC2, fC3) + 
				fC1 * Determinant2x2(fA2, fA3, fB2, fB3);
	}

	static FLOAT32 MinorDeterminant(const Matrix4x4& rkMat, UINT32 uiRow, UINT32 uiCol)
	{
		// COMMENT : http://www.codeproject.com/csharp/Matrix.asp
		FLOAT32 fM3x3[3][3];
		for(UINT32 r = 0, m = 0; r < 4; ++r)
		{
			if(r == uiRow) {continue;}
			for(UINT32 c = 0, n = 0; c < 4; ++c)
			{
				if(c == uiCol) {continue;}
				fM3x3[m][n] = rkMat.m[r][c];
				++n;
			}
			++m;
		}
		return Determinant3x3(fM3x3[0][0], fM3x3[0][1], fM3x3[0][2], 
							  fM3x3[1][0], fM3x3[1][1], fM3x3[1][2], 
							  fM3x3[2][0], fM3x3[2][1], fM3x3[2][2]);
	}

	static Matrix4x4 MatAdjoint(const Matrix4x4* pkMat)
	{
		// COMMENT : http://www.codeproject.com/csharp/Matrix.asp
		Matrix4x4 kMatRet;
		for(UINT32 r = 0; r < 4; ++r)
		{
			for(UINT32 c = 0; c < 4; ++c)
			{
				kMatRet.m[c][r] = powf(-1.0f, (FLOAT32)(r + c)) * MinorDeterminant(*pkMat, r, c);
			}
		}
		return kMatRet;
	}

	FLOAT32 Matrix4x4::Determinant() const
	{
		// COMMENT : http://www.acm.org/pubs/tog/GraphicsGems/gems/MatrixInvert.c
		return  _11 * Determinant3x3(_22, _32, _42, _23, _33, _43, _24, _34, _44) - 
				_12 * Determinant3x3(_21, _31, _41, _23, _33, _43, _24, _34, _44) + 
				_13 * Determinant3x3(_21, _31, _41, _22, _32, _42, _24, _34, _44) - 
				_14 * Determinant3x3(_21, _31, _41, _22, _32, _42, _23, _33, _43);
	}

	Matrix4x4 Matrix4x4::operator-() const
	{
		const FLOAT32 fDeterminant = Determinant();
		if(fabsf(fDeterminant) < FLT_EPSILON) {*this;}
		return MatAdjoint(this) / fDeterminant;
	}

	Matrix4x4::Matrix4x4(const Vector4& rkRow0, const Vector4& rkRow1, 
		const Vector4& rkRow2, const Vector4& rkRow3)
	{
		_11 = rkRow0.x; _12 = rkRow0.y; _13 = rkRow0.z; _14 = rkRow0.w;
		_21 = rkRow1.x; _22 = rkRow1.y; _23 = rkRow1.z; _24 = rkRow1.w;
		_31 = rkRow2.x; _32 = rkRow2.y; _33 = rkRow2.z; _34 = rkRow2.w;
		_41 = rkRow3.x; _42 = rkRow3.y; _43 = rkRow3.z; _44 = rkRow3.w;
	}

	const Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& rhs)
	{
		FLOAT32 f11 = _11 * rhs._11 + _12 * rhs._21 + _13 * rhs._31 + _14 * rhs._41;
		FLOAT32 f12 = _11 * rhs._12 + _12 * rhs._22 + _13 * rhs._32 + _14 * rhs._42;
		FLOAT32 f13 = _11 * rhs._13 + _12 * rhs._23 + _13 * rhs._33 + _14 * rhs._43;
		FLOAT32 f14 = _11 * rhs._14 + _12 * rhs._24 + _13 * rhs._34 + _14 * rhs._44;

		FLOAT32 f21 = _21 * rhs._11 + _22 * rhs._21 + _23 * rhs._31 + _24 * rhs._41;
		FLOAT32 f22 = _21 * rhs._12 + _22 * rhs._22 + _23 * rhs._32 + _24 * rhs._42;
		FLOAT32 f23 = _21 * rhs._13 + _22 * rhs._23 + _23 * rhs._33 + _24 * rhs._43;
		FLOAT32 f24 = _21 * rhs._14 + _22 * rhs._24 + _23 * rhs._34 + _24 * rhs._44;

		FLOAT32 f31 = _31 * rhs._11 + _32 * rhs._21 + _33 * rhs._31 + _34 * rhs._41;
		FLOAT32 f32 = _31 * rhs._12 + _32 * rhs._22 + _33 * rhs._32 + _34 * rhs._42;
		FLOAT32 f33 = _31 * rhs._13 + _32 * rhs._23 + _33 * rhs._33 + _34 * rhs._43;
		FLOAT32 f34 = _31 * rhs._14 + _32 * rhs._24 + _33 * rhs._34 + _34 * rhs._44;

		FLOAT32 f41 = _41 * rhs._11 + _42 * rhs._21 + _43 * rhs._31 + _44 * rhs._41;
		FLOAT32 f42 = _41 * rhs._12 + _42 * rhs._22 + _43 * rhs._32 + _44 * rhs._42;
		FLOAT32 f43 = _41 * rhs._13 + _42 * rhs._23 + _43 * rhs._33 + _44 * rhs._43;
		FLOAT32 f44 = _41 * rhs._14 + _42 * rhs._24 + _43 * rhs._34 + _44 * rhs._44;

		_11 = f11; _12 = f12; _13 = f13; _14 = f14;
		_21 = f21; _22 = f22; _23 = f23; _24 = f24;
		_31 = f31; _32 = f32; _33 = f33; _34 = f34;
		_41 = f41; _42 = f42; _43 = f43; _44 = f44;

		return *this;
	}

	Matrix4x4 Matrix4x4::operator*(const Matrix4x4& rkMat) const
	{
		FLOAT32 f11 = _11 * rkMat._11 + _12 * rkMat._21 + _13 * rkMat._31 + _14 * rkMat._41;
		FLOAT32 f12 = _11 * rkMat._12 + _12 * rkMat._22 + _13 * rkMat._32 + _14 * rkMat._42;
		FLOAT32 f13 = _11 * rkMat._13 + _12 * rkMat._23 + _13 * rkMat._33 + _14 * rkMat._43;
		FLOAT32 f14 = _11 * rkMat._14 + _12 * rkMat._24 + _13 * rkMat._34 + _14 * rkMat._44;

		FLOAT32 f21 = _21 * rkMat._11 + _22 * rkMat._21 + _23 * rkMat._31 + _24 * rkMat._41;
		FLOAT32 f22 = _21 * rkMat._12 + _22 * rkMat._22 + _23 * rkMat._32 + _24 * rkMat._42;
		FLOAT32 f23 = _21 * rkMat._13 + _22 * rkMat._23 + _23 * rkMat._33 + _24 * rkMat._43;
		FLOAT32 f24 = _21 * rkMat._14 + _22 * rkMat._24 + _23 * rkMat._34 + _24 * rkMat._44;

		FLOAT32 f31 = _31 * rkMat._11 + _32 * rkMat._21 + _33 * rkMat._31 + _34 * rkMat._41;
		FLOAT32 f32 = _31 * rkMat._12 + _32 * rkMat._22 + _33 * rkMat._32 + _34 * rkMat._42;
		FLOAT32 f33 = _31 * rkMat._13 + _32 * rkMat._23 + _33 * rkMat._33 + _34 * rkMat._43;
		FLOAT32 f34 = _31 * rkMat._14 + _32 * rkMat._24 + _33 * rkMat._34 + _34 * rkMat._44;

		FLOAT32 f41 = _41 * rkMat._11 + _42 * rkMat._21 + _43 * rkMat._31 + _44 * rkMat._41;
		FLOAT32 f42 = _41 * rkMat._12 + _42 * rkMat._22 + _43 * rkMat._32 + _44 * rkMat._42;
		FLOAT32 f43 = _41 * rkMat._13 + _42 * rkMat._23 + _43 * rkMat._33 + _44 * rkMat._43;
		FLOAT32 f44 = _41 * rkMat._14 + _42 * rkMat._24 + _43 * rkMat._34 + _44 * rkMat._44;

		return Matrix4x4(f11, f12, f13, f14, 
						 f21, f22, f23, f24, 
						 f31, f32, f33, f34, 
						 f41, f42, f43, f44);
	}
}