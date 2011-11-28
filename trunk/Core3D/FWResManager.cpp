#include "FWResManager.h"
#include "FWApplication.h"
#include "FWGraphics.h"
#include "FWFileIO.h"
#include "FWModel.h"
#include "FWTexture.h"

#include "libpng/png.h"
#include <Windows.h>

namespace Core3D
{
	//-------------------------------------------------------------------------------------------------
	// COMMENT : .Obj model file
	void* LoadObjModel(FWResManager* pkResMgr, tstring strFileName)
	{
		FWFileIO* pkFileIO	= pkResMgr->GetApplication()->GetFileIO();
		char* pData			= NULL;
		UINT32 uiLength		= pkFileIO->ReadFile(strFileName, (BYTE8**)&pData, true);
		if(0 == uiLength) {return NULL;}

		FWObjModel* pkModel	= new FWObjModel(pkResMgr);
		bool bResult		= pkModel->LoadModel(pData);
		CORE3D_SAFE_DELETEARRAY(pData);

		if(false == bResult) {CORE3D_SAFE_DELETE(pkModel); return NULL;}
		return pkModel;
	}

	void UnloadObjModel(FWResManager* pkResMgr, void* pvResource)
	{
		FWObjModel* pkModel = reinterpret_cast<FWObjModel*>(pvResource);
		CORE3D_SAFE_DELETE(pkModel);
	}
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// COMMENT : .png texture file
	void PNGReadData(png_structp pkPNG, png_bytep pData, png_size_t nLength)
	{
		memcpy(pData, pkPNG->io_ptr, nLength);
		pkPNG->io_ptr = reinterpret_cast<BYTE8*>(pkPNG->io_ptr) + nLength;
	}

	bool LoadPNGTexture(Texture** ppkTexture, const BYTE8* pData, Device* pkDevice)
	{
		png_structp pkPNG		= png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
		if(NULL == pkPNG)		{return false;}

		png_infop pkInfo		= png_create_info_struct(pkPNG);
		if(NULL == pkInfo)		{png_destroy_read_struct(&pkPNG, (png_infopp)NULL, (png_infopp)NULL); return false;}

		png_infop pkEndInfo		= png_create_info_struct(pkPNG);
		if(NULL == pkEndInfo)	{png_destroy_read_struct(&pkPNG, &pkInfo, (png_infopp)NULL); return false;}

		if(setjmp(pkPNG->jmpbuf)) {png_destroy_read_struct(&pkPNG, &pkInfo, &pkEndInfo); return false;}

		png_set_read_fn(pkPNG, (png_voidp*)pData, PNGReadData);
		png_read_info(pkPNG, pkInfo);

		UINT32 uiDimX			= png_get_image_width(pkPNG, pkInfo);
		UINT32 uiDimY			= png_get_image_height(pkPNG, pkInfo);
		INT32 iColorType		= png_get_color_type(pkPNG, pkInfo);
		if(PNG_COLOR_TYPE_PALETTE == iColorType && png_get_bit_depth(pkPNG, pkInfo) <= 8)	{png_set_expand(pkPNG);}
		if(PNG_COLOR_TYPE_GRAY == iColorType && png_get_bit_depth(pkPNG, pkInfo) < 8)		{png_set_expand(pkPNG);}
		if(PNG_COLOR_TYPE_GRAY == iColorType && PNG_COLOR_TYPE_GRAY_ALPHA == iColorType)	{png_set_gray_to_rgb(pkPNG);}
		if(16 == png_get_bit_depth(pkPNG, pkInfo))											{png_set_strip_16(pkPNG);}

		png_read_update_info(pkPNG, pkInfo);

		iColorType				= png_get_color_type(pkPNG, pkInfo);
		bool bHasAlpha			= (PNG_COLOR_TYPE_RGB_ALPHA == iColorType);
		Format eTextureFormat	= bHasAlpha ? FMT_R32G32B32A32F : FMT_R32G32B32F;

		if(CORE3D_FAILED(pkDevice->CreateTexture(ppkTexture, uiDimX, uiDimY, 0, eTextureFormat)))
		{
			png_destroy_read_struct(&pkPNG, &pkInfo, &pkEndInfo);
			return false;
		}

		BYTE8* pDataBuffer	= new BYTE8[uiDimY * png_get_rowbytes(pkPNG, pkInfo)];
		BYTE8* pCurrentData = pDataBuffer;
		BYTE8** ppRows		= new BYTE8*[uiDimY];
		for(UINT32 i = 0; i < uiDimY; ++i)
		{
			ppRows[i]		= pCurrentData;
			pCurrentData	+= png_get_rowbytes(pkPNG, pkInfo);
		}

		png_read_image(pkPNG, ppRows);
		CORE3D_SAFE_DELETEARRAY(ppRows);

		png_read_end(pkPNG, pkEndInfo);
		png_destroy_read_struct(&pkPNG, &pkInfo, &pkEndInfo);

		FLOAT32* pfTexData = NULL;
		Result eResLock = (*ppkTexture)->LockRect(0, (void**)&pfTexData, NULL);
		if(CORE3D_FAILED(eResLock))
		{
			CORE3D_SAFE_DELETEARRAY(pDataBuffer);
			return false;
		}

		const FLOAT32 COLOR_SCALE	= 1.0f / 255.0f;
		pCurrentData				= pDataBuffer;
		for(UINT32 uiY = 0; uiY < uiDimY; ++uiY)
		{
			for(UINT32 uiX = 0; uiX < uiDimX; ++uiX)
			{
				(*pfTexData++) = (FLOAT32)(*pCurrentData++) * COLOR_SCALE;
				(*pfTexData++) = (FLOAT32)(*pCurrentData++) * COLOR_SCALE;
				(*pfTexData++) = (FLOAT32)(*pCurrentData++) * COLOR_SCALE;
				if(true == bHasAlpha) {(*pfTexData++) = (FLOAT32)(*pCurrentData++) * COLOR_SCALE;}
			}
		}
		CORE3D_SAFE_DELETEARRAY(pDataBuffer);
		(*ppkTexture)->UnlockRect(0);
		return true;
	}
	//-------------------------------------------------------------------------------------------------

