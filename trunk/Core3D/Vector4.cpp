#include "Vector4.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"

namespace Core3D
{
	Vector4::Vector4(const Vector2& rkVal)
		: x(rkVal.x), y(rkVal.y), z(0.0f), w(0.0f)
	{

	}

	Vector4::Vector4(const Vector3& rkVal)
		: x(rkVal.x), y(rkVal.y), z(rkVal.z), w(0.0f)
	{

	}

	const Vector4& Vector4::operator=(const Vector2& rkVal)
	{
		x = rkVal.x; y = rkVal.y; z = 0.0f; w = 0.0f;
		return *this;
	}

	const Vector4& Vector4::operator=(const Vector3& rkVal)
	{
		x = rkVal.x; y = rkVal.y; z = rkVal.z; w = 0.0f;
		return *this;
	}

	const Vector4& Vector4::operator*=(const Matrix4x4& rkMat)
	{
		FLOAT32 fX = rkMat._11 * x + rkMat._21 * y + rkMat._31 * z + rkMat._41 * w;
		FLOAT32 fY = rkMat._12 * x + rkMat._22 * y + rkMat._32 * z + rkMat._42 * w;
		FLOAT32 fZ = rkMat._13 * x + rkMat._23 * y + rkMat._33 * z + rkMat._43 * w;
		FLOAT32 fW = rkMat._14 * x + rkMat._24 * y + rkMat._34 * z + rkMat._44 * w;
		x = fX; y = fY; z = fZ; w = fW;
		return *this;
	}

	Vector4 Vector4::operator*(const Matrix4x4& rkMat) const
	{
		FLOAT32 fX = rkMat._11 * x + rkMat._21 * y + rkMat._31 * z + rkMat._41 * w;
		FLOAT32 fY = rkMat._12 * x + rkMat._22 * y + rkMat._32 * z + rkMat._42 * w;
		FLOAT32 fZ = rkMat._13 * x + rkMat._23 * y + rkMat._33 * z + rkMat._43 * w;
		FLOAT32 fW = rkMat._14 * x + rkMat._24 * y + rkMat._34 * z + rkMat._44 * w;
		return Vector4(fX, fY, fZ, fW);
	}
}