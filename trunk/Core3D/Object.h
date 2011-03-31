#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DTypes.h"

namespace Core3D
{
	class Device;
	class Object : public RefObject
	{
	protected:
		Object();
		virtual ~Object();
		friend Result CreateObject(Object** ppkObject);

	public:
		Result CreateDevice(Device** ppkDevice, DeviceParameters* pkDeviceParameters);
	};

	Result CreateObject(Object** ppkObject);
}