	//-------------------------------------------------------------------------------------------------
	// COMMENT : Core3D texture file
	void* LoadTexture(FWResManager* pkResMgr, tstring strFileName)
	{
		FWGraphics* pkGraphics	= pkResMgr->GetApplication()->GetGraphics();
		FWFileIO* pkFileIO		= pkResMgr->GetApplication()->GetFileIO();
		BYTE8* pData			= NULL;
		UINT32 uiLength			= pkFileIO->ReadFile(strFileName, &pData);
		if(0 == uiLength) {return NULL;}

		Texture* pkTexture		= NULL;
		bool bResult			= LoadPNGTexture(&pkTexture, pData, pkGraphics->GetDevice());
		CORE3D_SAFE_DELETEARRAY(pData);
		if(false == bResult) {return 0;}

		pkTexture->GenerateMipSubLevels(0);
		return new FWTexture(pkResMgr, pkTexture);
	}

	void* LoadCubeTexture(FWResManager* pkResMgr, tstring strFileName)
	{
		FWGraphics* pkGraphics	= pkResMgr->GetApplication()->GetGraphics();
		FWFileIO* pkFileIO		= pkResMgr->GetApplication()->GetFileIO();
		BYTE8* pData			= NULL;
		UINT32 uiLength			= pkFileIO->ReadFile(strFileName, &pData, true);
		if(0 == uiLength) {return NULL;}

		std::vector<tstring> vecFileNames;
	#ifdef _UNICODE
		tchar strBuffer[256];
		::MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<char*>(pData), -1, strBuffer, sizeof(strBuffer));
		tchar* pCurrentPosition = strBuffer;
	#else
		tchar* pCurrentPosition	= reinterpret_cast<tchar*>(pData);
	#endif
		while(true)
		{
			tchar* pEndOfLine	= _tcschr(pCurrentPosition, _T('\n'));
			if(NULL != pEndOfLine)	{*pEndOfLine = NULL;}

			if(0 == _tcslen(pCurrentPosition)) {break;}
			
			tstring strTexture	= pCurrentPosition;
			vecFileNames.push_back(strTexture);

			if(NULL != pEndOfLine)	{pCurrentPosition = pEndOfLine + 1;}
			else					{break;}
		}
		CORE3D_SAFE_DELETEARRAY(pData);

