#include "FWTexture.h"
#include "FWGraphics.h"
#include "FWResManager.h"
#include "FWApplication.h"

namespace Core3D
{
	FWTexture::FWTexture(FWResManager* pkResMgr, Texture* pkTexture)
	{
		// COMMENT : Standard texture
		m_pkResManager		= pkResMgr;
		m_uiNumTextures		= 1;
		m_ppkTextures		= new Texture*[m_uiNumTextures];
		m_ppkTextures[0]	= pkTexture;
		m_pkCubeTexture		= NULL;
		m_eTextureType		= TEXTURETYPE_DEFAULT;
	}

	FWTexture::FWTexture(FWResManager* pkResMgr, CubeTexture* pkTexture)
	{
		// COMMENT : Cube texture
		m_pkResManager		= pkResMgr;
		m_uiNumTextures		= 0;
		m_ppkTextures		= NULL;
		m_pkCubeTexture		= pkTexture;
		m_eTextureType		= TEXTURETYPE_CUBE;
	}

	FWTexture::FWTexture(FWResManager* pkResMgr, UINT32 uiNumTextures, FLOAT32 fFPS, Texture** ppkTexture)
	{
		// COMMENT : Animated texture
		m_pkResManager		= pkResMgr;
		m_uiNumTextures		= uiNumTextures;
		m_fFPS				= fFPS;
		m_ppkTextures		= ppkTexture;
		m_fCurrentTexture	= 0.0f;
		m_pkCubeTexture		= NULL;
		m_eTextureType		= TEXTURETYPE_DEFAULT;
	}

	FWTexture::~FWTexture()
	{
		CORE3D_SAFE_RELEASE(m_pkCubeTexture);
		for(UINT32 i = 0; i < m_uiNumTextures; ++i) {CORE3D_SAFE_RELEASE(m_ppkTextures[i]);}
		CORE3D_SAFE_DELETEARRAY(m_ppkTextures);
	}

	FWResManager* FWTexture::GetResManager()
	{
		return m_pkResManager;
	}

	BaseTexture* FWTexture::GetTexture()
	{
		if(1 == m_uiNumTextures)	{return m_ppkTextures[0];}
		if(NULL != m_pkCubeTexture) {return m_pkCubeTexture;}

		m_fCurrentTexture += m_fFPS * m_pkResManager->GetApplication()->GetInvFPS();
		UINT32 uiTexIndex = static_cast<UINT32>(m_fCurrentTexture);
		if((uiTexIndex >= m_uiNumTextures) || (uiTexIndex < 0))
		{
			m_fCurrentTexture	= 0.0f;
			uiTexIndex			= 0;
		}
		return m_ppkTextures[uiTexIndex];
	}

	FWTexture::TextureType FWTexture::GetTextureType() const
	{
		return m_eTextureType;
	}
}