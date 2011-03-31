#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

namespace Core3D
{
	Vector2::Vector2(const Vector3& rkVal)
		: x(rkVal.x), y(rkVal.y)
	{

	}

	Vector2::Vector2(const Vector4& rkVal)
		: x(rkVal.x), y(rkVal.y)
	{

	}

	const Vector2& Vector2::operator=(const Vector3& rkVal)
	{
		x = rkVal.x; y = rkVal.y;
		return *this;
	}

	const Vector2& Vector2::operator=(const Vector4& rkVal)
	{
		x = rkVal.x; y = rkVal.y;
		return *this;
	}
}