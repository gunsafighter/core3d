#include "FWFileIO.h"
#include "FWApplication.h"
#include <stdio.h>

namespace Core3D
{
	FWFileIO::FWFileIO(FWApplication* pkApp)
	{
		m_pkApplication = pkApp;
	}

	FWFileIO::~FWFileIO()
	{

	}

	bool FWFileIO::Initialize()
	{
		return true;
	}

	UINT32 FWFileIO::ReadFile(tstring strFileName, BYTE8** ppData, bool bText /* = false */)
	{
		const tchar* lpszFilePath	= DiskFilePath(strFileName);
		const tchar* lpszMode		= bText ? _T("rt") : _T("rb");

		FILE* pkFile = NULL;
		_tfopen_s(&pkFile, lpszFilePath, lpszMode);
		if(NULL == pkFile) {return 0;}

		fseek(pkFile, 0, SEEK_END);
		UINT32 uiSize = ftell(pkFile);
		rewind(pkFile);

		*ppData = new BYTE8[bText ? uiSize + 1 : uiSize];
		uiSize	= static_cast<UINT32>(fread(*ppData, 1, uiSize, pkFile));
		fclose(pkFile);

		if(true == bText) {(*ppData)[uiSize] = 0;}
		return uiSize;
	}

	UINT32 FWFileIO::ReadFile(tstring strFileName, BYTE8* pData, UINT32 uiSize, bool bText /* = false */)
	{
		const tchar* lpszFilePath	= DiskFilePath(strFileName);
		const tchar* lpszMode		= bText ? _T("rt") : _T("rb");

		FILE* pkFile = NULL;
		_tfopen_s(&pkFile, lpszFilePath, lpszMode);
		if(NULL == pkFile) {return 0;}

		fseek(pkFile, 0, SEEK_END);
		UINT32 uiFileSize = ftell(pkFile);
		rewind(pkFile);

		uiFileSize = static_cast<UINT32>(fread(pData, 1, (uiFileSize < uiSize) ? uiFileSize : uiSize, pkFile));
		fclose(pkFile);

		if(true == bText) {pData[uiFileSize] = 0;}
		return uiFileSize;
	}

	UINT32 FWFileIO::WriteFile(tstring strFileName, BYTE8* pData, UINT32 uiSize, bool bText /* = false */)
	{
		const tchar* lpszFilePath	= DiskFilePath(strFileName);
		const tchar* lpszMode		= bText ? _T("wt") : _T("wb");

		FILE* pkFile = NULL;
		_tfopen_s(&pkFile, lpszFilePath, lpszMode);
		if(NULL == pkFile) {return 0;}

		UINT32 uiWrittenBytes = static_cast<UINT32>(fwrite(pData, 1, uiSize, pkFile));
		fclose(pkFile);
		
		return uiWrittenBytes;
	}

	bool FWFileIO::FileExists(tstring strFileName)
	{
		const tchar* lpszFilePath = DiskFilePath(strFileName);

		FILE* pkFile = NULL;
		_tfopen_s(&pkFile, lpszFilePath, _T("rb"));
		if(NULL == pkFile) {return false;}
		fclose(pkFile);
		return true;
	}

	FWApplication* FWFileIO::GetApplication()
	{
		return m_pkApplication;
	}

	const tchar* FWFileIO::DiskFilePath(tstring strFileName)
	{
		static tchar szPath[512] = _T("");
		_stprintf_s(szPath, 512, _T("%s/%s"), DATA_PATH, strFileName.c_str());
		return szPath;
	}
}