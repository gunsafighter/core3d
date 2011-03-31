#include "IndexBuffer.h"
#include "Device.h"

namespace Core3D
{
	IndexBuffer::IndexBuffer(Device* pkDevice)
		: m_pkDevice(pkDevice)
	{
		m_pkDevice->AddRef();
	}

	IndexBuffer::~IndexBuffer()
	{
		CORE3D_SAFE_DELETEARRAY(m_pData);
		CORE3D_SAFE_RELEASE(m_pkDevice);
	}

	Result IndexBuffer::Create(UINT32 uiLength, Format eFormat)
	{
		if(0 == uiLength)
		{
			CORE3D_ERROR(_T("IndexBuffer::Create() - Parameter length is 0.\n"));
			return INVALID_PARAMETERS;
		}

		if(FMT_INDEX16 != eFormat && FMT_INDEX32 != eFormat)
		{
			CORE3D_ERROR(_T("IndexBuffer::Create() - Invalid format specified.\n"));
			return INVALID_FORMAT;
		}

		m_uiLength	= uiLength;
		m_eFormat	= eFormat;
		m_pData		= new BYTE8[m_uiLength];
		if(NULL == m_pData)
		{
			CORE3D_ERROR(_T("IndexBuffer::Create() - Out of memory, cannot create index buffer.\n"));
			return OUT_OF_MEMORY;
		}
		return OK;
	}

	Result IndexBuffer::GetPointer(UINT32 uiOffset, void** ppvData)
	{
		if(NULL == ppvData)
		{
			CORE3D_ERROR(_T("IndexBuffer::GetPointer() - Parameter output data points to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		if(uiOffset >= m_uiLength)
		{
			*ppvData = NULL;
			CORE3D_ERROR(_T("IndexBuffer::GetPointer() - Offset is larger than index buffer length.\n"));
			return INVALID_PARAMETERS;
		}
		
		*ppvData = &m_pData[uiOffset];
		return OK;
	}

	Result IndexBuffer::GetVertexIndex(UINT32 uiArrayIndex, UINT32& ruiValue)
	{
		switch(m_eFormat)
		{
		case FMT_INDEX16:
			{
				if(uiArrayIndex >= (m_uiLength / 2))
				{
					CORE3D_ERROR(_T("IndexBuffer::GetVertexIndex() - Parameter array-index exceeds vertex buffer size.\n"));
					return INVALID_PARAMETERS;
				}
				const UINT16* pData = reinterpret_cast<UINT16*>(m_pData);
				ruiValue			= pData[uiArrayIndex];
			}
			return OK;
		case FMT_INDEX32:
			{
				if(uiArrayIndex >= (m_uiLength / 4))
				{
					CORE3D_ERROR(_T("IndexBuffer::GetVertexIndex() - Parameter array-index exceeds vertex buffer size.\n"));
					return INVALID_PARAMETERS;
				}
				const UINT32* pData = reinterpret_cast<UINT32*>(m_pData);
				ruiValue			= pData[uiArrayIndex];
			}
			return OK;
		}
		CORE3D_ERROR(_T("IndexBuffer::GetVertexIndex() - Invalid format.\n"));
		return INVALID_FORMAT;
	}

	Device* IndexBuffer::GetDevice()
	{
		if(NULL != m_pkDevice) {m_pkDevice->AddRef();}
		return m_pkDevice;
	}

	UINT32 IndexBuffer::GetLength()
	{
		return m_uiLength;
	}

	Format IndexBuffer::GetFormat()
	{
		return m_eFormat;
	}
}