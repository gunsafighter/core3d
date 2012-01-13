#include "Core3DMath.h"

namespace Core3D
{
	//-----------------------------------------------------------
	// VECTOR2 Functions
	//-----------------------------------------------------------
	FLOAT32 Vec2Dot(const Vector2& rkVecA, const Vector2& rkVecB)
	{
		return rkVecA.x * rkVecB.x + rkVecA.y * rkVecB.y;
	}

	Vector2& Vec2Lerp(Vector2& rkOut, const Vector2& rkVecA, const Vector2& rkVecB, FLOAT32 fInterpolation)
	{
		rkOut.x = rkVecA.x + (rkVecB.x - rkVecA.x) * fInterpolation;
		rkOut.y = rkVecA.y + (rkVecB.y - rkVecA.y) * fInterpolation;
		return rkOut;
	}
	//-----------------------------------------------------------

	//-----------------------------------------------------------
	// VECTOR3 Functions
	//-----------------------------------------------------------
	Vector3& Vec3Cross(Vector3& rkOut, const Vector3& rkVecA, const Vector3& rkVecB)
	{
		rkOut.x = rkVecA.y * rkVecB.z - rkVecA.z * rkVecB.y;
		rkOut.y = rkVecA.z * rkVecB.x - rkVecA.x * rkVecB.z;
		rkOut.z = rkVecB.x * rkVecB.y - rkVecA.y * rkVecB.x;
		return rkOut;
	}

	FLOAT32 Vec3Dot(const Vector3& rkVecA, const Vector3& rkVecB)
	{
		return rkVecA.x * rkVecB.x + rkVecA.y * rkVecB.y + rkVecA.z * rkVecB.z;
	}

	Vector3& Vec3Lerp(Vector3& rkOut, const Vector3& rkVecA, const Vector3& rkVecB, FLOAT32 fInterpolation)
	{
		rkOut.x = rkVecA.x + (rkVecB.x - rkVecA.x) * fInterpolation;
		rkOut.y = rkVecA.y + (rkVecB.y - rkVecA.y) * fInterpolation;
		rkOut.z = rkVecA.z + (rkVecB.z - rkVecA.z) * fInterpolation;
		return rkOut;
	}

	Vector3& Vec3TransformNormal(Vector3& rkOut, const Vector3& rkVec, const Matrix4x4& rkMat)
	{
		Vector4 kVector = Vector4(rkVec.x, rkVec.y, rkVec.z, 0.0f) * rkMat;
		rkOut			= Vector3(kVector.x, kVector.y, kVector.z);
		return rkOut;
	}
	//-----------------------------------------------------------

	//-----------------------------------------------------------
	// VECTOR4 Functions
	//-----------------------------------------------------------
	FLOAT32 Vec4Dot(const Vector4& rkVecA, const Vector4& rkVecB)
	{
		return rkVecA.x * rkVecB.x + rkVecA.y * rkVecB.y + rkVecA.z * rkVecB.z + rkVecA.w * rkVecB.w;
	}

	Vector4& Vec4Lerp(Vector4& rkOut, const Vector4& rkVecA, const Vector4& rkVecB, FLOAT32 fInterpolation)
	{
		rkOut.x = rkVecA.x + (rkVecB.x - rkVecA.x) * fInterpolation;
		rkOut.y = rkVecA.y + (rkVecB.y - rkVecA.y) * fInterpolation;
		rkOut.z = rkVecA.z + (rkVecB.z - rkVecA.z) * fInterpolation;
		rkOut.w = rkVecA.w + (rkVecB.w - rkVecA.w) * fInterpolation;
		return rkOut;
	}
	//-----------------------------------------------------------

