#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Framework Library for Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////
#include "FWType.h"
#include <stack>

namespace Core3D
{
	class FWApplication;
	class FWCamera;
	class FWStateBlock;
	struct CreationFlags;
	class FWGraphics
	{
	protected:
		friend class FWApplication;
		friend class FWStateBlock;
		friend class FWCamera;
	#	ifdef WIN32
		friend class FWApplicationWin32;
	#	endif

		FWGraphics(FWApplication* pkApp);
		~FWGraphics();

		bool Initialize(const CreationFlags& rkCreateFlags);
		void SetCurrentCamera(FWCamera* pkCamera);
	public:
		FWApplication*	GetApplication();
		FWCamera*		GetCurrentCamera();

		inline Object*	GetObject() {return m_pkObject;}
		inline Device*	GetDevice() {return m_pkDevice;}

		//------------------------------------------------------------------
		// COMMENT : Sub-systems
		void			PushStateBlock();
		void			PopStateBlock();
		//------------------------------------------------------------------

		//------------------------------------------------------------------
		// COMMENT : Use these functions instead of the device's set-functions
		Result			SetRenderState(RenderState eRenderState, UINT32 uiValue);
		Result			SetVertexFormat(VertexFormat* pkVertexFormat);
		void			SetPrimitiveAssembler(PrimitiveAssembler* pkPrimitiveAssembler);
		Result			SetVertexShader(VertexShader* pkVertexShader);
		void			SetTriangleShader(TriangleShader* pkTriangleShader);
		Result			SetPixelShader(PixelShader* pkPixelShader);
		Result			SetIndexBuffer(IndexBuffer* pkIndexBuffer);
		Result			SetVertexStream(UINT32 uiStream, VertexBuffer* pkVertexBuffer, UINT32 uiOffset, UINT32 uiStride);
		Result			SetTexture(UINT32 uiSampler, BaseTexture* pkTexture);
		Result			SetTextureSamplerState(UINT32 uiSampler, TextureSamplerState eTextureSamplerState, UINT32 uiState);
		void			SetRenderTarget(RenderTarget* pkRenderTarget);
		Result			SetScissorRect(const Rect& rkScissorRect);
		//------------------------------------------------------------------
	private:
		FWApplication*				m_pkApplication;
		Object*						m_pkObject;
		Device*						m_pkDevice;
		std::stack<FWStateBlock*>	m_kStateBlocks;
	protected:
		FWCamera*					m_pkCurrentCamera;
	};
}