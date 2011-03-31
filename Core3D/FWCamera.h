#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Framework Library for Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////
#include "FWType.h"

namespace Core3D
{
	class FWGraphics;
	class FWCamera
	{
	public:
		enum Visibility
		{
			VISIBILITY_COMPLETYOUT = 0,
			VISIBILITY_PARTLY,
			VISIBILITY_COMPLETYIN
		};

		FWCamera(FWGraphics* pkGraphics);
		virtual ~FWCamera();

		bool CreateRenderCamera(UINT32 uiWidth, UINT32 uiHeight, 
			Format eFmtFrameBuffer = FMT_R32G32B32F, bool bDepthBuffer = true);
		void CalculateProjection(FLOAT32 fFOV, FLOAT32 fViewDistance, 
			FLOAT32 fNearClippingPlane = 1.0f, FLOAT32 fAspect = 4.0f / 3.0f);
		void CalculateView();

		void BeginRender();
		void EndRender(bool bPresentToScreen = false);
		void ClearToSceneColor(const Rect* pkRect = NULL);

		Visibility				SphereVisible(const Vector3& rkOrigin, FLOAT32 fRadius);
		Visibility				BoxVisible(const Vector3& rkLower, const Vector3& rkUpper);
		FWGraphics*				GetGraphics();

		virtual void			RenderPass(INT32 iPass = -1) = 0;
	private:
		void					BuildFrustum();
	public:
		inline RenderTarget*	GetRenderTarget()								{return m_pkRenderTarget;}
		inline void				SetWorldMatrix(const Matrix4x4& rkMat)			{m_kMatWorld = rkMat;}
		inline void				SetViewMatrix(const Matrix4x4& rkMat)			{m_kMatView = rkMat;}
		inline void				SetProjectionMatrix(const Matrix4x4& rkMat)		{m_kMatProjection = rkMat;}

		inline const Matrix4x4& GetWorldMatrix()						const	{return m_kMatWorld;}
		inline const Matrix4x4& GetViewMatrix()							const	{return m_kMatView;}
		inline const Matrix4x4& GetProjectionMatrix()					const	{return m_kMatProjection;}

		inline FLOAT32			GetFOV()								const	{return m_fFOV;}
		inline FLOAT32			GetAspect()								const	{return m_fAspect;}
		inline FLOAT32			GetNearClippingPlane()					const	{return m_fNearClippingPlane;}
		inline FLOAT32			GetViewDistance()						const	{return m_fViewDistance;}

		inline void				SetPosition(const Vector3& rkPosition)			{m_kPosition = rkPosition;}
		inline void				SetPositionRel(const Vector3& rkPositionRel)	{m_kPosition += rkPositionRel;}
		inline const Vector3&	GetPosition()							const	{return m_kPosition;}

		inline void				SetRotation(const Vector3& rkRotation)
		{
			Matrix4x4 kMat;
			Core3D::MatrixRotationYawPitchRoll(kMat, rkRotation);
			Core3D::QuaternionRotationMatrix(m_kOrientation, kMat);
		}
		inline void				SetRotationRel(const Vector3& rkRotationRel)
		{
			Matrix4x4 kMat;
			Core3D::MatrixRotationYawPitchRoll(kMat, rkRotationRel);
			Quaternion kQuat;
			Core3D::QuaternionRotationMatrix(kQuat, kMat);
			m_kOrientation *= kQuat;
		}
		inline void				SetLookAt(const Vector3& rkPosition, const Vector3& rkUp)
		{
			Matrix4x4 kMat;
			Core3D::MatrixLookAtLH(kMat, m_kPosition, rkPosition, rkUp);
			Core3D::QuaternionRotationMatrix(m_kOrientation, kMat);
		}
		inline void				SetOrientation(const Quaternion& rkOrientation) {m_kOrientation = rkOrientation;}
		inline const Quaternion& GetOrientation()						const	{return m_kOrientation;}

		inline const Vector3&	GetDirection()							const	{return m_kDirection;}
		inline const Vector3&	GetRight()								const	{return m_kRight;}
		inline const Vector3&	GetUp()									const	{return m_kUp;}
	public:
		FWGraphics*		m_pkGraphics;
		RenderTarget*	m_pkRenderTarget;
		
		bool			m_bLockedSurfaceViewport;
		
		Matrix4x4		m_kMatWorld;
		Matrix4x4		m_kMatView;
		Matrix4x4		m_kMatProjection;
		
		Plane			m_akFrustum[6];
		
		FLOAT32			m_fFOV;
		FLOAT32			m_fAspect;
		FLOAT32			m_fNearClippingPlane;
		FLOAT32			m_fViewDistance;
		
		Vector3			m_kPosition;
		Quaternion		m_kOrientation;
		Vector3			m_kUp;
		Vector3			m_kRight;
		Vector3			m_kDirection;
	};
}