	//-----------------------------------------------------------
	// MATRIX4X4 Functions
	//-----------------------------------------------------------
	Matrix4x4& MatrixTranspose(Matrix4x4& rkOut, const Matrix4x4& rkMat)
	{
		rkOut._11 = rkMat._11; rkOut._12 = rkMat._21; rkOut._13 = rkMat._31; rkOut._14 = rkMat._41;
		rkOut._21 = rkMat._12; rkOut._22 = rkMat._22; rkOut._23 = rkMat._32; rkOut._24 = rkMat._42;
		rkOut._31 = rkMat._13; rkOut._32 = rkMat._23; rkOut._33 = rkMat._33; rkOut._34 = rkMat._43;
		rkOut._41 = rkMat._14; rkOut._42 = rkMat._24; rkOut._43 = rkMat._34; rkOut._44 = rkMat._44;
		return rkOut;
	}

	Matrix4x4& MatrixIdentity(Matrix4x4& rkOut)
	{
		rkOut._11 = 1.0f; rkOut._12 = 0.0f; rkOut._13 = 0.0f; rkOut._14 = 0.0f;
		rkOut._21 = 0.0f; rkOut._22 = 1.0f; rkOut._23 = 0.0f; rkOut._24 = 0.0f;
		rkOut._31 = 0.0f; rkOut._32 = 0.0f; rkOut._33 = 1.0f; rkOut._34 = 0.0f;
		rkOut._41 = 0.0f; rkOut._42 = 0.0f; rkOut._43 = 0.0f; rkOut._44 = 1.0f;
		return rkOut;
	}

	Matrix4x4& MatrixScaling(Matrix4x4& rkOut, FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ)
	{
		MatrixIdentity(rkOut);
		rkOut._11 = fX; rkOut._22 = fY; rkOut._33 = fZ;
		return rkOut;
	}

	Matrix4x4& MatrixScaling(Matrix4x4& rkOut, const Vector3& rkScale)
	{
		return MatrixScaling(rkOut, rkScale.x, rkScale.y, rkScale.z);
	}

	Matrix4x4& MatrixTranslation(Matrix4x4& rkOut, FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ)
	{
		MatrixIdentity(rkOut);
		rkOut._41 = fX; rkOut._42 = fY; rkOut._43 = fZ;
		return rkOut;
	}

	Matrix4x4& MatrixTranslation(Matrix4x4& rkOut, const Vector3& rkTrans)
	{
		return MatrixTranslation(rkOut, rkTrans.x, rkTrans.y, rkTrans.z);
	}

	Matrix4x4& MatrixRotationX(Matrix4x4& rkOut, FLOAT32 fRot)
	{
		FLOAT32 fSin = sinf(fRot);
		FLOAT32 fCos = cosf(fRot);
		
		MatrixIdentity(rkOut);
		rkOut._22 = fCos;  rkOut._23 = fSin;
		rkOut._32 = -fSin; rkOut._33 = fCos;
		return rkOut;
	}

	Matrix4x4& MatrixRotationY(Matrix4x4& rkOut, FLOAT32 fRot)
	{
		FLOAT32 fSin = sinf(fRot);
		FLOAT32 fCos = cosf(fRot);

		MatrixIdentity(rkOut);
		rkOut._11 = fCos; rkOut._13 = -fSin;
		rkOut._31 = fSin; rkOut._33 = fCos;
		return rkOut;
	}

	Matrix4x4& MatrixRotationZ(Matrix4x4& rkOut, FLOAT32 fRot)
	{
		FLOAT32 fSin = sinf(fRot);
		FLOAT32 fCos = cosf(fRot);

		MatrixIdentity(rkOut);
		rkOut._11 = fCos;  rkOut._12 = fSin;
		rkOut._21 = -fSin; rkOut._22 = fCos;
		return rkOut;
	}

	Matrix4x4& MatrixRotationYawPitchRoll(Matrix4x4& rkOut, FLOAT32 fYaw, FLOAT32 fPitch, FLOAT32 fRoll)
	{
		Matrix4x4 kMatYaw, kMatPitch, kMatRoll;
		MatrixRotationY(kMatYaw, fYaw);
		MatrixRotationX(kMatPitch, fPitch);
		MatrixRotationZ(kMatRoll, fRoll);
		rkOut = kMatRoll * kMatPitch * kMatYaw;
		return rkOut;
	}

