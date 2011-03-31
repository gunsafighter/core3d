#pragma once
//////////////////////////////////////////////////////////////////////////
// Core3D : Framework Library for Software Graphic API
// Copyright (C) 2009 DevCoder <renderwizard@gmail.com>
//////////////////////////////////////////////////////////////////////////
#include <string>
#include "Core3D.h"

#ifdef _UNICODE
	typedef std::wstring	tstring;
	typedef wchar_t			tchar;
#else
	typedef std::string		tstring;
	typedef char			tchar;
#endif