		UINT32 uiNumTextures = static_cast<UINT32>(vecFileNames.size());
		if(6 != uiNumTextures) {return NULL;}

		UINT32 uiEdgeLength		= 0;
		Format eFmtCubeFormat	= FMT_R32G32B32F;
		Texture** ppkTextures	= new Texture*[uiNumTextures];
		UINT32 ui;
		for(ui = 0; ui < uiNumTextures; ++ui)
		{
			ppkTextures[ui]		= NULL;
			BYTE8* pTexData		= NULL;
			UINT32 uiTexLength	= pkFileIO->ReadFile(vecFileNames[ui], &pTexData);
			if(0 == uiTexLength)
			{
				for(UINT32 uj = 0; uj < ui; ++uj) {CORE3D_SAFE_RELEASE(ppkTextures[uj]);}
				CORE3D_SAFE_DELETEARRAY(ppkTextures);
				return NULL;
			}

			bool bResult = LoadPNGTexture(&ppkTextures[ui], pTexData, pkGraphics->GetDevice());
			CORE3D_SAFE_DELETEARRAY(pTexData);
			if(false == bResult)
			{
				for(UINT32 uj = 0; uj < ui; ++uj) {CORE3D_SAFE_RELEASE(ppkTextures[uj]);}
				CORE3D_SAFE_DELETEARRAY(ppkTextures);
				return NULL;
			}

			if(0 == ui)
			{
				uiEdgeLength	= ppkTextures[ui]->GetWidth();
				eFmtCubeFormat	= ppkTextures[ui]->GetFormat();
			}

			if( (ppkTextures[ui]->GetWidth() != ppkTextures[ui]->GetHeight()) || 
				(ppkTextures[ui]->GetWidth() != uiEdgeLength) || 
				(ppkTextures[ui]->GetFormat() != eFmtCubeFormat) )
			{
				for(UINT32 uj = 0; uj < ui; ++uj) {CORE3D_SAFE_RELEASE(ppkTextures[uj]);}
				CORE3D_SAFE_DELETEARRAY(ppkTextures);
				return NULL;
			}
		}

		CubeTexture* pkCubeTexture = NULL;
		if(CORE3D_FAILED(pkGraphics->GetDevice()->CreateCubeTexture(&pkCubeTexture, uiEdgeLength, 0, eFmtCubeFormat)))
		{
			for(UINT32 uj = 0; uj < ui; ++uj) {CORE3D_SAFE_RELEASE(ppkTextures[uj]);}
			CORE3D_SAFE_DELETEARRAY(ppkTextures);
			return NULL;
		}

		UINT32 uiNumBytes = uiEdgeLength * uiEdgeLength;
		switch(eFmtCubeFormat)
		{
		case FMT_R32F:			uiNumBytes *= sizeof(FLOAT32);		break;
		case FMT_R32G32F:		uiNumBytes *= 2 * sizeof(FLOAT32);	break;
		case FMT_R32G32B32F:	uiNumBytes *= 3 * sizeof(FLOAT32);	break;
		case FMT_R32G32B32A32F: uiNumBytes *= 4 * sizeof(FLOAT32);	break;
		default: break;
		}

		for(UINT32 uiFace = CF_POSITIVE_X; uiFace <= CF_NEGATIVE_Z; ++uiFace)
		{
			BYTE8* pDest	= NULL;
			pkCubeTexture->LockRect((CubeFaces)uiFace, 0, (void**)&pDest, NULL);

			BYTE8* pSrc		= NULL;
			ppkTextures[uiFace]->LockRect(0, (void**)&pSrc, NULL);
			memcpy(pDest, pSrc, uiNumBytes);
			ppkTextures[uiFace]->UnlockRect(0);
			CORE3D_SAFE_RELEASE(ppkTextures[uiFace]);

			pkCubeTexture->UnlockRect((CubeFaces)uiFace, 0);
		}
		CORE3D_SAFE_DELETEARRAY(ppkTextures);

