#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <tchar.h>
#include <memory.h>
#include <vector>

#include <float.h>

// COMMENT : Basic macro definitions
#define CORE3D_SAFE_RELEASE(p)		{if((p)) {(p)->Release(); (p) = NULL;}}
#define CORE3D_SAFE_DELETE(p)		{if((p)) {delete (p); (p) = NULL;}}
#define CORE3D_SAFE_DELETEARRAY(p)	{if((p)) {delete[] (p); (p) = NULL;}}

namespace Core3D
{
	// COMMENT : Basic variable definitions
	typedef char			INT8;
	typedef short			INT16;
	typedef int				INT32;
	typedef unsigned char	UINT8;
	typedef unsigned short	UINT16;
	typedef unsigned int	UINT32;
	typedef float			FLOAT32;
	typedef double			FLOAT64;
	typedef unsigned char	BYTE8;

	// COMMENT : Describes function return values
	enum Result
	{
		OK = 0,
		UNKNOWN, 
		INVALID_PARAMETERS, 
		OUT_OF_MEMORY, 
		INVALID_FORMAT, 
		INVALID_STATE
	};

	// COMMENT : custom boolean type
	enum BoolType
	{
		BT_FALSE = 0,
		BT_TRUE,
	};

	// COMMENT : Sets FPU to truncation mode and single precision.
	inline void		FpuTruncate()	{_set_controlfp(_RC_DOWN + _PC_24, _MCW_RC | _MCW_PC);}
	// COMMENT : Resets FPU to default.
	inline void		FpuReset()		{_set_controlfp(_CW_DEFAULT, _MCW_RC | _MCW_PC);}
	// COMMENT : Performs fast float to integer conversion.
	inline INT32	FtoL(FLOAT32 f)
	{
		static INT32 temp;
		__asm
		{
			fld f
			fistp temp
		}
		return temp;
	}

	// COMMENT : RefObject is the base class for all Core3D classes.
	// It implements a reference counter with functions AddRef() and Release() known from COM interfaces
	class RefObject
	{
	protected:
		RefObject() : m_uiRefCount(1)	{}
		virtual ~RefObject()			{}
	private:
		RefObject(const RefObject&);
		RefObject& operator=(const RefObject&);
	public:
		inline void AddRef()	{++m_uiRefCount;}
		inline void Release()	{if(0 == (--m_uiRefCount)) {delete this;}}
	private:
		UINT32 m_uiRefCount;
	};
}