	Matrix4x4& MatrixRotationYawPitchRoll(Matrix4x4& rkOut, const Vector3& rkRot)
	{
		return MatrixRotationYawPitchRoll(rkOut, rkRot.x, rkRot.y, rkRot.z);
	}

	Matrix4x4& MatrixRotationAxis(Matrix4x4& rkOut, const Vector3& rkAxis, FLOAT32 fRot)
	{
		// COMMENT : http://www.euclideanspace.com/maths/algebra/matrix/orthogonal/rotation/openforum.htm
		FLOAT32 fSin	= sinf(fRot);
		FLOAT32 fCos	= cosf(fRot);
		FLOAT32 fInvCos = 1.0f - fCos;

		rkOut._11 = fInvCos * rkAxis.x * rkAxis.x + fCos;
		rkOut._12 = fInvCos * rkAxis.x * rkAxis.y - rkAxis.z * fSin;
		rkOut._13 = fInvCos * rkAxis.x * rkAxis.z + rkAxis.y * fSin;
		rkOut._14 = 0.0f;

		rkOut._21 = fInvCos * rkAxis.x * rkAxis.y + rkAxis.z * fSin;
		rkOut._22 = fInvCos * rkAxis.y * rkAxis.y + fCos;
		rkOut._23 = fInvCos * rkAxis.y * rkAxis.z - rkAxis.x * fSin;
		rkOut._24 = 0.0f;

		rkOut._31 = fInvCos * rkAxis.x * rkAxis.z - rkAxis.y * fSin;
		rkOut._32 = fInvCos * rkAxis.y * rkAxis.z + rkAxis.x * fSin;
		rkOut._33 = fInvCos * rkAxis.z * rkAxis.z + fCos;
		rkOut._34 = 0.0f;

		rkOut._41 = 0.0f; rkOut._42 = 0.0f; rkOut._43 = 0.0f; rkOut._44 = 1.0f;
		return rkOut;
	}

	Matrix4x4& MatrixRotationQuaternion(Matrix4x4& rkOut, const Quaternion& rkQuat)
	{
		FLOAT32 fSquaredX = rkQuat.x * rkQuat.x;
		FLOAT32 fSquaredY = rkQuat.y * rkQuat.y;
		FLOAT32 fSquaredZ = rkQuat.z * rkQuat.z;
		FLOAT32 fSquaredW = rkQuat.w * rkQuat.w;

		rkOut._11 = fSquaredW + fSquaredX - fSquaredY - fSquaredZ;
		rkOut._12 = 2.0f * (rkQuat.x * rkQuat.y - rkQuat.w * rkQuat.z);
		rkOut._13 = 2.0f * (rkQuat.x * rkQuat.z + rkQuat.w * rkQuat.y);
		rkOut._14 = 0.0f;

		rkOut._21 = 2.0f * (rkQuat.x * rkQuat.y + rkQuat.w * rkQuat.z);
		rkOut._22 = fSquaredW - fSquaredX + fSquaredY - fSquaredZ;
		rkOut._23 = 2.0f * (rkQuat.y * rkQuat.z - rkQuat.w * rkQuat.x);
		rkOut._24 = 0.0f;

		rkOut._31 = 2.0f * (rkQuat.x * rkQuat.z - rkQuat.w * rkQuat.y);
		rkOut._32 = 2.0f * (rkQuat.y * rkQuat.z + rkQuat.w * rkQuat.x);
		rkOut._33 = fSquaredW - fSquaredX - fSquaredY + fSquaredZ;
		rkOut._34 = 0.0f;

		rkOut._41 = 0.0f; rkOut._42 = 0.0f; rkOut._43 = 0.0f;
		rkOut._44 = fSquaredW + fSquaredX + fSquaredY + fSquaredZ;
		return rkOut;
	}

