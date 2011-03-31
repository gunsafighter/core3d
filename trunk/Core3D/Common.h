#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include "Core3DBase.h"
#include <float.h>
#include <math.h>

#define CORE3D_PI 3.141592654f

namespace Core3D
{
	// COMMENT : Converts radians to degrees
	inline 
	FLOAT32 RadToDeg(const FLOAT32 fVal) {return 180.0f * fVal / CORE3D_PI;}
	// COMMENT : Converts degrees to radians
	inline 
	FLOAT32 DegToRad(const FLOAT32 fVal) {return CORE3D_PI * fVal / 180.0f;}
	// COMMENT : Clamps a template value
	template<class T> 
	inline 
	T Clamp(const T Val, const T Lower, const T Upper)
	{
		if(Val <= Lower)		{return Lower;}
		else if(Val >= Upper)	{return Upper;}
		return Val;
	}
	// COMMENT : Clamps a floating-point value to [0.0f, 1.0f]
	inline 
	FLOAT32 Saturate(const FLOAT32 fVal)
	{
		return Clamp<FLOAT32>(fVal, 0.0f, 1.0f);
	}
	// COMMENT : Linearly interpolates between two values
	inline 
	FLOAT32 Lerp(const FLOAT32 fValA, const FLOAT32 fValB, const FLOAT32 fInterpolation)
	{
		return fValA + (fValB - fValA) * fInterpolation;
	}
}