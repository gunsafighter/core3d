#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4x4.h"
#include "Plane.h"

namespace Core3D
{
	FLOAT32		Vec2Dot(const Vector2& rkVecA, const Vector2& rkVecB);
	Vector2&	Vec2Lerp(Vector2& rkOut, const Vector2& rkVecA, const Vector2& rkVecB, FLOAT32 fInterpolation);

	Vector3&	Vec3Cross(Vector3& rkOut, const Vector3& rkVecA, const Vector3& rkVecB);
	FLOAT32		Vec3Dot(const Vector3& rkVecA, const Vector3& rkVecB);
	Vector3&	Vec3Lerp(Vector3& rkOut, const Vector3& rkVecA, const Vector3& rkVecB, FLOAT32 fInterpolation);
	Vector3&	Vec3TransformNormal(Vector3& rkOut, const Vector3& rkVec, const Matrix4x4& rkMat);

	FLOAT32		Vec4Dot(const Vector4& rkVecA, const Vector4& rkVecB);
	Vector4&	Vec4Lerp(Vector4& rkOut, const Vector4& rkVecA, const Vector4& rkVecB, FLOAT32 fInterpolation);

	Matrix4x4&	MatrixTranspose(Matrix4x4& rkOut, const Matrix4x4& rkMat);
	Matrix4x4&	MatrixIdentity(Matrix4x4& rkOut);

	Matrix4x4&	MatrixScaling(Matrix4x4& rkOut, FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ);
	Matrix4x4&	MatrixScaling(Matrix4x4& rkOut, const Vector3& rkScale);

	Matrix4x4&	MatrixTranslation(Matrix4x4& rkOut, FLOAT32 fX, FLOAT32 fY, FLOAT32 fZ);
	Matrix4x4&	MatrixTranslation(Matrix4x4& rkOut, const Vector3& rkTrans);

	Matrix4x4&	MatrixRotationX(Matrix4x4& rkOut, FLOAT32 fRot);
	Matrix4x4&	MatrixRotationY(Matrix4x4& rkOut, FLOAT32 fRot);
	Matrix4x4&	MatrixRotationZ(Matrix4x4& rkOut, FLOAT32 fRot);

	Matrix4x4&	MatrixRotationYawPitchRoll(Matrix4x4& rkOut, FLOAT32 fYaw, FLOAT32 fPitch, FLOAT32 fRoll);
	Matrix4x4&	MatrixRotationYawPitchRoll(Matrix4x4& rkOut, const Vector3& rkRot);

	Matrix4x4&	MatrixRotationAxis(Matrix4x4& rkOut, const Vector3& rkAxis, FLOAT32 fRot);
	Matrix4x4&	MatrixRotationQuaternion(Matrix4x4& rkOut, const Quaternion& rkQuat);

	Matrix4x4&	MatrixLookAtLH(Matrix4x4& rkOut, const Vector3& rkEye, const Vector3& rkAt, const Vector3& rkUp);
	Matrix4x4&	MatrixLookAtRH(Matrix4x4& rkOut, const Vector3& rkEye, const Vector3& rkAt, const Vector3& rkUp);

	Matrix4x4&	MatrixOrthoLH(Matrix4x4& rkOut, FLOAT32 fWidth, FLOAT32 fHeight, FLOAT32 fZNear, FLOAT32 fZFar);
	Matrix4x4&	MatrixOrthoRH(Matrix4x4& rkOut, FLOAT32 fWidth, FLOAT32 fHeight, FLOAT32 fZNear, FLOAT32 fZFar);
	Matrix4x4&	MatrixOrthoOffCenterLH(Matrix4x4& rkOut, FLOAT32 fLeft, FLOAT32 fRight, FLOAT32 fBottom, FLOAT32 fTop, 
		FLOAT32 fZNear, FLOAT32 fZFar);
	Matrix4x4&	MatrixOrthoOffCenterRH(Matrix4x4& rkOut, FLOAT32 fLeft, FLOAT32 fRight, FLOAT32 fBottom, FLOAT32 fTop, 
		FLOAT32 fZNear, FLOAT32 fZFar);

	Matrix4x4&	MatrixPerspectiveFovLH(Matrix4x4& rkOut, FLOAT32 fFovY, FLOAT32 fAspect, FLOAT32 fZNear, FLOAT32 fZFar);
	Matrix4x4&	MatrixPerspectiveFovRH(Matrix4x4& rkOut, FLOAT32 fFovY, FLOAT32 fAspect, FLOAT32 fZNear, FLOAT32 fZFar);
	Matrix4x4&	MatrixPerspectiveLH(Matrix4x4& rkOut, FLOAT32 fWidth, FLOAT32 fHeight, FLOAT32 fZNear, FLOAT32 fZFar);
	Matrix4x4&	MatrixPerspectiveRH(Matrix4x4& rkOut, FLOAT32 fWidth, FLOAT32 fHeight, FLOAT32 fZNear, FLOAT32 fZFar);
	Matrix4x4&	MatrixPerspectiveOffCenterLH(Matrix4x4& rkOut, FLOAT32 fLeft, FLOAT32 fRight, FLOAT32 fBottom, FLOAT32 fTop, 
		FLOAT32 fZNear, FLOAT32 fZFar);
	Matrix4x4&	MatrixPerspectiveOffCenterRH(Matrix4x4& rkOut, FLOAT32 fLeft, FLOAT32 fRight, FLOAT32 fBottom, FLOAT32 fTop, 
		FLOAT32 fZNear, FLOAT32 fZFar);

	Matrix4x4&	MatrixViewport(Matrix4x4& rkOut, UINT32 uiX, UINT32 uiY, UINT32 uiWidth, UINT32 uiHeight, 
		FLOAT32 fZNear, FLOAT32 fZFar);

	Matrix4x4&	MatrixInvert(Matrix4x4& rkOut, const Matrix4x4& rkMat);

	Quaternion& QuaternionIdentity(Quaternion& rkOut);
	Quaternion& QuaternionRotationMatrix(Quaternion& rkOut, const Matrix4x4& rkMat);
	Quaternion& QuaternionSLerp(Quaternion& rkOut, const Quaternion& rkQuatA, const Quaternion& rkQuatB, FLOAT32 fLerp);
	void		QuaternionToAxisAngle(const Quaternion& rkQuat, Vector3& rkAxis, FLOAT32& rfAngle);
}