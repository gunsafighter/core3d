#include "FWStateBlock.h"
#include "FWGraphics.h"
#include "FWCamera.h"

namespace Core3D
{
	FWStateBlock::FWStateBlock(FWGraphics* pkGraphics)
	{
		m_pkGraphics					= pkGraphics;
		m_bChangedVertexFormat			= false;
		m_bChangedPrimitiveAssembler	= false;
		m_bChangedVertexShader			= false;
		m_bChangedTriangleShader		= false;
		m_bChangedPixelShader			= false;
		m_bChangedIndexBuffer			= false;
		m_bChangedRenderTarget			= false;
		m_bChangedScissorRect			= false;
		m_bChangedCamera				= false;
	}

	FWStateBlock::~FWStateBlock()
	{
		RestoreStates();
	}

	Result FWStateBlock::SetRenderState(RenderState eRenderState, UINT32 uiValue)
	{
		Device* pkDevice = m_pkGraphics->GetDevice();
		// COMMENT : Record state if not yet saved
		if(m_mapRenderStates.find(eRenderState) == m_mapRenderStates.end())
		{
			UINT32 uiCurrentValue;
			if(CORE3D_SUCCESSFUL(pkDevice->GetRenderState(eRenderState, uiCurrentValue)))
			{
				if(uiCurrentValue == uiValue) {return OK;}
				m_mapRenderStates[eRenderState] = uiCurrentValue;
			}
		}
		return pkDevice->SetRenderState(eRenderState, uiValue);
	}

	Result FWStateBlock::SetVertexFormat(VertexFormat* pkVertexFormat)
	{
		Device* pkDevice = m_pkGraphics->GetDevice();
		// COMMENT : Record state if not yet saved
		if(false == m_bChangedVertexFormat)
		{
			m_pkVertexFormat = pkDevice->GetVertexFormat();
			if(m_pkVertexFormat == pkVertexFormat)
			{
				CORE3D_SAFE_RELEASE(m_pkVertexFormat);
				return OK;
			}
			m_bChangedVertexFormat = true;
		}
		return pkDevice->SetVertexFormat(pkVertexFormat);
	}

	void FWStateBlock::SetPrimitiveAssembler(PrimitiveAssembler* pkPrimitiveAssembler)
	{
		Device* pkDevice = m_pkGraphics->GetDevice();
		// COMMENT : Record state if not yet saved
		if(false == m_bChangedPrimitiveAssembler)
		{
			m_pkPrimitiveAssembler = pkDevice->GetPrimitiveAssembler();
			if(m_pkPrimitiveAssembler == pkPrimitiveAssembler)
			{
				CORE3D_SAFE_RELEASE(m_pkPrimitiveAssembler);
				return;
			}
			m_bChangedPrimitiveAssembler = true;
		}
		pkDevice->SetPrimitiveAssembler(pkPrimitiveAssembler);
	}

	Result FWStateBlock::SetVertexShader(VertexShader* pkVertexShader)
	{
		Device* pkDevice = m_pkGraphics->GetDevice();
		// COMMENT : Record state if not yet saved
		if(false == m_bChangedVertexShader)
		{
			m_pkVertexShader = pkDevice->GetVertexShader();
			if(m_pkVertexShader == pkVertexShader)
			{
				CORE3D_SAFE_RELEASE(m_pkVertexShader);
				return OK;
			}
			m_bChangedVertexShader = true;
		}
		return pkDevice->SetVertexShader(pkVertexShader);
	}

	void FWStateBlock::SetTriangleShader(TriangleShader* pkTriangleShader)
	{
		Device* pkDevice = m_pkGraphics->GetDevice();
		// COMMENT : Record state if not yet saved
		if(false == m_bChangedTriangleShader)
		{
			m_pkTriangleShader = pkDevice->GetTriangleShader();
			if(m_pkTriangleShader == pkTriangleShader)
			{
				CORE3D_SAFE_RELEASE(m_pkTriangleShader);
				return;
			}
			m_bChangedTriangleShader = true;
		}
		pkDevice->SetTriangleShader(pkTriangleShader);
	}

	Result FWStateBlock::SetPixelShader(PixelShader* pkPixelShader)
	{
		Device* pkDevice = m_pkGraphics->GetDevice();
		// COMMENT : Record state if not yet saved
		if(false == m_bChangedPixelShader)
		{
			m_pkPixelShader = pkDevice->GetPixelShader();
			if(m_pkPixelShader == pkPixelShader)
			{
				CORE3D_SAFE_RELEASE(m_pkPixelShader);
				return OK;
			}
			m_bChangedPixelShader = true;
		}
		return pkDevice->SetPixelShader(pkPixelShader);
	}

