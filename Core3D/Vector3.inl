
namespace Core3D
{
	inline Vector3::Vector3()
		: x(0.0f), y(0.0f), z(0.0f)
	{

	}

	inline Vector3::Vector3(const Vector3& rhs)
		: x(rhs.x), y(rhs.y), z(rhs.z)
	{

	}

	inline Vector3::Vector3(FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ)
		: x(fX), y(fY), z(fZ)
	{

	}

	inline const Vector3& Vector3::operator=(const Vector3& rhs)
	{
		x = rhs.x; y = rhs.y; z = rhs.z;
		return *this;
	}

	inline const Vector3& Vector3::operator=(FLOAT32 fX)
	{
		x = fX; y = 0.0f; z = 0.0f;
		return *this;
	}

	inline Vector3::operator FLOAT32*()
	{
		return &x;
	}

	inline Vector3::operator const FLOAT32*() const
	{
		return &x;
	}

	inline Vector3 Vector3::operator+() const
	{
		return *this;
	}

	inline Vector3 Vector3::operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	inline const Vector3& Vector3::operator+=(const Vector3& rhs)
	{
		x += rhs.x; y += rhs.y; z += rhs.z;
		return *this;
	}

	inline const Vector3& Vector3::operator-=(const Vector3& rhs)
	{
		x -= rhs.x; y -= rhs.y; z -= rhs.z;
		return *this;
	}

	inline const Vector3& Vector3::operator*=(const Vector3& rhs)
	{
		x *= rhs.x; y *= rhs.y; z *= rhs.z;
		return *this;
	}

	inline const Vector3& Vector3::operator*=(FLOAT32 fVal)
	{
		x *= fVal; y *= fVal; z *= fVal;
		return *this;
	}

	inline const Vector3& Vector3::operator/=(FLOAT32 fVal)
	{
		const FLOAT32 fInv = 1.0f / fVal;
		x *= fInv; y *= fInv; z *= fInv;
		return *this;
	}

	inline Vector3 Vector3::operator+(const Vector3& rkVal) const
	{
		return Vector3(x + rkVal.x, y + rkVal.y, z + rkVal.z);
	}

	inline Vector3 Vector3::operator-(const Vector3& rkVal) const
	{
		return Vector3(x - rkVal.x, y - rkVal.y, z - rkVal.z);
	}

	inline Vector3 Vector3::operator*(const Vector3& rkVal) const
	{
		return Vector3(x * rkVal.x, y * rkVal.y, z * rkVal.z);
	}

	inline Vector3 Vector3::operator*(FLOAT32 fVal) const
	{
		return Vector3(x * fVal, y * fVal, z * fVal);
	}

	inline Vector3 Vector3::operator/(FLOAT32 fVal) const
	{
		const FLOAT32 fInv = 1.0f / fVal;
		return Vector3(x * fInv, y * fInv, z * fInv);
	}

	inline FLOAT32 Vector3::Length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	inline FLOAT32 Vector3::LengthSq() const
	{
		return x * x + y * y + z * z;
	}

	inline Vector3& Vector3::Normalize()
	{
		const FLOAT32 fLength = Length();
#ifdef _DEBUG
		if(fLength > FLT_EPSILON)
#endif
		{
			const FLOAT32 fInv = 1.0f / fLength;
			x *= fInv; y *= fInv; z *= fInv;
		}
		return *this;
	}

	inline Vector3& Vector3::Clamp(FLOAT32 fLower, FLOAT32 fUpper)
	{
		x = Core3D::Clamp<FLOAT32>(x, fLower, fUpper);
		y = Core3D::Clamp<FLOAT32>(y, fLower, fUpper);
		z = Core3D::Clamp<FLOAT32>(z, fLower, fUpper);
		return *this;
	}

	inline Vector3& Vector3::Saturate()
	{
		return this->Clamp(0.0f, 1.0f);
	}
}