	Matrix4x4& MatrixLookAtLH(Matrix4x4& rkOut, const Vector3& rkEye, const Vector3& rkAt, const Vector3& rkUp)
	{
		Vector3 kZAxis = rkAt - rkEye;
		kZAxis.Normalize();

		Vector3 kXAxis, kYAxis;
		Vec3Cross(kXAxis, rkUp, kZAxis);
		kXAxis.Normalize();
		Vec3Cross(kYAxis, kZAxis, kXAxis);
		kYAxis.Normalize();

		MatrixIdentity(rkOut);
		rkOut._11 = kXAxis.x; rkOut._12 = kYAxis.x; rkOut._13 = kZAxis.x;
		rkOut._21 = kXAxis.y; rkOut._22 = kYAxis.y; rkOut._23 = kZAxis.y;
		rkOut._31 = kXAxis.z; rkOut._32 = kYAxis.z; rkOut._33 = kZAxis.z;
		rkOut._41 = -Vec3Dot(kXAxis, rkEye); rkOut._42 = -Vec3Dot(kYAxis, rkEye); rkOut._43 = -Vec3Dot(kZAxis, rkEye);
		return rkOut;
	}

	Matrix4x4& MatrixLookAtRH(Matrix4x4& rkOut, const Vector3& rkEye, const Vector3& rkAt, const Vector3& rkUp)
	{
		Vector3 kZAxis = rkEye - rkAt;
		kZAxis.Normalize();

		Vector3 kXAxis, kYAxis;
		Vec3Cross(kXAxis, rkUp, kZAxis);
		kXAxis.Normalize();
		Vec3Cross(kYAxis, kZAxis, kXAxis);
		kYAxis.Normalize();

		MatrixIdentity(rkOut);
		rkOut._11 = kXAxis.x; rkOut._12 = kYAxis.x; rkOut._13 = kZAxis.x;
		rkOut._21 = kXAxis.y; rkOut._22 = kYAxis.y; rkOut._23 = kZAxis.y;
		rkOut._31 = kXAxis.z; rkOut._32 = kYAxis.z; rkOut._33 = kZAxis.z;
		rkOut._41 = -Vec3Dot(kXAxis, rkEye); rkOut._42 = -Vec3Dot(kYAxis, rkEye); rkOut._43 = -Vec3Dot(kZAxis, rkEye);
		return rkOut;
	}

	Matrix4x4& MatrixOrthoLH(Matrix4x4& rkOut, FLOAT32 fWidth, FLOAT32 fHeight, FLOAT32 fZNear, FLOAT32 fZFar)
	{
		return MatrixOrthoOffCenterLH(rkOut, -fWidth * 0.5f, fWidth * 0.5f, -fHeight * 0.5f, fHeight * 0.5f, fZNear, fZFar);
	}

	Matrix4x4& MatrixOrthoRH(Matrix4x4& rkOut, FLOAT32 fWidth, FLOAT32 fHeight, FLOAT32 fZNear, FLOAT32 fZFar)
	{
		return MatrixOrthoOffCenterRH(rkOut, -fWidth * 0.5f, fWidth * 0.5f, -fHeight * 0.5f, fHeight * 0.5f, fZNear, fZFar);
	}

	Matrix4x4& MatrixOrthoOffCenterLH(Matrix4x4& rkOut, FLOAT32 fLeft, FLOAT32 fRight, FLOAT32 fBottom, FLOAT32 fTop, FLOAT32 fZNear, FLOAT32 fZFar)
	{
		MatrixIdentity(rkOut);
		rkOut._11 = 2.0f / (fRight - fLeft);
		rkOut._22 = 2.0f / (fTop - fBottom);
		rkOut._33 = 1.0f / (fZFar - fZNear);
		rkOut._41 = (fLeft + fRight) / (fLeft - fRight); rkOut._42 = (fBottom + fTop) / (fBottom - fTop); rkOut._43 = fZNear / (fZNear - fZFar);
		return rkOut;
	}

