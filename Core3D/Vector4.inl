
namespace Core3D
{
	inline Vector4::Vector4()
		: x(0.0f), y(0.0f), z(0.0f), w(0.0f)
	{

	}

	inline Vector4::Vector4(const Vector4& rhs)
		: x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
	{

	}

	inline Vector4::Vector4(FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ, FLOAT32 fW)
		: x(fX), y(fY), z(fZ), w(fW)
	{

	}

	inline const Vector4& Vector4::operator=(const Vector4& rhs)
	{
		x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;
		return *this;
	}

	inline const Vector4& Vector4::operator=(FLOAT32 fVal)
	{
		x = fVal; y = 0.0f; z = 0.0f; w = 0.0f;
		return *this;
	}

	inline Vector4::operator FLOAT32*()
	{
		return &x;
	}

	inline Vector4::operator const FLOAT32*() const
	{
		return &x;
	}

	inline Vector4 Vector4::operator+() const
	{
		return *this;
	}

	inline Vector4 Vector4::operator-() const
	{
		return Vector4(-x, -y, -z, -w);
	}

	inline const Vector4& Vector4::operator+=(const Vector4& rhs)
	{
		x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w;
		return *this;
	}

	inline const Vector4& Vector4::operator-=(const Vector4& rhs)
	{
		x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w;
		return *this;
	}

	inline const Vector4& Vector4::operator*=(const Vector4& rhs)
	{
		x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w;
		return *this;
	}

	inline const Vector4& Vector4::operator*=(FLOAT32 fVal)
	{
		x *= fVal; y *= fVal; z *= fVal; w *= fVal;
		return *this;
	}

	inline const Vector4& Vector4::operator/=(FLOAT32 fVal)
	{
		const FLOAT32 fInv = 1.0f / fVal;
		x *= fInv; y *= fInv; z *= fInv; w *= fInv;
		return *this;
	}

	inline Vector4 Vector4::operator+(const Vector4& rkVal) const
	{
		return Vector4(x + rkVal.x, y + rkVal.y, z + rkVal.z, w + rkVal.w);
	}

	inline Vector4 Vector4::operator-(const Vector4& rkVal) const
	{
		return Vector4(x - rkVal.x, y - rkVal.y, z - rkVal.z, w - rkVal.w);
	}

	inline Vector4 Vector4::operator*(const Vector4& rkVal) const
	{
		return Vector4(x * rkVal.x, y * rkVal.y, z * rkVal.z, w * rkVal.w);
	}

	inline Vector4 Vector4::operator*(FLOAT32 fVal) const
	{
		return Vector4(x * fVal, y * fVal, z * fVal, w * fVal);
	}

	inline Vector4 Vector4::operator/(FLOAT32 fVal) const
	{
		const FLOAT32 fInv = 1.0f / fVal;
		return Vector4(x * fInv, y * fInv, z * fInv, w * fInv);
	}

	inline FLOAT32 Vector4::Length() const
	{
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	inline FLOAT32 Vector4::LengthSq() const
	{
		return x * x + y * y + z * z + w * w;
	}

	inline Vector4& Vector4::Normailze()
	{
		const FLOAT32 fLength = Length();
#ifdef _DEBUG
		if(fLength > FLT_EPSILON)
#endif
		{
			const FLOAT32 fInv = 1.0f / fLength;
			x *= fInv; y *= fInv; z *= fInv; w *= fInv;
		}
		return *this;
	}

	inline Vector4& Vector4::Homogenize(const FLOAT32 fInvW /* = 0.0f */)
	{
		const FLOAT32 fInv = (0.0f == fInvW) ? (1.0f / w) : fInvW;
		x *= fInv; y *= fInv; z *= fInv; w = 1.0f;

		return *this;
	}

	inline Vector4& Vector4::Clamp(FLOAT32 fLower, FLOAT32 fUpper)
	{
		x = Core3D::Clamp<FLOAT32>(x, fLower, fUpper);
		y = Core3D::Clamp<FLOAT32>(y, fLower, fUpper);
		z = Core3D::Clamp<FLOAT32>(z, fLower, fUpper);
		w = Core3D::Clamp<FLOAT32>(w, fLower, fUpper);
		return *this;
	}

	inline Vector4& Vector4::Saturate()
	{
		return this->Clamp(0.0f, 1.0f);
	}
}