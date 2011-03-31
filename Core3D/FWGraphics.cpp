#include "FWGraphics.h"
#include "FWApplication.h"
#include "FWStateBlock.h"

namespace Core3D
{
	FWGraphics::FWGraphics(FWApplication* pkApp)
	{
		m_pkApplication = pkApp;
		m_pkObject		= NULL;
		m_pkDevice		= NULL;
	}

	FWGraphics::~FWGraphics()
	{
		while(!m_kStateBlocks.empty()) {PopStateBlock();}
		CORE3D_SAFE_RELEASE(m_pkDevice);
		CORE3D_SAFE_RELEASE(m_pkObject);
	}

	bool FWGraphics::Initialize(const CreationFlags& rkCreateFlags)
	{
		// COMMENT : Initialize Core3D object
		if(CORE3D_FAILED(Core3D::CreateObject(&m_pkObject))) {return false;}

		// COMMENT : Initialize Core3D device parameters (Defaulting to 32 bit colors in full-screen mode)
		DeviceParameters kParamsDevice = 
		{
			m_pkApplication->GetWindowHandle(), 
			m_pkApplication->GetWindowed(), 32, 
			m_pkApplication->GetWindowWidth(), m_pkApplication->GetWindowHeight()
		};

		if(CORE3D_FAILED(m_pkObject->CreateDevice(&m_pkDevice, &kParamsDevice)))
		{
			CORE3D_SAFE_RELEASE(m_pkObject);
			return false;
		}

		// COMMENT : Create sub-systems
		PushStateBlock();
		return true;
	}

	void FWGraphics::SetCurrentCamera(FWCamera* pkCamera)
	{
		m_pkCurrentCamera = pkCamera;
	}

	FWApplication* FWGraphics::GetApplication()
	{
		return m_pkApplication;
	}

	FWCamera* FWGraphics::GetCurrentCamera()
	{
		return m_pkCurrentCamera;
	}

	void FWGraphics::PushStateBlock()
	{
		FWStateBlock* pkNewStateBlock = new FWStateBlock(this);
		m_kStateBlocks.push(pkNewStateBlock);
	}

	void FWGraphics::PopStateBlock()
	{
		if(!m_kStateBlocks.empty())
		{
			CORE3D_SAFE_DELETE(m_kStateBlocks.top());
			m_kStateBlocks.pop();
		}
	}

	Result FWGraphics::SetRenderState(RenderState eRenderState, UINT32 uiValue)
	{
		if(m_kStateBlocks.empty()) {return UNKNOWN;}
		return m_kStateBlocks.top()->SetRenderState(eRenderState, uiValue);
	}

	Result FWGraphics::SetVertexFormat(VertexFormat* pkVertexFormat)
	{
		if(m_kStateBlocks.empty()) {return UNKNOWN;}
		return m_kStateBlocks.top()->SetVertexFormat(pkVertexFormat);
	}

	void FWGraphics::SetPrimitiveAssembler(PrimitiveAssembler* pkPrimitiveAssembler)
	{
		if(m_kStateBlocks.empty()) {return;}
		m_kStateBlocks.top()->SetPrimitiveAssembler(pkPrimitiveAssembler);
	}

	Result FWGraphics::SetVertexShader(VertexShader* pkVertexShader)
	{
		if(m_kStateBlocks.empty()) {return UNKNOWN;}
		return m_kStateBlocks.top()->SetVertexShader(pkVertexShader);
	}

	void FWGraphics::SetTriangleShader(TriangleShader* pkTriangleShader)
	{
		if(m_kStateBlocks.empty()) {return;}
		m_kStateBlocks.top()->SetTriangleShader(pkTriangleShader);
	}

	Result FWGraphics::SetPixelShader(PixelShader* pkPixelShader)
	{
		if(m_kStateBlocks.empty()) {return UNKNOWN;}
		return m_kStateBlocks.top()->SetPixelShader(pkPixelShader);
	}

	Result FWGraphics::SetIndexBuffer(IndexBuffer* pkIndexBuffer)
	{
		if(m_kStateBlocks.empty()) {return UNKNOWN;}
		return m_kStateBlocks.top()->SetIndexBuffer(pkIndexBuffer);
	}

	Result FWGraphics::SetVertexStream(UINT32 uiStream, VertexBuffer* pkVertexBuffer, UINT32 uiOffset, UINT32 uiStride)
	{
		if(m_kStateBlocks.empty()) {return UNKNOWN;}
		return m_kStateBlocks.top()->SetVertexStream(uiStream, pkVertexBuffer, uiOffset, uiStride);
	}

	Result FWGraphics::SetTexture(UINT32 uiSampler, BaseTexture* pkTexture)
	{
		if(m_kStateBlocks.empty()) {return UNKNOWN;}
		return m_kStateBlocks.top()->SetTexture(uiSampler, pkTexture);
	}

	Result FWGraphics::SetTextureSamplerState(UINT32 uiSampler, TextureSamplerState eTextureSamplerState, UINT32 uiState)
	{
		if(m_kStateBlocks.empty()) {return UNKNOWN;}
		return m_kStateBlocks.top()->SetTextureSamplerState(uiSampler, eTextureSamplerState, uiState);
	}

	void FWGraphics::SetRenderTarget(RenderTarget* pkRenderTarget)
	{
		if(m_kStateBlocks.empty()) {return;}
		m_kStateBlocks.top()->SetRenderTarget(pkRenderTarget);
	}

	Result FWGraphics::SetScissorRect(const Rect& rkScissorRect)
	{
		if(m_kStateBlocks.empty()) {return UNKNOWN;}
		return m_kStateBlocks.top()->SetScissorRect(rkScissorRect);
	}
}