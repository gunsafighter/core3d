#include "Plane.h"
#include "Core3DMath.h"

namespace Core3D
{
	FLOAT32 Plane::operator*(const Vector3& rkVal) const
	{
		return Vec3Dot(kNormal, rkVal) + d;
	}

	FLOAT32 Plane::operator*(const Vector4& rkVal) const
	{
		return Vec3Dot(kNormal, rkVal) + d * rkVal.w;
	}
}