	Result FWStateBlock::SetIndexBuffer(IndexBuffer* pkIndexBuffer)
	{
		Device* pkDevice = m_pkGraphics->GetDevice();
		// COMMENT : Record state if not yet saved
		if(false == m_bChangedIndexBuffer)
		{
			m_pkIndexBuffer = pkDevice->GetIndexBuffer();
			if(m_pkIndexBuffer == pkIndexBuffer)
			{
				CORE3D_SAFE_RELEASE(m_pkIndexBuffer);
				return OK;
			}
			m_bChangedIndexBuffer = true;
		}
		return pkDevice->SetIndexBuffer(pkIndexBuffer);
	}

	Result FWStateBlock::SetVertexStream(UINT32 uiStream, VertexBuffer* pkVertexBuffer, UINT32 uiOffset, UINT32 uiStride)
	{
		Device* pkDevice = m_pkGraphics->GetDevice();
		// COMMENT : Record state if not yet saved
		if(m_mapVertexStreams.find(uiStream) == m_mapVertexStreams.end())
		{
			VertexStreamInfo kStreamInfo;
			if(CORE3D_SUCCESSFUL(pkDevice->GetVertexStream(uiStream, &kStreamInfo.pkVertexBuffer, kStreamInfo.uiOffset, kStreamInfo.uiStride)))
			{
				if( kStreamInfo.pkVertexBuffer == pkVertexBuffer	&& 
					kStreamInfo.uiOffset == uiOffset				&& 
					kStreamInfo.uiStride == uiStride )
				{
					CORE3D_SAFE_RELEASE(kStreamInfo.pkVertexBuffer);
					return OK;
				}
				m_mapVertexStreams[uiStream] = kStreamInfo;
			}
		}
		return pkDevice->SetVertexStream(uiStream, pkVertexBuffer, uiOffset, uiStride);
	}

	Result FWStateBlock::SetTexture(UINT32 uiSampler, BaseTexture* pkTexture)
	{
		Device* pkDevice = m_pkGraphics->GetDevice();
		// COMMENT : Record state if not yet saved
		if(m_mapTextures.find(uiSampler) == m_mapTextures.end())
		{
			BaseTexture* pkCurrentTexture;
			if(CORE3D_SUCCESSFUL(pkDevice->GetTexture(uiSampler, &pkCurrentTexture)))
			{
				if(pkCurrentTexture == pkTexture)
				{
					CORE3D_SAFE_RELEASE(pkCurrentTexture);
					return OK;
				}
				m_mapTextures[uiSampler] = pkCurrentTexture;
			}
		}
		return pkDevice->SetTexture(uiSampler, pkTexture);
	}

	Result FWStateBlock::SetTextureSamplerState(UINT32 uiSampler, TextureSamplerState eTextureSamplerState, UINT32 uiState)
	{
		Device* pkDevice	= m_pkGraphics->GetDevice();
		// COMMENT : Record state if not yet saved
		UINT32 uiIndex		= uiSampler * (UINT32)TSS_NUMTEXTURESAMPLERSTATES + (UINT32)eTextureSamplerState;
		if(m_mapTextureSamplerStates.find(uiIndex) == m_mapTextureSamplerStates.end())
		{
			UINT32 uiCurrentState;
			if(CORE3D_SUCCESSFUL(pkDevice->GetTextureSamplerState(uiSampler, eTextureSamplerState, uiCurrentState)))
			{
				if(uiCurrentState == uiState) {return OK;}
				m_mapTextureSamplerStates[uiIndex] = uiCurrentState;
			}
		}
		return pkDevice->SetTextureSamplerState(uiSampler, eTextureSamplerState, uiState);
	}

	Result FWStateBlock::SetScissorRect(const Rect& rkScissorRect)
	{
		Device* pkDevice = m_pkGraphics->GetDevice();
		// COMMENT : Record state if not yet saved
		if(false == m_bChangedScissorRect)
		{
			m_kScissorRect			= pkDevice->GetScissorRect();
			m_bChangedScissorRect	= true;
		}
		return pkDevice->SetScissorRect(rkScissorRect);
	}

	void FWStateBlock::SetRenderTarget(RenderTarget* pkRenderTarget)
	{
		Device* pkDevice = m_pkGraphics->GetDevice();
		// COMMENT : Record state if not yet saved
		if(false == m_bChangedRenderTarget)
		{
			m_pkRenderTarget		= pkDevice->GetRenderTarget();
			if(m_pkRenderTarget == pkRenderTarget) {CORE3D_SAFE_RELEASE(m_pkRenderTarget); return;}
			m_bChangedRenderTarget	= true;
		}
		pkDevice->SetRenderTarget(pkRenderTarget);
	}

