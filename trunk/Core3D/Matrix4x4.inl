
namespace Core3D
{
	inline Matrix4x4::Matrix4x4()
	{

	}

	inline Matrix4x4::Matrix4x4(const Matrix4x4& rhs)
	{
		_11 = rhs._11; _12 = rhs._12; _13 = rhs._13; _14 = rhs._14;
		_21 = rhs._21; _22 = rhs._22; _23 = rhs._23; _24 = rhs._24;
		_31 = rhs._31; _32 = rhs._32; _33 = rhs._33; _34 = rhs._34;
		_41 = rhs._41; _42 = rhs._42; _43 = rhs._43; _44 = rhs._44;
	}

	inline Matrix4x4::Matrix4x4(
		FLOAT32 f11, FLOAT32 f12, FLOAT32 f13, FLOAT32 f14, 
		FLOAT32 f21, FLOAT32 f22, FLOAT32 f23, FLOAT32 f24, 
		FLOAT32 f31, FLOAT32 f32, FLOAT32 f33, FLOAT32 f34, 
		FLOAT32 f41, FLOAT32 f42, FLOAT32 f43, FLOAT32 f44)
	{
		_11 = f11; _12 = f12; _13 = f13; _14 = f14;
		_21 = f21; _22 = f22; _23 = f23; _24 = f24;
		_31 = f31; _32 = f32; _33 = f33; _34 = f34;
		_41 = f41; _42 = f42; _43 = f43; _44 = f44;
	}

	inline const Matrix4x4& Matrix4x4::operator=(const Matrix4x4& rhs)
	{
		_11 = rhs._11; _12 = rhs._12; _13 = rhs._13; _14 = rhs._14;
		_21 = rhs._21; _22 = rhs._22; _23 = rhs._23; _24 = rhs._24;
		_31 = rhs._31; _32 = rhs._32; _33 = rhs._33; _34 = rhs._34;
		_41 = rhs._41; _42 = rhs._42; _43 = rhs._43; _44 = rhs._44;
		return *this;
	}

	inline Matrix4x4::operator FLOAT32*()
	{
		return &_11;
	}

	inline Matrix4x4::operator const FLOAT32*() const
	{
		return &_11;
	}

	inline FLOAT32 Matrix4x4::operator()(UINT32 uiRow, UINT32 uiCol) const
	{
		return m[uiRow][uiCol];
	}

	inline Matrix4x4 Matrix4x4::operator+() const
	{
		return *this;
	}

	inline const Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& rhs)
	{
		_11 += rhs._11; _12 += rhs._12; _13 += rhs._13; _14 += rhs._14;
		_21 += rhs._21; _22 += rhs._22; _23 += rhs._23; _24 += rhs._24;
		_31 += rhs._31; _32 += rhs._32; _33 += rhs._33; _34 += rhs._34;
		_41 += rhs._41; _42 += rhs._42; _43 += rhs._43; _44 += rhs._44;
		return *this;
	}

	inline const Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& rhs)
	{
		_11 -= rhs._11; _12 -= rhs._12; _13 -= rhs._13; _14 -= rhs._14;
		_21 -= rhs._21; _22 -= rhs._22; _23 -= rhs._23; _24 -= rhs._24;
		_31 -= rhs._31; _32 -= rhs._32; _33 -= rhs._33; _34 -= rhs._34;
		_41 -= rhs._41; _42 -= rhs._42; _43 -= rhs._43; _44 -= rhs._44;
		return *this;
	}

	inline const Matrix4x4& Matrix4x4::operator*=(FLOAT32 fVal)
	{
		_11 *= fVal; _12 *= fVal; _13 *= fVal; _14 *= fVal;
		_21 *= fVal; _22 *= fVal; _23 *= fVal; _24 *= fVal;
		_31 *= fVal; _32 *= fVal; _33 *= fVal; _34 *= fVal;
		_41 *= fVal; _42 *= fVal; _43 *= fVal; _44 *= fVal;
		return *this;
	}

	inline const Matrix4x4& Matrix4x4::operator/=(FLOAT32 fVal)
	{
		const FLOAT32 fInv = 1.0f / fVal;
		_11 *= fInv; _12 *= fInv; _13 *= fInv; _14 *= fInv;
		_21 *= fInv; _22 *= fInv; _23 *= fInv; _24 *= fInv;
		_31 *= fInv; _32 *= fInv; _33 *= fInv; _34 *= fInv;
		_41 *= fInv; _42 *= fInv; _43 *= fInv; _44 *= fInv;
		return *this;
	}

	inline Matrix4x4 Matrix4x4::operator+(const Matrix4x4& rkMat) const
	{
		return Matrix4x4(
			_11 + rkMat._11, _12 + rkMat._12, _13 + rkMat._13, _14 + rkMat._14, 
			_21 + rkMat._21, _22 + rkMat._22, _23 + rkMat._23, _24 + rkMat._24, 
			_31 + rkMat._31, _32 + rkMat._32, _33 + rkMat._33, _34 + rkMat._34, 
			_41 + rkMat._41, _42 + rkMat._42, _43 + rkMat._43, _44 + rkMat._44);
	}

	inline Matrix4x4 Matrix4x4::operator-(const Matrix4x4& rkMat) const
	{
		return Matrix4x4(
			_11 - rkMat._11, _12 - rkMat._12, _13 - rkMat._13, _14 - rkMat._14, 
			_21 - rkMat._21, _22 - rkMat._22, _23 - rkMat._23, _24 - rkMat._24, 
			_31 - rkMat._31, _32 - rkMat._32, _33 - rkMat._33, _34 - rkMat._34, 
			_41 - rkMat._41, _42 - rkMat._42, _43 - rkMat._43, _44 - rkMat._44);
	}

	inline Matrix4x4 Matrix4x4::operator*(FLOAT32 fVal) const
	{
		return Matrix4x4(
			_11 * fVal, _12 * fVal, _13 * fVal, _14 * fVal, 
			_21 * fVal, _22 * fVal, _23 * fVal, _24 * fVal, 
			_31 * fVal, _32 * fVal, _33 * fVal, _34 * fVal, 
			_41 * fVal, _42 * fVal, _43 * fVal, _44 * fVal);
	}

	inline Matrix4x4 Matrix4x4::operator/(FLOAT32 fVal) const
	{
		const FLOAT32 fInv = 1.0f / fVal;
		return Matrix4x4(
			_11 * fInv, _12 * fInv, _13 * fInv, _14 * fInv, 
			_21 * fInv, _22 * fInv, _23 * fInv, _24 * fInv, 
			_31 * fInv, _32 * fInv, _33 * fInv, _34 * fInv, 
			_41 * fInv, _42 * fInv, _43 * fInv, _44 * fInv);
	}
}