		pkCubeTexture->GenerateMipSubLevels(0);
		return new FWTexture(pkResMgr, pkCubeTexture);
	}

	void* LoadAnimatedTexture(FWResManager* pkResMgr, tstring strFileName)
	{
		FWGraphics* pkGraphics	= pkResMgr->GetApplication()->GetGraphics();
		FWFileIO* pkFileIO		= pkResMgr->GetApplication()->GetFileIO();
		BYTE8* pData			= NULL;
		UINT32 uiLength			= pkFileIO->ReadFile(strFileName, &pData, true);
		if(0 == uiLength) {return NULL;}

		FLOAT32 fFPS			= 0.0f;
		std::vector<tstring> vecFileNames;
		tchar* pCurrentPosition	= reinterpret_cast<tchar*>(pData);

		_stscanf_s(pCurrentPosition, _T("%f"), &fFPS);
		pCurrentPosition		= _tcschr(pCurrentPosition, _T('\n'));
		if(NULL == pCurrentPosition)	{CORE3D_SAFE_DELETEARRAY(pData); return NULL;}
		else							{pCurrentPosition += 1;}

		if(fFPS <= 0.0f) {CORE3D_SAFE_DELETEARRAY(pData); return NULL;}

		while(true)
		{
			tchar* pEndOfLine	= _tcschr(pCurrentPosition, _T('\n'));
			if(NULL != pEndOfLine)	{*pEndOfLine = NULL;}
			
			if(0 == _tcslen(pCurrentPosition)) {break;}
			
			tstring strTexture	= pCurrentPosition;
			vecFileNames.push_back(strTexture);

			if(NULL != pEndOfLine)	{pCurrentPosition = pEndOfLine + 1;}
			else					{break;}
		}
		CORE3D_SAFE_DELETEARRAY(pData);

		UINT32 uiNumTextures	= static_cast<UINT32>(vecFileNames.size());
		if(0 == uiNumTextures) {return NULL;}

		Texture** ppkTexture	= new Texture*[uiNumTextures];
		for(UINT32 ui = 0; ui < uiNumTextures; ++ui)
		{
			BYTE8* pTexData		= NULL;
			UINT32 uiTexLength	= pkFileIO->ReadFile(vecFileNames[ui], &pTexData);
			if(0 == uiTexLength)
			{
				for(UINT32 uj = 0; uj < ui; ++uj) {CORE3D_SAFE_RELEASE(ppkTexture[uj]);}
				CORE3D_SAFE_DELETEARRAY(ppkTexture);
				return NULL;
			}

			bool bResult = LoadPNGTexture(&ppkTexture[ui], pTexData, pkGraphics->GetDevice());
			CORE3D_SAFE_DELETEARRAY(pTexData);
			if(false == bResult)
			{
				for(UINT32 uj = 0; uj < ui; ++uj) {CORE3D_SAFE_RELEASE(ppkTexture[uj]);}
				CORE3D_SAFE_DELETEARRAY(ppkTexture);
				return NULL;
			}
			ppkTexture[ui]->GenerateMipSubLevels(0);
		}
		
		return new FWTexture(pkResMgr, uiNumTextures, fFPS, ppkTexture);
	}

	void UnloadTexture(FWResManager* pkResMgr, void* pvResource)
	{
		FWTexture* pkTexture = reinterpret_cast<FWTexture*>(pvResource);
		CORE3D_SAFE_DELETE(pkTexture);
	}
	//-------------------------------------------------------------------------------------------------

	FWResManager::FWResManager(FWApplication* pkApp)
	{
		m_pkApplication			= pkApp;
		m_uiNumLoadResources	= 0;
	}

	FWResManager::~FWResManager()
	{
		while(m_vecManagedResources.size())
		{
			ReleaseResource(m_vecManagedResources.begin()->hResource);
		}
	}

	bool FWResManager::Initialize()
	{
		RegisterResourceExtension(_T("obj"),	LoadObjModel,			UnloadObjModel);
		RegisterResourceExtension(_T("png"),	LoadTexture,			UnloadTexture);
		RegisterResourceExtension(_T("cube"),	LoadCubeTexture,		UnloadTexture);
		RegisterResourceExtension(_T("anim"),	LoadAnimatedTexture,	UnloadTexture);
		return true;
	}

	void FWResManager::RegisterResourceExtension(tstring strExtension, PFN_LOADFUNCTION pfnLoadFunction, PFN_UNLOADFUNCTION pfnUnloadFunction)
	{
		m_mapRegEntityExtensionsLoad[strExtension]		= pfnLoadFunction;
		m_mapRegEntityExtensionsUnload[strExtension]	= pfnUnloadFunction;
	}

	HRESOURCE FWResManager::LoadResource(tstring strFileName)
	{
		// COMMENT : Look if we have already loaded this resource
		for(std::vector<ManagedResource>::iterator iterManagedRes = m_vecManagedResources.begin(); 
			iterManagedRes != m_vecManagedResources.end(); ++iterManagedRes)
		{
			if(iterManagedRes->strFileName == strFileName)
			{
				++iterManagedRes->uiReferences;
				return iterManagedRes->hResource;
			}
		}

		// COMMENT : We have to load it from the disk
		const tchar* lpszCheck	= strFileName.c_str();
		const tchar* lpszExt	= NULL;
		while(*lpszCheck)
		{
			if(_T('.') == *lpszCheck) {lpszExt = lpszCheck;}
			++lpszCheck;
		}
		if(NULL == lpszExt) {return NULL;}

		tstring strExtension				= (lpszExt + 1);
		PFN_LOADFUNCTION pfnLoadFunction	= m_mapRegEntityExtensionsLoad[strExtension];
		if(NULL == pfnLoadFunction) {return NULL;}

		ManagedResource kNewResource;
		kNewResource.pvResource		= pfnLoadFunction(this, strFileName);
		if(NULL == kNewResource.pvResource) {return NULL;}

		kNewResource.hResource		= ++m_uiNumLoadResources;
		kNewResource.uiReferences	= 1;
		kNewResource.strFileName	= strFileName;
		kNewResource.strExtension	= strExtension;
		m_vecManagedResources.push_back(kNewResource);
		return kNewResource.hResource;
	}

	std::vector<FWResManager::ManagedResource>::iterator 
	FWResManager::GetManagedResourceIterator(HRESOURCE hRes)
	{
		if(0 == hRes) {return m_vecManagedResources.end();}
		for(std::vector<ManagedResource>::iterator iterManagedRes = m_vecManagedResources.begin(); 
			iterManagedRes != m_vecManagedResources.end(); ++iterManagedRes)
		{
			if(iterManagedRes->hResource == hRes) {return iterManagedRes;}
		}
		return m_vecManagedResources.end();
	}

	void FWResManager::ReleaseResource(HRESOURCE hRes)
	{
		std::vector<ManagedResource>::iterator iterManagedRes = GetManagedResourceIterator(hRes);
		if(iterManagedRes != m_vecManagedResources.end())
		{
			if(0 == --iterManagedRes->uiReferences)
			{
				PFN_UNLOADFUNCTION pfnUnloadFunction	= m_mapRegEntityExtensionsUnload[iterManagedRes->strExtension];
				UINT32 uiOldSize						= static_cast<UINT32>(m_vecManagedResources.size());
				if(NULL != pfnUnloadFunction)
				{
					pfnUnloadFunction(this, iterManagedRes->pvResource);
				}
				if(uiOldSize != static_cast<UINT32>(m_vecManagedResources.size()))
				{
					iterManagedRes = GetManagedResourceIterator(hRes);
				}
				m_vecManagedResources.erase(iterManagedRes);
			}
		}
	}

	void* FWResManager::GetResource(HRESOURCE hRes)
	{
		std::vector<ManagedResource>::iterator iterManagedRes = GetManagedResourceIterator(hRes);
		if(iterManagedRes != m_vecManagedResources.end()) {return iterManagedRes->pvResource;}
		return NULL;
	}

	FWApplication* FWResManager::GetApplication()
	{
		return m_pkApplication;
	}
}