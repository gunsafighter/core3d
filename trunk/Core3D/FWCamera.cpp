#include "FWCamera.h"
#include "FWGraphics.h"
#include "FWApplication.h"
#include "FWScene.h"

namespace Core3D
{
	FWCamera::FWCamera(FWGraphics* pkGraphics)
	{
		m_pkGraphics = pkGraphics;
		if(CORE3D_FAILED(m_pkGraphics->GetDevice()->CreateRenderTarget(&m_pkRenderTarget)))
		{
			m_pkRenderTarget = NULL;
		}
		m_bLockedSurfaceViewport = false;

		Core3D::MatrixIdentity(m_kMatWorld);
		Core3D::MatrixIdentity(m_kMatView);
		Core3D::MatrixIdentity(m_kMatProjection);

		BuildFrustum();
	}

	FWCamera::~FWCamera()
	{
		CORE3D_SAFE_RELEASE(m_pkRenderTarget);
	}

	bool FWCamera::CreateRenderCamera(UINT32 uiWidth, UINT32 uiHeight, Format eFmtFrameBuffer /* = FMT_R32G32B32F */, bool bDepthBuffer /* = true */)
	{
		if(true == m_bLockedSurfaceViewport) {return false;}

		Device* pkDevice		= m_pkGraphics->GetDevice();
		// COMMENT : Create the render texture
		Surface* pkColorBuffer	= NULL;
		if(CORE3D_FAILED(pkDevice->CreateSurface(&pkColorBuffer, uiWidth, uiHeight, eFmtFrameBuffer)))
		{
			return false;
		}

		// COMMENT : Create the depth texture
		Surface* pkDepthBuffer	= NULL;
		if(true == bDepthBuffer)
		{
			if(CORE3D_FAILED(pkDevice->CreateSurface(&pkDepthBuffer, uiWidth, uiHeight, FMT_R32F)))
			{
				CORE3D_SAFE_RELEASE(pkColorBuffer);
				return false;
			}
		}

		// COMMENT : Set the view-port
		Matrix4x4 kMatViewport;
		Core3D::MatrixViewport(kMatViewport, 0, 0, uiWidth, uiHeight, 0.0f, 1.0f);
		m_pkRenderTarget->SetViewportMatrix(kMatViewport);
		m_pkRenderTarget->SetColorBuffer(pkColorBuffer);
		m_pkRenderTarget->SetDepthBuffer(pkDepthBuffer);

		CORE3D_SAFE_RELEASE(pkColorBuffer);
		CORE3D_SAFE_RELEASE(pkDepthBuffer);

		// COMMENT : Don't allow any more changes to surface / view-port
		m_bLockedSurfaceViewport = true;
		return true;
	}

	void FWCamera::BeginRender()
	{
		m_pkGraphics->PushStateBlock();
		
		m_pkGraphics->SetRenderTarget(m_pkRenderTarget);
		m_pkGraphics->SetCurrentCamera(this);
	}

	void FWCamera::ClearToSceneColor(const Rect* pkRect /* = NULL */)
	{
		FWScene* pkScene = m_pkGraphics->GetApplication()->GetScene();
		m_pkRenderTarget->ClearColorBuffer(pkScene->GetClearColor(), pkRect);
		m_pkRenderTarget->ClearDepthBuffer(1.0f, pkRect);
	}

	void FWCamera::EndRender(bool bPresentToScreen /* = false */)
	{
		m_pkGraphics->PopStateBlock();

		if(true == bPresentToScreen) {m_pkGraphics->GetDevice()->Present(m_pkRenderTarget);}
	}

	void FWCamera::CalculateProjection(FLOAT32 fFOV, FLOAT32 fViewDistance, FLOAT32 fNearClippingPlane /* = 1.0f */, FLOAT32 fAspect /* = 4.0f / 3.0f */)
	{
		Core3D::MatrixPerspectiveFovLH(m_kMatProjection, fFOV, fAspect, fNearClippingPlane, fViewDistance);

		m_fFOV					= fFOV;
		m_fAspect				= fAspect;
		m_fNearClippingPlane	= fNearClippingPlane;
		m_fViewDistance			= fViewDistance;
	}

