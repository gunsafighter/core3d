#include "FWModel.h"
#include "FWGraphics.h"
#include "FWResManager.h"
#include "FWApplication.h"

namespace Core3D
{
	FWObjModel::FWObjModel(FWResManager* pkResMgr)
	{
		m_pkResManager		= pkResMgr;
		m_uiNumFaces		= 0;
		m_pkVertexFormat	= NULL;
		m_pkVertexBuffer	= NULL;

		Device* pkDevice	= m_pkResManager->GetApplication()->GetGraphics()->GetDevice();
		VertexElement akVertexDeclaration[] = 
		{
			CORE3D_VERTEXFORMAT_DECL(0, VET_VECTOR3, 0),
			CORE3D_VERTEXFORMAT_DECL(0, VET_VECTOR3, 1),
			CORE3D_VERTEXFORMAT_DECL(0, VET_VECTOR2, 2),
			CORE3D_VERTEXFORMAT_DECL(0, VET_VECTOR3, 3)
		};
		pkDevice->CreateVertexFormat(&m_pkVertexFormat, akVertexDeclaration, sizeof(akVertexDeclaration));
	}

	FWObjModel::~FWObjModel()
	{
		CORE3D_SAFE_RELEASE(m_pkVertexBuffer);
		CORE3D_SAFE_RELEASE(m_pkVertexFormat);
	}

	bool FWObjModel::LoadModel(const char* pData)
	{
		std::vector<Vector3> vecPositions;
		std::vector<Vector3> vecNormals;
		std::vector<Vector2> vecTexCoords;
		std::vector<FWObjModel::VertexData>	vecFinalVertices;

		const char* pCurrentPos = pData;
		while(*pCurrentPos)
		{
			switch(*pCurrentPos)
			{
			case 'v':
				{
					pCurrentPos++;
					switch(*pCurrentPos++)
					{
					case ' ': // POSITION
						{
							Vector3 kPosition;
							sscanf_s(pCurrentPos, "%f %f %f", &kPosition.x, &kPosition.y, &kPosition.z);
							kPosition.z = -kPosition.z;
							vecPositions.push_back(kPosition);
						}
						break;
					case 'n': // NORMAL
						{
							Vector3 kNormal;
							sscanf_s(pCurrentPos, "%f %f %f", &kNormal.x, &kNormal.y, &kNormal.z);
							kNormal.z = -kNormal.z;
							vecNormals.push_back(kNormal);
						}
						break;
					case 't': // TEXCOORD
						{
							Vector2 kTexCoord;
							sscanf_s(pCurrentPos, "%f %f", &kTexCoord.x, &kTexCoord.y);
							kTexCoord.y = 1 - kTexCoord.y;
							vecTexCoords.push_back(kTexCoord);
						}
						break;
					default: break;
					}
				}
				break;
			case 'f':
				{
					pCurrentPos++;
					UINT32 auiPosIndices[3], auiNormalIndices[3], auiTexIndices[3];
					sscanf_s(pCurrentPos, "%i/%i/%i %i/%i/%i %i/%i/%i", 
						&auiPosIndices[0], &auiTexIndices[0], &auiNormalIndices[0], 
						&auiPosIndices[1], &auiTexIndices[1], &auiNormalIndices[1], 
						&auiPosIndices[2], &auiTexIndices[2], &auiNormalIndices[2]);

					FWObjModel::VertexData akNewVertex[3];
					for(UINT32 uiVertex = 0; uiVertex < 3; ++uiVertex)
					{
						akNewVertex[uiVertex].kPosition		= vecPositions[auiPosIndices[uiVertex] - 1];
						akNewVertex[uiVertex].kNormal		= vecNormals[auiNormalIndices[uiVertex] - 1];
						akNewVertex[uiVertex].kTexCoord0	= vecTexCoords[auiTexIndices[uiVertex] - 1];
					}

					// COMMENT : Calculate triangle tangent vector
					const Vector3 akDelta[2] = 
					{
						akNewVertex[1].kPosition - akNewVertex[0].kPosition, 
						akNewVertex[2].kPosition = akNewVertex[0].kPosition
					};
					const FLOAT32 afDeltaV[2] = 
					{
						akNewVertex[1].kTexCoord0.y - akNewVertex[0].kTexCoord0.y, 
						akNewVertex[2].kTexCoord0.y - akNewVertex[0].kTexCoord0.y
					};
					const Vector3 kTangent = ((akDelta[0] * afDeltaV[1]) - (akDelta[1] * afDeltaV[0])).Normalize();
					akNewVertex[0].kTangent = akNewVertex[1].kTangent = akNewVertex[2].kTangent = kTangent;

					vecFinalVertices.push_back(akNewVertex[2]);
					vecFinalVertices.push_back(akNewVertex[1]);
					vecFinalVertices.push_back(akNewVertex[0]);
					++m_uiNumFaces;
				}
				break;
			default: break;
			}

			// COMMENT : Advance to next line
			while(0 != *pCurrentPos)
			{
				if('\n' == *pCurrentPos++) {break;}
			}
		}

		// COMMENT : Fill the vertex buffer
		Device* pkDevice = m_pkResManager->GetApplication()->GetGraphics()->GetDevice();
		if(CORE3D_FAILED(pkDevice->CreateVertexBuffer(&m_pkVertexBuffer, 
			sizeof(FWObjModel::VertexData) * (UINT32)vecFinalVertices.size())))
		{
			return false;
		}

		FWObjModel::VertexData* pkDest = NULL;
		if(CORE3D_FAILED(m_pkVertexBuffer->GetPointer(0, (void**)&pkDest)))
		{
			return false;
		}

		for(std::vector<FWObjModel::VertexData>::iterator iterVertex = vecFinalVertices.begin(); 
			iterVertex != vecFinalVertices.end(); ++iterVertex, ++pkDest)
		{
			memcpy(pkDest, (FWObjModel::VertexData*)&(*iterVertex), sizeof(FWObjModel::VertexData));
		}
		return true;
	}

	FWResManager* FWObjModel::GetResManager()
	{
		return m_pkResManager;
	}
}