	void FWStateBlock::SetCurrentCamera(FWCamera* pkCamera)
	{
		// COMMENT : Record state if not yet saved
		if(false == m_bChangedCamera)
		{
			m_pkCamera			= m_pkGraphics->m_pkCurrentCamera;
			if(m_pkCamera == pkCamera) {return;}
			m_bChangedCamera	= true;
		}
		m_pkGraphics->m_pkCurrentCamera = pkCamera;
	}

	void FWStateBlock::RestoreStates()
	{
		Device* pkDevice = m_pkGraphics->GetDevice();
		for(std::map<RenderState, UINT32>::iterator iterRenderState = m_mapRenderStates.begin(); 
			iterRenderState != m_mapRenderStates.end(); ++iterRenderState)
		{
			pkDevice->SetRenderState(iterRenderState->first, iterRenderState->second);
		}

		if(true == m_bChangedVertexFormat)
		{
			pkDevice->SetVertexFormat(m_pkVertexFormat);
			CORE3D_SAFE_RELEASE(m_pkVertexFormat);
			m_bChangedVertexFormat = false;
		}

		if(true == m_bChangedPrimitiveAssembler)
		{
			pkDevice->SetPrimitiveAssembler(m_pkPrimitiveAssembler);
			CORE3D_SAFE_RELEASE(m_pkPrimitiveAssembler);
			m_bChangedPrimitiveAssembler = false;
		}

		if(true == m_bChangedVertexShader)
		{
			pkDevice->SetVertexShader(m_pkVertexShader);
			CORE3D_SAFE_RELEASE(m_pkVertexShader);
			m_bChangedVertexShader = false;
		}

		if(true == m_bChangedTriangleShader)
		{
			pkDevice->SetTriangleShader(m_pkTriangleShader);
			CORE3D_SAFE_RELEASE(m_pkTriangleShader);
			m_bChangedTriangleShader = false;
		}

		if(true == m_bChangedPixelShader)
		{
			pkDevice->SetPixelShader(m_pkPixelShader);
			CORE3D_SAFE_RELEASE(m_pkPixelShader);
			m_bChangedPixelShader = false;
		}

		if(true == m_bChangedIndexBuffer)
		{
			pkDevice->SetIndexBuffer(m_pkIndexBuffer);
			CORE3D_SAFE_RELEASE(m_pkIndexBuffer);
			m_bChangedIndexBuffer = false;
		}

		if(true == m_bChangedRenderTarget)
		{
			pkDevice->SetRenderTarget(m_pkRenderTarget);
			CORE3D_SAFE_RELEASE(m_pkRenderTarget);
			m_bChangedRenderTarget = false;
		}

		if(true == m_bChangedScissorRect)
		{
			pkDevice->SetScissorRect(m_kScissorRect);
			m_bChangedScissorRect = false;
		}

		if(true == m_bChangedCamera)
		{
			m_pkGraphics->m_pkCurrentCamera = m_pkCamera;
			m_bChangedCamera = false;
		}

		for(std::map<UINT32, VertexStreamInfo>::iterator iterVertexStream = m_mapVertexStreams.begin(); 
			iterVertexStream != m_mapVertexStreams.end(); ++iterVertexStream)
		{
			pkDevice->SetVertexStream(iterVertexStream->first, iterVertexStream->second.pkVertexBuffer, iterVertexStream->second.uiOffset, iterVertexStream->second.uiStride);
			CORE3D_SAFE_RELEASE(iterVertexStream->second.pkVertexBuffer);
		}
		m_mapVertexStreams.clear();

		for(std::map<UINT32, BaseTexture*>::iterator iterTexture = m_mapTextures.begin(); 
			iterTexture != m_mapTextures.end(); ++iterTexture)
		{
			pkDevice->SetTexture(iterTexture->first, iterTexture->second);
			CORE3D_SAFE_RELEASE(iterTexture->second);
		}
		m_mapTextures.clear();

		for(std::map<UINT32, UINT32>::iterator iterTextureSamplerState = m_mapTextureSamplerStates.begin(); 
			iterTextureSamplerState != m_mapTextureSamplerStates.end(); ++iterTextureSamplerState)
		{
			UINT32 uiSamplerNumber	= iterTextureSamplerState->first / (UINT32)TSS_NUMTEXTURESAMPLERSTATES;
			UINT32 uiSamplerState	= iterTextureSamplerState->first - uiSamplerNumber * (UINT32)TSS_NUMTEXTURESAMPLERSTATES;
			pkDevice->SetTextureSamplerState(uiSamplerNumber, (TextureSamplerState)uiSamplerState, iterTextureSamplerState->second);
		}
		m_mapTextureSamplerStates.clear();
	}

	FWGraphics* FWStateBlock::GetGraphics()
	{
		return m_pkGraphics;
	}
}