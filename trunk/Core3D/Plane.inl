
namespace Core3D
{
	inline Plane::Plane()
	{

	}

	inline Plane::Plane(const Plane& rhs)
		: kNormal(rhs.kNormal), d(rhs.d)
	{

	}

	inline Plane::Plane(FLOAT32 fA, FLOAT32 fB, FLOAT32 fC, FLOAT32 fD)
		: d(fD)
	{
		kNormal = Vector3(fA, fB, fC);
	}

	inline Plane::Plane(const Vector3& rkNormal, FLOAT32 fD)
		: kNormal(rkNormal), d(fD)
	{

	}

	inline const Plane& Plane::operator=(const Plane& rhs)
	{
		kNormal = rhs.kNormal;
		d		= rhs.d;
		return *this;
	}

	inline Plane::operator FLOAT32*()
	{
		return &kNormal.x;
	}

	inline Plane::operator const FLOAT32*() const
	{
		return &kNormal.x;
	}

	inline Plane Plane::operator+() const
	{
		return *this;
	}

	inline Plane Plane::operator-() const
	{
		return Plane(-kNormal, d);
	}
}