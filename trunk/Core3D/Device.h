#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DTypes.h"

namespace Core3D
{
	class Object;
	class PresentTarget;
	class VertexFormat;
	class PrimitiveAssembler;
	class VertexShader;
	class TriangleShader;
	class PixelShader;
	class IndexBuffer;
	class RenderTarget;
	class VertexBuffer;
	class BaseTexture;
	class Surface;
	class Texture;
	class CubeTexture;
	class Volume;
	class VolumeTexture;
	class Device : public RefObject
	{
	public:
		Object* GetObject();
		const DeviceParameters& GetDeviceParameters();
		Result	Present(RenderTarget* pkRenderTarget);
		
		Result	DrawPrimitive(PrimitiveType ePrimitiveType, UINT32 uiStartVertex, UINT32 uiPrimitiveCount);
		Result	DrawIndexedPrimitive(PrimitiveType ePrimitiveType, UINT32 uiBaseVertexIndex, UINT32 uiMinIndex, 
			UINT32 uiNumVertices, UINT32 uiStartIndex, UINT32 uiPrimitiveCount);
		Result	DrawDynamicPrimitive(UINT uiStartVertex, UINT32 uiNumVertices);
		
		Result	CreateVertexFormat(VertexFormat** ppkVertexFormat, const VertexElement* pkVertexDeclaration, UINT32 uiVertexDeclSize);
		Result	CreateIndexBuffer(IndexBuffer** ppkIndexBuffer, UINT32 uiLength, Format eFormat);
		Result	CreateVertexBuffer(VertexBuffer** ppkVertexBuffer, UINT32 uiLength);
		Result	CreateSurface(Surface** ppkSurface, UINT32 uiWidth, UINT32 uiHeight, Format eFormat);
		Result	CreateTexture(Texture** ppkTexture, UINT32 uiWidth, UINT32 uiHeight, UINT32 uiMipLevels, Format eFormat);
		Result	CreateCubeTexture(CubeTexture** ppkCubeTexture, UINT32 uiEdgeLength, UINT32 uiMipLevels, Format eFormat);
		Result	CreateVolume(Volume** ppkVolume, UINT32 uiWidth, UINT32 uiHeight, UINT32 uiDepth, Format eFormat);
		Result	CreateVolumeTexture(VolumeTexture** ppkVolumeTexture, UINT32 uiWidth, UINT32 uiHeight, UINT32 uiDepth, 
			UINT32 uiMipLevels, Format eFormat);
		Result	CreateRenderTarget(RenderTarget** ppkRenderTarget);
		
		Result	SetRenderState(RenderState eRenderState, UINT32 uiValue);
		Result	GetRenderState(RenderState eRenderState, UINT32& ruiValue);
		
		Result	SetVertexFormat(VertexFormat* pkVertexFormat);
		VertexFormat* GetVertexFormat();

		void	SetPrimitiveAssembler(PrimitiveAssembler* pkPrimitiveAsembler);
		PrimitiveAssembler* GetPrimitiveAssembler();

		Result SetVertexShader(VertexShader* pkVertexShader);
		VertexShader* GetVertexShader();

		void	SetTriangleShader(TriangleShader* pkTriangleShader);
		TriangleShader* GetTriangleShader();

		Result	SetPixelShader(PixelShader* pkPixelShader);
		PixelShader* GetPixelShader();

		Result	SetIndexBuffer(IndexBuffer* pkIndexBuffer);
		IndexBuffer* GetIndexBuffer();

		Result	SetVertexStream(UINT32 uiStreamNumber, VertexBuffer* pkVertexBuffer, UINT32 uiOffset, UINT32 uiStride);
		Result	GetVertexStream(UINT32 uiStreamNumber, VertexBuffer** ppkVertexBuffer, UINT32& ruiOffset, UINT32& ruiStride);

		Result	SetTexture(UINT32 uiSamplerNumber, BaseTexture* pkTexture);
		Result	GetTexture(UINT32 uiSamplerNumber, BaseTexture** ppkTexture);

