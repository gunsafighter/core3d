
namespace Core3D
{
	inline Vector2::Vector2()
		: x(0.0f), y(0.0f)
	{

	}
	
	inline Vector2::Vector2(const Vector2& rhs)
		: x(rhs.x), y(rhs.y)
	{

	}
	
	inline Vector2::Vector2(FLOAT32 fX, FLOAT32 fY)
		: x(fX), y(fY)
	{

	}

	inline const Vector2& Vector2::operator=(const Vector2& rhs)
	{
		x = rhs.x; y = rhs.y;
		return *this;
	}

	inline const Vector2& Vector2::operator=(FLOAT32 fX)
	{
		x = fX; y = 0.0f;
		return *this;
	}

	inline Vector2::operator FLOAT32*()
	{
		return &x;
	}

	inline Vector2::operator const FLOAT32*() const
	{
		return &x;
	}

	inline Vector2 Vector2::operator+() const
	{
		return *this;
	}

	inline Vector2 Vector2::operator-() const
	{
		return Vector2(-x,-y);
	}

	inline const Vector2& Vector2::operator+=(const Vector2& rhs)
	{
		x += rhs.x; y += rhs.y;
		return *this;
	}

	inline const Vector2& Vector2::operator-=(const Vector2& rhs)
	{
		x -= rhs.x; y -= rhs.y;
		return *this;
	}

	inline const Vector2& Vector2::operator*=(const Vector2& rhs)
	{
		x *= rhs.x; y *= rhs.y;
		return *this;
	}

	inline const Vector2& Vector2::operator*=(FLOAT32 fVal)
	{
		x *= fVal; y *= fVal;
		return *this;
	}

	inline const Vector2& Vector2::operator/=(FLOAT32 fVal)
	{
		const FLOAT32 fInv = 1.0f / fVal;
		x *= fInv; y *= fInv;
		return *this;
	}

	inline Vector2 Vector2::operator+(const Vector2& rkVal) const
	{
		return Vector2(x + rkVal.x, y + rkVal.y);
	}

	inline Vector2 Vector2::operator-(const Vector2& rkVal) const
	{
		return Vector2(x - rkVal.x, y - rkVal.y);
	}

	inline Vector2 Vector2::operator*(const Vector2& rkVal) const
	{
		return Vector2(x * rkVal.x, y * rkVal.y);
	}

	inline Vector2 Vector2::operator*(FLOAT32 fVal) const
	{
		return Vector2(x * fVal, y * fVal);
	}

	inline Vector2 Vector2::operator/(FLOAT32 fVal) const
	{
		const FLOAT32 fInv = 1.0f / fVal;
		return Vector2(x * fInv, y * fInv);
	}

	inline FLOAT32 Vector2::Length() const
	{
		return sqrtf(x * x + y * y);
	}

	inline FLOAT32 Vector2::LengthSq() const
	{
		return x * x + y * y;
	}

	inline Vector2& Vector2::Normalize()
	{
		const FLOAT32 fLength = Length();
#ifdef _DEBUG
		if(fLength > FLT_EPSILON)
#endif
		{
			const FLOAT32 fInv = 1.0f / fLength;
			x *= fInv; y *= fInv;
		}
		return *this;
	}
}