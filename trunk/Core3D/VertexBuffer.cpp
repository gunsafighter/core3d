#include "VertexBuffer.h"
#include "Device.h"

namespace Core3D
{
	VertexBuffer::VertexBuffer(Device* pkDevice)
		: m_pkDevice(pkDevice)
		, m_pData(NULL)
	{
		m_pkDevice->AddRef();
	}

	VertexBuffer::~VertexBuffer()
	{
		CORE3D_SAFE_DELETEARRAY(m_pData);
		CORE3D_SAFE_RELEASE(m_pkDevice);
	}

	Result VertexBuffer::Create(UINT32 uiLength)
	{
		if(0 == uiLength)
		{
			CORE3D_ERROR(_T("VertexBuffer::Create() - Parameter length is 0.\n"));
			return INVALID_PARAMETERS;
		}

		m_uiLength	= uiLength;
		m_pData		= new BYTE8[uiLength];
		if(NULL == m_pData)
		{
			CORE3D_ERROR(_T("VertexBuffer::Create() - Out of memory, cannot create vertex buffer.\n"));
			return OUT_OF_MEMORY;
		}
		return OK;
	}

	Result VertexBuffer::GetPointer(UINT32 uiOffset, void** ppvData)
	{
		if(NULL == ppvData)
		{
			CORE3D_ERROR(_T("VertexBuffer::GetPointer() - Parameter data pointer pointer to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		if(uiOffset >= m_uiLength)
		{
			*ppvData = NULL;
			CORE3D_ERROR(_T("VertexBuffer::GetPointer() - Offset exceeds vertex buffer length.\n"));
			return INVALID_PARAMETERS;
		}

		*ppvData = &m_pData[uiOffset];
		return OK;
	}

	Device* VertexBuffer::GetDevice()
	{
		if(NULL != m_pkDevice) {m_pkDevice->AddRef();}
		return m_pkDevice;
	}

	UINT32 VertexBuffer::GetLength()
	{
		return m_uiLength;
	}
}