	void FWCamera::CalculateView()
	{
		Matrix4x4 kMatRotation, kMatTranslation;
		Core3D::MatrixTranslation(kMatTranslation, -m_kPosition);
		Core3D::MatrixRotationQuaternion(kMatRotation, m_kOrientation);
		m_kMatView = kMatTranslation * kMatRotation;

		// COMMENT : Update camera axis
		Core3D::MatrixTranspose(kMatRotation, kMatRotation);
		m_kDirection.x	= kMatRotation._31;
		m_kDirection.y	= kMatRotation._32;
		m_kDirection.z	= kMatRotation._33;

		m_kUp.x			= kMatRotation._21;
		m_kUp.y			= kMatRotation._22;
		m_kUp.z			= kMatRotation._23;

		m_kRight.x		= kMatRotation._11;
		m_kRight.y		= kMatRotation._12;
		m_kRight.z		= kMatRotation._13;

		// COMMENT : Re-build frustum
		BuildFrustum();
	}

	FWCamera::Visibility FWCamera::SphereVisible(const Vector3& rkOrigin, FLOAT32 fRadius)
	{
		Vector3 kOrigin = rkOrigin * GetWorldMatrix();
		// COMMENT : Accounts for scaling matrix
		// TODO : Only enables scales along X-Axis
		Vector3 kRadius(fRadius, 0.0f, 0.0f);
		Core3D::Vec3TransformNormal(kRadius, kRadius, GetWorldMatrix());

		Plane* pkFrustum = m_akFrustum;
		for(UINT32 i = 0; i < 6; ++i, ++pkFrustum)
		{
			if((*pkFrustum * kOrigin) < -fRadius) {return VISIBILITY_COMPLETYOUT;}
		}
		return VISIBILITY_COMPLETYIN;
	}

	FWCamera::Visibility FWCamera::BoxVisible(const Vector3& rkLower, const Vector3& rkUpper)
	{
		Vector3 kVectorA(rkLower.x, rkLower.y, rkLower.z);
		kVectorA *= GetWorldMatrix();
		Vector3 kVectorB(rkUpper.x, rkLower.y, rkLower.z);
		kVectorB *= GetWorldMatrix();
		Vector3 kVectorC(rkLower.x, rkUpper.y, rkLower.z);
		kVectorC *= GetWorldMatrix();
		Vector3 kVectorD(rkUpper.x, rkUpper.y, rkLower.z);
		kVectorD *= GetWorldMatrix();

		Vector3 kVectorE(rkLower.x, rkLower.y, rkUpper.z);
		kVectorE *= GetWorldMatrix();
		Vector3 kVectorF(rkUpper.x, rkLower.y, rkUpper.z);
		kVectorF *= GetWorldMatrix();
		Vector3 kVectorG(rkLower.x, rkUpper.y, rkUpper.z);
		kVectorG *= GetWorldMatrix();
		Vector3 kVectorH(rkUpper.x, rkUpper.y, rkUpper.z);
		kVectorH *= GetWorldMatrix();

	#	define V_IN			1
	#	define V_OUT		2
	#	define V_INTERSECT	3
		
		BYTE8 nMode = 0;
		Plane* pkFrustum = m_akFrustum;
		for(UINT32 i = 0; i < 6; ++i, ++pkFrustum)
		{
			nMode &= V_OUT; // Clear IN bit
			if((*pkFrustum * kVectorA) >= 0.0f) {nMode |= V_IN;} else {nMode |= V_OUT;}
			if(V_INTERSECT == nMode)			{continue;}

			if((*pkFrustum * kVectorB) >= 0.0f) {nMode |= V_IN;} else {nMode |= V_OUT;}
			if(V_INTERSECT == nMode)			{continue;}

			if((*pkFrustum * kVectorC) >= 0.0f) {nMode |= V_IN;} else {nMode |= V_OUT;}
			if(V_INTERSECT == nMode)			{continue;}

			if((*pkFrustum * kVectorD) >= 0.0f) {nMode |= V_IN;} else {nMode |= V_OUT;}
			if(V_INTERSECT == nMode)			{continue;}


			if((*pkFrustum * kVectorE) >= 0.0f) {nMode |= V_IN;} else {nMode |= V_OUT;}
			if(V_INTERSECT == nMode)			{continue;}
			
			if((*pkFrustum * kVectorF) >= 0.0f) {nMode |= V_IN;} else {nMode |= V_OUT;}
			if(V_INTERSECT == nMode)			{continue;}

			if((*pkFrustum * kVectorG) >= 0.0f) {nMode |= V_IN;} else {nMode |= V_OUT;}
			if(V_INTERSECT == nMode)			{continue;}

			if((*pkFrustum * kVectorH) >= 0.0f) {nMode |= V_IN;} else {nMode |= V_OUT;}
			if(V_INTERSECT == nMode)			{continue;}

			if(V_IN == nMode) {continue;}
			return VISIBILITY_COMPLETYOUT;
		}

		if(V_INTERSECT == nMode) {return VISIBILITY_PARTLY;}
		return VISIBILITY_COMPLETYIN;
	}

