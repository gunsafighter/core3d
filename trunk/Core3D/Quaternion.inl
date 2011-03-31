
namespace Core3D
{
	inline Quaternion::Quaternion()
	{

	}

	inline Quaternion::Quaternion(const Quaternion& rhs)
		: x(rhs.x), y(rhs.y), z(rhs.z), w(rhs.w)
	{

	}

	inline Quaternion::Quaternion(FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ, FLOAT32 fW)
		: x(fX), y(fY), z(fZ), w(fW)
	{

	}

	inline const Quaternion& Quaternion::operator=(const Quaternion& rhs)
	{
		x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;
		return *this;
	}

	inline Quaternion::operator FLOAT32*()
	{
		return &x;
	}

	inline Quaternion::operator const FLOAT32*() const
	{
		return &x;
	}

	inline Quaternion Quaternion::operator+() const
	{
		return *this;
	}

	inline Quaternion Quaternion::operator-() const
	{
		return Quaternion(-x, -y, -z, w);
	}

	inline const Quaternion& Quaternion::operator+=(const Quaternion& rhs)
	{
		x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w;
		return *this;
	}

	inline const Quaternion& Quaternion::operator-=(const Quaternion& rhs)
	{
		x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w;
		return *this;
	}

	inline const Quaternion& Quaternion::operator*=(const Quaternion& rhs)
	{
		Quaternion kResult;
		kResult.x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
		kResult.y = w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x;
		kResult.z = w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w;
		kResult.w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;
		
		x = kResult.x; y = kResult.y; z = kResult.z; w = kResult.w;

		return *this;
	}

	inline const Quaternion& Quaternion::operator*=(FLOAT32 fVal)
	{
		x *= fVal; y *= fVal; z *= fVal; w *= fVal;
		return *this;
	}

	inline const Quaternion& Quaternion::operator/=(FLOAT32 fVal)
	{
		const FLOAT32 fInv = 1.0f / fVal;
		x *= fInv; y *= fInv; z *= fInv; w *= fInv;
		return *this;
	}

	inline Quaternion Quaternion::operator+(const Quaternion& rkQuat) const
	{
		return Quaternion(x + rkQuat.x, y + rkQuat.y, z + rkQuat.z, w + rkQuat.w);
	}

	inline Quaternion Quaternion::operator-(const Quaternion& rkQuat) const
	{
		return Quaternion(x - rkQuat.x, y - rkQuat.y, z - rkQuat.z, w - rkQuat.w);
	}

	inline Quaternion Quaternion::operator*(const Quaternion& rkQuat) const
	{
		Quaternion kResult;
		kResult.x = w * rkQuat.x + x * rkQuat.w + y * rkQuat.z - z * rkQuat.y;
		kResult.y = w * rkQuat.y - x * rkQuat.z + y * rkQuat.w + z * rkQuat.x;
		kResult.z = w * rkQuat.z + x * rkQuat.y - y * rkQuat.x + z * rkQuat.w;
		kResult.w = w * rkQuat.w - x * rkQuat.x - y * rkQuat.y - z * rkQuat.z;
		return kResult;
	}

	inline Quaternion Quaternion::operator*(FLOAT32 fVal) const
	{
		return Quaternion(x * fVal, y * fVal, z * fVal, w * fVal);
	}

	inline Quaternion Quaternion::operator/(FLOAT32 fVal) const
	{
		const FLOAT32 fInv = 1.0f / fVal;
		return Quaternion(x * fInv, y * fInv, z * fInv, w * fInv);
	}

	inline FLOAT32 Quaternion::Length() const
	{
		return sqrtf(x * x + y * y + z * z + w * w);
	}

	inline FLOAT32 Quaternion::LengthSq() const
	{
		return x * x + y * y + z * z + w * w;
	}

	inline Quaternion& Quaternion::Normalize()
	{
		const FLOAT32 fLength = Length();
		if(FLT_EPSILON <= fLength)
		{
			const FLOAT32 fInv = 1.0f / fLength;
			x *= fInv; y *= fInv; z *= fInv; w *= fInv;
		}
		return *this;
	}
}