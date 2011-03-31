#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Framework Library for Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////
#include "FWType.h"
#include <map>

namespace Core3D
{
	class FWCamera;
	class FWGraphics;
	class FWStateBlock
	{
	protected:
		friend class FWGraphics;
		FWStateBlock(FWGraphics* pkGraphics);
	public:
		~FWStateBlock();

		void	RestoreStates();
		
		Result	SetRenderState(RenderState eRenderState, UINT32 uiValue);
		Result	SetVertexFormat(VertexFormat* pkVertexFormat);
		void	SetPrimitiveAssembler(PrimitiveAssembler* pkPrimitiveAssembler);
		Result	SetVertexShader(VertexShader* pkVertexShader);
		void	SetTriangleShader(TriangleShader* pkTriangleShader);
		Result	SetPixelShader(PixelShader* pkPixelShader);
		Result	SetIndexBuffer(IndexBuffer* pkIndexBuffer);
		Result	SetVertexStream(UINT32 uiStream, VertexBuffer* pkVertexBuffer, UINT32 uiOffset, UINT32 uiStride);
		Result	SetTexture(UINT32 uiSampler, BaseTexture* pkTexture);
		Result	SetTextureSamplerState(UINT32 uiSampler, TextureSamplerState eTextureSamplerState, UINT32 uiState);
		void	SetRenderTarget(RenderTarget* pkRenderTarget);
		Result	SetScissorRect(const Rect& rkScissorRect);
		void	SetCurrentCamera(FWCamera* pkCamera);
		FWGraphics* GetGraphics();
	private:
		struct VertexStreamInfo
		{
			VertexBuffer*	pkVertexBuffer;
			UINT32			uiOffset;
			UINT32			uiStride;
		};
	private:
		FWGraphics*						m_pkGraphics;
		std::map<RenderState, UINT32>	m_mapRenderStates;
		
		bool							m_bChangedVertexFormat;
		VertexFormat*					m_pkVertexFormat;

		bool							m_bChangedPrimitiveAssembler;
		PrimitiveAssembler*				m_pkPrimitiveAssembler;

		bool							m_bChangedVertexShader;
		VertexShader*					m_pkVertexShader;

		bool							m_bChangedTriangleShader;
		TriangleShader*					m_pkTriangleShader;

		bool							m_bChangedPixelShader;
		PixelShader*					m_pkPixelShader;

		bool							m_bChangedIndexBuffer;
		IndexBuffer*					m_pkIndexBuffer;

		std::map<UINT32, VertexStreamInfo> m_mapVertexStreams;
		std::map<UINT32, BaseTexture*>	m_mapTextures;
		std::map<UINT32, UINT32>		m_mapTextureSamplerStates;

		bool							m_bChangedRenderTarget;
		RenderTarget*					m_pkRenderTarget;

		bool							m_bChangedScissorRect;
		Rect							m_kScissorRect;

		bool							m_bChangedCamera;
		FWCamera*						m_pkCamera;
	};
}