	Matrix4x4& MatrixOrthoOffCenterRH(Matrix4x4& rkOut, FLOAT32 fLeft, FLOAT32 fRight, FLOAT32 fBottom, FLOAT32 fTop, FLOAT32 fZNear, FLOAT32 fZFar)
	{
		MatrixIdentity(rkOut);
		rkOut._11 = 2.0f / (fRight - fLeft);
		rkOut._22 = 2.0f / (fTop - fBottom);
		rkOut._33 = 1.0f / (fZNear - fZFar);
		rkOut._41 = (fLeft + fRight) / (fLeft - fRight); rkOut._42 = (fBottom + fTop) / (fBottom - fTop); rkOut._43 = fZNear / (fZNear - fZFar);
		return rkOut;
	}

	Matrix4x4& MatrixPerspectiveFovLH(Matrix4x4& rkOut, FLOAT32 fFovY, FLOAT32 fAspect, FLOAT32 fZNear, FLOAT32 fZFar)
	{
		FLOAT32 fViewSpaceHeight	= 1.0f / tanf(fFovY * 0.5f);
		FLOAT32 fViewSpaceWidth		= fViewSpaceHeight / fAspect;

		MatrixIdentity(rkOut);
		rkOut._11 = fViewSpaceWidth;
		rkOut._22 = fViewSpaceHeight;
		rkOut._33 = fZFar / (fZFar - fZNear);			rkOut._34 = 1.0f;
		rkOut._43 = -fZNear * fZFar / (fZFar - fZNear); rkOut._44 = 0.0f;
		return rkOut;
	}

	Matrix4x4& MatrixPerspectiveFovRH(Matrix4x4& rkOut, FLOAT32 fFovY, FLOAT32 fAspect, FLOAT32 fZNear, FLOAT32 fZFar)
	{
		FLOAT32 fViewSpaceHeight	= 1.0f / tanf(fFovY * 0.5f);
		FLOAT32 fViewSpaceWidth		= fViewSpaceHeight / fAspect;

		MatrixIdentity(rkOut);
		rkOut._11 = fViewSpaceWidth;
		rkOut._22 = fViewSpaceHeight;
		rkOut._33 = fZFar / (fZNear - fZFar);			rkOut._34 = -1.0f;
		rkOut._43 = fZNear * fZFar / (fZNear - fZFar);	rkOut._44 = 0.0f;
		return rkOut;
	}

	Matrix4x4& MatrixPerspectiveLH(Matrix4x4& rkOut, FLOAT32 fWidth, FLOAT32 fHeight, FLOAT32 fZNear, FLOAT32 fZFar)
	{
		MatrixIdentity(rkOut);
		rkOut._11 = 2.0f * fZNear / fWidth;
		rkOut._22 = 2.0f * fZNear / fHeight;
		rkOut._33 = fZFar / (fZFar - fZNear);			rkOut._34 = 1.0f;
		rkOut._43 = -fZNear * fZFar / (fZFar - fZNear);	rkOut._44 = 0.0f;
		return rkOut;
	}

	Matrix4x4& MatrixPerspectiveRH(Matrix4x4& rkOut, FLOAT32 fWidth, FLOAT32 fHeight, FLOAT32 fZNear, FLOAT32 fZFar)
	{
		MatrixIdentity(rkOut);
		rkOut._11 = 2.0f * fZNear / fWidth;
		rkOut._22 = 2.0f * fZNear / fHeight;
		rkOut._33 = fZFar / (fZNear - fZFar);			rkOut._34 = -1.0f;
		rkOut._43 = fZNear * fZFar / (fZNear - fZFar);	rkOut._44 = 0.0f;
		return rkOut;
	}

