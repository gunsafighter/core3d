#include "BaseTexture.h"
#include "Device.h"

namespace Core3D
{
	BaseTexture::BaseTexture(Device* pkDevice)
		: m_pkDevice(pkDevice)
	{
		m_pkDevice->AddRef();
	}

	BaseTexture::~BaseTexture()
	{
		CORE3D_SAFE_RELEASE(m_pkDevice);
	}

	Device* BaseTexture::GetDevice()
	{
		if(NULL != m_pkDevice) {m_pkDevice->AddRef();}
		return m_pkDevice;
	}
}