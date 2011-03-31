#include "Object.h"
#include "Device.h"

namespace Core3D
{
	Object::Object()
	{

	}

	Object::~Object()
	{

	}

	Result Object::CreateDevice(Device** ppkDevice, DeviceParameters* pkDeviceParameters)
	{
		if(NULL == ppkDevice)
		{
			CORE3D_ERROR(_T("Object::CreateDevice() - Parameter device pointer pointer to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		if(NULL == pkDeviceParameters)
		{
			*ppkDevice = NULL;
			CORE3D_ERROR(_T("Object::CreateDevice() - Parameter device-parameter pointer to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		*ppkDevice = new Device(this, pkDeviceParameters);
		if(NULL == *ppkDevice)
		{
			CORE3D_ERROR(_T("Object::CreateDevice() - Out of memory, cannot create device.\n"));
			return OUT_OF_MEMORY;
		}
		return (*ppkDevice)->Create();
	}

	Result CreateObject(Object** ppkObject)
	{
		if(NULL == ppkObject)
		{
			CORE3D_ERROR(_T("CreateObject() - Parameter object pointer to NULL.\n"));
			return INVALID_PARAMETERS;
		}

		*ppkObject = new Object;
		if(NULL == *ppkObject)
		{
			CORE3D_ERROR(_T("CreateObject() - Out of memory, cannot create object instance.\n"));
			return OUT_OF_MEMORY;
		}
		return OK;
	}
}