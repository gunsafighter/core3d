#include "Device.h"
#include "Object.h"
#include "BaseTexture.h"
#include "CubeTexture.h"
#include "IndexBuffer.h"
#include "PresentTarget.h"
#include "RenderTarget.h"
#include "Shaders.h"
#include "Surface.h"
#include "Texture.h"
#include "PrimitiveAssembler.h"
#include "VertexBuffer.h"
#include "VertexFormat.h"
#include "Volume.h"
#include "VolumeTexture.h"

namespace Core3D
{
	Device::Device(Object* pkParent, const DeviceParameters* pkDeviceParameters)
		: m_pkParent(pkParent)
		, m_pkPresentTarget(NULL)
		, m_pkVertexFormat(NULL)
		, m_pkPrimitiveAssembler(NULL)
		, m_pkVertexShader(NULL)
		, m_pkTriangleShader(NULL)
		, m_pkPixelShader(NULL)
		, m_pkIndexBuffer(NULL)
		, m_pkRenderTarget(NULL)
	{
		m_pkParent->AddRef();

		memcpy(&m_kDeviceParameters, pkDeviceParameters, sizeof(DeviceParameters));
		
		memset(m_akVertexStreams,	0, sizeof(m_akVertexStreams));
		memset(m_akTextureSamplers, 0, sizeof(m_akTextureSamplers));
		memset(&m_rcScissorRect,	0, sizeof(m_rcScissorRect));
		memset(&m_kRenderInfo,		0, sizeof(m_kRenderInfo));
		memset(&m_kTriangleInfo,	0, sizeof(m_kTriangleInfo));
		memset(m_akVertexCache,		0, sizeof(m_akVertexCache));
		memset(m_akClipVertices,	0, sizeof(m_akClipVertices));
		memset(m_aapkClipVertices,	0, sizeof(m_aapkClipVertices));

		SetDefaultRenderStates();
		SetDefaultTextureSamplerStates();
		SetDefaultClippingPlanes();
	}

	Device::~Device()
	{
		CORE3D_SAFE_RELEASE(m_pkPresentTarget);
		CORE3D_SAFE_RELEASE(m_pkParent);
	}

	Result Device::Create()
	{
		m_pkPresentTarget = new PresentTargetWin32(this);

		if(NULL == m_pkPresentTarget)
		{
			CORE3D_ERROR(_T("Device::Create() - Out of memory, cannot create present target.\n"));
			return OUT_OF_MEMORY;
		}
		return m_pkPresentTarget->Create();
	}

	void Device::SetDefaultRenderStates()
	{
		SetRenderState(RS_ZENABLE, BT_TRUE);
		SetRenderState(RS_ZWRITEENABLE, BT_TRUE);
		SetRenderState(RS_ZFUNC, CMP_LESS);

		SetRenderState(RS_COLORWRITEENABLE, BT_TRUE);
		SetRenderState(RS_FILLMODE, FILL_SOLID);
		SetRenderState(RS_CULLMODE, CULL_CCW);

		SetRenderState(RS_SUBDIVISIONMODE, SUBDIV_NONE);
		SetRenderState(RS_SUBDIVISIONLEVELS, 1);
		SetRenderState(RS_SUBDIVISIONPOSITIONREGISTER, 0);
		SetRenderState(RS_SUBDIVISIONNORMALREGISTER, 1);
		const FLOAT32 DEFAULT_SUBDIVISION_MAX_SCREENAREA = 1.0f;
		SetRenderState(RS_SUBDIVISIONMAXSCREENAREA, *((UINT32*)&DEFAULT_SUBDIVISION_MAX_SCREENAREA));
		SetRenderState(RS_SUBDIVISIONMAXINNERLEVELS, 1);

		SetRenderState(RS_SCISSORTESTENABLE, BT_FALSE);
		SetRenderState(RS_LINETHICKNESS, 1);
	}

	void Device::SetDefaultTextureSamplerStates()
	{
		const FLOAT32 MIP_LOD_BIAS	= 0.0f;
		const FLOAT32 MAX_MIP_LEVEL = 16.0f;
		for(UINT32 uiTextureSampler = 0; uiTextureSampler < MAX_TEXTURE_SAMPLERS; ++uiTextureSampler)
		{
			SetTextureSamplerState(uiTextureSampler, TSS_ADDRESSU, TA_WRAP);
			SetTextureSamplerState(uiTextureSampler, TSS_ADDRESSV, TA_WRAP);
			SetTextureSamplerState(uiTextureSampler, TSS_ADDRESSW, TA_WRAP);

			SetTextureSamplerState(uiTextureSampler, TSS_MINFILTER, TF_LINEAR);
			SetTextureSamplerState(uiTextureSampler, TSS_MAGFILTER, TF_LINEAR);
			SetTextureSamplerState(uiTextureSampler, TSS_MIPFILTER, TF_POINT);

			SetTextureSamplerState(uiTextureSampler, TSS_MIPLODBIAS, *((UINT32*)&MIP_LOD_BIAS));
			SetTextureSamplerState(uiTextureSampler, TSS_MAXMIPLEVEL, *((UINT32*)&MAX_MIP_LEVEL));
		}
	}

	void Device::SetDefaultClippingPlanes()
	{
		m_kRenderInfo.akClippingPlanes[CP_LEFT]		= Plane( 1.0f,  0.0f,  0.0f, 1.0f);
		m_kRenderInfo.akClippingPlanes[CP_RIGHT]	= Plane(-1.0f,  0.0f,  0.0f, 1.0f);
		m_kRenderInfo.akClippingPlanes[CP_TOP]		= Plane( 0.0f, -1.0f,  0.0f, 1.0f);
		m_kRenderInfo.akClippingPlanes[CP_BOTTOM]	= Plane( 0.0f,  1.0f,  0.0f, 1.0f);
		m_kRenderInfo.akClippingPlanes[CP_NEAR]		= Plane( 0.0f,  0.0f,  1.0f, 0.0f);
		m_kRenderInfo.akClippingPlanes[CP_FAR]		= Plane( 0.0f,  0.0f, -1.0f, 1.0f);

		// COMMENT : Enable the default clipping planes.
		for(UINT32 uiPlane = CP_LEFT; uiPlane <= CP_FAR; ++uiPlane)
		{
			m_kRenderInfo.abClippingPlaneEnabled[uiPlane] = true;
		}
	}

	Result Device::SetRenderState(RenderState eRenderState, UINT32 uiValue)
	{
		if(eRenderState >= RS_NUMRENDERSTATES)
		{
			CORE3D_ERROR(_T("Device::SetRenderState() - Invalid render-state.\n"));
			return INVALID_PARAMETERS;
		}
		m_auiRenderStates[eRenderState] = uiValue;
		return OK;
	}

	Result Device::GetRenderState(RenderState eRenderState, UINT32& ruiValue)
	{
		if(eRenderState >= RS_NUMRENDERSTATES)
		{
			ruiValue = 0;
			CORE3D_ERROR(_T("Device::GetRenderState() - Invalid render-state.\n"));
			return INVALID_PARAMETERS;
		}
		ruiValue = m_auiRenderStates[eRenderState];
		return OK;
	}

	Result Device::SetVertexFormat(VertexFormat* pkVertexFormat)
	{
		m_pkVertexFormat = pkVertexFormat;
		if(NULL == pkVertexFormat) {return OK;}

		// COMMENT : Initialize part of internal render-information structure.
		for(UINT32 uiReg = 0; uiReg < VERTEX_SHADER_REGISTERS; ++uiReg)
		{
			m_kRenderInfo.aeVSInputs[uiReg] = SRT_UNUSED;
		}

		const VertexElement* pkCurrentVertexElement = m_pkVertexFormat->GetElements();
		UINT32 uiElement = m_pkVertexFormat->GetNumVertexElements();
		while(uiElement--)
		{
			switch(pkCurrentVertexElement->eType)
			{
			case VET_FLOAT32: m_kRenderInfo.aeVSInputs[pkCurrentVertexElement->uiRegister] = SRT_FLOAT32; break;
			case VET_VECTOR2: m_kRenderInfo.aeVSInputs[pkCurrentVertexElement->uiRegister] = SRT_VECTOR2; break;
			case VET_VECTOR3: m_kRenderInfo.aeVSInputs[pkCurrentVertexElement->uiRegister] = SRT_VECTOR3; break;
			case VET_VECTOR4: m_kRenderInfo.aeVSInputs[pkCurrentVertexElement->uiRegister] = SRT_VECTOR4; break;
			default: CORE3D_ERROR(_T("Device::SetVertexFormat() - Invalid vertex element type.\n")); return INVALID_PARAMETERS;
			}
			++pkCurrentVertexElement;
		}
		return OK;
	}

	VertexFormat* Device::GetVertexFormat()
	{
		if(NULL != m_pkVertexFormat) {m_pkVertexFormat->AddRef();}
		return m_pkVertexFormat;
	}

	void Device::SetPrimitiveAssembler(PrimitiveAssembler* pkPrimitiveAsembler)
	{
		m_pkPrimitiveAssembler = pkPrimitiveAsembler;
	}

	PrimitiveAssembler* Device::GetPrimitiveAssembler()
	{
		if(NULL != m_pkPrimitiveAssembler) {m_pkPrimitiveAssembler->AddRef();}
		return m_pkPrimitiveAssembler;
	}

	Result Device::SetVertexShader(VertexShader* pkVertexShader)
	{
		if(NULL == pkVertexShader)
		{
			m_pkVertexShader = pkVertexShader;
			return OK;
		}

		// COMMENT : Validate vertex shader register output types.
		for(UINT32 uiReg = 0; uiReg < VERTEX_SHADER_REGISTERS; ++uiReg)
		{
			UINT32 uiType = pkVertexShader->GetOutputRegisters(uiReg);
			if((uiType < SRT_UNUSED) || (uiType > SRT_VECTOR4))
			{
				CORE3D_ERROR(_T("Device::SetVertexShader() - Type of vertex-shader output register is invalid.\n"));
				return INVALID_STATE;
			}
		}
		m_pkVertexShader = pkVertexShader;
		return OK;
	}

	VertexShader* Device::GetVertexShader()
	{
		if(NULL != m_pkVertexShader) {m_pkVertexShader->AddRef();}
		return m_pkVertexShader;
	}

	void Device::SetTriangleShader(TriangleShader* pkTriangleShader)
	{
		m_pkTriangleShader = pkTriangleShader;
	}

	TriangleShader* Device::GetTriangleShader()
	{
		if(NULL != m_pkTriangleShader) {m_pkTriangleShader->AddRef();}
		return m_pkTriangleShader;
	}

	Result Device::SetPixelShader(PixelShader* pkPixelShader)
	{
		m_pkPixelShader = pkPixelShader;
		return OK;
	}

	PixelShader* Device::GetPixelShader()
	{
		if(NULL != m_pkPixelShader) {m_pkPixelShader->AddRef();}
		return m_pkPixelShader;
	}

	Result Device::SetIndexBuffer(IndexBuffer* pkIndexBuffer)
	{
		m_pkIndexBuffer = pkIndexBuffer;
		return OK;
	}

	IndexBuffer* Device::GetIndexBuffer()
	{
		if(NULL != m_pkIndexBuffer) {m_pkIndexBuffer->AddRef();}
		return m_pkIndexBuffer;
	}

	Result Device::SetVertexStream(UINT32 uiStreamNumber, VertexBuffer* pkVertexBuffer, UINT32 uiOffset, UINT32 uiStride)
	{
		if(uiStreamNumber >= MAX_VERTEX_STREAMS)
		{
			CORE3D_ERROR(_T("Device::SetVertexStream() - Stream number exceeds number of available vertex streams.\n"));
			return INVALID_PARAMETERS;
		}

		if(0 == uiStride)
		{
			CORE3D_ERROR(_T("Device::SetVertexStream() - Stride is 0.\n"));
			return INVALID_PARAMETERS;
		}

		m_akVertexStreams[uiStreamNumber].pkVertexBuffer	= pkVertexBuffer;
		m_akVertexStreams[uiStreamNumber].uiOffset			= uiOffset;
		m_akVertexStreams[uiStreamNumber].uiStride			= uiStride;
		return OK;
	}

	Result Device::GetVertexStream(UINT32 uiStreamNumber, VertexBuffer** ppkVertexBuffer, UINT32& ruiOffset, UINT32& ruiStride)
	{
		if(uiStreamNumber >= MAX_VERTEX_STREAMS)
		{
			if(NULL != ppkVertexBuffer) {*ppkVertexBuffer = NULL;}
			ruiOffset = 0;
			ruiStride = 0;
			CORE3D_ERROR(_T("Device::GetVertexStream() - Stream number exceeds number of available vertex stream.\n"));
			return INVALID_PARAMETERS;
		}

		if(NULL != ppkVertexBuffer)
		{
			*ppkVertexBuffer = m_akVertexStreams[uiStreamNumber].pkVertexBuffer;
			if(NULL != m_akVertexStreams[uiStreamNumber].pkVertexBuffer) {m_akVertexStreams[uiStreamNumber].pkVertexBuffer->AddRef();}
		}
		ruiOffset = m_akVertexStreams[uiStreamNumber].uiOffset;
		ruiStride = m_akVertexStreams[uiStreamNumber].uiStride;
		return OK;
	}

	Result Device::SetTexture(UINT32 uiSamplerNumber, BaseTexture* pkTexture)
	{
		if(uiSamplerNumber >= MAX_TEXTURE_SAMPLERS)
		{
			CORE3D_ERROR(_T("Device::SetTexture() - Sampler number exceeds number of available texture samplers.\n"));
			return INVALID_PARAMETERS;
		}

		m_akTextureSamplers[uiSamplerNumber].pkTexture = pkTexture;
		if(NULL != pkTexture)
		{
			m_akTextureSamplers[uiSamplerNumber].eTextureSampleInput = pkTexture->GetTextureSampleInput();
		}
		return OK;
	}

