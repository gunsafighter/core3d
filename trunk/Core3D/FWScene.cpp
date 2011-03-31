#include "FWScene.h"
#include "FWGraphics.h"
#include "FWApplication.h"
#include "FWEntity.h"
#include "FWLight.h"

namespace Core3D
{
	FWScene::FWScene(FWApplication* pkApp)
	{
		m_pkApplication			= pkApp;
		m_uiNumCreatedEntities	= 0;
		m_uiNumCreatedLights	= 0;
		m_uiCurrentLight		= 0;
		SetClearColor(Vector4(0.30f, 0.25f, 0.35f, 1.0f));
		SetAmbientLightColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	}

	FWScene::~FWScene()
	{
		while(m_vecSceneEntities.size())	{ReleaseEntity(m_vecSceneEntities.begin()->hEntity);}
		while(m_vecSceneLights.size())		{ReleaseLight(m_vecSceneLights.begin()->hLight);}
	}

	bool FWScene::Initialize()
	{
		return true;
	}

	void FWScene::RegisterEntityType(tstring strTypeName, PFN_CREATEFUNCTION pfnCreateFunction)
	{
		m_mapRegEntityTypes[strTypeName] = pfnCreateFunction;
	}

	HENTITY FWScene::CreateEntity(tstring strTypeName, bool bSceneProcess /* = true */)
	{
		PFN_CREATEFUNCTION pfnCreateFunction = m_mapRegEntityTypes[strTypeName];
		if(NULL == pfnCreateFunction)	{return 0;}
		
		FWEntity* pkEntity = pfnCreateFunction(this);
		if(NULL == pkEntity)			{return 0;}

		SceneEntity kNewEntity = {++m_uiNumCreatedEntities, pkEntity, bSceneProcess};
		m_vecSceneEntities.push_back(kNewEntity);
		return kNewEntity.hEntity;
	}

	std::vector<FWScene::SceneEntity>::iterator 
	FWScene::GetSceneEntityIterator(HENTITY hEntity)
	{
		if(0 == hEntity) {return m_vecSceneEntities.end();}
		for(std::vector<SceneEntity>::iterator iterSceneEntity = m_vecSceneEntities.begin(); 
			iterSceneEntity != m_vecSceneEntities.end(); ++iterSceneEntity)
		{
			if(iterSceneEntity->hEntity == hEntity) {return iterSceneEntity;}
		}
		return m_vecSceneEntities.end();
	}

	void FWScene::ReleaseEntity(HENTITY hEntity)
	{
		std::vector<SceneEntity>::iterator iterSceneEntity = GetSceneEntityIterator(hEntity);
		if(iterSceneEntity != m_vecSceneEntities.end())
		{
			UINT32 uiOldSize = static_cast<UINT32>(m_vecSceneEntities.size());
			if(NULL != iterSceneEntity->pkEntity)
			{
				CORE3D_SAFE_DELETE(iterSceneEntity->pkEntity);
			}
			if(uiOldSize != static_cast<UINT32>(m_vecSceneEntities.size()))
			{
				iterSceneEntity = GetSceneEntityIterator(hEntity);
			}
			m_vecSceneEntities.erase(iterSceneEntity);
		}
	}

	FWEntity* FWScene::GetEntity(HENTITY hEntity)
	{
		std::vector<SceneEntity>::iterator iterSceneEntity = GetSceneEntityIterator(hEntity);
		if(iterSceneEntity != m_vecSceneEntities.end()) {return iterSceneEntity->pkEntity;}
		return NULL;
	}

	HLIGHT FWScene::CreateLight()
	{
		SceneLight kNewLight = {++m_uiNumCreatedLights, new FWLight(this)};
		m_vecSceneLights.push_back(kNewLight);
		return kNewLight.hLight;
	}

	std::vector<FWScene::SceneLight>::iterator 
	FWScene::GetSceneLightIterator(HLIGHT hLight)
	{
		if(0 == hLight) {return m_vecSceneLights.end();}
		for(std::vector<SceneLight>::iterator iterSceneLight = m_vecSceneLights.begin(); 
			iterSceneLight != m_vecSceneLights.end(); ++iterSceneLight)
		{
			if(iterSceneLight->hLight == hLight) {return iterSceneLight;}
		}
		return m_vecSceneLights.end();
	}

	void FWScene::ReleaseLight(HLIGHT hLight)
	{
		std::vector<SceneLight>::iterator iterSceneLight = GetSceneLightIterator(hLight);
		if(iterSceneLight != m_vecSceneLights.end())
		{
			UINT32 uiOldSize = static_cast<UINT32>(m_vecSceneLights.size());
			if(NULL != iterSceneLight->pkLight)
			{
				CORE3D_SAFE_DELETE(iterSceneLight->pkLight);
			}
			if(uiOldSize != static_cast<UINT32>(m_vecSceneLights.size()))
			{
				iterSceneLight = GetSceneLightIterator(hLight);
			}
			m_vecSceneLights.erase(iterSceneLight);
		}
	}

	FWLight* FWScene::GetLight(HLIGHT hLight)
	{
		std::vector<SceneLight>::iterator iterSceneLight = GetSceneLightIterator(hLight);
		if(iterSceneLight != m_vecSceneLights.end()) {return iterSceneLight->pkLight;}
		return NULL;
	}

	void FWScene::FrameMove()
	{
		for(std::vector<SceneEntity>::iterator iterSceneEntity = m_vecSceneEntities.begin(); 
			iterSceneEntity != m_vecSceneEntities.end(); ++iterSceneEntity)
		{
			if(false == iterSceneEntity->bSceneProcess) {continue;}
			iterSceneEntity->pkEntity->FrameMove();
		}
	}

	void FWScene::Render(UINT32 uiPass)
	{
		FWGraphics* pkGraphics = m_pkApplication->GetGraphics();
		for(std::vector<SceneEntity>::iterator iterSceneEntity = m_vecSceneEntities.begin(); 
			iterSceneEntity != m_vecSceneEntities.end(); ++iterSceneEntity)
		{
			if(false == iterSceneEntity->bSceneProcess) {continue;}
			pkGraphics->PushStateBlock();
			iterSceneEntity->pkEntity->Render(uiPass);
			pkGraphics->PopStateBlock();
		}
	}

	FWApplication* FWScene::GetApplication()
	{
		return m_pkApplication;
	}

	FWLight* FWScene::GetLightFromNum(UINT32 uiNum)
	{
		return m_vecSceneLights[uiNum].pkLight;
	}

	FWLight* FWScene::GetCurrentLight()
	{
		return GetLightFromNum(m_uiCurrentLight);
	}
}