		Result	SetTextureSamplerState(UINT32 uiSamplerNumber, TextureSamplerState eTextureSamplerState, UINT32 uiState);
		Result	GetTextureSamplerState(UINT32 uiSamplerNumber, TextureSamplerState eTextureSamplerState, UINT32& ruiState);

		Result	SampleTexture(Vector4& rkColor, UINT32 uiSamplerNumber, FLOAT32 fU, FLOAT32 fV, FLOAT32 fW, 
			const Vector4* pkXGradient, const Vector4* pkYGradient);

		void	SetRenderTarget(RenderTarget* pkRenderTarget);
		RenderTarget* GetRenderTarget();

		Result	SetScissorRect(const Rect& rcScissorRect);
		Rect	GetScissorRect();

		Result	SetDepthBounds(FLOAT32 fMinZ, FLOAT32 fMaxZ);
		void	GetDepthBounds(FLOAT32& rfMinZ, FLOAT32& rfMaxZ);

		Result	SetClippingPlane(ClippingPlanes eIndex, const Plane* pkPlane);
		Result	GetClippingPlane(ClippingPlanes eIndex, Plane& rkPlane);

		UINT32	GetRenderedPixels();
	private:
		void	SetDefaultRenderStates();
		void	SetDefaultTextureSamplerStates();
		void	SetDefaultClippingPlanes();

		Result	PreRender();
		void	PostRender();

		Result	DecodeVertexStream(VertexShaderInput& rkVertexShaderInput, UINT32 uiVertex);
		Result	FetchVertex(VertexCacheEntry** ppkVertex, UINT32 uiVertex);

		void	ProcessTriangle(const VertexShaderOutput* pkVSOutput0, 
			const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2);

		void	InterpolateVertexShaderInput(VertexShaderInput* pkVSInput, const VertexShaderInput* pkVSInputA, 
			const VertexShaderInput* pkVSInputB, FLOAT32 fInterpolation);
		void	InterpolateVertexShaderOutput(VertexShaderOutput* pkVSOutput, const VertexShaderOutput* pkVSOutputA, 
			const VertexShaderOutput* pkVSOutputB, FLOAT32 fInterpolation);

		void	MultiplyVertexShaderOutputRegisters(VertexShaderOutput* pkDest, const VertexShaderOutput* pkSrc, FLOAT32 fVal);

		void	SubdivideTriangleSimple(UINT32 uiSubdivisionLevel, const VertexShaderOutput* pkVSOutput0, 
			const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2);
		void	SubdivideTriangleSmooth(UINT32 uiSubdivisionLevel, const VertexShaderOutput* pkVSOutput0, 
			const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2);
		void	SubdivideTriangleAdaptive(const VertexShaderOutput* pkVSOutput0, const VertexShaderOutput* pkVSOutput1, 
			const VertexShaderOutput* pkVSOutput2);

		void	SubdivideTriangleAdaptiveSubdivideEdges(UINT32 uiSubdivisionLevel, 
			const VertexShaderOutput* pkVSOutputEdge0, const VertexShaderOutput* pkVSOutputEdge1, 
			const VertexShaderOutput* pkVSOutputCenter);
		void	SubdivideTriangleAdaptiveSubdivideInnerPart(UINT32 uiSubdivisionLevel, 
			const VertexShaderOutput* pkVSOutput0, const VertexShaderOutput* pkVSOutput1, 
			const VertexShaderOutput* pkVSOutput2);

		UINT32	ClipToPlane(UINT32 uiNumVertices, UINT32 uiStage, const Plane& rkPlane, bool bHomogenous);

		void	DrawTriangle(const VertexShaderOutput* pkVSOutput0, const VertexShaderOutput* pkVSOutput1, 
			const VertexShaderOutput* pkVSOutput2);

		bool	CullTriangle(const VertexShaderOutput* pkVSOutput0, 
			const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2);

		void	ProjectVertex(VertexShaderOutput* pkVSOutput);

		void	CalculateTriangleGradients(const VertexShaderOutput* pkVSOutput0, 
			const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2);
		void	SetVSOutputFromGradient(VertexShaderOutput* pkVSOutput, FLOAT32 fX, FLOAT32 fY);
		void	StepXVSOutputFromGradient(VertexShaderOutput* pkVSOutput);

