#include "Vector3.h"
#include "Vector2.h"
#include "Vector4.h"
#include "Matrix4x4.h"

namespace Core3D
{
	Vector3::Vector3(const Vector2& rkVal)
		: x(rkVal.x), y(rkVal.y), z(0.0f)
	{

	}

	Vector3::Vector3(const Vector4& rkVal)
		: x(rkVal.x), y(rkVal.y), z(rkVal.z)
	{

	}

	const Vector3& Vector3::operator=(const Vector2& rkVal)
	{
		x = rkVal.x; y = rkVal.y; z = 0.0f;
		return *this;
	}

	const Vector3& Vector3::operator=(const Vector4& rkVal)
	{
		x = rkVal.x; y = rkVal.y; z = rkVal.z;
		return *this;
	}

	const Vector3& Vector3::operator*=(const Matrix4x4& rkMat)
	{
		*this = (Vector4(x, y, z, 1.0f) * rkMat).Homogenize();
		return *this;
	}

	Vector3 Vector3::operator*(const Matrix4x4& rkMat) const
	{
		return (Vector4(x, y, z, 1.0f) * rkMat).Homogenize();
	}
}