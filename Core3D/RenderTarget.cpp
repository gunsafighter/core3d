#include "RenderTarget.h"
#include "Device.h"
#include "Surface.h"

namespace Core3D
{
	RenderTarget::RenderTarget(Device* pkDevice)
		: m_pkDevice(pkDevice)
		, m_pkColorBuffer(NULL)
		, m_pkDepthBuffer(NULL)
	{
		m_pkDevice->AddRef();
	}

	RenderTarget::~RenderTarget()
	{
		CORE3D_SAFE_RELEASE(m_pkColorBuffer);
		CORE3D_SAFE_RELEASE(m_pkDepthBuffer);
		CORE3D_SAFE_RELEASE(m_pkDevice);
	}

	Device* RenderTarget::GetDevice()
	{
		if(NULL != m_pkDevice) {m_pkDevice->AddRef();}
		return m_pkDevice;
	}

	Result RenderTarget::ClearColorBuffer(const Vector4& rkColor, const Rect* pkRect)
	{
		if(NULL == m_pkColorBuffer)
		{
			CORE3D_ERROR(_T("RenderTarget::ClearColorBuffer() - No frame buffer has been set.\n"));
			return INVALID_STATE;
		}
		return m_pkColorBuffer->Clear(rkColor, pkRect);
	}

	Result RenderTarget::ClearDepthBuffer(FLOAT32 fDepth, const Rect* pkRect)
	{
		if(NULL == m_pkDepthBuffer)
		{
			CORE3D_ERROR(_T("RenderTarget::ClearDepthBuffer() - No depth buffer has been set.\n"));
			return INVALID_STATE;
		}
		return m_pkDepthBuffer->Clear(Vector4(fDepth, 0.0f, 0.0f, 0.0f), pkRect);
	}

	Result RenderTarget::SetColorBuffer(Surface* pkColorBuffer)
	{
		if(NULL != pkColorBuffer)
		{
			if((pkColorBuffer->GetFormat() < FMT_R32F) || (pkColorBuffer->GetFormat() > FMT_R32G32B32A32F))
			{
				CORE3D_ERROR(_T("RenderTarget::SetColorBuffer() - Invalid texture format.\n"));
				return INVALID_FORMAT;
			}

			if(NULL != m_pkDepthBuffer)
			{
				if(	(m_pkDepthBuffer->GetWidth()  != pkColorBuffer->GetWidth()) || 
					(m_pkDepthBuffer->GetHeight() != pkColorBuffer->GetHeight()) )
				{
					CORE3D_ERROR(_T("RenderTarget::SetColorBuffer() - Frame buffer and depth buffer dimensions are not equal.\n"));
					return INVALID_FORMAT;
				}
			}
		}
		CORE3D_SAFE_RELEASE(m_pkColorBuffer);
		m_pkColorBuffer = pkColorBuffer;
		if(NULL != m_pkColorBuffer) {m_pkColorBuffer->AddRef();}
		return OK;
	}

	Result RenderTarget::SetDepthBuffer(Surface* pkDepthBuffer)
	{
		if(NULL != pkDepthBuffer)
		{
			if(FMT_R32F != pkDepthBuffer->GetFormat())
			{
				CORE3D_ERROR(_T("RenderTarget::SetDepthBuffer() - Invalid texture format.\n"));
				return INVALID_FORMAT;
			}

			if(NULL != m_pkColorBuffer)
			{
				if(	(pkDepthBuffer->GetWidth()  != m_pkColorBuffer->GetWidth()) || 
					(pkDepthBuffer->GetHeight() != m_pkColorBuffer->GetHeight()) )
				{
					CORE3D_ERROR(_T("RenderTarget::SetDepthBuffer() - Depth buffer and frame buffer dimensions are not equal.\n"));
					return INVALID_FORMAT;
				}
			}
		}
		CORE3D_SAFE_RELEASE(m_pkDepthBuffer);
		m_pkDepthBuffer = pkDepthBuffer;
		if(NULL != m_pkDepthBuffer) {m_pkDepthBuffer->AddRef();}
		return OK;
	}

	Surface* RenderTarget::GetColorBuffer()
	{
		if(NULL != m_pkColorBuffer) {m_pkColorBuffer->AddRef();}
		return m_pkColorBuffer;
	}

	Surface* RenderTarget::GetDepthBuffer()
	{
		if(NULL != m_pkDepthBuffer) {m_pkDepthBuffer->AddRef();}
		return m_pkDepthBuffer;
	}

	void RenderTarget::SetViewportMatrix(const Matrix4x4& rkMatViewport)
	{
		m_kMatViewport = rkMatViewport;
	}

	const Matrix4x4& RenderTarget::GetViewportMatrix()
	{
		return m_kMatViewport;
	}
}