		void	RasterizeTriangle(const VertexShaderOutput* pkVSOutput0, 
			const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2);
		void	RasterizeLine(const VertexShaderOutput* pkVSOutput0, const VertexShaderOutput* pkVSOutput1);
		void	RasterizeScanlineColorOnly(UINT32 uiY, UINT32 uiX, UINT32 uiX2, VertexShaderOutput* pkVSOutput);
		void	RasterizeScanlineColorOnlyMightKillPixels(UINT32 uiY, UINT32 uiX, UINT32 uiX2, 
			VertexShaderOutput* pkVSOutput);
		void	RasterizeScanlineColorDepth(UINT32 uiY, UINT32 uiX, UINT32 uiX2, VertexShaderOutput* pkVSOutput);

		void	DrawPixelColorOnly(UINT32 uiX, UINT32 uiY, const VertexShaderOutput* pkVSOutput);
		void	DrawPixelColorDepth(UINT32 uiX, UINT32 uiY, const VertexShaderOutput* pkVSOutput);
	protected:
		friend class Object;

		Device(Object* pkParent, const DeviceParameters* pkDeviceParameters);
		~Device();

		Result Create();
	private:
		struct VertexStream
		{
			VertexBuffer*	pkVertexBuffer;
			UINT32			uiOffset;
			UINT32			uiStride;
		};

		struct TextureSampler
		{
			BaseTexture*		pkTexture;
			UINT32				auiTexureSamplerStates[TSS_NUMTEXTURESAMPLERSTATES];
			TextureSampleInput	eTextureSampleInput;
		};

		struct RenderInfo
		{
			ShaderRegType	aeVSInputs[VERTEX_SHADER_REGISTERS];
			ShaderRegType	aeVSOutputs[PIXEL_SHADER_REGISTERS];

			FLOAT32*		pfFrameData;
			UINT32			uiColorFloats;
			UINT32			uiColorBufferPitch;
			bool			bColorWrite;

			FLOAT32*		pfDepthData;
			UINT32			uiDepthBufferPitch;
			CmpFunc			eDepthCompare;
			bool			bDepthWrite;

			void (Device::*pfnRasterizeScanLine)(UINT32, UINT32, UINT32, VertexShaderOutput*);
			void (Device::*pfnDrawPixel)(UINT32, UINT32, const VertexShaderOutput*);

			UINT32			uiRenderedPixels;
			Rect			rcViewportRect;
			Plane			akClippingPlanes[CP_NUMPLANES];
			bool			abClippingPlaneEnabled[CP_NUMPLANES];
			Plane			akScissorPlanes[4];
		};
	private:
		Object*				m_pkParent;
		DeviceParameters	m_kDeviceParameters;
		PresentTarget*		m_pkPresentTarget;
		UINT32				m_auiRenderStates[RS_NUMRENDERSTATES];

		VertexFormat*		m_pkVertexFormat;
		PrimitiveAssembler*	m_pkPrimitiveAssembler;
		VertexShader*		m_pkVertexShader;
		TriangleShader*		m_pkTriangleShader;
		PixelShader*		m_pkPixelShader;
		IndexBuffer*		m_pkIndexBuffer;

		VertexStream		m_akVertexStreams[MAX_VERTEX_STREAMS];
		TextureSampler		m_akTextureSamplers[MAX_TEXTURE_SAMPLERS];

		RenderTarget*		m_pkRenderTarget;
		Rect				m_rcScissorRect;

		RenderInfo			m_kRenderInfo;
		TriangleInfo		m_kTriangleInfo;
		UINT32				m_uiNumValidCacheEntries;
		UINT32				m_uiFetchedVertices;
		VertexCacheEntry	m_akVertexCache[VERTEX_CACH_SIZE];

		VertexShaderOutput	m_akClipVertices[20];
		UINT32				m_uiNextFreeClipVertex;
		VertexShaderOutput*	m_aapkClipVertices[2][20];
	};
}