	void FWCamera::BuildFrustum()
	{
		// COMMENT : Calculate frustum planes
		Matrix4x4 kMatFrustum = GetViewMatrix() * GetProjectionMatrix();

		// COMMENT : Near
		m_akFrustum[0] = Plane(
			kMatFrustum._14 + kMatFrustum._13, 
			kMatFrustum._24 + kMatFrustum._23, 
			kMatFrustum._34 + kMatFrustum._33, 
			kMatFrustum._44 + kMatFrustum._43);

		// COMMENT : Far
		m_akFrustum[1] = Plane(
			kMatFrustum._14 - kMatFrustum._13, 
			kMatFrustum._24 - kMatFrustum._23, 
			kMatFrustum._34 - kMatFrustum._33, 
			kMatFrustum._44 - kMatFrustum._43);

		// COMMENT : Left
		m_akFrustum[2] = Plane(
			kMatFrustum._14 + kMatFrustum._11, 
			kMatFrustum._24 + kMatFrustum._21, 
			kMatFrustum._34 + kMatFrustum._31, 
			kMatFrustum._44 + kMatFrustum._41);

		// COMMENT : Right
		m_akFrustum[3] = Plane(
			kMatFrustum._14 - kMatFrustum._11, 
			kMatFrustum._24 - kMatFrustum._21, 
			kMatFrustum._34 - kMatFrustum._31, 
			kMatFrustum._44 - kMatFrustum._41);

		// COMMENT : Top
		m_akFrustum[4] = Plane(
			kMatFrustum._14 - kMatFrustum._12, 
			kMatFrustum._24 - kMatFrustum._22, 
			kMatFrustum._34 - kMatFrustum._32, 
			kMatFrustum._44 - kMatFrustum._42);

		// COMMENT : Bottom
		m_akFrustum[5] = Plane(
			kMatFrustum._14 + kMatFrustum._12, 
			kMatFrustum._24 + kMatFrustum._22, 
			kMatFrustum._34 + kMatFrustum._32, 
			kMatFrustum._44 + kMatFrustum._42);

		m_akFrustum[0].kNormal.Normalize();
		m_akFrustum[1].kNormal.Normalize();
		m_akFrustum[2].kNormal.Normalize();
		m_akFrustum[3].kNormal.Normalize();
		m_akFrustum[4].kNormal.Normalize();
		m_akFrustum[5].kNormal.Normalize();
	}

	FWGraphics* FWCamera::GetGraphics()
	{
		return m_pkGraphics;
	}
}