	Result Device::GetTexture(UINT32 uiSamplerNumber, BaseTexture** ppkTexture)
	{
		if(uiSamplerNumber >= MAX_TEXTURE_SAMPLERS)
		{
			if(NULL != ppkTexture) {*ppkTexture = NULL;}
			CORE3D_ERROR(_T("Device::GetTexture() - Sampler number exceeds number of available texture samplers.\n"));
			return INVALID_PARAMETERS;
		}

		if(NULL == ppkTexture)
		{
			CORE3D_ERROR(_T("Device::GetTexture() - Parameter output texture pointer to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		*ppkTexture = m_akTextureSamplers[uiSamplerNumber].pkTexture;
		if(NULL != m_akTextureSamplers[uiSamplerNumber].pkTexture) {m_akTextureSamplers[uiSamplerNumber].pkTexture->AddRef();}
		return OK;
	}

	Result Device::SetTextureSamplerState(UINT32 uiSamplerNumber, TextureSamplerState eTextureSamplerState, UINT32 uiState)
	{
		if(uiSamplerNumber >= MAX_TEXTURE_SAMPLERS)
		{
			CORE3D_ERROR(_T("Device::SetTextureSamplerState() - Sampler number exceeds number of available texture samplers.\n"));
			return INVALID_PARAMETERS;
		}

		if(eTextureSamplerState >= TSS_NUMTEXTURESAMPLERSTATES)
		{
			CORE3D_ERROR(_T("Device::SetTextureSamplerState() - Invalid texture sampler state.\n"));
			return INVALID_PARAMETERS;
		}
		m_akTextureSamplers[uiSamplerNumber].auiTexureSamplerStates[eTextureSamplerState] = uiState;
		return OK;
	}

	Result Device::GetTextureSamplerState(UINT32 uiSamplerNumber, TextureSamplerState eTextureSamplerState, UINT32& ruiState)
	{
		if(uiSamplerNumber >= MAX_TEXTURE_SAMPLERS)
		{
			ruiState = 0;
			CORE3D_ERROR(_T("Device::GetTextureSamplerState() - Sampler number exceeds number of available texture samplers.\n"));
			return INVALID_PARAMETERS;
		}

		if(eTextureSamplerState >= TSS_NUMTEXTURESAMPLERSTATES)
		{
			CORE3D_ERROR(_T("Device::GetTextureSamplerState() - Invalid texture sampler state.\n"));
			return INVALID_PARAMETERS;
		}
		ruiState = m_akTextureSamplers[uiSamplerNumber].auiTexureSamplerStates[eTextureSamplerState];
		return OK;
	}

	Result Device::SampleTexture(Vector4& rkColor, UINT32 uiSamplerNumber, FLOAT32 fU, FLOAT32 fV, FLOAT32 fW, const Vector4* pkXGradient, const Vector4* pkYGradient)
	{
		if(uiSamplerNumber >= MAX_TEXTURE_SAMPLERS)
		{
			rkColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
			CORE3D_ERROR(_T("Device::SampleTexture() - Sample number exceeds number of available texture samplers.\n"));
			return INVALID_PARAMETERS;
		}

		const TextureSampler& rkTextureSampler	= m_akTextureSamplers[uiSamplerNumber];
		BaseTexture* pkTexture					= rkTextureSampler.pkTexture;
		if(NULL == pkTexture)
		{
			rkColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
			return OK;
		}

		switch(rkTextureSampler.eTextureSampleInput)
		{
		case TSI_VECTOR:
			if((0.0f == fU) && (0.0f == fV) && (0.0f == fW))
			{
				rkColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
				CORE3D_ERROR(_T("Device::SampleTexture() - Sampling vector [u, v, w] = [0, 0, 0].\n"));
				return INVALID_PARAMETERS;
			}
			break;
		case TSI_3COORDS:
			switch(rkTextureSampler.auiTexureSamplerStates[TSS_ADDRESSW])
			{
			case TA_WRAP:	fW -= ((FLOAT32)Core3D::FtoL(fW));
			case TA_CLAMP:	fW = Core3D::Saturate(fW);			break;
			default:
				rkColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
				CORE3D_ERROR(_T("Device::SampleTexture() - Value of texture sampler state TSS_ADDRESSW is invalid.\n"));
				return INVALID_STATE;
			}
		case TSI_2COORDS:
			switch(rkTextureSampler.auiTexureSamplerStates[TSS_ADDRESSU])
			{
			case TA_WRAP:	fU -= ((FLOAT32)Core3D::FtoL(fU));
			case TA_CLAMP:	fU = Core3D::Saturate(fU);			break;
			default:
				rkColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
				CORE3D_ERROR(_T("Device::SampleTexture() - Value of texture sampler state TSS_ADDRESSU is invalid.\n"));
				return INVALID_STATE;
			}

			switch(rkTextureSampler.auiTexureSamplerStates[TSS_ADDRESSV])
			{
			case TA_WRAP:	fV -= ((FLOAT32)Core3D::FtoL(fV));
			case TA_CLAMP:	fV = Core3D::Saturate(fV);			break;
			default:
				rkColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
				CORE3D_ERROR(_T("Device::SampleTexture() - Value of texture sampler state TSS_ADDRESSV is invalid.\n"));
				return INVALID_STATE;
			}
			break;
		default:
			rkColor = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
			CORE3D_ERROR(_T("Device::SampleTexture() - Invalid texture sampling input.\n"));
			return INVALID_STATE;
		}

		return pkTexture->SampleTexture(rkColor, fU, fV, fW, pkXGradient, pkYGradient, rkTextureSampler.auiTexureSamplerStates);
	}

	void Device::SetRenderTarget(RenderTarget* pkRenderTarget)
	{
		m_pkRenderTarget = pkRenderTarget;
	}

	RenderTarget* Device::GetRenderTarget()
	{
		if(NULL != m_pkRenderTarget) {m_pkRenderTarget->AddRef();}
		return m_pkRenderTarget;
	}

	Result Device::SetScissorRect(const Rect& rcScissorRect)
	{
		if(	(rcScissorRect.uiLeft >= rcScissorRect.uiRight) || 
			(rcScissorRect.uiTop >= rcScissorRect.uiBottom) )
		{
			CORE3D_ERROR(_T("Device::SetScissorRect() - Invalid scissor rect.\n"));
			return INVALID_PARAMETERS;
		}

		m_rcScissorRect = rcScissorRect;
		// COMMENT : Construct planes for clipping to the scissor rect.
		m_kRenderInfo.akScissorPlanes[0] = Plane( 1.0f,  0.0f, 0.0f, -(FLOAT32)m_rcScissorRect.uiLeft);
		m_kRenderInfo.akScissorPlanes[1] = Plane(-1.0f,  0.0f, 0.0f,  (FLOAT32)m_rcScissorRect.uiRight);
		m_kRenderInfo.akScissorPlanes[2] = Plane( 0.0f,  1.0f, 0.0f, -(FLOAT32)m_rcScissorRect.uiTop);
		m_kRenderInfo.akScissorPlanes[3] = Plane( 0.0f, -1.0f, 0.0f,  (FLOAT32)m_rcScissorRect.uiBottom);

		return OK;
	}

	Rect Device::GetScissorRect()
	{
		return m_rcScissorRect;
	}

	Result Device::SetDepthBounds(FLOAT32 fMinZ, FLOAT32 fMaxZ)
	{
		if( (fMinZ < 0.0f) || (fMinZ > 1.0f) || 
			(fMaxZ < 0.0f) || (fMaxZ > 1.0f) || 
			(fMinZ >= fMaxZ) )
		{
			CORE3D_ERROR(_T("Device::SetDepthBounds() - Invalid depth bounds.\n"));
			return INVALID_PARAMETERS;
		}

		m_kRenderInfo.akClippingPlanes[CP_NEAR].d	= fMinZ;
		m_kRenderInfo.akClippingPlanes[CP_FAR].d	= fMaxZ;

		return OK;
	}

	void Device::GetDepthBounds(FLOAT32& rfMinZ, FLOAT32& rfMaxZ)
	{
		rfMinZ = m_kRenderInfo.akClippingPlanes[CP_NEAR].d;
		rfMaxZ = m_kRenderInfo.akClippingPlanes[CP_FAR].d;
	}

	Result Device::SetClippingPlane(ClippingPlanes eIndex, const Plane* pkPlane)
	{
		if((eIndex < CP_USER0) || (eIndex >= CP_NUMPLANES)) {return INVALID_PARAMETERS;}

		if(NULL != pkPlane)
		{
			m_kRenderInfo.akClippingPlanes[eIndex]			= *pkPlane;
			m_kRenderInfo.abClippingPlaneEnabled[eIndex]	= true;
		}
		else
		{
			m_kRenderInfo.abClippingPlaneEnabled[eIndex]	= false;
		}
		return OK;
	}

	Result Device::GetClippingPlane(ClippingPlanes eIndex, Plane& rkPlane)
	{
		if((eIndex < CP_USER0) || (eIndex >= CP_NUMPLANES))			{return INVALID_PARAMETERS;}
		if(false == m_kRenderInfo.abClippingPlaneEnabled[eIndex])	{return INVALID_STATE;}

		rkPlane = m_kRenderInfo.akClippingPlanes[eIndex];
		return OK;
	}

	UINT32 Device::GetRenderedPixels()
	{
		return m_kRenderInfo.uiRenderedPixels;
	}

	Result Device::CreateVertexFormat(VertexFormat** ppkVertexFormat, const VertexElement* pkVertexDeclaration, UINT32 uiVertexDeclSize)
	{
		if(NULL == ppkVertexFormat)
		{
			CORE3D_ERROR(_T("Device::CreateVertexFormat() - Parameter vertex-format pointers to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		*ppkVertexFormat = new VertexFormat(this);
		if(NULL == (*ppkVertexFormat))
		{
			CORE3D_ERROR(_T("Device::CreateVertexFormat() - Out of memory, cannot create vertex format.\n"));
			return OUT_OF_MEMORY;
		}

		Result eResult = (*ppkVertexFormat)->Create(pkVertexDeclaration, uiVertexDeclSize);
		if(CORE3D_FAILED(eResult))
		{
			CORE3D_SAFE_RELEASE(*ppkVertexFormat);
			return eResult;
		}
		return OK;
	}

	Result Device::CreateIndexBuffer(IndexBuffer** ppkIndexBuffer, UINT32 uiLength, Format eFormat)
	{
		if(NULL == ppkIndexBuffer)
		{
			CORE3D_ERROR(_T("Device::CreateIndexBuffer() - Parameter index-buffer pointers to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		*ppkIndexBuffer = new IndexBuffer(this);
		if(NULL == (*ppkIndexBuffer))
		{
			CORE3D_ERROR(_T("Device::CreateIndexBuffer() - Out of memory, cannot create indexbuffer.\n"));
			return OUT_OF_MEMORY;
		}

		Result eResult = (*ppkIndexBuffer)->Create(uiLength, eFormat);
		if(CORE3D_FAILED(eResult))
		{
			CORE3D_SAFE_RELEASE(*ppkIndexBuffer);
			return eResult;
		}
		return OK;
	}

	Result Device::CreateVertexBuffer(VertexBuffer** ppkVertexBuffer, UINT32 uiLength)
	{
		if(NULL == ppkVertexBuffer)
		{
			CORE3D_ERROR(_T("Device::CreateVertexBuffer() - Parameter vertex-buffer pointers to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		*ppkVertexBuffer = new VertexBuffer(this);
		if(NULL == (*ppkVertexBuffer))
		{
			CORE3D_ERROR(_T("Device::CreateVertexBuffer() - Out of memory, cannot create vertexbuffer.\n"));
			return OUT_OF_MEMORY;
		}

		Result eResult = (*ppkVertexBuffer)->Create(uiLength);
		if(CORE3D_FAILED(eResult))
		{
			CORE3D_SAFE_RELEASE(*ppkVertexBuffer);
			return eResult;
		}
		return OK;
	}

	Result Device::CreateSurface(Surface** ppkSurface, UINT32 uiWidth, UINT32 uiHeight, Format eFormat)
	{
		if(NULL == ppkSurface)
		{
			CORE3D_ERROR(_T("Device::CreateSurface() - Parameter surface pointers to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		*ppkSurface = new Surface(this);
		if(NULL == (*ppkSurface))
		{
			CORE3D_ERROR(_T("Device::CreateSurface() - Out of memory, cannot create surface.\n"));
			return OUT_OF_MEMORY;
		}

		Result eResult = (*ppkSurface)->Create(uiWidth, uiHeight, eFormat);
		if(CORE3D_FAILED(eResult))
		{
			CORE3D_SAFE_RELEASE(*ppkSurface);
			return eResult;
		}
		return OK;
	}

	Result Device::CreateTexture(Texture** ppkTexture, UINT32 uiWidth, UINT32 uiHeight, UINT32 uiMipLevels, Format eFormat)
	{
		if(NULL == ppkTexture)
		{
			CORE3D_ERROR(_T("Device::CreateTexture() - Parameter texture pointers to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		*ppkTexture = new Texture(this);
		if(NULL == (*ppkTexture))
		{
			CORE3D_ERROR(_T("Device::CreateTexture() - Out of memory, cannot create texture.\n"));
			return OUT_OF_MEMORY;
		}

		Result eResult = (*ppkTexture)->Create(uiWidth, uiHeight, uiMipLevels, eFormat);
		if(CORE3D_FAILED(eResult))
		{
			CORE3D_SAFE_RELEASE(*ppkTexture);
			return eResult;
		}
		return OK;
	}

	Result Device::CreateCubeTexture(CubeTexture** ppkCubeTexture, UINT32 uiEdgeLength, UINT32 uiMipLevels, Format eFormat)
	{
		if(NULL == ppkCubeTexture)
		{
			CORE3D_ERROR(_T("Device::CreateCubeTexture() - Parameter cube-texture pointers to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		*ppkCubeTexture = new CubeTexture(this);
		if(NULL == (*ppkCubeTexture))
		{
			CORE3D_ERROR(_T("Device::CreateCubeTexture() - Out of memory, cannot create cube-texture.\n"));
			return OUT_OF_MEMORY;
		}

		Result eResult = (*ppkCubeTexture)->Create(uiEdgeLength, uiMipLevels, eFormat);
		if(CORE3D_FAILED(eResult))
		{
			CORE3D_SAFE_RELEASE(*ppkCubeTexture);
			return eResult;
		}
		return OK;
	}

	Result Device::CreateVolume(Volume** ppkVolume, UINT32 uiWidth, UINT32 uiHeight, UINT32 uiDepth, Format eFormat)
	{
		if(NULL == ppkVolume)
		{
			CORE3D_ERROR(_T("Device::CreateVolume() - Parameter volume pointers to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		*ppkVolume = new Volume(this);
		if(NULL == (*ppkVolume))
		{
			CORE3D_ERROR(_T("Device::CreateVolume() - Out of memory, cannot create volume.\n"));
			return OUT_OF_MEMORY;
		}

		Result eResult = (*ppkVolume)->Create(uiWidth, uiHeight, uiDepth, eFormat);
		if(CORE3D_FAILED(eResult))
		{
			CORE3D_SAFE_RELEASE(*ppkVolume);
			return eResult;
		}
		return OK;
	}

	Result Device::CreateVolumeTexture(VolumeTexture** ppkVolumeTexture, UINT32 uiWidth, UINT32 uiHeight, UINT32 uiDepth, UINT32 uiMipLevels, Format eFormat)
	{
		if(NULL == ppkVolumeTexture)
		{
			CORE3D_ERROR(_T("Device::CreateVolumeTexture() - Parameter volume-texture pointers to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		*ppkVolumeTexture = new VolumeTexture(this);
		if(NULL == (*ppkVolumeTexture))
		{
			CORE3D_ERROR(_T("Device::CreateVolumeTexture() - Out of memory, cannot create volume-texture.\n"));
			return OUT_OF_MEMORY;
		}

		Result eResult = (*ppkVolumeTexture)->Create(uiWidth, uiHeight, uiDepth, uiMipLevels, eFormat);
		if(CORE3D_FAILED(eResult))
		{
			CORE3D_SAFE_RELEASE(*ppkVolumeTexture);
			return eResult;
		}
		return OK;
	}

	Result Device::CreateRenderTarget(RenderTarget** ppkRenderTarget)
	{
		if(NULL == ppkRenderTarget)
		{
			CORE3D_ERROR(_T("Device::CreateRenderTarget() - Parameter render-target pointers to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		*ppkRenderTarget = new RenderTarget(this);
		if(NULL == (*ppkRenderTarget))
		{
			CORE3D_ERROR(_T("Device::CreateRenderTarget() - Out of memory, cannot create render-target.\n"));
			return OUT_OF_MEMORY;
		}
		return OK;
	}

	Object* Device::GetObject()
	{
		if(NULL != m_pkParent) {m_pkParent->AddRef();}
		return m_pkParent;
	}

	const DeviceParameters& Device::GetDeviceParameters()
	{
		return m_kDeviceParameters;
	}

	Result Device::Present(RenderTarget* pkRenderTarget)
	{
		if(NULL == pkRenderTarget)
		{
			CORE3D_ERROR(_T("Device::Present() - Parameter render-target pointers to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		// COMMENT : Get pointer to the color-buffer of the render-target
		Surface* pkColorBuffer = pkRenderTarget->GetColorBuffer();
		if(NULL == pkColorBuffer)
		{
			CORE3D_ERROR(_T("Device::Present() - Render-target doesn't have a color-buffer attached.\n"));
			return INVALID_STATE;
		}

		if( (pkColorBuffer->GetWidth() != m_kDeviceParameters.uiBackBufferWidth) || 
			(pkColorBuffer->GetHeight() != m_kDeviceParameters.uiBackBufferHeight) )
		{
			CORE3D_SAFE_RELEASE(pkColorBuffer);
			CORE3D_ERROR(_T("Device::Present() - Color-buffer's dimensions don't match back-buffer.\n"));
			return INVALID_STATE;
		}

		UINT32 uiFloats = pkColorBuffer->GetFormatFloats();
		if(uiFloats < 3)
		{
			CORE3D_SAFE_RELEASE(pkColorBuffer);
			CORE3D_ERROR(_T("Device::Present() - Invalid color-buffer format (Only FMT_R32G32B32F and FMT_R32G32B32A32F are supported).\n"));
			return INVALID_FORMAT;
		}

		FLOAT32* pfSource = NULL;
		if(CORE3D_FAILED(pkColorBuffer->LockRect((void**)&pfSource, NULL)))
		{
			CORE3D_SAFE_RELEASE(pkColorBuffer);
			CORE3D_ERROR(_T("Device::Present() - Couldn't access color-buffer.\n"));
			return UNKNOWN;
		}

		Result eResult = m_pkPresentTarget->Present(pfSource, uiFloats);
		pkColorBuffer->UnlockRect();
		CORE3D_SAFE_RELEASE(pkColorBuffer);
		
		return eResult;
	}

	Result Device::PreRender()
	{
		if(NULL == m_pkVertexFormat)
		{
			CORE3D_ERROR(_T("Device::PreRender() - No vertex format has been set.\n"));
			return INVALID_STATE;
		}

		if(NULL == m_pkVertexShader)
		{
			CORE3D_ERROR(_T("Device::PreRender() - No vertex shader has been set.\n"));
			return INVALID_STATE;
		}

		if(NULL == m_pkPixelShader)
		{
			CORE3D_ERROR(_T("Device::PreRender() - No pixel shader has been set.\n"));
			return INVALID_STATE;
		}

		if(NULL == m_pkRenderTarget)
		{
			CORE3D_ERROR(_T("Device::PreRender() - No render-target has been set.\n"));
			return INVALID_STATE;
		}

		const Matrix4x4& rkMatViewport			= m_pkRenderTarget->GetViewportMatrix();
		m_kRenderInfo.rcViewportRect.uiLeft		= static_cast<UINT32>(Core3D::FtoL(rkMatViewport._41 - rkMatViewport._11));
		m_kRenderInfo.rcViewportRect.uiRight	= static_cast<UINT32>(Core3D::FtoL(rkMatViewport._41 + rkMatViewport._11));
		m_kRenderInfo.rcViewportRect.uiTop		= static_cast<UINT32>(Core3D::FtoL(rkMatViewport._42 + rkMatViewport._22));
		m_kRenderInfo.rcViewportRect.uiBottom	= static_cast<UINT32>(Core3D::FtoL(rkMatViewport._42 - rkMatViewport._22));

		if( (m_kRenderInfo.rcViewportRect.uiLeft >= m_kRenderInfo.rcViewportRect.uiRight)	|| 
			(m_kRenderInfo.rcViewportRect.uiTop >= m_kRenderInfo.rcViewportRect.uiBottom)	)
		{
			CORE3D_ERROR(_T("Device::PreRender() - Invalid view-port matrix.\n"));
			return INVALID_STATE;
		}

		Surface* pkColorBuffer = m_pkRenderTarget->GetColorBuffer();
		Surface* pkDepthBuffer = m_pkRenderTarget->GetDepthBuffer();
		if((NULL == pkColorBuffer) && (NULL == pkDepthBuffer))
		{
			CORE3D_ERROR(_T("Device::PreRender() - Render-target has no associated frame and depth-buffer.\n"));
			return INVALID_STATE;
		}

		if( (NULL != pkColorBuffer) && ((pkColorBuffer->GetWidth() < m_kRenderInfo.rcViewportRect.uiRight) || 
			(pkColorBuffer->GetHeight() < m_kRenderInfo.rcViewportRect.uiBottom)) )
		{
			CORE3D_SAFE_RELEASE(pkColorBuffer);
			CORE3D_SAFE_RELEASE(pkDepthBuffer);
			CORE3D_ERROR(_T("Device::PreRender() - Color-buffer's dimensions are smaller than set view-port.\n"));
			return INVALID_STATE;
		}

		if( (NULL != pkDepthBuffer) && ((pkDepthBuffer->GetWidth() < m_kRenderInfo.rcViewportRect.uiRight) || 
			(pkDepthBuffer->GetHeight() < m_kRenderInfo.rcViewportRect.uiBottom)) )
		{
			CORE3D_SAFE_RELEASE(pkColorBuffer);
			CORE3D_SAFE_RELEASE(pkDepthBuffer);
			CORE3D_ERROR(_T("Device::PreRender() - Depth-buffer's dimensions are smaller than set view-port.\n"));
			return INVALID_STATE;
		}

		CORE3D_SAFE_RELEASE(pkColorBuffer);
		CORE3D_SAFE_RELEASE(pkDepthBuffer);

		const VertexStream* pkCurrentVertexStream = m_akVertexStreams;
		for(UINT32 uiStream = 0; uiStream <= m_pkVertexFormat->GetHighestStream(); ++uiStream, ++pkCurrentVertexStream)
		{
			if(NULL == pkCurrentVertexStream->pkVertexBuffer)
			{
				CORE3D_ERROR(_T("Device::PreRender() - Vertex format references an empty vertex stream.\n"));
				return INVALID_STATE;
			}
		}

		// COMMENT : Check status of scissor testing
		if(BT_FALSE != m_auiRenderStates[RS_SCISSORTESTENABLE])
		{
			if( false == (m_rcScissorRect.uiLeft	>=	m_kRenderInfo.rcViewportRect.uiLeft && m_rcScissorRect.uiLeft	<	m_kRenderInfo.rcViewportRect.uiRight)	|| 
				false == (m_rcScissorRect.uiRight	>	m_kRenderInfo.rcViewportRect.uiLeft && m_rcScissorRect.uiRight	<=	m_kRenderInfo.rcViewportRect.uiRight)	|| 
				false == (m_rcScissorRect.uiTop		>=	m_kRenderInfo.rcViewportRect.uiTop	&& m_rcScissorRect.uiTop	<	m_kRenderInfo.rcViewportRect.uiBottom)	|| 
				false == (m_rcScissorRect.uiBottom	>	m_kRenderInfo.rcViewportRect.uiTop	&& m_rcScissorRect.uiBottom <=	m_kRenderInfo.rcViewportRect.uiBottom)	)
			{
				CORE3D_ERROR(_T("Device::PreRender() - Scissor rect exceeds view-port's dimensions.\n"));
				return INVALID_STATE;
			}
		}

		// COMMENT : Check line thickness
		if(0 == m_auiRenderStates[RS_LINETHICKNESS])
		{
			CORE3D_ERROR(_T("Device::PreRender() - Line thickness is invalid.\n"));
			return INVALID_STATE;
		}

		// COMMENT : Check if render-states for subdivision mode are valid
		switch(m_auiRenderStates[RS_SUBDIVISIONMODE])
		{
		case SUBDIV_NONE:
			break;
		case SUBDIV_SIMPLE:
			if(0 == m_auiRenderStates[RS_SUBDIVISIONLEVELS])
			{
				CORE3D_ERROR(_T("Device::PreRender() - Subdivision levels for simple subdivision are 0.\n"));
				return INVALID_STATE;
			}
			break;
		case SUBDIV_SMOOTH:
			if(0 == m_auiRenderStates[RS_SUBDIVISIONLEVELS])
			{
				CORE3D_ERROR(_T("Device::PreRender() - Subdivision levels for smooth subdivision are 0.\n"));
				return INVALID_STATE;
			}
			else if(m_auiRenderStates[RS_SUBDIVISIONPOSITIONREGISTER] >= VERTEX_SHADER_REGISTERS)
			{
				CORE3D_ERROR(_T("Device::PreRender() - Position register for smooth subdivision exceeds number of avilable vertex shader input registers.\n"));
				return INVALID_STATE;
			}
			else if(m_auiRenderStates[RS_SUBDIVISIONNORMALREGISTER] >= VERTEX_SHADER_REGISTERS)
			{
				CORE3D_ERROR(_T("Device::PreRender() - Normal register for smooth subdivision exceeds number of avilable vertex shader input registers.\n"));
				return INVALID_STATE;
			}
			break;
		case SUBDIV_ADAPTIVE:
			if((0 == m_auiRenderStates[RS_SUBDIVISIONLEVELS]) && (0 == m_auiRenderStates[RS_SUBDIVISIONMAXINNERLEVELS]))
			{
				CORE3D_ERROR(_T("Device::PreRender() - Both subdivision levels for adaptive subdivision are 0.\n"));
				return INVALID_STATE;
			}
			else if(*(FLOAT32*)&m_auiRenderStates[RS_SUBDIVISIONMAXSCREENAREA] <= 0.0f)
			{
				CORE3D_ERROR(_T("Device::PreRender() - Max screen-area for smooth subdivision is <= 0.0f.\n"));
				return INVALID_STATE;
			}
			break;
		default:
			CORE3D_ERROR(_T("Device::PreRender() - Value of renderstate RS_SUBDIVISIONMODE is invalid.\n"));
			return  INVALID_STATE;
		}

		// COMMENT : Check for valid device states, which won't produce any output
		if((BT_FALSE != m_auiRenderStates[RS_ZENABLE]) && (CMP_NEVER == m_auiRenderStates[RS_ZFUNC]))
		{
			CORE3D_NOTIFY(_T("Device::PreRender() - Nothing will be rendered[Depth buffering has been enabled and the compare function has been set to CMP_NEVER")
						  _T("- Nothing will be rendered to the screen].\n"));
		}

		if(BT_FALSE != m_auiRenderStates[RS_SCISSORTESTENABLE])
		{
			if( m_rcScissorRect.uiLeft == m_rcScissorRect.uiRight || 
				m_rcScissorRect.uiTop == m_rcScissorRect.uiBottom )
			{
				CORE3D_NOTIFY(_T("Device::PreRender() = Nothing will be rendered[Scissor testing is enabled and scissor rect spans across an area of 0 pixels].\n"));
			}
		}

		if((BT_FALSE == m_auiRenderStates[RS_COLORWRITEENABLE]) && (BT_FALSE == m_auiRenderStates[RS_ZENABLE] || BT_FALSE == m_auiRenderStates[RS_ZWRITEENABLE]))
		{
			CORE3D_NOTIFY(_T("Device::PreRender() - Nothing will be rendered[Writing to the color-buffer and the depth-buffer has been disabled].\n"));
		}

		// COMMENT : Add more checks
		// Initialize internal render info structure
		for(UINT32 uiReg = 0; uiReg < PIXEL_SHADER_REGISTERS; ++uiReg)
		{
			m_kRenderInfo.aeVSOutputs[uiReg] = m_pkVertexShader->GetOutputRegisters(uiReg);
		}

		// COMMENT : Get color-buffer related states
		pkColorBuffer = m_pkRenderTarget->GetColorBuffer();
		if(NULL != pkColorBuffer)
		{
			Result eResult = pkColorBuffer->LockRect((void**)&m_kRenderInfo.pfFrameData, NULL);
			if(CORE3D_FAILED(eResult))
			{
				CORE3D_SAFE_RELEASE(pkColorBuffer);
				CORE3D_ERROR(_T("Device::PreRender() - Couldn't access color-buffer.\n"));
				return eResult;
			}

			m_kRenderInfo.uiColorFloats = pkColorBuffer->GetFormatFloats();
			if(0 == m_kRenderInfo.uiColorFloats)
			{
				pkColorBuffer->UnlockRect();
				CORE3D_SAFE_RELEASE(pkColorBuffer);
				return UNKNOWN;
			}

			m_kRenderInfo.uiColorBufferPitch	= pkColorBuffer->GetWidth() * m_kRenderInfo.uiColorFloats;
			m_kRenderInfo.bColorWrite			= BT_TRUE == m_auiRenderStates[RS_COLORWRITEENABLE] ? true : false;
		}
		else
		{
			m_kRenderInfo.pfFrameData			= NULL;
			m_kRenderInfo.uiColorFloats			= 0;
			m_kRenderInfo.uiColorBufferPitch	= 0;
			m_kRenderInfo.bColorWrite			= false;
		}

		// COMMENT : Get depth-buffer related states
		pkDepthBuffer = BT_TRUE == m_auiRenderStates[RS_ZENABLE] ? m_pkRenderTarget->GetDepthBuffer() : NULL;
		if(NULL != pkDepthBuffer)
		{
			Result eResult = pkDepthBuffer->LockRect((void**)&m_kRenderInfo.pfDepthData, NULL);
			if(CORE3D_FAILED(eResult))
			{
				CORE3D_SAFE_RELEASE(pkColorBuffer);
				CORE3D_SAFE_RELEASE(pkDepthBuffer);
				CORE3D_ERROR(_T("Device::PreRender() - Couldn't access depth-buffer.\n"));
				if(NULL != m_kRenderInfo.pfFrameData) {pkColorBuffer->UnlockRect();}
				return eResult;
			}

			m_kRenderInfo.uiDepthBufferPitch	= pkDepthBuffer->GetWidth();
			m_kRenderInfo.eDepthCompare			= (CmpFunc)m_auiRenderStates[RS_ZFUNC];
			m_kRenderInfo.bDepthWrite			= BT_TRUE == m_auiRenderStates[RS_ZWRITEENABLE] ? true : false;
		}
		else
		{
			m_kRenderInfo.pfDepthData			= NULL;
			m_kRenderInfo.uiDepthBufferPitch	= 0;
			m_kRenderInfo.eDepthCompare			= CMP_ALWAYS;
			m_kRenderInfo.bDepthWrite			= false;
		}

		CORE3D_SAFE_RELEASE(pkColorBuffer);
		CORE3D_SAFE_RELEASE(pkDepthBuffer);

		// COMMENT : Reset pixel counter to 0
		m_kRenderInfo.uiRenderedPixels = 0;

		// COMMENT : Depending on m_pkPixelShader->GetShaderOutput() chose the appropriate
		// RasterizeScanline function and assign it to the function pointer
		switch(m_pkPixelShader->GetShaderOutput())
		{
		case PSO_COLORONLY:
			m_kRenderInfo.pfnRasterizeScanLine	= m_pkPixelShader->MightKillPixels() ? &Device::RasterizeScanlineColorOnlyMightKillPixels : &Device::RasterizeScanlineColorOnly;
			m_kRenderInfo.pfnDrawPixel			= &Device::DrawPixelColorOnly;
			break;
		case PSO_COLORDEPTH:
			m_kRenderInfo.pfnRasterizeScanLine	= &Device::RasterizeScanlineColorDepth;
			m_kRenderInfo.pfnDrawPixel			= &Device::DrawPixelColorDepth;
			break;
		default: CORE3D_ERROR(_T("Device::PreRender() - Type of pixelshader is invalid.\n")); return INVALID_STATE;
		}

		// COMMENT : Initialize shader's pointer to the rendering device
		// Have to do this right before drawing and not at set time, because a shader
		// may be used with different devices
		m_pkVertexShader->SetDevice(this);
		m_pkPixelShader->SetDevice(this);
		if(NULL != m_pkTriangleShader) {m_pkTriangleShader->SetDevice(this);}

		// COMMENT : Initialize pixel shader's pointers to info structures
		m_pkPixelShader->SetInfo(m_kRenderInfo.aeVSOutputs, &m_kTriangleInfo);

		// COMMENT : Initialize vertex cache
		m_uiNumValidCacheEntries	= 0;
		m_uiFetchedVertices			= 0;

		// COMMENT : FtoL() returns expected integer values
		Core3D::FpuTruncate();
		return OK;
	}

	void Device::PostRender()
	{
		// COMMENT : Reset FPU to (default)rounding mode
		Core3D::FpuReset();

		if(NULL != m_kRenderInfo.pfFrameData)
		{
			Surface* pkColorBuffer = m_pkRenderTarget->GetColorBuffer();
			if(NULL != pkColorBuffer) {pkColorBuffer->UnlockRect();}
			CORE3D_SAFE_RELEASE(pkColorBuffer);
		}

		if(NULL != m_kRenderInfo.pfDepthData)
		{
			Surface* pkDepthBuffer = m_pkRenderTarget->GetDepthBuffer();
			if(NULL != pkDepthBuffer) {pkDepthBuffer->UnlockRect();}
			CORE3D_SAFE_RELEASE(pkDepthBuffer);
		}
	}

	Result Device::DecodeVertexStream(VertexShaderInput& rkVertexShaderInput, UINT32 uiVertex)
	{
		const BYTE8* apkVertex[MAX_VERTEX_STREAMS];
		const VertexStream* pkCurrentVertexStream = m_akVertexStreams;
		for(UINT32 uiStream = 0; uiStream <= m_pkVertexFormat->GetHighestStream(); ++uiStream, ++pkCurrentVertexStream)
		{
			const UINT32 uiOffset = pkCurrentVertexStream->uiOffset + uiVertex * pkCurrentVertexStream->uiStride;
			if(uiOffset >= pkCurrentVertexStream->pkVertexBuffer->GetLength())
			{
				CORE3D_ERROR(_T("Device::DecodeVertexStream() - Vertex stream offset exceeds vertex buffer length.\n"));
				return UNKNOWN;
			}
			Result eResult = pkCurrentVertexStream->pkVertexBuffer->GetPointer(uiOffset, (void**)&apkVertex[uiStream]);
			if(CORE3D_FAILED(eResult)) {return eResult;}
		}

		// COMMENT : Fill vertex-information structure, which can be passed to the vertex shader,
		// with data from the vertex-streams, depending on the current vertex-format.
		const VertexElement* pkCurrentVertexElement = m_pkVertexFormat->GetElements();
		UINT32 uiElement							= m_pkVertexFormat->GetNumVertexElements();
		while(uiElement--)
		{
			ShaderReg& rkRegister = rkVertexShaderInput.kShaderInputs[pkCurrentVertexElement->uiRegister];
			const FLOAT32* pfData = reinterpret_cast<const FLOAT32*>(apkVertex[pkCurrentVertexElement->uiStream]);
			switch(pkCurrentVertexElement->eType)
			{
			case VET_FLOAT32:
				apkVertex[pkCurrentVertexElement->uiStream] += sizeof(FLOAT32);
				rkRegister = ShaderReg(pfData[0], 0.0f, 0.0f, 1.0f);
				break;
			case VET_VECTOR2:
				apkVertex[pkCurrentVertexElement->uiStream] += 2 * sizeof(FLOAT32);
				rkRegister = ShaderReg(pfData[0], pfData[1], 0.0f, 1.0f);
				break;
			case VET_VECTOR3:
				apkVertex[pkCurrentVertexElement->uiStream] += 3 * sizeof(FLOAT32);
				rkRegister = ShaderReg(pfData[0], pfData[1], pfData[2], 1.0f);
				break;
			case VET_VECTOR4:
				apkVertex[pkCurrentVertexElement->uiStream] += 4 * sizeof(FLOAT32);
				rkRegister = ShaderReg(pfData[0], pfData[1], pfData[2], pfData[3]);
				break;
			}
			++pkCurrentVertexElement;
		}
		return OK;
	}

	Result Device::FetchVertex(VertexCacheEntry** ppkVertex, UINT32 uiVertex)
	{
		// COMMENT : Check if the incoming point already points to the desired vertex.
		if(NULL != *ppkVertex && (*ppkVertex)->uiVertexIndex == uiVertex)
		{
			(*ppkVertex)->uiFetchTime = m_uiFetchedVertices++;
			return OK;
		}

		const bool bCanAppend = (m_uiNumValidCacheEntries < VERTEX_CACH_SIZE);

		// COMMENT : Find vertex in cache
		VertexCacheEntry* pkCacheEntry	= &m_akVertexCache[0];
		VertexCacheEntry* pkDestEntry	= pkCacheEntry;
		for(UINT32 uiEntry = 0; uiEntry < m_uiNumValidCacheEntries; ++uiEntry, ++pkCacheEntry)
		{
			if(pkCacheEntry->uiVertexIndex == uiVertex)
			{
				// COMMENT : Vertex is already in cache, return it
				pkCacheEntry->uiFetchTime	= m_uiFetchedVertices++;
				*ppkVertex					= pkCacheEntry;
				return OK;
			}

			// COMMENT : The vertex cache is full, so we have to look for the oldest entry
			// and replace it in case we cannot find the desired vertex in the cache.
			if(pkCacheEntry->uiFetchTime < pkDestEntry->uiFetchTime)
			{
				pkDestEntry = pkCacheEntry;
			}
		}

		// COMMENT : The cache is not full yet, so we can append a new vertex.
		if(bCanAppend) {pkDestEntry = &m_akVertexCache[m_uiNumValidCacheEntries++];}

		// COMMENT : Update the destination cache entry and return it
		pkDestEntry->uiVertexIndex	= uiVertex;
		pkDestEntry->uiFetchTime	= m_uiFetchedVertices++;

		Result eResult = DecodeVertexStream(pkDestEntry->kVertexOutput.kSourceInput, uiVertex);
		if(CORE3D_FAILED(eResult)) {return eResult;}

		m_pkVertexShader->Execute(pkDestEntry->kVertexOutput.kSourceInput.kShaderInputs, 
			pkDestEntry->kVertexOutput.kPosition, pkDestEntry->kVertexOutput.kShaderOutputs);

		*ppkVertex = pkDestEntry;
		return OK;
	}

	void Device::ProcessTriangle(const VertexShaderOutput* pkVSOutput0, const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2)
	{
		switch(m_auiRenderStates[RS_SUBDIVISIONMODE])
		{
		case SUBDIV_NONE:		DrawTriangle(pkVSOutput0, pkVSOutput1, pkVSOutput2);				break;
		case SUBDIV_SIMPLE:		SubdivideTriangleSimple(0, pkVSOutput0, pkVSOutput1, pkVSOutput2);	break;
		case SUBDIV_SMOOTH:		SubdivideTriangleSmooth(0, pkVSOutput0, pkVSOutput1, pkVSOutput2);	break;
		case SUBDIV_ADAPTIVE:	SubdivideTriangleAdaptive(pkVSOutput0, pkVSOutput1, pkVSOutput2);	break;
		}
	}

	Result Device::DrawPrimitive(PrimitiveType ePrimitiveType, UINT32 uiStartVertex, UINT32 uiPrimitiveCount)
	{
		if(0 == uiPrimitiveCount)
		{
			CORE3D_ERROR(_T("Device::DrawPrimitive() - Primitive count is 0.\n"));
			return INVALID_PARAMETERS;
		}

		// COMMENT : Process incoming vertices
		UINT32 uiNumVertices = 0;
		switch(ePrimitiveType)
		{
		case PT_TRIANGLEFAN:	uiNumVertices = uiPrimitiveCount + 2; break;
		case PT_TRIANGLESTRIP:	uiNumVertices = uiPrimitiveCount + 2; break;
		case PT_TRIANGLELIST:	uiNumVertices = uiPrimitiveCount * 3; break;
		default: CORE3D_ERROR(_T("Device::DrawPrimitive() - Invalid primitive type specified.\n")); return INVALID_PARAMETERS;
		}

		Result eCheck = PreRender();
		if(CORE3D_FAILED(eCheck)) {return eCheck;}

		UINT32 auiVertexIndices[3]	= {uiStartVertex, uiStartVertex + 1, uiStartVertex + 2};
		bool bFlip					= false; // Used when drawing triangle strips
		while(uiPrimitiveCount--)
		{
			VertexCacheEntry* apkVertices[3] = {NULL, NULL, NULL};
			for(UINT32 uiVertex = 0; uiVertex < 3; ++uiVertex)
			{
				Result eFetch = FetchVertex(&apkVertices[uiVertex], auiVertexIndices[uiVertex]);
				if(CORE3D_FAILED(eFetch))
				{
					CORE3D_ERROR(_T("Device::DrawPrimitive() - Couldn't fetch vertex from streams.\n"));
					PostRender();
					return eFetch;
				}
			}

			if(bFlip)	{ProcessTriangle(&apkVertices[0]->kVertexOutput, &apkVertices[2]->kVertexOutput, &apkVertices[1]->kVertexOutput);}
			else		{ProcessTriangle(&apkVertices[0]->kVertexOutput, &apkVertices[1]->kVertexOutput, &apkVertices[2]->kVertexOutput);}

			// COMMENT : Prepare vertex-indices for the next triangle
			switch(ePrimitiveType)
			{
			case PT_TRIANGLEFAN:
				auiVertexIndices[1] = auiVertexIndices[2];
				++auiVertexIndices[2];
				break;
			case PT_TRIANGLESTRIP:
				bFlip = !bFlip;
				auiVertexIndices[0] = auiVertexIndices[1];
				auiVertexIndices[1] = auiVertexIndices[2];
				++auiVertexIndices[2];
				break;
			case PT_TRIANGLELIST:
				auiVertexIndices[0] += 3;
				auiVertexIndices[1] += 3;
				auiVertexIndices[2] += 3;
				break;
			}
		}

		PostRender();
		return OK;
	}

	Result Device::DrawIndexedPrimitive(PrimitiveType ePrimitiveType, UINT32 uiBaseVertexIndex, UINT32 uiMinIndex, UINT32 uiNumVertices, UINT32 uiStartIndex, UINT32 uiPrimitiveCount)
	{
		if(0 == uiPrimitiveCount)
		{
			CORE3D_ERROR(_T("Device::DrawIndexedPrimitive() - Primitive count is 0.\n"));
			return INVALID_PARAMETERS;
		}

		if(0 == uiNumVertices)
		{
			CORE3D_ERROR(_T("Device::DrawIndexedPrimitive() - Number of vertices is 0.\n"));
			return INVALID_PARAMETERS;
		}

		if(FALSE == (PT_TRIANGLEFAN == ePrimitiveType || PT_TRIANGLESTRIP == ePrimitiveType || PT_TRIANGLELIST == ePrimitiveType))
		{
			CORE3D_ERROR(_T("Device::DrawIndexedPrimitive() - Invalid primitive type specified.\n"));
			return INVALID_PARAMETERS;
		}

		if(NULL == m_pkIndexBuffer)
		{
			CORE3D_ERROR(_T("Device::DrawIndexedPrimitive() - No index-buffer has been set.\n"));
			return INVALID_STATE;
		}

		Result eResult = PreRender();
		if(CORE3D_FAILED(eResult)) {return eResult;}

		UINT32 auiIndexIndices[3]	= {uiStartIndex, uiStartIndex + 1, uiStartIndex + 2};
		bool bFlip					= false; // Used when drawing triangle strips
		while(uiPrimitiveCount--)
		{
			VertexCacheEntry* apkVertices[3] = {NULL, NULL, NULL};
			for(UINT32 uiVertex = 0; uiVertex < 3; ++uiVertex)
			{
				UINT32 uiVertexIndex	= 0;
				Result eVertexIndex		= m_pkIndexBuffer->GetVertexIndex(auiIndexIndices[uiVertex], uiVertexIndex);
				if(CORE3D_FAILED(eVertexIndex))
				{
					CORE3D_ERROR(_T("Device::DrawIndexedPrimitive() - Couldn't read vertex index from index-buffer.\n"));
					PostRender();
					return eVertexIndex;
				}

				Result eFetch = FetchVertex(&apkVertices[uiVertex], uiVertexIndex + uiBaseVertexIndex);
				if(CORE3D_FAILED(eFetch))
				{
					CORE3D_ERROR(_T("Device::DrawIndexedPrimitive() - Couldn't fetch vertex from streams.\n"));
					PostRender();
					return eFetch;
				}
			}

			if(true == bFlip)	{ProcessTriangle(&apkVertices[0]->kVertexOutput, &apkVertices[2]->kVertexOutput, &apkVertices[1]->kVertexOutput);}
			else				{ProcessTriangle(&apkVertices[0]->kVertexOutput, &apkVertices[1]->kVertexOutput, &apkVertices[2]->kVertexOutput);}

			// COMMENT : Prepare vertex indices for the next triangle
			switch(ePrimitiveType)
			{
			case PT_TRIANGLEFAN:
				auiIndexIndices[1] = auiIndexIndices[2];
				++auiIndexIndices[2];
				break;
			case PT_TRIANGLESTRIP:
				bFlip = !bFlip;
				auiIndexIndices[0] = auiIndexIndices[1];
				auiIndexIndices[1] = auiIndexIndices[2];
				++auiIndexIndices[2];
				break;
			case PT_TRIANGLELIST:
				auiIndexIndices[0] += 3;
				auiIndexIndices[1] += 3;
				auiIndexIndices[2] += 3;
				break;
			}
		}
		
		PostRender();
		return OK;
	}

	Result Device::DrawDynamicPrimitive(UINT uiStartVertex, UINT32 uiNumVertices)
	{
		if(0 == uiNumVertices)
		{
			CORE3D_ERROR(_T("Device::DrawDynamicPrimitive() - Number of vertices is 0.\n"));
			return INVALID_PARAMETERS;
		}

		if(NULL == m_pkPrimitiveAssembler)
		{
			CORE3D_ERROR(_T("Device::DrawDynamicPrimitive() - No primitive assembler has been set.\n"));
			return INVALID_STATE;
		}

		Result eResult = PreRender();
		if(CORE3D_FAILED(eResult)) {return eResult;}

		std::vector<UINT32> vecVertexIndices;
		PrimitiveType ePrimitiveType = m_pkPrimitiveAssembler->Execute(vecVertexIndices, uiNumVertices);

		UINT32 uiPrimitiveCount = 0;
		switch(ePrimitiveType)
		{
		case PT_TRIANGLEFAN:	uiPrimitiveCount = static_cast<UINT32>(vecVertexIndices.size()) - 2; break;
		case PT_TRIANGLESTRIP:	uiPrimitiveCount = static_cast<UINT32>(vecVertexIndices.size()) - 2; break;
		case PT_TRIANGLELIST:	uiPrimitiveCount = static_cast<UINT32>(vecVertexIndices.size()) / 3; break;
		default: CORE3D_ERROR(_T("Device::DrawDynamicPrimitive() - Invalid primitive type specified.\n")); return INVALID_PARAMETERS;
		}
		if(0 == uiPrimitiveCount) {return OK;}

		std::vector<UINT32>::iterator iterVertexIndex = vecVertexIndices.begin();
		UINT auiVertexIndices[3]	= {*iterVertexIndex++, *iterVertexIndex++, *iterVertexIndex++};
		bool bFlip					= false; // Used when drawing triangle strips
		while(--uiPrimitiveCount)
		{
			VertexCacheEntry* apkVertices[3] = {NULL, NULL, NULL};
			for(UINT32 uiVertex = 0; uiVertex < 3; ++uiVertex)
			{
				Result eFetch = FetchVertex(&apkVertices[uiVertex], auiVertexIndices[uiVertex]);
				if(CORE3D_FAILED(eFetch))
				{
					CORE3D_ERROR(_T("Device::DrawIndexedPrimitive() - Couldn't fetch vertex from streams.\n"));
					PostRender();
					return eFetch;
				}
			}

			if(true == bFlip)	{ProcessTriangle(&apkVertices[0]->kVertexOutput, &apkVertices[2]->kVertexOutput, &apkVertices[1]->kVertexOutput);}
			else				{ProcessTriangle(&apkVertices[0]->kVertexOutput, &apkVertices[1]->kVertexOutput, &apkVertices[2]->kVertexOutput);}

			// COMMENT : Prepare vertex-indices for the next triangle
			switch(ePrimitiveType)
			{
			case PT_TRIANGLEFAN:
				auiVertexIndices[1] = auiVertexIndices[2];
				auiVertexIndices[2] = *iterVertexIndex++;
				break;
			case PT_TRIANGLESTRIP:
				bFlip = !bFlip;
				auiVertexIndices[0] = auiVertexIndices[1];
				auiVertexIndices[1] = auiVertexIndices[2];
				auiVertexIndices[2] = *iterVertexIndex++;
				break;
			case PT_TRIANGLELIST:
				auiVertexIndices[0] = *iterVertexIndex++;
				auiVertexIndices[1] = *iterVertexIndex++;
				auiVertexIndices[2] = *iterVertexIndex++;
				break;
			}
		}

		PostRender();
		return OK;
	}

	void Device::InterpolateVertexShaderOutput(VertexShaderOutput* pkVSOutput, const VertexShaderOutput* pkVSOutputA, const VertexShaderOutput* pkVSOutputB, FLOAT32 fInterpolation)
	{
		// COMMENT : Interpolate vertex position
		Core3D::Vec4Lerp(pkVSOutput->kPosition, pkVSOutputA->kPosition, pkVSOutputB->kPosition, fInterpolation);

		// COMMENT : Interpolate registers
		ShaderReg* pkOutput			= pkVSOutput->kShaderOutputs;
		const ShaderReg* pkOutputA	= pkVSOutputA->kShaderOutputs;
		const ShaderReg* pkOutputB	= pkVSOutputB->kShaderOutputs;

		for(UINT32 uiReg = 0; uiReg < PIXEL_SHADER_REGISTERS; ++uiReg, ++pkOutput, ++pkOutputA, ++pkOutputB)
		{
			switch(m_kRenderInfo.aeVSOutputs[uiReg])
			{
			case SRT_VECTOR4: pkOutput->w = Core3D::Lerp(pkOutputA->w, pkOutputB->w, fInterpolation);
			case SRT_VECTOR3: pkOutput->z = Core3D::Lerp(pkOutputA->z, pkOutputB->z, fInterpolation);
			case SRT_VECTOR2: pkOutput->y = Core3D::Lerp(pkOutputA->y, pkOutputB->y, fInterpolation);
			case SRT_FLOAT32: pkOutput->x = Core3D::Lerp(pkOutputA->x, pkOutputB->x, fInterpolation);
			case SRT_UNUSED:  break;
			}
		}
	}

	void Device::MultiplyVertexShaderOutputRegisters(VertexShaderOutput* pkDest, const VertexShaderOutput* pkSrc, FLOAT32 fVal)
	{
		ShaderReg* pkDestReg		= pkDest->kShaderOutputs;
		const ShaderReg* pkSrcReg	= pkSrc->kShaderOutputs;
		for(UINT32 uiReg = 0; uiReg < PIXEL_SHADER_REGISTERS; ++uiReg, ++pkDestReg, ++pkSrcReg)
		{
			switch(m_kRenderInfo.aeVSOutputs[uiReg])
			{
			case SRT_VECTOR4: pkDestReg->w = pkSrcReg->w * fVal;
			case SRT_VECTOR3: pkDestReg->z = pkSrcReg->z * fVal;
			case SRT_VECTOR2: pkDestReg->y = pkSrcReg->y * fVal;
			case SRT_FLOAT32: pkDestReg->x = pkSrcReg->x * fVal;
			case SRT_UNUSED:  break;
			}
		}
	}

	void Device::InterpolateVertexShaderInput(VertexShaderInput* pkVSInput, const VertexShaderInput* pkVSInputA, const VertexShaderInput* pkVSInputB, FLOAT32 fInterpolation)
	{
		// COMMENT : Interpolate registers
		ShaderReg* pkOutput			= pkVSInput->kShaderInputs;
		const ShaderReg* pkInputA	= pkVSInputA->kShaderInputs;
		const ShaderReg* pkInputB	= pkVSInputB->kShaderInputs;
		for(UINT32 uiReg = 0; uiReg < VERTEX_SHADER_REGISTERS; ++uiReg, ++pkOutput, ++pkInputA, ++pkInputB)
		{
			switch(m_kRenderInfo.aeVSInputs[uiReg])
			{
			case SRT_FLOAT32:
				pkOutput->x = Core3D::Lerp(pkInputA->x, pkInputB->x, fInterpolation);
				pkOutput->y = 0.0f; pkOutput->z = 0.0f; pkOutput->w = 1.0f;
				break;
			case SRT_VECTOR2:
				pkOutput->x = Core3D::Lerp(pkInputA->x, pkInputB->x, fInterpolation);
				pkOutput->y = Core3D::Lerp(pkInputA->y, pkInputB->y, fInterpolation);
				pkOutput->z = 0.0f; pkOutput->w = 1.0f;
				break;
			case SRT_VECTOR3:
				pkOutput->x = Core3D::Lerp(pkInputA->x, pkInputB->x, fInterpolation);
				pkOutput->y = Core3D::Lerp(pkInputA->y, pkInputB->y, fInterpolation);
				pkOutput->z = Core3D::Lerp(pkInputA->z, pkInputB->z, fInterpolation);
				pkOutput->w = 1.0f;
				break;
			case SRT_VECTOR4:
				pkOutput->x = Core3D::Lerp(pkInputA->x, pkInputB->x, fInterpolation);
				pkOutput->y = Core3D::Lerp(pkInputA->y, pkInputB->y, fInterpolation);
				pkOutput->z = Core3D::Lerp(pkInputA->z, pkInputB->z, fInterpolation);
				pkOutput->w = Core3D::Lerp(pkInputA->w, pkInputB->w, fInterpolation);
				break;
			case SRT_UNUSED:
				break;
			}
		}
	}

	void Device::ProjectVertex(VertexShaderOutput* pkVSOutput)
	{
		if(pkVSOutput->kPosition.w < FLT_EPSILON) {return;}

		const FLOAT32 fInvW		= 1.0f / pkVSOutput->kPosition.w;
		pkVSOutput->kPosition.Homogenize(fInvW);
		pkVSOutput->kPosition	*= m_pkRenderTarget->GetViewportMatrix();

		// COMMENT : Divide shader output registers by W; this way we can interpolate them lineary while rasterizing
		pkVSOutput->kPosition.w	= fInvW;
		MultiplyVertexShaderOutputRegisters(pkVSOutput, pkVSOutput, pkVSOutput->kPosition.w);
	}

	void Device::SubdivideTriangleSimple(UINT32 uiSubdivisionLevel, const VertexShaderOutput* pkVSOutput0, const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2)
	{
		// COMMENT : In case the triangle has been subdivided to the requested level, draw it.
		if(uiSubdivisionLevel >= m_auiRenderStates[RS_SUBDIVISIONLEVELS])
		{
			DrawTriangle(pkVSOutput0, pkVSOutput1, pkVSOutput2);
			return;
		}
		++uiSubdivisionLevel;

		// COMMENT : Generate three new vertices: in the middle of each edge
		// Interpolate inputs for the new vertices(we're splitting the triangle's edges)
		VertexShaderOutput akNewVSOutputs[3];
		InterpolateVertexShaderInput(&akNewVSOutputs[0].kSourceInput, &pkVSOutput0->kSourceInput, &pkVSOutput1->kSourceInput, 0.5f); // Edge between V0 and V1
		InterpolateVertexShaderInput(&akNewVSOutputs[1].kSourceInput, &pkVSOutput1->kSourceInput, &pkVSOutput2->kSourceInput, 0.5f); // Edge between V1 and V2
		InterpolateVertexShaderInput(&akNewVSOutputs[2].kSourceInput, &pkVSOutput2->kSourceInput, &pkVSOutput0->kSourceInput, 0.5f); // Edge between V2 and V0

		// COMMENT : Calculate new vertex shader outputs
		VertexShaderOutput* pkCurrentVSOutput = akNewVSOutputs;
		for(UINT32 i = 0; i < 3; ++i, ++pkCurrentVSOutput)
		{
			m_pkVertexShader->Execute(pkCurrentVSOutput->kSourceInput.kShaderInputs, pkCurrentVSOutput->kPosition, pkCurrentVSOutput->kShaderOutputs);
		}

		SubdivideTriangleSimple(uiSubdivisionLevel, pkVSOutput0, &akNewVSOutputs[0], &akNewVSOutputs[2]);
		SubdivideTriangleSimple(uiSubdivisionLevel, pkVSOutput1, &akNewVSOutputs[1], &akNewVSOutputs[0]);
		SubdivideTriangleSimple(uiSubdivisionLevel, pkVSOutput2, &akNewVSOutputs[2], &akNewVSOutputs[1]);
		SubdivideTriangleSimple(uiSubdivisionLevel, &akNewVSOutputs[0], &akNewVSOutputs[1], &akNewVSOutputs[2]);
	}

	void Device::SubdivideTriangleSmooth(UINT32 uiSubdivisionLevel, const VertexShaderOutput* pkVSOutput0, const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2)
	{
		static const FLOAT32 MULT_DEVIDE_BY_SIX = 1.0f / 6.0f;
		// COMMENT : In case the triangle has been subdivided to the  requested level, draw it
		if(uiSubdivisionLevel >= m_auiRenderStates[RS_SUBDIVISIONLEVELS])
		{
			DrawTriangle(pkVSOutput0, pkVSOutput1, pkVSOutput2);
			return;
		}
		++uiSubdivisionLevel;

		// COMMENT : Generate three new vertices: in the middle of each edge
		// Interpolate inputs for the new vertices(we're splitting the triangle's edges)
		VertexShaderOutput akNewVSOutputs[3];
		InterpolateVertexShaderInput(&akNewVSOutputs[0].kSourceInput, &pkVSOutput0->kSourceInput, &pkVSOutput1->kSourceInput, 0.5f); // Edge between V0 and V1
		InterpolateVertexShaderInput(&akNewVSOutputs[1].kSourceInput, &pkVSOutput1->kSourceInput, &pkVSOutput2->kSourceInput, 0.5f); // Edge between V1 and V2
		InterpolateVertexShaderInput(&akNewVSOutputs[2].kSourceInput, &pkVSOutput2->kSourceInput, &pkVSOutput0->kSourceInput, 0.5f); // Edge between V2 and V0

		// COMMENT : Offset positions using normals as a base
		const UINT32 uiPos		= m_auiRenderStates[RS_SUBDIVISIONPOSITIONREGISTER];
		const UINT32 uiNormal	= m_auiRenderStates[RS_SUBDIVISIONNORMALREGISTER];

		// COMMENT : Normal vectors should be re-normalized(they're not unit length anymore due
		// to linear interpolation) for best results, but because the error is very small
		// this step is skipped.
		const ShaderReg* apkShaderInputs[3] = {pkVSOutput0->kSourceInput.kShaderInputs, pkVSOutput1->kSourceInput.kShaderInputs, pkVSOutput2->kSourceInput.kShaderInputs};

		// COMMENT : Offset middle of edge between V0 and V1
		{
			const Vector3 kNormalA	= apkShaderInputs[0][uiNormal] * Core3D::Vec3Dot((Vector3)apkShaderInputs[1][uiPos] - (Vector3)apkShaderInputs[0][uiPos], apkShaderInputs[0][uiNormal]);
			const Vector3 kNormalB	= apkShaderInputs[1][uiNormal] * Core3D::Vec3Dot((Vector3)apkShaderInputs[0][uiPos] - (Vector3)apkShaderInputs[1][uiPos], apkShaderInputs[1][uiNormal]);
			Vector4& rkPosition		= akNewVSOutputs[0].kSourceInput.kShaderInputs[uiPos];
			rkPosition				-= (kNormalA + kNormalB) * MULT_DEVIDE_BY_SIX;
		}

		// COMMENT : Offset middle of edge between V1 and V2
		{
			const Vector3 kNormalA	= apkShaderInputs[1][uiNormal] * Core3D::Vec3Dot((Vector3)apkShaderInputs[2][uiPos] - (Vector3)apkShaderInputs[1][uiPos], apkShaderInputs[1][uiNormal]);
			const Vector3 kNormalB	= apkShaderInputs[2][uiNormal] * Core3D::Vec3Dot((Vector3)apkShaderInputs[1][uiPos] - (Vector3)apkShaderInputs[2][uiPos], apkShaderInputs[2][uiNormal]);
			Vector4& rkPosition		= akNewVSOutputs[1].kSourceInput.kShaderInputs[uiPos];
			rkPosition				-= (kNormalA + kNormalB) * MULT_DEVIDE_BY_SIX;
		}

		// COMMENT : Offset middle of edge between V2 and V0
		{
			const Vector3 kNormalA	= apkShaderInputs[2][uiNormal] * Core3D::Vec3Dot((Vector3)apkShaderInputs[0][uiPos] - (Vector3)apkShaderInputs[2][uiPos], apkShaderInputs[2][uiNormal]);
			const Vector3 kNormalB	= apkShaderInputs[0][uiNormal] * Core3D::Vec3Dot((Vector3)apkShaderInputs[2][uiPos] - (Vector3)apkShaderInputs[0][uiPos], apkShaderInputs[0][uiNormal]);
			Vector4& rkPosition		= akNewVSOutputs[2].kSourceInput.kShaderInputs[uiPos];
			rkPosition				-= (kNormalA + kNormalB) * MULT_DEVIDE_BY_SIX;
		}

		// COMMENT : Calculate new vertex shader outputs
		VertexShaderOutput* pkCurrentVSOutput = akNewVSOutputs;
		for(UINT32 i = 0; i < 3; ++i, ++pkCurrentVSOutput)
		{
			m_pkVertexShader->Execute(pkCurrentVSOutput->kSourceInput.kShaderInputs, pkCurrentVSOutput->kPosition, pkCurrentVSOutput->kShaderOutputs);
		}

		SubdivideTriangleSmooth(uiSubdivisionLevel, pkVSOutput0, &akNewVSOutputs[0], &akNewVSOutputs[2]);
		SubdivideTriangleSmooth(uiSubdivisionLevel, pkVSOutput1, &akNewVSOutputs[1], &akNewVSOutputs[0]);
		SubdivideTriangleSmooth(uiSubdivisionLevel, pkVSOutput2, &akNewVSOutputs[2], &akNewVSOutputs[1]);
		SubdivideTriangleSmooth(uiSubdivisionLevel, &akNewVSOutputs[0], &akNewVSOutputs[1], &akNewVSOutputs[2]);
	}

	void Device::SubdivideTriangleAdaptiveSubdivideInnerPart(UINT32 uiSubdivisionLevel, const VertexShaderOutput* pkVSOutput0, const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2)
	{
		static const FLOAT32 MULT_DIVIDIE_BY_TREE1 = 1.0f / 3.0f;
		// COMMENT : Information about subdivision level: here we are counting the maximum inner subdivisions
		if(uiSubdivisionLevel >= m_auiRenderStates[RS_SUBDIVISIONMAXINNERLEVELS])
		{
			DrawTriangle(pkVSOutput0, pkVSOutput1, pkVSOutput2);
			return;
		}

		// COMMENT : Check area of triangle in screen space
		{
			Vector4 akPos[3] = {pkVSOutput0->kPosition, pkVSOutput1->kPosition, pkVSOutput2->kPosition};
			for(UINT32 uiVertex = 0; uiVertex < 3; ++uiVertex)
			{
				// TODO : Should actually be clipped to view frustum
				// Project vertex position + Scale to render-target's viewport
				akPos[uiVertex].Homogenize();
				akPos[uiVertex] *= m_pkRenderTarget->GetViewportMatrix();
			}

			const Vector3 k0To1 = (Vector3)akPos[1] - (Vector3)akPos[0];
			const Vector3 k0To2 = (Vector3)akPos[2] - (Vector3)akPos[0];
			Vector3 kNormal;
			Core3D::Vec3Cross(kNormal, k0To1, k0To2);
			const FLOAT32 fArea = 0.5f * kNormal.Length();
			if(fArea < *(FLOAT32*)&m_auiRenderStates[RS_SUBDIVISIONMAXSCREENAREA])
			{
				DrawTriangle(pkVSOutput0, pkVSOutput1, pkVSOutput2);
				return;
			}
		}

		// COMMENT : Continue splitting: find center vertex and call subdivide innter part for the three new vertices
		++uiSubdivisionLevel;

		// COMMENT : Average inputs for the center vertex
		const Vector4* apkShaderInputs[3] = {pkVSOutput0->kSourceInput.kShaderInputs, pkVSOutput1->kSourceInput.kShaderInputs, pkVSOutput2->kSourceInput.kShaderInputs};
		VertexShaderOutput kVSOutputCenter;
		for(UINT32 i = 0; i < VERTEX_SHADER_REGISTERS; ++i)
		{
			kVSOutputCenter.kSourceInput.kShaderInputs[i] = (apkShaderInputs[0][i] + apkShaderInputs[1][i] + apkShaderInputs[2][i]) * MULT_DIVIDIE_BY_TREE1;
		}

		// COMMENT : Call vertex shader
		m_pkVertexShader->Execute(kVSOutputCenter.kSourceInput.kShaderInputs, kVSOutputCenter.kPosition, kVSOutputCenter.kShaderOutputs);

		// COMMENT : Split outer triangle edges
		SubdivideTriangleAdaptiveSubdivideInnerPart(uiSubdivisionLevel, pkVSOutput0, pkVSOutput1, &kVSOutputCenter);
		SubdivideTriangleAdaptiveSubdivideInnerPart(uiSubdivisionLevel, pkVSOutput1, pkVSOutput2, &kVSOutputCenter);
		SubdivideTriangleAdaptiveSubdivideInnerPart(uiSubdivisionLevel, pkVSOutput2, pkVSOutput0, &kVSOutputCenter);
	}

	void Device::SubdivideTriangleAdaptiveSubdivideEdges(UINT32 uiSubdivisionLevel, const VertexShaderOutput* pkVSOutputEdge0, const VertexShaderOutput* pkVSOutputEdge1, const VertexShaderOutput* pkVSOutputCenter)
	{
		// COMMENT : In case the triangle edges have been subdivided to the requested level, begin adaptive subdivision of inner part
		if(uiSubdivisionLevel >= m_auiRenderStates[RS_SUBDIVISIONLEVELS])
		{
			SubdivideTriangleAdaptiveSubdivideInnerPart(0, pkVSOutputEdge0, pkVSOutputEdge1, pkVSOutputCenter);
			return;
		}
		++uiSubdivisionLevel;

		// COMMENT : Split edge and call subdivide-edge recursively
		VertexShaderOutput kVSOutputMiddleEdge;
		InterpolateVertexShaderInput(&kVSOutputMiddleEdge.kSourceInput, &pkVSOutputEdge0->kSourceInput, &pkVSOutputEdge1->kSourceInput, 0.5f);

		// COMMENT : Call vertex shader
		m_pkVertexShader->Execute(kVSOutputMiddleEdge.kSourceInput.kShaderInputs, kVSOutputMiddleEdge.kPosition, kVSOutputMiddleEdge.kShaderOutputs);

		SubdivideTriangleAdaptiveSubdivideEdges(uiSubdivisionLevel, pkVSOutputEdge0, &kVSOutputMiddleEdge, pkVSOutputCenter);
		SubdivideTriangleAdaptiveSubdivideEdges(uiSubdivisionLevel, &kVSOutputMiddleEdge, pkVSOutputEdge1, pkVSOutputCenter);
	}

	void Device::SubdivideTriangleAdaptive(const VertexShaderOutput* pkVSOutput0, const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2)
	{
		static const FLOAT32 MUL_DIVIDE_BY_TREE2 = 1.0f / 3.0f;
		// COMMENT : Average inputs for the center vertex
		const ShaderReg* apkShaderInputs[3] = {pkVSOutput0->kSourceInput.kShaderInputs, pkVSOutput1->kSourceInput.kShaderInputs, pkVSOutput2->kSourceInput.kShaderInputs};

		VertexShaderOutput kVSOutputCenter;
		for(UINT32 i = 0; i < VERTEX_SHADER_REGISTERS; ++i)
		{
			kVSOutputCenter.kSourceInput.kShaderInputs[i] = (apkShaderInputs[0][i] + apkShaderInputs[1][i] + apkShaderInputs[2][i]) * MUL_DIVIDE_BY_TREE2;
		}

		// COMMENT : Call vertex shader
		m_pkVertexShader->Execute(kVSOutputCenter.kSourceInput.kShaderInputs, kVSOutputCenter.kPosition, kVSOutputCenter.kShaderOutputs);

		// COMMENT : Split outer triangle edge
		SubdivideTriangleAdaptiveSubdivideEdges(0, pkVSOutput0, pkVSOutput1, &kVSOutputCenter);
		SubdivideTriangleAdaptiveSubdivideEdges(0, pkVSOutput1, pkVSOutput2, &kVSOutputCenter);
		SubdivideTriangleAdaptiveSubdivideEdges(0, pkVSOutput2, pkVSOutput0, &kVSOutputCenter);
	}

	bool Device::CullTriangle(const VertexShaderOutput* pkVSOutput0, const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2)
	{
		// COMMENT : Do back-face culling
		if(CULL_NONE == m_auiRenderStates[RS_CULLMODE]) {return false;}

		const FLOAT32 DIR_TEST = (pkVSOutput1->kPosition.x - pkVSOutput0->kPosition.x) * (pkVSOutput2->kPosition.y - pkVSOutput0->kPosition.y) - 
			(pkVSOutput1->kPosition.y - pkVSOutput0->kPosition.y) * (pkVSOutput2->kPosition.x - pkVSOutput0->kPosition.x);
		if(CULL_CCW == m_auiRenderStates[RS_CULLMODE])
		{
			if(DIR_TEST <= 0.0f) {return true;}
		}
		else
		{
			if(DIR_TEST >= 0.0f) {return true;}
		}
		return false;
	}

	UINT32 Device::ClipToPlane(UINT32 uiNumVertices, UINT32 uiStage, const Plane& rkPlane, bool bHomogenous)
	{
		VertexShaderOutput** ppkSrcVertices		= m_aapkClipVertices[uiStage];
		VertexShaderOutput** ppkDestVertices	= m_aapkClipVertices[(uiStage + 1) & 1];

		UINT32 uiNumClippedVertices = 0;
		for(UINT32 i = 0, j = 1; i < uiNumVertices; ++i, ++j)
		{
			// COMMENT : Wrap over
			if(j == uiNumVertices) {j = 0;}
			// COMMENT : Distances of current and next vertex to clipping plane.
			FLOAT32 fDI, fDJ;
			if(true == bHomogenous)
			{
				fDI = rkPlane * ppkSrcVertices[i]->kPosition;
				fDJ = rkPlane * ppkSrcVertices[j]->kPosition;
			}
			else
			{
				fDI = rkPlane * (*(Vector3*)&ppkSrcVertices[i]->kPosition);
				fDJ = rkPlane * (*(Vector3*)&ppkSrcVertices[j]->kPosition);
			}

			if(fDI >= 0.0f)
			{
				ppkDestVertices[uiNumClippedVertices++] = ppkSrcVertices[i];
				if(fDJ < 0.0f)
				{
					InterpolateVertexShaderOutput(&m_akClipVertices[m_uiNextFreeClipVertex], ppkSrcVertices[i], ppkSrcVertices[j], fDI / (fDI - fDJ));
					ppkDestVertices[uiNumClippedVertices++] = &m_akClipVertices[m_uiNextFreeClipVertex];
					m_uiNextFreeClipVertex = (m_uiNextFreeClipVertex + 1) % 20;
				#	ifdef _DEBUG
					if(0 == m_uiNextFreeClipVertex) {CORE3D_NOTIFY(_T("Wrap over: vertex creation during clipping! array too small...?\n"));}
				#	endif
				}
			}
			else
			{
				if(fDJ >= 0.0f)
				{
					InterpolateVertexShaderOutput(&m_akClipVertices[m_uiNextFreeClipVertex], ppkSrcVertices[j], ppkSrcVertices[i], fDJ / (fDJ - fDI));
					ppkDestVertices[uiNumClippedVertices++] = &m_akClipVertices[m_uiNextFreeClipVertex];
					m_uiNextFreeClipVertex = (m_uiNextFreeClipVertex + 1) % 20;
				#	ifdef _DEBUG
					if(0 == m_uiNextFreeClipVertex) {CORE3D_NOTIFY(_T("Wrap over: vertex creation during clipping! array too small...?\n"));}
				#	endif
				}
			}
		}
		return uiNumClippedVertices;
	}

	void Device::DrawTriangle(const VertexShaderOutput* pkVSOutput0, const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2)
	{
		// COMMENT : Prepare triangle for homogeneous clipping
		UINT32 uiNumVertices	= 3;
		memcpy(&m_akClipVertices[0], pkVSOutput0, sizeof(VertexShaderOutput));
		memcpy(&m_akClipVertices[1], pkVSOutput1, sizeof(VertexShaderOutput));
		memcpy(&m_akClipVertices[2], pkVSOutput2, sizeof(VertexShaderOutput));
		m_uiNextFreeClipVertex	= 3;

		UINT32 uiStage			= 0;
		m_aapkClipVertices[uiStage][0] = &m_akClipVertices[0];
		m_aapkClipVertices[uiStage][1] = &m_akClipVertices[1];
		m_aapkClipVertices[uiStage][2] = &m_akClipVertices[2];

		// COMMENT : Call the triangle shader
		if(NULL != m_pkTriangleShader)
		{
			if(false == m_pkTriangleShader->Execute(m_aapkClipVertices[0][0]->kShaderOutputs, 
				m_aapkClipVertices[0][1]->kShaderOutputs, m_aapkClipVertices[0][2]->kShaderOutputs))
			{
				// COMMENT : Triangle got rejected.
				return;
			}
		}

		// COMMENT : Perform clipping to the frustum planes
		for(UINT32 uiPlane = 0; uiPlane < CP_NUMPLANES; ++uiPlane)
		{
			if(false == m_kRenderInfo.abClippingPlaneEnabled[uiPlane]) {continue;}

			uiNumVertices	= ClipToPlane(uiNumVertices, uiStage, m_kRenderInfo.akClippingPlanes[uiPlane], true);
			if(uiNumVertices < 3) {return;}
			uiStage			= (uiStage + 1) & 1;
		}

		// COMMENT : Project and rasterize the clipped triangle
		UINT32 uiVertex;
		// COMMENT : Project the first three vertices for culling
		VertexShaderOutput** ppkSrc = m_aapkClipVertices[uiStage];
		for(uiVertex = 0; uiVertex < 3; ++uiVertex)
		{
			ProjectVertex(ppkSrc[uiVertex]);
		}

		// COMMENT : We do not have to check for culling for each sub-polygon of the triangle, as they
		// are all in the same plane. If the first polygon is culled then all other polygons
		// would be culled, too.
		if(true == CullTriangle(ppkSrc[0], ppkSrc[1], ppkSrc[2])) {return;}

		// COMMENT : Project the remaining vertices
		for(uiVertex = 3; uiVertex < uiNumVertices; ++uiVertex)
		{
			ProjectVertex(ppkSrc[uiVertex]);
		}

		// COMMENT : Perform clipping(in screen-space) to the scissor rectangle if enabled
		if(BT_TRUE == m_auiRenderStates[RS_SCISSORTESTENABLE])
		{
			for(UINT32 uiPlane = 0; uiPlane < 4; ++uiPlane)
			{
				uiNumVertices	= ClipToPlane(uiNumVertices, uiStage, m_kRenderInfo.akScissorPlanes[uiPlane], false);
				if(uiNumVertices < 3) {return;}
				uiStage			= (uiStage + 1) & 1;
			}

			// COMMENT : New source for rasterization after scissoring
			ppkSrc = m_aapkClipVertices[uiStage];
		}

		for(uiVertex = 1; uiVertex < uiNumVertices - 1; ++uiVertex)
		{
			RasterizeTriangle(ppkSrc[0], ppkSrc[uiVertex], ppkSrc[uiVertex + 1]);
		}
	}

	void Device::CalculateTriangleGradients(const VertexShaderOutput* pkVSOutput0, const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2)
	{
		const FLOAT32 DELTA_X[2]	= {pkVSOutput1->kPosition.x - pkVSOutput0->kPosition.x, pkVSOutput2->kPosition.x - pkVSOutput0->kPosition.x};
		const FLOAT32 DELTA_Y[2]	= {pkVSOutput1->kPosition.y - pkVSOutput0->kPosition.y, pkVSOutput2->kPosition.y - pkVSOutput0->kPosition.y};
		m_kTriangleInfo.fCommonGradient = 1.0f / (DELTA_X[0] * DELTA_Y[1] - DELTA_X[1] * DELTA_Y[0]);
		m_kTriangleInfo.pkBaseVertex	= pkVSOutput0;

		// COMMENT : The derivatives with respect to the Y-coordinate are negated, because in screen-space the Y-axis is reversed.
		const FLOAT32 DELTA_Z[2]	= {pkVSOutput1->kPosition.z - pkVSOutput0->kPosition.z, pkVSOutput2->kPosition.z - pkVSOutput0->kPosition.z};
		m_kTriangleInfo.fZDdx		=  (DELTA_Z[0] * DELTA_Y[1] - DELTA_Z[1] * DELTA_Y[0]) * m_kTriangleInfo.fCommonGradient;
		m_kTriangleInfo.fZDdy		= -(DELTA_Z[0] * DELTA_X[1] - DELTA_Z[1] * DELTA_X[0]) * m_kTriangleInfo.fCommonGradient;

		const FLOAT32 DELTA_W[2]	= {pkVSOutput1->kPosition.w - pkVSOutput0->kPosition.w, pkVSOutput2->kPosition.w - pkVSOutput0->kPosition.w};
		m_kTriangleInfo.fWDdx		=  (DELTA_W[0] * DELTA_Y[1] - DELTA_W[1] * DELTA_Y[0]) * m_kTriangleInfo.fCommonGradient;
		m_kTriangleInfo.fWDdy		= -(DELTA_W[0] * DELTA_X[1] - DELTA_W[1] * DELTA_X[0]) * m_kTriangleInfo.fCommonGradient;

		ShaderReg* pkDestDdx = m_kTriangleInfo.kShaderOutputsDdx;
		ShaderReg* pkDestDdy = m_kTriangleInfo.kShaderOutputsDdy;
		for(UINT32 uiReg = 0; uiReg < PIXEL_SHADER_REGISTERS; ++uiReg, ++pkDestDdx, ++pkDestDdy)
		{
			switch(m_kRenderInfo.aeVSOutputs[uiReg])
			{
			case SRT_VECTOR4:
				{
					const FLOAT32 DELTA_REG_VAL[2] = {pkVSOutput1->kShaderOutputs[uiReg].w - pkVSOutput0->kShaderOutputs[uiReg].w, 
					pkVSOutput2->kShaderOutputs[uiReg].w - pkVSOutput0->kShaderOutputs[uiReg].w};
					pkDestDdx->w =  (DELTA_REG_VAL[0] * DELTA_Y[1] - DELTA_REG_VAL[1] * DELTA_Y[0]) * m_kTriangleInfo.fCommonGradient;
					pkDestDdy->w = -(DELTA_REG_VAL[0] * DELTA_X[1] - DELTA_REG_VAL[1] * DELTA_X[0]) * m_kTriangleInfo.fCommonGradient;
				}
			case SRT_VECTOR3:
				{
					const FLOAT32 DELTA_REG_VAL[2] = {pkVSOutput1->kShaderOutputs[uiReg].z - pkVSOutput0->kShaderOutputs[uiReg].z, 
					pkVSOutput2->kShaderOutputs[uiReg].z - pkVSOutput0->kShaderOutputs[uiReg].z};
					pkDestDdx->z =  (DELTA_REG_VAL[0] * DELTA_Y[1] - DELTA_REG_VAL[1] * DELTA_Y[0]) * m_kTriangleInfo.fCommonGradient;
					pkDestDdy->z = -(DELTA_REG_VAL[0] * DELTA_X[1] - DELTA_REG_VAL[1] * DELTA_X[0]) * m_kTriangleInfo.fCommonGradient;
				}
			case SRT_VECTOR2:
				{
					const FLOAT32 DELTA_REG_VAL[2] = {pkVSOutput1->kShaderOutputs[uiReg].y - pkVSOutput0->kShaderOutputs[uiReg].y, 
					pkVSOutput2->kShaderOutputs[uiReg].y - pkVSOutput0->kShaderOutputs[uiReg].y};
					pkDestDdx->y =  (DELTA_REG_VAL[0] * DELTA_Y[1] - DELTA_REG_VAL[1] * DELTA_Y[0]) * m_kTriangleInfo.fCommonGradient;
					pkDestDdy->y = -(DELTA_REG_VAL[0] * DELTA_X[1] - DELTA_REG_VAL[1] * DELTA_X[0]) * m_kTriangleInfo.fCommonGradient;
				}
			case SRT_FLOAT32:
				{
					const FLOAT32 DELTA_REG_VAL[2] = {pkVSOutput1->kShaderOutputs[uiReg].x - pkVSOutput0->kShaderOutputs[uiReg].x, 
					pkVSOutput2->kShaderOutputs[uiReg].x - pkVSOutput0->kShaderOutputs[uiReg].x};
					pkDestDdx->x =  (DELTA_REG_VAL[0] * DELTA_Y[1] - DELTA_REG_VAL[1] * DELTA_Y[0]) * m_kTriangleInfo.fCommonGradient;
					pkDestDdy->x = -(DELTA_REG_VAL[0] * DELTA_X[1] - DELTA_REG_VAL[1] * DELTA_X[0]) * m_kTriangleInfo.fCommonGradient;
				}
			case SRT_UNUSED:
			default: break;
			}
		}
	}

	void Device::SetVSOutputFromGradient(VertexShaderOutput* pkVSOutput, FLOAT32 fX, FLOAT32 fY)
	{
		const FLOAT32 OFFSET_X	= (fX - m_kTriangleInfo.pkBaseVertex->kPosition.x);
		const FLOAT32 OFFSET_Y	= (fY - m_kTriangleInfo.pkBaseVertex->kPosition.y);

		pkVSOutput->kPosition.z = m_kTriangleInfo.pkBaseVertex->kPosition.z + 
			m_kTriangleInfo.fZDdx * OFFSET_X + m_kTriangleInfo.fZDdy * OFFSET_Y;
		pkVSOutput->kPosition.w = m_kTriangleInfo.pkBaseVertex->kPosition.w + 
			m_kTriangleInfo.fWDdx * OFFSET_X + m_kTriangleInfo.fWDdy * OFFSET_Y;

		ShaderReg* pkDest		= pkVSOutput->kShaderOutputs;
		const ShaderReg* BASE	= m_kTriangleInfo.pkBaseVertex->kShaderOutputs;
		const ShaderReg* DDX	= m_kTriangleInfo.kShaderOutputsDdx;
		const ShaderReg* DDY	= m_kTriangleInfo.kShaderOutputsDdy;
		for(UINT32 uiReg = 0; uiReg < PIXEL_SHADER_REGISTERS; ++uiReg, ++pkDest, ++BASE, ++DDX, ++DDY)
		{
			// COMMENT : The following assignments to pkDest automatically zero out unused components
			switch(m_kRenderInfo.aeVSOutputs[uiReg])
			{
			case SRT_FLOAT32:
				*pkDest = *(FLOAT32*)BASE + *(FLOAT32*)DDX * OFFSET_X + *(FLOAT32*)DDY * OFFSET_Y; break;
			case SRT_VECTOR2:
				*pkDest = *(Vector2*)BASE + *(Vector2*)DDX * OFFSET_X + *(Vector2*)DDY * OFFSET_Y; break;
			case SRT_VECTOR3:
				*pkDest = *(Vector3*)BASE + *(Vector3*)DDX * OFFSET_X + *(Vector3*)DDY * OFFSET_Y; break;
			case SRT_VECTOR4:
				*pkDest = *(Vector4*)BASE + *(Vector4*)DDX * OFFSET_X + *(Vector4*)DDY * OFFSET_Y; break;
			case SRT_UNUSED:
			default: break;
			}
		}
	}

	void Device::StepXVSOutputFromGradient(VertexShaderOutput* pkVSOutput)
	{
		pkVSOutput->kPosition.z += m_kTriangleInfo.fZDdx;
		pkVSOutput->kPosition.w += m_kTriangleInfo.fWDdx;

		ShaderReg* pkDest		= pkVSOutput->kShaderOutputs;
		const ShaderReg* DDX	= m_kTriangleInfo.kShaderOutputsDdx;
		for(UINT32 uiReg = 0; uiReg < PIXEL_SHADER_REGISTERS; ++uiReg, ++pkDest, ++DDX)
		{
			switch(m_kRenderInfo.aeVSOutputs[uiReg])
			{
			case SRT_VECTOR4: pkDest->w += DDX->w;
			case SRT_VECTOR3: pkDest->z += DDX->z;
			case SRT_VECTOR2: pkDest->y += DDX->y;
			case SRT_FLOAT32: pkDest->x += DDX->x;
			case SRT_UNUSED:
			default: break;
			}
		}
	}

	void Device::RasterizeTriangle(const VertexShaderOutput* pkVSOutput0, const VertexShaderOutput* pkVSOutput1, const VertexShaderOutput* pkVSOutput2)
	{
		CalculateTriangleGradients(pkVSOutput0, pkVSOutput1, pkVSOutput2);
		// COMMENT : If in wire-frame mode draw triangle edges as lines
		if(FILL_WIREFRAME == m_auiRenderStates[RS_FILLMODE])
		{
			RasterizeLine(pkVSOutput0, pkVSOutput1);
			RasterizeLine(pkVSOutput1, pkVSOutput2);
			RasterizeLine(pkVSOutput2, pkVSOutput0);
			return;
		}

		// COMMENT : Sort vertices by Y-coordinate
		const VertexShaderOutput* VERTICES[3] = {pkVSOutput0, pkVSOutput1, pkVSOutput2};
		if(pkVSOutput1->kPosition.y < VERTICES[0]->kPosition.y) {VERTICES[1] = VERTICES[0]; VERTICES[0] = pkVSOutput1;}
		if(pkVSOutput2->kPosition.y < VERTICES[0]->kPosition.y) {VERTICES[2] = VERTICES[1]; VERTICES[1] = VERTICES[0]; VERTICES[0] = pkVSOutput2;}
		else 
		if(pkVSOutput2->kPosition.y < VERTICES[1]->kPosition.y) {VERTICES[2] = VERTICES[1]; VERTICES[1] = pkVSOutput2;}

		// COMMENT : Screen-space position references
		const Vector4& vA = VERTICES[0]->kPosition;
		const Vector4& vB = VERTICES[1]->kPosition;
		const Vector4& vC = VERTICES[2]->kPosition;

		// COMMENT : Calculate slopes for stepping
		const FLOAT32 STEP_X[3] = 
		{
			((vB.y - vA.y) > 0.0f) ? (vB.x - vA.x) / (vB.y - vA.y) : 0.0f,
			((vC.y - vA.y) > 0.0f) ? (vC.x - vA.x) / (vC.y - vA.y) : 0.0f,
			((vC.y - vB.y) > 0.0f) ? (vC.x - vB.x) / (vC.y - vB.y) : 0.0f
		};

		// COMMENT : Begin rasterization
		FLOAT32 afX[2] = {vA.x, vA.x};
		for(UINT32 uiPart = 0; uiPart < 2; ++uiPart)
		{
			UINT32 auiY[2]		= {0, 0};
			FLOAT32 afDeltaX[2] = {0.0f, 0.0f};

			switch(uiPart)
			{
			case 0: // Draw upper triangle part
				{
					auiY[0] = static_cast<UINT32>(Core3D::FtoL(ceilf(vA.y)));
					auiY[1] = static_cast<UINT32>(Core3D::FtoL(ceilf(vB.y)));
					
					if(STEP_X[0] > STEP_X[1]) // left <-> right?
					{
						afDeltaX[0] = STEP_X[1];
						afDeltaX[1] = STEP_X[0];
					}
					else
					{
						afDeltaX[0] = STEP_X[0];
						afDeltaX[1] = STEP_X[1];
					}
					
					const FLOAT32 PRE_STEP_Y = static_cast<FLOAT32>(auiY[0]) - vA.y;
					afX[0] += afDeltaX[0] * PRE_STEP_Y;
					afX[1] += afDeltaX[1] * PRE_STEP_Y;
				}
				break;
			case 1: // Draw lower triangle part
				{
					auiY[0] = static_cast<UINT32>(Core3D::FtoL(ceilf(vB.y)));
					auiY[1] = static_cast<UINT32>(Core3D::FtoL(ceilf(vC.y)));
					const FLOAT32 PRE_STEP_Y = static_cast<FLOAT32>(auiY[0]) - vB.y;

					if(STEP_X[1] > STEP_X[2]) // left <-> right?
					{
						afDeltaX[0] = STEP_X[1];
						afDeltaX[1] = STEP_X[2];
						afX[1]		= vB.x + afDeltaX[1] * PRE_STEP_Y;
					}
					else
					{
						afDeltaX[0] = STEP_X[2];
						afDeltaX[1] = STEP_X[1];
						afX[0]		= vB.x + afDeltaX[0] * PRE_STEP_Y;
					}
				}
				break;
			}

			for( ; auiY[0] < auiY[1]; ++auiY[0], afX[0] += afDeltaX[0], afX[1] += afDeltaX[1])
			{
				const INT32 aiX[2] = {Core3D::FtoL(ceilf(afX[0])), Core3D::FtoL(ceilf(afX[1]))};
				VertexShaderOutput kVSOutput;
				SetVSOutputFromGradient(&kVSOutput, static_cast<FLOAT32>(aiX[0]), static_cast<FLOAT32>(auiY[0]));
				m_kTriangleInfo.uiCurrentPixelY = auiY[0];
				(this->*m_kRenderInfo.pfnRasterizeScanLine)(auiY[0], static_cast<UINT32>(aiX[0]), static_cast<UINT32>(aiX[1]), &kVSOutput);
			}
		}
	}

	void Device::RasterizeScanlineColorOnly(UINT32 uiY, UINT32 uiX, UINT32 uiX2, VertexShaderOutput* pkVSOutput)
	{
		FLOAT32* pfFrameData = m_kRenderInfo.pfFrameData + (uiY * m_kRenderInfo.uiColorBufferPitch + uiX * m_kRenderInfo.uiColorFloats);
		FLOAT32* pfDepthData = m_kRenderInfo.pfDepthData + (uiY * m_kRenderInfo.uiDepthBufferPitch + uiX);

		for( ; uiX < uiX2; ++uiX, pfFrameData += m_kRenderInfo.uiColorFloats, ++pfDepthData, StepXVSOutputFromGradient(pkVSOutput))
		{
			// COMMENT : Get depth of current pixel
			FLOAT32 fDepth = pkVSOutput->kPosition.z;
			
			// COMMENT : Perform depth test
			switch(m_kRenderInfo.eDepthCompare)
			{
			case CMP_NEVER:			return;
			case CMP_EQUAL:			if(fabsf(fDepth - *pfDepthData) < FLT_EPSILON)	{break;} else {continue;}
			case CMP_NOTEQUAL:		if(fabsf(fDepth - *pfDepthData) >= FLT_EPSILON) {break;} else {continue;}
			case CMP_LESS:			if(fDepth < *pfDepthData)	{break;} else {continue;}
			case CMP_LESSEQUAL:		if(fDepth <= *pfDepthData)	{break;} else {continue;}
			case CMP_GREATEREQUAL:	if(fDepth >= *pfDepthData)	{break;} else {continue;}
			case CMP_GREATER:		if(fDepth > *pfDepthData)	{break;} else {continue;}
			case CMP_ALWAYS:		break;
			}

			// COMMENT : Passed depth test - update depth buffer
			if(true == m_kRenderInfo.bDepthWrite)
			{
				*pfDepthData = fDepth;
			}

			if(true == m_kRenderInfo.bColorWrite)
			{
				VertexShaderOutput kPSInput;
				m_kTriangleInfo.fCurrentPixelInvW = 1.0f / pkVSOutput->kPosition.w;
				MultiplyVertexShaderOutputRegisters(&kPSInput, pkVSOutput, m_kTriangleInfo.fCurrentPixelInvW);

				// NOTE: kPSInput now only contains valid register data, position etc. are not initialized
				// Read in current pixel's color in the color-buffer
				Vector4 kPixelColor(0.0f, 0.0f, 0.0f, 1.0f);
				switch(m_kRenderInfo.uiColorFloats)
				{
				case 4: kPixelColor.a = pfFrameData[3];
				case 3: kPixelColor.b = pfFrameData[2];
				case 2: kPixelColor.g = pfFrameData[1];
				case 1: kPixelColor.r = pfFrameData[0];
				}

				// COMMENT : Execute the pixel shader
				m_kTriangleInfo.uiCurrentPixelX = uiX;
				m_pkPixelShader->Execute(kPSInput.kShaderOutputs, kPixelColor, fDepth);

				// COMMENT : Write the new color to the color-buffer
				switch(m_kRenderInfo.uiColorFloats)
				{
				case 4: pfFrameData[3] = kPixelColor.a;
				case 3: pfFrameData[2] = kPixelColor.b;
				case 2: pfFrameData[1] = kPixelColor.g;
				case 1: pfFrameData[0] = kPixelColor.r;
				}
			}
			++m_kRenderInfo.uiRenderedPixels;
		}
	}

	void Device::RasterizeScanlineColorOnlyMightKillPixels(UINT32 uiY, UINT32 uiX, UINT32 uiX2, VertexShaderOutput* pkVSOutput)
	{
		FLOAT32* pfFrameData = m_kRenderInfo.pfFrameData + (uiY * m_kRenderInfo.uiColorBufferPitch + uiX * m_kRenderInfo.uiColorFloats);
		FLOAT32* pfDepthData = m_kRenderInfo.pfDepthData + (uiY * m_kRenderInfo.uiDepthBufferPitch + uiX);

		for( ; uiX < uiX2; ++uiX, pfFrameData += m_kRenderInfo.uiColorFloats, ++pfDepthData, StepXVSOutputFromGradient(pkVSOutput))
		{
			// COMMENT : Get depth of current pixel
			FLOAT32 fDepth = pkVSOutput->kPosition.z;

			// COMMENT : Perform depth test
			switch(m_kRenderInfo.eDepthCompare)
			{
			case CMP_NEVER:			return;
			case CMP_EQUAL:			if(fabsf(fDepth - *pfDepthData) < FLT_EPSILON)	{break;} else {continue;}
			case CMP_NOTEQUAL:		if(fabsf(fDepth - *pfDepthData) >= FLT_EPSILON) {break;} else {continue;}
			case CMP_LESS:			if(fDepth < *pfDepthData)	{break;} else {continue;}
			case CMP_LESSEQUAL:		if(fDepth <= *pfDepthData)	{break;} else {continue;}
			case CMP_GREATEREQUAL:	if(fDepth >= *pfDepthData)	{break;} else {continue;}
			case CMP_GREATER:		if(fDepth > *pfDepthData)	{break;} else {continue;}
			case CMP_ALWAYS:		break;
			}

			if(true == m_kRenderInfo.bColorWrite || true == m_kRenderInfo.bDepthWrite)
			{
				VertexShaderOutput kPSInput;
				m_kTriangleInfo.fCurrentPixelInvW = 1.0f / pkVSOutput->kPosition.w;
				MultiplyVertexShaderOutputRegisters(&kPSInput, pkVSOutput, m_kTriangleInfo.fCurrentPixelInvW);

				// NOTE: kPSInput now only contains valid register data, position etc. are not initialized
				// Read in current pixel's color in the color-buffer
				Vector4 kPixelColor(0.0f, 0.0f, 0.0f, 1.0f);
				switch(m_kRenderInfo.uiColorFloats)
				{
				case 4: kPixelColor.a = pfFrameData[3];
				case 3: kPixelColor.b = pfFrameData[2];
				case 2: kPixelColor.g = pfFrameData[1];
				case 1: kPixelColor.r = pfFrameData[0];
				}

				// COMMENT : Execute the pixel shader
				m_kTriangleInfo.uiCurrentPixelX = uiX;
				if(false == m_pkPixelShader->Execute(kPSInput.kShaderOutputs, kPixelColor, fDepth))
				{
					// COMMENT : Pixel got killed
					continue;
				}

				// COMMENT : Passed depth test and pixel was not killed, so update depth-buffer
				if(true == m_kRenderInfo.bDepthWrite)
				{
					*pfDepthData = fDepth;
				}

				// COMMENT : Write the new color to the color-buffer
				if(true == m_kRenderInfo.bColorWrite)
				{
					switch(m_kRenderInfo.uiColorFloats)
					{
					case 4: pfFrameData[3] = kPixelColor.a;
					case 3: pfFrameData[2] = kPixelColor.b;
					case 2: pfFrameData[1] = kPixelColor.g;
					case 1: pfFrameData[0] = kPixelColor.r;
					}
				}
			}
			++m_kRenderInfo.uiRenderedPixels;
		}
	}

	void Device::RasterizeScanlineColorDepth(UINT32 uiY, UINT32 uiX, UINT32 uiX2, VertexShaderOutput* pkVSOutput)
	{
		FLOAT32* pfFrameData = m_kRenderInfo.pfFrameData + (uiY * m_kRenderInfo.uiColorBufferPitch + uiX * m_kRenderInfo.uiColorFloats);
		FLOAT32* pfDepthData = m_kRenderInfo.pfDepthData + (uiY * m_kRenderInfo.uiDepthBufferPitch + uiX);

		for( ; uiX < uiX2; ++uiX, pfFrameData += m_kRenderInfo.uiColorFloats, ++ pfDepthData, StepXVSOutputFromGradient(pkVSOutput))
		{
			VertexShaderOutput kPSInput;
			m_kTriangleInfo.fCurrentPixelInvW = 1.0f / pkVSOutput->kPosition.w;
			MultiplyVertexShaderOutputRegisters(&kPSInput, pkVSOutput, m_kTriangleInfo.fCurrentPixelInvW);

			// NOTE: kPSInput now only contains valid register data, position etc. are not initialized
			// Read in current color-buffer color
			Vector4 kPixelColor(0.0f, 0.0f, 0.0f, 1.0f);
			switch(m_kRenderInfo.uiColorFloats)
			{
			case 4: kPixelColor.a = pfFrameData[3];
			case 3: kPixelColor.b = pfFrameData[2];
			case 2: kPixelColor.g = pfFrameData[1];
			case 1: kPixelColor.r = pfFrameData[0];
			}

			// COMMENT : Get depth of current pixel
			FLOAT32 fDepth = pkVSOutput->kPosition.z;

			// COMMENT : Execute pixel shader
			m_kTriangleInfo.uiCurrentPixelX = uiX;
			if(false == m_pkPixelShader->Execute(kPSInput.kShaderOutputs, kPixelColor, fDepth))
			{
				// COMMENT : Pixel got killed
				continue;
			}

			// COMMENT : Perform depth test
			switch(m_kRenderInfo.eDepthCompare)
			{
			case CMP_NEVER:			return;
			case CMP_EQUAL:			if(fabsf(fDepth - *pfDepthData) < FLT_EPSILON)	{break;} else {continue;}
			case CMP_NOTEQUAL:		if(fabsf(fDepth - *pfDepthData) >= FLT_EPSILON) {break;} else {continue;}
			case CMP_LESS:			if(fDepth < *pfDepthData)	{break;} else {continue;}
			case CMP_LESSEQUAL:		if(fDepth <= *pfDepthData)	{break;} else {continue;}
			case CMP_GREATEREQUAL:	if(fDepth >= *pfDepthData)	{break;} else {continue;}
			case CMP_GREATER:		if(fDepth > *pfDepthData)	{break;} else {continue;}
			case CMP_ALWAYS:		break;
			}

			// COMMENT : Passed depth test, so update depth buffer
			if(true == m_kRenderInfo.bDepthWrite)
			{
				*pfDepthData = fDepth;
			}

			// COMMENT : Write new color to color buffer
			if(true == m_kRenderInfo.bColorWrite)
			{
				switch(m_kRenderInfo.uiColorFloats)
				{
				case 4: pfFrameData[3] = kPixelColor.a;
				case 3: pfFrameData[2] = kPixelColor.b;
				case 2: pfFrameData[1] = kPixelColor.g;
				case 1: pfFrameData[0] = kPixelColor.r;
				}
			}
			++m_kRenderInfo.uiRenderedPixels;
		}
	}

	void Device::RasterizeLine(const VertexShaderOutput* pkVSOutput0, const VertexShaderOutput* pkVSOutput1)
	{
		const Vector4& vA = pkVSOutput0->kPosition;
		const Vector4& vB = pkVSOutput1->kPosition;

		const UINT32 INT_COORDS_A[2]	= {static_cast<UINT32>(Core3D::FtoL(vA.x)), static_cast<UINT32>(Core3D::FtoL(vA.y))};
		const FLOAT32 DELTA_X			= vB.x - vA.x;
		const FLOAT32 DELTA_Y			= vB.y - vA.y;

		const INT32 LINE_THICKNESS_HALF	= -(static_cast<INT32>(m_auiRenderStates[RS_LINETHICKNESS] / 2));
		const INT32 POS_OFFSET			= (m_auiRenderStates[RS_LINETHICKNESS] & 1) ? 0 : 1;

		if(fabsf(DELTA_X) > fabsf(DELTA_Y))
		{
			// COMMENT : Drawing a line which is more horizontal than vertical
			const INT32 PIXELS_X		= Core3D::FtoL((FLOAT32)DELTA_X);
			if(0 == PIXELS_X) {return;}

			const INT32 SIGN_X			= PIXELS_X > 0 ? 1 : -1;
			const FLOAT32 SLOPE			= DELTA_Y / DELTA_X;

			FLOAT32 fInterpolation		= SIGN_X > 0 ? 0.0f : 1.0f;
			const FLOAT32 INTERPOLATION_STEP = static_cast<const FLOAT32>(SIGN_X) / static_cast<const FLOAT32>(abs(PIXELS_X) - 1);

			for(INT32 i = 0; i != PIXELS_X; i += SIGN_X, fInterpolation += INTERPOLATION_STEP)
			{
				UINT32 uiPixelX			= INT_COORDS_A[0] + i;
				UINT32 uiPixelY			= INT_COORDS_A[1] + Core3D::FtoL((FLOAT32)(SLOPE * i));

				VertexShaderOutput kPSInput;
				SetVSOutputFromGradient(&kPSInput, static_cast<FLOAT32>(uiPixelX), static_cast<FLOAT32>(uiPixelY));
				m_kTriangleInfo.fCurrentPixelInvW = 1.0f / kPSInput.kPosition.w;
				MultiplyVertexShaderOutputRegisters(&kPSInput, &kPSInput, m_kTriangleInfo.fCurrentPixelInvW);

				if(uiPixelY > m_pkRenderTarget->GetColorBuffer()->GetHeight()) {continue;} // uiPixelY의 값이 Color Buffer의 크기 보다 클 수 있어 그 범위를 제한 함.
				if(0 == LINE_THICKNESS_HALF) {(this->*m_kRenderInfo.pfnDrawPixel)(uiPixelX, uiPixelY, &kPSInput);}
				else
				{
					for(INT32 j = LINE_THICKNESS_HALF + POS_OFFSET; j <= -LINE_THICKNESS_HALF; ++j)
					{
						INT32 iNewPixelY = uiPixelY + j;
						if( (iNewPixelY < static_cast<INT32>(m_kRenderInfo.rcViewportRect.uiTop))	|| 
							(iNewPixelY >= static_cast<INT32>(m_kRenderInfo.rcViewportRect.uiBottom)) )
						{
							continue;
						}
						(this->*m_kRenderInfo.pfnDrawPixel)(uiPixelX, static_cast<UINT32>(iNewPixelY), &kPSInput);
					}
				}
			}
		}
		else
		{
			const INT32 PIXELS_Y		= Core3D::FtoL((FLOAT32)DELTA_Y);
			if(0 == PIXELS_Y) {return;}

			const INT32 SIGN_Y			= PIXELS_Y > 0 ? 1 : -1;
			const FLOAT32 SLOPE			= DELTA_X / DELTA_Y;

			FLOAT32 fInterpolation		= SIGN_Y > 0 ? 0.0f : 1.0f;
			const FLOAT32 INTERPOLATION_STEP = static_cast<const FLOAT32>(SIGN_Y) / static_cast<const FLOAT32>(abs(PIXELS_Y) - 1);

			for(INT32 i = 0; i != PIXELS_Y; i += SIGN_Y, fInterpolation += INTERPOLATION_STEP)
			{
				UINT32 uiPixelX			= INT_COORDS_A[0] + Core3D::FtoL((FLOAT32)(SLOPE * i));
				UINT32 uiPixelY			= INT_COORDS_A[1] + i;

				VertexShaderOutput kPSInput;
				SetVSOutputFromGradient(&kPSInput, static_cast<FLOAT>(uiPixelX), static_cast<FLOAT32>(uiPixelY));
				m_kTriangleInfo.fCurrentPixelInvW = 1.0f / kPSInput.kPosition.w;
				MultiplyVertexShaderOutputRegisters(&kPSInput, &kPSInput, m_kTriangleInfo.fCurrentPixelInvW);

				if(uiPixelY > m_pkRenderTarget->GetColorBuffer()->GetHeight()) {continue;} // uiPixelY의 값이 Color Buffer의 크기 보다 클 수 있어 그 범위를 제한 함.
				if(0 == LINE_THICKNESS_HALF) {(this->*m_kRenderInfo.pfnDrawPixel)(uiPixelX, uiPixelY, &kPSInput);}
				else
				{
					for(INT32 j = LINE_THICKNESS_HALF + POS_OFFSET; j <= -LINE_THICKNESS_HALF; ++j)
					{
						INT32 iNewPixelX = uiPixelX + j;
						if( (iNewPixelX < static_cast<INT32>(m_kRenderInfo.rcViewportRect.uiLeft)) || 
							(iNewPixelX >= static_cast<INT32>(m_kRenderInfo.rcViewportRect.uiRight)) )
						{
							continue;
						}
						(this->*m_kRenderInfo.pfnDrawPixel)(static_cast<UINT32>(iNewPixelX), uiPixelY, &kPSInput);
					}
				}
			}
		}
	}

	void Device::DrawPixelColorOnly(UINT32 uiX, UINT32 uiY, const VertexShaderOutput* pkVSOutput)
	{
		FLOAT32* pfFrameData = m_kRenderInfo.pfFrameData + (uiY * m_kRenderInfo.uiColorBufferPitch + uiX * m_kRenderInfo.uiColorFloats);
		FLOAT32* pfDepthData = m_kRenderInfo.pfDepthData + (uiY * m_kRenderInfo.uiDepthBufferPitch + uiX);

		// COMMENT : Perform depth test
		switch(m_kRenderInfo.eDepthCompare)
		{
		case CMP_NEVER:			return;
		case CMP_EQUAL:			if(fabsf(pkVSOutput->kPosition.z - *pfDepthData) < FLT_EPSILON)		{break;} else {return;}
		case CMP_NOTEQUAL:		if(fabsf(pkVSOutput->kPosition.z - *pfDepthData) >= FLT_EPSILON)	{break;} else {return;}
		case CMP_LESS:			if(pkVSOutput->kPosition.z < *pfDepthData)	{break;} else {return;}
		case CMP_LESSEQUAL:		if(pkVSOutput->kPosition.z <= *pfDepthData) {break;} else {return;}
		case CMP_GREATEREQUAL:	if(pkVSOutput->kPosition.z >= *pfDepthData) {break;} else {return;}
		case CMP_GREATER:		if(pkVSOutput->kPosition.z > *pfDepthData)	{break;} else {return;}
		case CMP_ALWAYS:		break;
		}

		if(true == m_kRenderInfo.bColorWrite || true == m_kRenderInfo.bDepthWrite)
		{
			// COMMENT : Read in current pixel's color in the color-buffer
			Vector4 kPixelColor(0.0f, 0.0f, 0.0f, 1.0f);
			switch(m_kRenderInfo.uiColorFloats)
			{
			case 4: kPixelColor.a = pfFrameData[3];
			case 3: kPixelColor.b = pfFrameData[2];
			case 2: kPixelColor.g = pfFrameData[1];
			case 1: kPixelColor.r = pfFrameData[0];
			}

			// COMMENT : Execute the pixel shader
			FLOAT32 fPSDepth = pkVSOutput->kPosition.z; // If we passed pkVSOutput->kPosition.z directly to the pixel shader,
														// It might modify it, which is not allowed in this function.
			m_kTriangleInfo.uiCurrentPixelX = uiX;
			m_kTriangleInfo.uiCurrentPixelY = uiY;

			if(false == m_pkPixelShader->Execute(pkVSOutput->kShaderOutputs, kPixelColor, fPSDepth))
			{
				// COMMENT : Pixel got killed
				return;
			}

			// COMMENT : Passed depth test and pixel was not killed, so update depth buffer
			if(true == m_kRenderInfo.bDepthWrite)
			{
				*pfDepthData = pkVSOutput->kPosition.z;
			}

			// COMMENT : Write the new color to the color buffer
			if(true == m_kRenderInfo.bColorWrite)
			{
				switch(m_kRenderInfo.uiColorFloats)
				{
				case 4: pfFrameData[3] = kPixelColor.a;
				case 3: pfFrameData[2] = kPixelColor.b;
				case 2: pfFrameData[1] = kPixelColor.g;
				case 1: pfFrameData[0] = kPixelColor.r;
				}
			}
		}
		++m_kRenderInfo.uiRenderedPixels;
	}

	void Device::DrawPixelColorDepth(UINT32 uiX, UINT32 uiY, const VertexShaderOutput* pkVSOutput)
	{
		FLOAT32* pfFrameData = m_kRenderInfo.pfFrameData + (uiY * m_kRenderInfo.uiColorBufferPitch + uiX * m_kRenderInfo.uiColorFloats);
		FLOAT32* pfDepthData = m_kRenderInfo.pfDepthData + (uiY * m_kRenderInfo.uiDepthBufferPitch + uiX);

		// COMMENT : Read in current pixel's color in the color buffer
		Vector4 kPixelColor(0.0f, 0.0f, 0.0f, 1.0f);
		switch(m_kRenderInfo.uiColorFloats)
		{
		case 4: kPixelColor.a = pfFrameData[3];
		case 3: kPixelColor.b = pfFrameData[2];
		case 2: kPixelColor.g = pfFrameData[1];
		case 1: kPixelColor.r = pfFrameData[0];
		}

		// COMMENT : Execute the pixel shader
		FLOAT32 fPSDepth = pkVSOutput->kPosition.z; // If we passed pkVSOutput->kPosition.z directly to the pixel shader,
													// It might modify it, which is not allowed in this function.
		m_kTriangleInfo.uiCurrentPixelX = uiX;
		m_kTriangleInfo.uiCurrentPixelY = uiY;

		if(false == m_pkPixelShader->Execute(pkVSOutput->kShaderOutputs, kPixelColor, fPSDepth))
		{
			// COMMENT : Pixel got killed
			return;
		}

		// COMMENT : Perform depth test
		switch(m_kRenderInfo.eDepthCompare)
		{
		case CMP_NEVER:			return;
		case CMP_EQUAL:			if(fabsf(fPSDepth - *pfDepthData) < FLT_EPSILON)	{break;} else {return;}
		case CMP_NOTEQUAL:		if(fabsf(fPSDepth - *pfDepthData) >= FLT_EPSILON)	{break;} else {return;}
		case CMP_LESS:			if(fPSDepth < *pfDepthData)		{break;} else {return;}
		case CMP_LESSEQUAL:		if(fPSDepth <= *pfDepthData)	{break;} else {return;}
		case CMP_GREATEREQUAL:	if(fPSDepth >= *pfDepthData)	{break;} else {return;}
		case CMP_GREATER:		if(fPSDepth > *pfDepthData)		{break;} else {return;}
		case CMP_ALWAYS:		break;
		}

		// COMMENT : Passed depth test and pixel was not killed, so update depth buffer
		if(true == m_kRenderInfo.bDepthWrite)
		{
			*pfDepthData = fPSDepth;
		}

		// COMMENT : Write the new color to the color buffer
		if(true == m_kRenderInfo.bColorWrite)
		{
			switch(m_kRenderInfo.uiColorFloats)
			{
			case 4: pfFrameData[3] = kPixelColor.a;
			case 3: pfFrameData[2] = kPixelColor.b;
			case 2: pfFrameData[1] = kPixelColor.g;
			case 1: pfFrameData[0] = kPixelColor.r;
			}
		}
		++m_kRenderInfo.uiRenderedPixels;
	}
}