	Matrix4x4& MatrixPerspectiveOffCenterLH(Matrix4x4& rkOut, FLOAT32 fLeft, FLOAT32 fRight, FLOAT32 fBottom, FLOAT32 fTop, FLOAT32 fZNear, FLOAT32 fZFar)
	{
		MatrixIdentity(rkOut);
		rkOut._11 = 2.0f * fZNear / (fRight - fLeft);
		rkOut._22 = 2.0f * fZNear / (fTop - fBottom);
		rkOut._31 = (fLeft + fRight) / (fLeft - fRight); rkOut._32 = (fBottom + fTop) / (fBottom - fTop); rkOut._33 = fZFar / (fZFar - fZNear); rkOut._34 = 1.0f;
		rkOut._43 = -fZNear * fZFar / (fZFar - fZNear);	 rkOut._44 = 0.0f;
		return rkOut;
	}

	Matrix4x4& MatrixPerspectiveOffCenterRH(Matrix4x4& rkOut, FLOAT32 fLeft, FLOAT32 fRight, FLOAT32 fBottom, FLOAT32 fTop, FLOAT32 fZNear, FLOAT32 fZFar)
	{
		MatrixIdentity(rkOut);
		rkOut._11 = 2.0f * fZNear / (fRight - fLeft);
		rkOut._22 = 2.0f * fZNear / (fTop - fBottom);
		rkOut._31 = (fLeft + fRight) / (fLeft - fRight); rkOut._32 = (fBottom + fTop) / (fBottom - fTop); rkOut._33 = fZFar / (fZNear - fZFar); rkOut._34 = -1.0f;
		rkOut._43 = fZNear * fZFar / (fZNear - fZFar);   rkOut._44 = 0.0f;
		return rkOut;
	}

	Matrix4x4& MatrixViewport(Matrix4x4& rkOut, UINT32 uiX, UINT32 uiY, UINT32 uiWidth, UINT32 uiHeight, FLOAT32 fZNear, FLOAT32 fZFar)
	{
		MatrixIdentity(rkOut);
		rkOut._11 = static_cast<FLOAT32>(uiWidth) * 0.5f;
		rkOut._22 = static_cast<FLOAT32>(uiHeight) * -0.5f;
		rkOut._33 = fZFar - fZNear;
		
		rkOut._41 = static_cast<FLOAT32>(uiX) + static_cast<FLOAT32>(uiWidth) * 0.5f;
		rkOut._42 = static_cast<FLOAT32>(uiY) + static_cast<FLOAT32>(uiHeight) * 0.5f;
		rkOut._43 = fZNear;
		return rkOut;
	}

	Matrix4x4& MatrixInvert(Matrix4x4& rkOut, const Matrix4x4& rkMat)
	{
		rkOut = -rkMat;
		return rkOut;
	}
	//-----------------------------------------------------------

	//-----------------------------------------------------------
	// QUATERNION Functions
	//-----------------------------------------------------------
	Quaternion& QuaternionIdentity(Quaternion& rkOut)
	{
		rkOut.x = rkOut.y = rkOut.z = 0.0f;
		rkOut.w = 1.0f;
		return rkOut;
	}

