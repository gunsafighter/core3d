#include "VertexFormat.h"
#include "Device.h"

namespace Core3D
{
	VertexFormat::VertexFormat(Device* pkDevice)
		: m_pkDevice(pkDevice)
		, m_pkElements(NULL)
	{
		m_pkDevice->AddRef();
	}

	VertexFormat::~VertexFormat()
	{
		CORE3D_SAFE_DELETEARRAY(m_pkElements);
		CORE3D_SAFE_RELEASE(m_pkDevice);
	}

	Result VertexFormat::Create(const VertexElement* pkVertexDeclaration, UINT32 uiVertexDeclSize)
	{
		if(NULL == pkVertexDeclaration)
		{
			CORE3D_ERROR(_T("VertexFormat::Create() - Parameter vertex declaration pointer to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		if(0 == uiVertexDeclSize)
		{
			CORE3D_ERROR(_T("VertexFormat::Create() - Parameter vertex declaration size is 0.\n"));
			return INVALID_PARAMETERS;
		}

		m_uiNumVertexElements	= uiVertexDeclSize / sizeof(VertexElement);
		m_uiHighestStream		= 0;
		const VertexElement* pkCurrentElement = pkVertexDeclaration;
		for(UINT32 uiElement = 0; uiElement < m_uiNumVertexElements; ++uiElement, ++pkCurrentElement)
		{
			if(pkCurrentElement->uiStream >= MAX_VERTEX_STREAMS)
			{
				CORE3D_ERROR(_T("VertexFormat::Create() - Vertex element's stream number exceeds number of available stream.\n"));
				return INVALID_PARAMETERS;
			}

			if(pkCurrentElement->uiRegister >= VERTEX_SHADER_REGISTERS)
			{
				CORE3D_ERROR(_T("VertexFormat::Create() - Vertex element's target register exceeds number of available shader register.\n"));
				return INVALID_PARAMETERS;
			}

			if(pkCurrentElement->eType < VET_FLOAT32 || VET_VECTOR4 < pkCurrentElement->eType)
			{
				CORE3D_ERROR(_T("VertexFormat::Create() - Vertex element has invalid type.\n"));
				return INVALID_PARAMETERS;
			}

			if(pkCurrentElement->uiStream > m_uiHighestStream)
			{
				m_uiHighestStream = pkCurrentElement->uiStream;
			}
		}

		m_pkElements = new VertexElement[m_uiNumVertexElements];
		if(NULL == m_pkElements)
		{
			CORE3D_ERROR(_T("VertexFormat::Create() - Out of memory, cannot create vertex element data.\n"));
			return OUT_OF_MEMORY;
		}

		memcpy(m_pkElements, pkVertexDeclaration, sizeof(VertexElement) * m_uiNumVertexElements);
		return OK;
	}

	Device* VertexFormat::GetDevice()
	{
		if(NULL != m_pkDevice) {m_pkDevice->AddRef();}
		return m_pkDevice;
	}

	UINT32 VertexFormat::GetNumVertexElements()
	{
		return m_uiNumVertexElements;
	}

	UINT32 VertexFormat::GetHighestStream()
	{
		return m_uiHighestStream;
	}

	VertexElement* VertexFormat::GetElements()
	{
		return m_pkElements;
	}
}