	Quaternion& QuaternionRotationMatrix(Quaternion& rkOut, const Matrix4x4& rkMat)
	{
		// COMMENT : http://www.gamasutra.com/features/19980703/quaternions_01.htm
		const FLOAT32 fDiagonal = rkMat._11 + rkMat._22 + rkMat._33;
		if(fDiagonal > 0.0f)
		{
			FLOAT32 fSqrt	= sqrtf(fDiagonal + 1.0f);
			rkOut.w			= fSqrt / 2.0f;
			fSqrt			= 0.5f / fSqrt;

			rkOut.x = (rkMat._32 - rkMat._23) * fSqrt;
			rkOut.y = (rkMat._13 - rkMat._31) * fSqrt;
			rkOut.z = (rkMat._21 - rkMat._12) * fSqrt;
		}
		else
		{
			const UINT32 NEXT[3] = {1, 2, 0};
			FLOAT32 fQuat[4];
			UINT32 i = 0;

			if(rkMat._22 > rkMat._11)	{i = 1;}
			if(rkMat._33 > rkMat(i, i)) {i = 2;}
			UINT32 j = NEXT[i];
			UINT32 k = NEXT[j];

			FLOAT32 fSqrt	= sqrtf(rkMat(i, i) - (rkMat(j, j) + rkMat(k, k)) + 1.0f);
			fQuat[i]		= fSqrt * 0.5f;

			if(FLT_EPSILON <= fSqrt) {fSqrt = 0.5f / fSqrt;}

			fQuat[3] = (rkMat(k, j) - rkMat(j, k)) * fSqrt;
			fQuat[j] = (rkMat(j, i) + rkMat(i, j)) * fSqrt;
			fQuat[k] = (rkMat(k, i) + rkMat(i, k)) * fSqrt;

			rkOut.x = fQuat[0];
			rkOut.y = fQuat[1];
			rkOut.z = fQuat[2];
			rkOut.w = fQuat[3];
		}
		return rkOut;
	}

	Quaternion& QuaternionSLerp(Quaternion& rkOut, const Quaternion& rkQuatA, const Quaternion& rkQuatB, FLOAT32 fLerp)
	{
		// COMMENT : http://www.gamasutra.com/features/19980703/quaternions_01.htm
		FLOAT32 fCos = rkQuatA.x * rkQuatB.x + rkQuatA.y * rkQuatB.y + rkQuatA.z * rkQuatB.z + rkQuatA.w * rkQuatB.w;
		FLOAT32 afTo1[4];
		if(fCos < 0.0f)
		{
			fCos		= -fCos;
			afTo1[0]	= -rkQuatB.x;
			afTo1[1]	= -rkQuatB.y;
			afTo1[2]	= -rkQuatB.z;
			afTo1[3]	= -rkQuatB.w;
		}
		else
		{
			afTo1[0]	= rkQuatB.x;
			afTo1[1]	= rkQuatB.y;
			afTo1[2]	= rkQuatB.z;
			afTo1[3]	= rkQuatB.w;
		}

		const FLOAT32 OMEGA		= acosf(fCos);
		const FLOAT32 INVSIN	= 1.0f / sinf(OMEGA);
		const FLOAT32 SCALE0	= sinf((1.0f - fLerp) * OMEGA) * INVSIN;
		const FLOAT32 SCALE1	= sinf(fLerp * OMEGA) * INVSIN;

		rkOut.x = SCALE0 * rkQuatB.x + SCALE1 * afTo1[0];
		rkOut.y = SCALE0 * rkQuatB.y + SCALE1 * afTo1[1];
		rkOut.z = SCALE0 * rkQuatB.z + SCALE1 * afTo1[2];
		rkOut.w = SCALE0 * rkQuatB.w + SCALE1 * afTo1[3];

		return rkOut;
	}

	void QuaternionToAxisAngle(const Quaternion& rkQuat, Vector3& rkAxis, FLOAT32& rfAngle)
	{
		const FLOAT32 SCALE = sqrtf(rkQuat.x * rkQuat.x + rkQuat.y * rkQuat.y + rkQuat.z * rkQuat.z);
		if(FLT_EPSILON <= SCALE)
		{
			FLOAT32 fInv = 1.0f / SCALE;
			rkAxis.x = rkQuat.x * fInv;
			rkAxis.y = rkQuat.y * fInv;
			rkAxis.z = rkQuat.z * fInv;
		}
		else
		{
			rkAxis.x = 0.0f;
			rkAxis.y = 1.0f;
			rkAxis.z = 0.0f;
		}
		rfAngle = 2.0f * acosf(rkQuat.w);
	}
	//-----------------------------------------------------------
}