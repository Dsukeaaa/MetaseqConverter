#include "MetPch.h"
#include "ConvertFBX.h"
#ifndef _DS_FORMAT_
#include "File/DsFormat.h"
#endif
#ifndef _OUTPUTRES_
#include "Res/OutputRes.h"
#endif
#include "Converter/ConvertFormat.h" 

#include "fbxsdk/scene/animation/fbxanimstack.h"
#include "fbxsdk/scene/constraint/fbxconstraint.h"

using namespace MetaseqConverter;
using namespace std;


static const std::string DMYPOLY_TOKEN = "dmypoly";
static const std::string MASTER_BONE_NAME = "Master";


ConvertFBX::ConvertFBX()
{
}


ConvertFBX::~ConvertFBX()
{
}

void ConvertFBX::Convert(const char* path, OutputRes& res)
{
	FbxManager* manager = FbxManager::Create();
	//if (NULL == manager)return;
	
	FbxImporter* importer = FbxImporter::Create(manager, "");
	//if (NULL == importer) return;
	
	FbxScene* scene = FbxScene::Create(manager, "");
	//if (NULL == scene) return;

	bool isImport = importer->Initialize(path);
	//if (!isImport) return;
	isImport = importer->Import(scene);
	//if (!isImport) return;

	importer->Destroy();

	_AccessNode(scene->GetRootNode(), res.dsAnimModel, res.dsAnimBone, res.lightData);
	_SetChildBone(scene->GetRootNode(), res.dsAnimBone, -1, NULL, res.dsCustomProperty);
	_AceessAnim(scene, res.dsAnimBone);
	_WeightNormalize(res.dsAnimModel, res.dsAnimBone);
	_FbxCoordTrans(res.dsAnimBone);
	_BorneScaleCalc(res.dsAnimBone);

	res.dataSize = res.dsAnimModel.dataSize + res.dsCustomProperty.GetDataSize();

	ConvertFormat(res);
	
	scene->Destroy();
	manager->Destroy();
}

void ConvertFBX::_AccessNode(FbxNode* pNode, DS_ANIM_MODEL& model, DS_ANIM_BONE& dsAnimBone, DS_LIGHT_DATA& lightData)
{
	const char* name = pNode->GetName();

	//FBXSDK_DLL::
	if (pNode)
	{
		//FbxMesh *pMesh = pNode->GetMesh();
		//if(pMesh) _AccessMesh(pMesh, model);
		
		FbxNodeAttribute* pAttribute = pNode->GetNodeAttribute();
		if (pAttribute)
		{
			FbxNodeAttribute::EType type = pAttribute->GetAttributeType();
			switch (type)
			{
			case FbxNodeAttribute::eMesh:
			{
				//FbxMesh *pMesh = static_cast<FbxMesh*>(pAttribute);
				FbxMesh *pMesh = pNode->GetMesh();
				DS_MAT offset; offset = pNode->EvaluateGlobalTransform().Double44(); 
				DS_MAT localOffset; localOffset = pNode->EvaluateLocalTransform().Double44();
				offset = DS_MAT::T(offset);	//列ベクトルっぽいので転置。

				//座標はFbxエクスポート時に変換されてる（オプション）
				_AccessMesh(pMesh, model, offset);
				_AccessBone(pMesh, dsAnimBone, model, offset);
			}
			break;

			case FbxNodeAttribute::eLight:
			{
				const FbxLight* pLight = pNode->GetLight();
				const FbxLight::EType lightType = pLight->LightType;
				if (FbxLight::ePoint == lightType){
					DS_LIGHT light;
					light.type = lightType;
					light.distance = static_cast<float>(pLight->DecayStart.Get());//ランプの距離の値
					light.energy = static_cast<float>(pLight->Intensity.Get());//ランプのエネルギー値の100倍
					light.color[0] = static_cast<float>(pLight->Color.Get()[0]);
					light.color[1] = static_cast<float>(pLight->Color.Get()[1]);
					light.color[2] = static_cast<float>(pLight->Color.Get()[2]);
					lightData.lights.push_back(light);
				}
				else{
					//ポイントライト以外は未対応
				}

			}
			break;


			default:
			{
			}
			break;

			}
		}

		const int childNum = pNode->GetChildCount();
		for (int i = 0; i < childNum; ++i)
		{
			_AccessNode(pNode->GetChild(i), model, dsAnimBone, lightData);
		}
	}
}

void ConvertFBX::_AccessMesh(FbxMesh *pMesh, DS_ANIM_MODEL& dsAnimModel, DS_MAT offset)
{
	//const int polygonVertexNum = pMesh->GetPolygonVertexCount();
	//const int *pIndexAry = pMesh->GetPolygonVertices();
	
	const int beforeVn = dsAnimModel.vn;
	const int beforeFn = dsAnimModel.fn;

	//リサイズ頂点
	{
		const vari_size vertexNum = static_cast<vari_size>(pMesh->GetControlPointsCount());
		const FbxVector4* src = pMesh->GetControlPoints();
		DS_VERTEX* pNewVertex = new DS_VERTEX[dsAnimModel.vn + vertexNum];
		for (vari_size i = 0; i < dsAnimModel.vn; ++i)
		{
			pNewVertex[i] = dsAnimModel.pVertex[i];
		}
		for (vari_size i = dsAnimModel.vn; i < (dsAnimModel.vn + vertexNum); ++i)
		{
			pNewVertex[i].x = static_cast<float>(src[i - dsAnimModel.vn][0]);
			pNewVertex[i].y = static_cast<float>(src[i - dsAnimModel.vn][1]);
			pNewVertex[i].z = static_cast<float>(src[i - dsAnimModel.vn][2]);
			pNewVertex[i].w = 1.0f;
			pNewVertex[i] = (offset*pNewVertex[i]);

			//追加分のデータサイズ
			dsAnimModel.dataSize += pNewVertex[i].GetSize();
			dsAnimModel.vertexSize += pNewVertex[i].GetSize();
		}
		delete[] dsAnimModel.pVertex;
		dsAnimModel.pVertex = pNewVertex;
		dsAnimModel.vn += vertexNum;
		DS_ANIM_MODEL::Mesh mesh;
		mesh.name = pMesh->GetName();
		mesh.vertexNum = vertexNum;
		dsAnimModel.meshInfo.push_back(mesh);
	}


	//ダミポリはオブジェクト名で判定
	const std::string nodeName = pMesh->GetNode()->GetName();
	std::string::size_type dmyPolyTokenPos = nodeName.find(DMYPOLY_TOKEN);
	if (std::string::npos != dmyPolyTokenPos){
		
		const vari_size vertexNum = static_cast<vari_size>(pMesh->GetControlPointsCount());
		const FbxVector4* src = pMesh->GetControlPoints();

		const vari_size polygonNum = static_cast<vari_size>(pMesh->GetPolygonCount());
		for (vari_size fi = 0; fi < polygonNum; ++fi)
		{
			std::vector<vari_size> tmpV;
			const vari_size vn = pMesh->GetPolygonSize(fi);
			assert(vn == 3);//ダミポリの頂点は3つ固定
			for (vari_size vi = 0; vi < vn; ++vi)
			{
				const vari_size vIdx = pMesh->GetPolygonVertex(fi, vi);
				assert(vIdx < vertexNum);
				tmpV.push_back( vIdx );
			}

			//一番直角に近い１組の辺を求める
			double minDot = DBL_MAX;
			vari_size baseIdx = -1;
			vari_size dIdxY = -1;
			vari_size dIdxZ = -1;
			for (vari_size vi0 = 0; vi0 < vn; ++vi0)
			{
				const vari_size vi1 = (vi0 + 1) % vn;
				const vari_size vi2 = (vi0 + 2) % vn;
				const FbxVector4 edge0 = src[vi1] - src[vi0];
				const FbxVector4 edge1 = src[vi2] - src[vi1];
				const double dot = fabs(edge0.DotProduct(edge1));
				if (dot < minDot){
					//dotが一番小さい=ここを直角を成す辺と見なす
					minDot = dot;
					baseIdx = vi1;

					//長いほうがZ方向。短い方がY方向
					const double len1 = edge0.Length();
					const double len2 = edge1.Length();
					if (len1 < len2){
						dIdxY = vi0;
						dIdxZ = vi2;
					}
					else{
						dIdxY = vi2;
						dIdxZ = vi0;
					}
				}
			}
			//ダミポリの座標と方向を示す頂点が分かったので保存
			DS_DMYPOLY dmyPoly;
			dmyPoly.vIndex[0] = baseIdx + beforeVn;//ダミポリの座標の頂点
			dmyPoly.vIndex[1] = dIdxY + beforeVn;//ダミポリのY方向の頂点
			dmyPoly.vIndex[2] = dIdxZ + beforeVn;//ダミポリのZ方向の頂点
			const std::string token = DMYPOLY_TOKEN;
			dmyPoly.id = std::atoi(nodeName.c_str() + token.length());
			dsAnimModel.dmyPoly.push_back(dmyPoly);
		}


	}else{
		//ダミポリオブジェクトは面とマテリアルは生成しない。別途ダミポリとして作る。頂点はスキンメッシュするので通常の頂点に含める

		//リサイズ面
		{
			const vari_size polygonNum = static_cast<vari_size>(pMesh->GetPolygonCount());
			DS_FACE* pNewFace = new DS_FACE[dsAnimModel.fn + polygonNum];
			for (vari_size i = 0; i < dsAnimModel.fn; ++i)
			{
				pNewFace[i].vn = dsAnimModel.pFace[i].vn;
				pNewFace[i].normal = dsAnimModel.pFace[i].normal;
				pNewFace[i].pIndex = new vari_size[dsAnimModel.pFace[i].vn];
				for (vari_size idx = 0; idx < dsAnimModel.pFace[i].vn; ++idx)
				{
					pNewFace[i].pIndex[idx] = dsAnimModel.pFace[i].pIndex[idx];
				}
			}

			for (vari_size i = dsAnimModel.fn; i < dsAnimModel.fn + polygonNum; ++i)
			{
				pNewFace[i].vn = pMesh->GetPolygonSize(static_cast<int>(i - dsAnimModel.fn));
				pNewFace[i].normal = DS_VERTEX();
				pNewFace[i].pIndex = new vari_size[pNewFace[i].vn];
				for (vari_size idx = 0; idx < pNewFace[i].vn; ++idx)
				{
					pNewFace[i].pIndex[idx] = pMesh->GetPolygonVertex(static_cast<int>(i - dsAnimModel.fn), static_cast<int>(idx));
					pNewFace[i].pIndex[idx] += beforeVn;//fbxではメッシュごとにindex独立してても、独自形式では独立してないので、前回のメッシュの頂点数分オフセットする。
				}
				//追加分のデータサイズ
				dsAnimModel.dataSize += pNewFace[i].GetSize();
				dsAnimModel.faceSize += pNewFace[i].GetSize();
			}

			delete[] dsAnimModel.pFace;
			dsAnimModel.pFace = pNewFace;
			dsAnimModel.fn += polygonNum;
		}

		DS_MATERIAL mtr;
		int dsTextureIdx = -1;
		//テクスチャ
		{
			FbxNode* pNode = pMesh->GetNode();
			const int mtrCount = pNode->GetMaterialCount();
			const int mtrNum = min(mtrCount, 1);
			for (int mi = 0; mi < mtrNum; ++mi)
			{
				FbxSurfaceMaterial* pMtr = pNode->GetMaterial(mi);
				mtr.name = pMtr->GetName();

				//既に登録済みがないか検知
				bool isFind = false;
				//それぞれのオブジェクトにコピーで持つ。そのため重複はOK。
				//for (DS_MATERIAL tmpMtr : dsAnimModel.mtr)
				//{
				//	if (tmpMtr.name == mtr.name)
				//	{
				//		isFind = true;
				//		break;
				//	}
				//}

				if (!isFind)
				{
					//材質
					FbxDouble3 ambient = pMtr->FindProperty(FbxSurfaceMaterial::sAmbient).Get<FbxDouble3>();
					FbxDouble ambient2 = pMtr->FindProperty(FbxSurfaceMaterial::sAmbientFactor).Get<FbxDouble>();
					FbxDouble3 diffuse = pMtr->FindProperty(FbxSurfaceMaterial::sDiffuse).Get<FbxDouble3>();
					FbxDouble diffuse2 = pMtr->FindProperty(FbxSurfaceMaterial::sDiffuseFactor).Get<FbxDouble>();
					FbxDouble3 emissive = pMtr->FindProperty(FbxSurfaceMaterial::sEmissive).Get<FbxDouble3>();
					FbxDouble emissive2 = pMtr->FindProperty(FbxSurfaceMaterial::sEmissiveFactor).Get<FbxDouble>();
					FbxDouble3 specular = pMtr->FindProperty(FbxSurfaceMaterial::sSpecular).Get<FbxDouble3>();
					FbxDouble specular2 = pMtr->FindProperty(FbxSurfaceMaterial::sSpecularFactor).Get<FbxDouble>();
					FbxDouble shininess = pMtr->FindProperty(FbxSurfaceMaterial::sShininess).Get<FbxDouble>();
					mtr.ambient[0] = ambient[0] * ambient2; mtr.ambient[1] = ambient[1] * ambient2; mtr.ambient[2] = ambient[2] * ambient2;
					mtr.diffuse[0] = diffuse[0] * diffuse2; mtr.diffuse[1] = diffuse[1] * diffuse2; mtr.diffuse[2] = diffuse[2] * diffuse2;
					mtr.emissive[0] = emissive[0] * emissive2; mtr.emissive[1] = emissive[1] * emissive2; mtr.emissive[2] = emissive[2] * emissive2;
					mtr.specular[0] = specular[0] * specular2; mtr.specular[1] = specular[1] * specular2; mtr.specular[2] = specular[2] * specular2;
					mtr.shininess = shininess;

					{//カスタムプロパティ マテリアル
						FbxPropertyHandle& handle = pMtr->GetPropertyHandle().Find(CUSTOM_PROP_NAME_MATERIAL, false);
						string customPropertyName = handle.GetName();
						if (!customPropertyName.empty()){
							double data = 0;
							if (handle.Get(&data, EFbxType::eFbxDouble)){
								mtr.materialParamId = static_cast<int>(data);
							}
						}
					}


					//ディフューズマテリアルのテクスチャを取る
					const FbxProperty property = pMtr->FindProperty(FbxSurfaceMaterial::sDiffuse);

					// 通常テクスチャの枚数をチェック
					const int numGeneralTexture = property.GetSrcObjectCount();
					// 各テクスチャについてテクスチャ情報をゲット
					for (int ti = 0; ti < numGeneralTexture; ++ti) {
						// i番目のテクスチャオブジェクト取得
						const FbxFileTexture* texture = FbxCast<FbxFileTexture>(property.GetSrcObject(ti));

						// テクスチャファイルパスを取得（フルパス）
						const char* fileName = texture->GetFileName();


						DS_TEXTURE dsTexture;
						dsTexture.albedoTexPath = fileName;

						//UV
						FbxStringList	uvsetName;
						pMesh->GetUVSetNames(uvsetName);
						int numUVSet = uvsetName.GetCount();

						FbxString name = uvsetName.GetStringAt(mi);//numUVSetとマテリアルの数は一緒、と予想・・・

						FbxVector2 texCoord;
						bool unmapped = false;
						const int polygonNum = static_cast<vari_size>(pMesh->GetPolygonCount());
						for (int pi = 0; pi < polygonNum; ++pi)
						{
							DS_TEXTURE::UV_FACE uvFace;

							uvFace.refGeomFaceIndex = beforeFn + pi; //↑のポリゴンの面のインデックス。１つの面に複数のテクスチャがあるので重複して参照してOK
							const int polySize = pMesh->GetPolygonSize(pi);
							for (int pvi = 0; pvi < polySize; ++pvi)
							{
								pMesh->GetPolygonVertexUV(pi, pvi, name, texCoord, unmapped);
								DS_TEXTURE::UV uv;
								uv.uv[0] = static_cast<float>(texCoord[0]);
								uv.uv[1] = static_cast<float>(texCoord[1]);
								uv.vertexIndex = pMesh->GetPolygonVertex(pi, pvi) + beforeVn;

								uvFace.index.push_back(static_cast<int>(dsTexture.uv.size()));

								//テクスチャは１メッシュごと。さらにメッシュに複数あればその分も次々追加
								dsTexture.uv.push_back(uv);
							}
							dsTexture.uvFace.push_back(uvFace);
						}

						if (ti == 0){
							{
								const FbxProperty normalProperty = pMtr->FindProperty(FbxSurfaceMaterial::sNormalMap);
								const int normalNum = normalProperty.GetSrcObjectCount();
								if (0 < normalNum){
									if (1 < numGeneralTexture){
										printf("1つのマテリアルに複数のテクスチャがあります。法線マップは最初の１つに対応するものしか生成されません");
									}
									if (1 < normalNum){
										printf("1つのマテリアルに複数の法線マップがあります。最初の１つしか生成されません");
									}
									const FbxFileTexture* normalTexture = FbxCast<FbxFileTexture>(normalProperty.GetSrcObject(0));
									dsTexture.normalTexPath = normalTexture->GetFileName();
								}
							}

							{
								const FbxProperty specularProperty = pMtr->FindProperty(FbxSurfaceMaterial::sSpecular);
								const int specularNum = specularProperty.GetSrcObjectCount();
								if (0 < specularNum){
									if (1 < numGeneralTexture){
										printf("1つのマテリアルに複数のテクスチャがあります。スペキュラマップは最初の１つに対応するものしか生成されません");
									}
									if (1 < specularNum){
										printf("1つのマテリアルに複数のスペキュラマップがあります。最初の１つしか生成されません");
									}
									const FbxFileTexture* specularTexture = FbxCast<FbxFileTexture>(specularProperty.GetSrcObject(0));
									dsTexture.specularTexPath = specularTexture->GetFileName();
								}
							}
						}

						mtr.texture.push_back(dsTexture);
						dsTextureIdx = static_cast<int>(mtr.texture.size()) - 1;
					}


					if (1 < mi)
					{
						printf("マテリアルの数が１つのメッシュに複数使われています。未対応です\n %s", pMtr->GetName());
					}

					dsAnimModel.mtr.push_back(mtr);
				}
			}
		}
		
	}
}

void ConvertFBX::_SetChildBone(FbxNode *pNode, DS_ANIM_BONE& dsAnimBone, int parentIdx, DS_BONE* pParentBone, DS_CUSTOM_PROPERTY& ragdollParam)
{
	if (pNode)
	{
		int curIdx = -1;
		const FbxNodeAttribute* pAttribute = pNode->GetNodeAttribute();
		if (pAttribute)
		{
			const FbxNodeAttribute::EType type = pAttribute->GetAttributeType();
			if (type == FbxNodeAttribute::eSkeleton)
			{
				//const FbxSkeleton* pSkeleton = static_cast<const FbxSkeleton*>(pAttribute);
				//double boneLen = pSkeleton->LimbLength.Get();//1.0で子→親への長さそのままらしい
				//double boneDist = pSkeleton->Size;//親と子までの長さだと思ったが何か違う

				const string nodeName = pNode->GetName();
				const int bn = dsAnimBone.bn;
				for (int bIdx = 0; bIdx < bn; ++bIdx)
				{
					const string boneName = dsAnimBone.pBone[bIdx].name;
					if (boneName == nodeName)
					{
						curIdx = bIdx;
							
						//親インデックスを保存
						dsAnimBone.pBone[bIdx].parentIdx = parentIdx;

						if (pParentBone )
						{
							//子ボーンを追加
							const int newChildNum = pParentBone->childNum + 1;
							int* pNewChildIndex = new int[newChildNum];
							for (int cbIdx = 0; cbIdx < pParentBone->childNum; ++cbIdx)
							{
								pNewChildIndex[cbIdx] = pParentBone->pChildIdx[cbIdx];
							}
							pNewChildIndex[pParentBone->childNum] = curIdx;//親に自分のインデックス追加

							pParentBone->childNum = newChildNum;
							delete[] pParentBone->pChildIdx;
							pParentBone->pChildIdx = pNewChildIndex;
						}

						{//カスタムプロパティ ラグドール
							FbxPropertyHandle& handle = pNode->GetPropertyHandle().Find(CUSTOM_PROP_NAME_RAGDOLL, false);
							string customPropertyName = handle.GetName();
							if (!customPropertyName.empty()){
								double data = 0;
								if (handle.Get(&data, EFbxType::eFbxDouble)){
									DS_RAGDOLL_PARAM_ID param;
									param.id = static_cast<int>(data);
									param.boneIdx = curIdx;
									ragdollParam.ragdoll.push_back(param);
								}
							}
						}

						break;
					}
				}
			}
		}
		const int childNum = pNode->GetChildCount();
		for (int i = 0; i < childNum; ++i)
		{
			if (-1 < curIdx)
			{
				_SetChildBone(pNode->GetChild(i), dsAnimBone, curIdx, &dsAnimBone.pBone[curIdx], ragdollParam);
			}
			else
			{
				//スケルトンノードではなかったので、引数をそのまま渡す。
				_SetChildBone(pNode->GetChild(i), dsAnimBone, parentIdx, pParentBone, ragdollParam);
			}
		}
	}
}

namespace
{
	int _GetVertexIndexOffset(const DS_ANIM_MODEL& dsAnimModel, const char* pMeshName)
	{
		int ret = 0;
		for each(const DS_ANIM_MODEL::Mesh& mesh in dsAnimModel.meshInfo)
		{
			if (mesh.name == pMeshName)
			{
				break;
			}
			ret += mesh.vertexNum;
		}
		return ret;
	}
}

void ConvertFBX::_AccessBone(FbxMesh *pMesh, DS_ANIM_BONE& dsAnimBone, DS_ANIM_MODEL& dsAnimModel, DS_MAT offset)
{
	static vector<string> s_createList;

	const char* name = pMesh->GetName();

	// スキンの数を取得
	int  skinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int sn = 0; sn < skinCount; ++sn) {


		// i番目のスキンを取得
		FbxSkin* pSkin = static_cast<FbxSkin *>(pMesh->GetDeformer(sn, FbxDeformer::eSkin));

		// クラスターの数を取得
		int clusterNum = pSkin->GetClusterCount();

		if (0 < clusterNum)
		{
			for (int idx = 0; idx < clusterNum; ++idx)
			{
				const FbxCluster* pCluster = pSkin->GetCluster(idx);
				std::string tmp1 = pCluster->GetName();
					
				bool isNewBone = true;
				int updateIdx = -1;
				for (int bi = 0; bi < dsAnimBone.bn; ++bi)
				{
					std::string tmp2 = dsAnimBone.pBone[bi].name;
					if (tmp1 == tmp2)
					{
						updateIdx = bi;
						isNewBone = false;
						break;
					}
				}
				if (isNewBone)
				{
					DS_BONE* pNewBone = new DS_BONE;
					const char* name = pCluster->GetName();
					const char* meshName = pMesh->GetName();
					const int	pointNum = pCluster->GetControlPointIndicesCount();
					pNewBone->indexNum = pointNum;
					const int*	pPointAry = pCluster->GetControlPointIndices();
					const double* pWeightAry = pCluster->GetControlPointWeights();
					pNewBone->pIndex = new vari_size[pointNum];
					pNewBone->pWeight = new float[pointNum];
					const int viOffset = _GetVertexIndexOffset(dsAnimModel, meshName);
					for (int pn = 0; pn < pointNum; ++pn)
					{
						//ボーンインデックスにメッシュ分オフセットしないとダメ
						pNewBone->pIndex[pn] = static_cast<vari_size>(pPointAry[pn]) + viOffset;
						pNewBone->pWeight[pn] = static_cast<float>(pWeightAry[pn]);
					}

					FbxAMatrix initMat;
					//pCluster->GetTransformMatrix(initMat);
					pCluster->GetTransformLinkMatrix(initMat);//親子関係 グローバルな気がする
					pNewBone->initMat = initMat.Double44();
					pNewBone->initMat = DS_MAT::T(pNewBone->initMat);

					//名前
					string nameStr(pCluster->GetName());
					pNewBone->nameSize = nameStr.size() + 1;
					pNewBone->name = new char[pNewBone->nameSize];
					memcpy(pNewBone->name, pCluster->GetName(), pNewBone->nameSize);
					
					pNewBone->isMaster = (nameStr == MASTER_BONE_NAME) ? (1) : (0);

					//リサイズボーン
					DS_BONE* pTmpBone = new DS_BONE[dsAnimBone.bn + 1];
					for (int bi = 0; bi < dsAnimBone.bn; ++bi)
					{
						pTmpBone[bi] = dsAnimBone.pBone[bi];
					}
					pTmpBone[dsAnimBone.bn] = (*pNewBone);

					delete[] dsAnimBone.pBone;
					dsAnimBone.pBone = pTmpBone;
					dsAnimBone.bn += 1;
				}
				else if (updateIdx > -1)
				{
					const char* meshName = pMesh->GetName();
					const int viOffset = _GetVertexIndexOffset(dsAnimModel, meshName);

					//この中から含まれてない頂点Indexを探して追加する。重複はしないはず。 
					const int	pointNum = pCluster->GetControlPointIndicesCount();
					const int*	pPointAry = pCluster->GetControlPointIndices();
					const double* pWeightAry = pCluster->GetControlPointWeights();

					std::vector<int> addIndex;
					for (int i = 0; i < pointNum; ++i)
					{
						const int check = pPointAry[i] + viOffset;
						bool isAdd = true;
						for (int ii = 0; ii < dsAnimBone.pBone[updateIdx].indexNum; ++ii)
						{
							if (check == dsAnimBone.pBone[updateIdx].pIndex[ii])
							{
								//既にこの頂点は存在する
								isAdd = false;
								assert(0 && "ありえない");
								break;
							}
						}

						if (isAdd)
						{
							addIndex.push_back(i);
						}
					}
					if(!addIndex.empty())
					{ 
						const int newNum = dsAnimBone.pBone[updateIdx].indexNum + addIndex.size();
						vari_size* pNewIdx = new vari_size[newNum];
						float* pNewWei = new float[newNum];
						for (int i = 0; i < dsAnimBone.pBone[updateIdx].indexNum; ++i)
						{
							pNewIdx[i] = dsAnimBone.pBone[updateIdx].pIndex[i];
							pNewWei[i] = dsAnimBone.pBone[updateIdx].pWeight[i];
						}
						for (int i = 0; i < static_cast<int>(addIndex.size()); ++i)
						{
							const int idx = addIndex[i];
							pNewIdx[i + dsAnimBone.pBone[updateIdx].indexNum] = pPointAry[idx] + viOffset;
							pNewWei[i + dsAnimBone.pBone[updateIdx].indexNum] = static_cast<float>(pWeightAry[idx]);
						}
						delete[] dsAnimBone.pBone[updateIdx].pIndex;
						delete[] dsAnimBone.pBone[updateIdx].pWeight;
						dsAnimBone.pBone[updateIdx].pIndex = pNewIdx;
						dsAnimBone.pBone[updateIdx].pWeight = pNewWei;
						dsAnimBone.pBone[updateIdx].indexNum = newNum;
					}
					 
				}
				else
				{
					assert(0&&"ありえない");
				}
			}
		}
	}
}

void ConvertFBX::_AceessAnim(FbxScene* pScene, DS_ANIM_BONE& dsAnimBone)
{
	vector<DS_ANIM*> anims;

	//FBXSDK_DLL::
	const int nbAnimStacks = pScene->GetSrcObjectCount<FbxAnimStack>();
	for (int stackIdx = 0; stackIdx < nbAnimStacks; ++stackIdx) 
	{
		FbxAnimStack* pAnimStack = pScene->GetSrcObject<FbxAnimStack>(stackIdx);
	
		const int nbAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
		for (int layerIdx = 0; layerIdx < nbAnimLayers; ++layerIdx)
		{
			FbxAnimLayer* plAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(layerIdx);
			const bool isBoneAnimLayer = _IsBoneAnimLayer(plAnimLayer, pScene->GetRootNode());
			if (isBoneAnimLayer)
			{
				DS_ANIM* anim = new DS_ANIM;
				anim->Init(dsAnimBone.bn);

				const string nameStr( plAnimLayer->GetName());

				anim->animNameSize = nameStr.size() + 1;
 				anim->animName = new char[anim->animNameSize];
				memcpy(anim->animName, nameStr.data(), anim->animNameSize);

				_AceessAnimLayer(plAnimLayer, pScene->GetRootNode(), *anim, dsAnimBone);

				anims.push_back(anim);
			}
		}
	}

	dsAnimBone.an = static_cast<long>(anims.size());
	dsAnimBone.pAnim = new DS_ANIM[anims.size()];
	
	for (long aIdx = 0; aIdx < dsAnimBone.an; ++aIdx)
	{
		dsAnimBone.pAnim[aIdx] = (*anims[aIdx]);
	}
	//DS_ANIM*an分リークしてるけど気にしない
}

bool ConvertFBX::_IsBoneAnimLayer(FbxAnimLayer* pAnimLayer, FbxNode* pNode) const
{
	const FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();
	if (pAttr)
	{
		if (FbxNodeAttribute::eSkeleton == pAttr->GetAttributeType())
		{
			FbxAnimCurve* pCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
			if (pCurve)
			{
				return true;
			}
		}
	}

	const int childNum = pNode->GetChildCount();
	for (int i = 0; i < childNum; ++i)
	{
		if (_IsBoneAnimLayer(pAnimLayer, pNode->GetChild(i)))
		{
			return true;
		}
	}
	return false;
}

void ConvertFBX::_AceessAnimLayer(FbxAnimLayer* pAnimLayer, FbxNode* pNode, DS_ANIM& dsAnim, DS_ANIM_BONE& dsAnimBone)
{
	const FbxNodeAttribute* pAttr = pNode->GetNodeAttribute();
	if (pAttr)
	{
		if (FbxNodeAttribute::eSkeleton == pAttr->GetAttributeType())
		{
			//位置
			{
				FbxAnimCurve* pCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
				if (pCurve)
				{
					_AceessAnimCurve(pCurve, pNode, dsAnim, dsAnimBone, POS_X);
				}
			}
			{
				FbxAnimCurve* pCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
				if (pCurve)
				{
					_AceessAnimCurve(pCurve, pNode, dsAnim, dsAnimBone, POS_Y);
				}
			}
			{
				FbxAnimCurve* pCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
				if (pCurve)
				{
					_AceessAnimCurve(pCurve, pNode, dsAnim, dsAnimBone, POS_Z);
				}
			}

			//回転
			{
				FbxAnimCurve* pCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
				if (pCurve)
				{
					_AceessAnimCurve(pCurve, pNode, dsAnim, dsAnimBone, ROT_X);
				}
			}
			{
				FbxAnimCurve* pCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
				if (pCurve)
				{
					_AceessAnimCurve(pCurve, pNode, dsAnim, dsAnimBone, ROT_Y);
				}
			}
			{
				FbxAnimCurve* pCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
				if (pCurve)
				{
					_AceessAnimCurve(pCurve, pNode, dsAnim, dsAnimBone, ROT_Z);
				}
			}

			//スケール
			{
				FbxAnimCurve* pCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
				if (pCurve)
				{
					_AceessAnimCurve(pCurve, pNode, dsAnim, dsAnimBone, SCL_X);
				}
			}
			{
				FbxAnimCurve* pCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
				if (pCurve)
				{
					_AceessAnimCurve(pCurve, pNode, dsAnim, dsAnimBone, SCL_Y);
				}
			}
			{
				FbxAnimCurve* pCurve = pNode->LclScaling.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
				if (pCurve)
				{
					_AceessAnimCurve(pCurve, pNode, dsAnim, dsAnimBone, SCL_Z);
				}
			}
		}
	}

	const int childNum =  pNode->GetChildCount();
	for (int i = 0; i < childNum; ++i)
	{
		_AceessAnimLayer(pAnimLayer, pNode->GetChild(i), dsAnim, dsAnimBone);
	}
}

void ConvertFBX::_AceessAnimCurve(FbxAnimCurve* pCurve, FbxNode* pNode, DS_ANIM& dsAnim, DS_ANIM_BONE& dsAnimBone, CurvePoseType type)
{
	const int lKeyCount = pCurve->KeyGetCount();
	
	//名前からインデックスに変換
	int boneIndex = -1;
	const string nodeName(pNode->GetName());
	for (int idx = 0; idx < dsAnimBone.bn; ++idx)
	{
		const string dsName(dsAnimBone.pBone[idx].name);
		if (dsName == nodeName)
		{
			boneIndex = idx;
			break;
		}
	}

	const bool isMaster = (nodeName == MASTER_BONE_NAME);


	if (0 <= boneIndex)
	{
		dsAnim.pose[boneIndex].isMaster = isMaster;

		for (int lCount = 0; lCount < lKeyCount; ++lCount)
		{
			const float lKeyValue = static_cast<float>(pCurve->KeyGetValue(lCount));
			const FbxTime lKeyTime = pCurve->KeyGetTime(lCount);
			
			switch (type)
			{
			case MetaseqConverter::ConvertFBX::POS_X:
				{
					if (dsAnim.pose[boneIndex].keyFrameTX == NULL)
					{
						dsAnim.pose[boneIndex].keyFrameTX = new DS_KEY_FRAME[lKeyCount];
						dsAnim.pose[boneIndex].keyFrameNumTX = lKeyCount;
					}
					const float scale = DS_VERTEX::Set(dsAnimBone.pBone[boneIndex].initMat.m[0]).Length3();

					dsAnim.pose[boneIndex].keyFrameTX[lCount].value = lKeyValue*scale;
					dsAnim.pose[boneIndex].keyFrameTX[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());

					//マスター移動量
					if (isMaster)
					{
						if (dsAnim.masterMove.keyFrameTX == NULL)
						{
							dsAnim.masterMove.keyFrameTX = new DS_KEY_FRAME[lKeyCount];
							dsAnim.masterMove.keyFrameNumTX = lKeyCount;
						}
						if (lCount == 0)
						{
							dsAnim.masterMove.keyFrameTX[lCount].value = 0;
						}
						else
						{
							dsAnim.masterMove.keyFrameTX[lCount].value = dsAnim.pose[boneIndex].keyFrameTX[lCount].value - dsAnim.pose[boneIndex].keyFrameTX[lCount - 1].value;
						}
						dsAnim.masterMove.keyFrameTX[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());
					}
				}
				break;
			case MetaseqConverter::ConvertFBX::POS_Y:
				{
					if (dsAnim.pose[boneIndex].keyFrameTY == NULL)
					{
						dsAnim.pose[boneIndex].keyFrameTY = new DS_KEY_FRAME[lKeyCount];
						dsAnim.pose[boneIndex].keyFrameNumTY = lKeyCount;
					}
					const float scale = DS_VERTEX::Set(dsAnimBone.pBone[boneIndex].initMat.m[1]).Length3();

					dsAnim.pose[boneIndex].keyFrameTY[lCount].value = lKeyValue*scale;
					dsAnim.pose[boneIndex].keyFrameTY[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());

					//マスター移動量
					if (isMaster)
					{
						if (dsAnim.masterMove.keyFrameTY == NULL)
						{
							dsAnim.masterMove.keyFrameTY = new DS_KEY_FRAME[lKeyCount];
							dsAnim.masterMove.keyFrameNumTY = lKeyCount;
						}
						if (lCount == 0)
						{
							dsAnim.masterMove.keyFrameTY[lCount].value = 0;
						}
						else
						{
							dsAnim.masterMove.keyFrameTY[lCount].value = dsAnim.pose[boneIndex].keyFrameTY[lCount].value - dsAnim.pose[boneIndex].keyFrameTY[lCount - 1].value;
						}
						dsAnim.masterMove.keyFrameTY[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());
					}
				}
				break;
			case MetaseqConverter::ConvertFBX::POS_Z:
				{
					if (dsAnim.pose[boneIndex].keyFrameTZ == NULL)
					{
						dsAnim.pose[boneIndex].keyFrameTZ = new DS_KEY_FRAME[lKeyCount];
						dsAnim.pose[boneIndex].keyFrameNumTZ = lKeyCount;
					}
					const float scale = DS_VERTEX::Set(dsAnimBone.pBone[boneIndex].initMat.m[2]).Length3();

					dsAnim.pose[boneIndex].keyFrameTZ[lCount].value = lKeyValue*scale;
					dsAnim.pose[boneIndex].keyFrameTZ[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());

					//マスター移動量
					if (isMaster)
					{
						if (dsAnim.masterMove.keyFrameTZ == NULL)
						{
							dsAnim.masterMove.keyFrameTZ = new DS_KEY_FRAME[lKeyCount];
							dsAnim.masterMove.keyFrameNumTZ = lKeyCount;
						}
						if (lCount == 0)
						{
							dsAnim.masterMove.keyFrameTZ[lCount].value = 0;
						}
						else
						{
							dsAnim.masterMove.keyFrameTZ[lCount].value = dsAnim.pose[boneIndex].keyFrameTZ[lCount].value - dsAnim.pose[boneIndex].keyFrameTZ[lCount - 1].value;
						}
						dsAnim.masterMove.keyFrameTZ[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());
					}
				}
				break;

			//オイラー角らしい
			case MetaseqConverter::ConvertFBX::ROT_X:
				{
					if (dsAnim.pose[boneIndex].keyFrameRX == NULL)
					{
						dsAnim.pose[boneIndex].keyFrameRX = new DS_KEY_FRAME[lKeyCount];
						dsAnim.pose[boneIndex].keyFrameNumRX = lKeyCount;
					}
					dsAnim.pose[boneIndex].keyFrameRX[lCount].value = lKeyValue;
					dsAnim.pose[boneIndex].keyFrameRX[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());

					//マスター移動量
					if (isMaster)
					{
						if (dsAnim.masterMove.keyFrameRX == NULL)
						{
							dsAnim.masterMove.keyFrameRX = new DS_KEY_FRAME[lKeyCount];
							dsAnim.masterMove.keyFrameNumRX = lKeyCount;
						}
						if (lCount == 0)
						{
							dsAnim.masterMove.keyFrameRX[lCount].value = 0;
						}
						else
						{
							dsAnim.masterMove.keyFrameRX[lCount].value = dsAnim.pose[boneIndex].keyFrameRX[lCount].value - dsAnim.pose[boneIndex].keyFrameRX[lCount - 1].value;
						}
						dsAnim.masterMove.keyFrameRX[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());
					}
				}
				break;
			case MetaseqConverter::ConvertFBX::ROT_Y:
				{
					if (dsAnim.pose[boneIndex].keyFrameRY == NULL)
					{
						dsAnim.pose[boneIndex].keyFrameRY = new DS_KEY_FRAME[lKeyCount];
						dsAnim.pose[boneIndex].keyFrameNumRY = lKeyCount;
					}
					dsAnim.pose[boneIndex].keyFrameRY[lCount].value = lKeyValue;
					dsAnim.pose[boneIndex].keyFrameRY[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());

					//マスター移動量
					if (isMaster)
					{
						if (dsAnim.masterMove.keyFrameRY == NULL)
						{
							dsAnim.masterMove.keyFrameRY = new DS_KEY_FRAME[lKeyCount];
							dsAnim.masterMove.keyFrameNumRY = lKeyCount;
						}
						if (lCount == 0)
						{
							dsAnim.masterMove.keyFrameRY[lCount].value = 0;
						}
						else
						{
							dsAnim.masterMove.keyFrameRY[lCount].value = dsAnim.pose[boneIndex].keyFrameRY[lCount].value - dsAnim.pose[boneIndex].keyFrameRY[lCount - 1].value;
						}
						dsAnim.masterMove.keyFrameRY[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());
					}
				}
				break;
			case MetaseqConverter::ConvertFBX::ROT_Z:
				{
					if (dsAnim.pose[boneIndex].keyFrameRZ == NULL)
					{
						dsAnim.pose[boneIndex].keyFrameRZ = new DS_KEY_FRAME[lKeyCount];
						dsAnim.pose[boneIndex].keyFrameNumRZ = lKeyCount;
					}
					dsAnim.pose[boneIndex].keyFrameRZ[lCount].value = lKeyValue;
					dsAnim.pose[boneIndex].keyFrameRZ[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());

					//マスター移動量
					if (isMaster)
					{
						if (dsAnim.masterMove.keyFrameRZ == NULL)
						{
							dsAnim.masterMove.keyFrameRZ = new DS_KEY_FRAME[lKeyCount];
							dsAnim.masterMove.keyFrameNumRZ = lKeyCount;
						}
						if (lCount == 0)
						{
							dsAnim.masterMove.keyFrameRZ[lCount].value = 0;
						}
						else
						{
							dsAnim.masterMove.keyFrameRZ[lCount].value = dsAnim.pose[boneIndex].keyFrameRZ[lCount].value - dsAnim.pose[boneIndex].keyFrameRZ[lCount - 1].value;
						}
						dsAnim.masterMove.keyFrameRZ[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());
					}
				}
				break;

			case MetaseqConverter::ConvertFBX::SCL_X:
				{
					if (dsAnim.pose[boneIndex].keyFrameSX == NULL)
					{
						dsAnim.pose[boneIndex].keyFrameSX = new DS_KEY_FRAME[lKeyCount];
						dsAnim.pose[boneIndex].keyFrameNumSX = lKeyCount;
					}
					dsAnim.pose[boneIndex].keyFrameSX[lCount].value = lKeyValue;
					dsAnim.pose[boneIndex].keyFrameSX[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());
				}
				break;
			case MetaseqConverter::ConvertFBX::SCL_Y:
				{
					if (dsAnim.pose[boneIndex].keyFrameSY == NULL)
					{
						dsAnim.pose[boneIndex].keyFrameSY = new DS_KEY_FRAME[lKeyCount];
						dsAnim.pose[boneIndex].keyFrameNumSY = lKeyCount;
					}
					dsAnim.pose[boneIndex].keyFrameSY[lCount].value = lKeyValue;
					dsAnim.pose[boneIndex].keyFrameSY[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());
				}
				break;
			case MetaseqConverter::ConvertFBX::SCL_Z:
				{
					if (dsAnim.pose[boneIndex].keyFrameSZ == NULL)
					{
						dsAnim.pose[boneIndex].keyFrameSZ = new DS_KEY_FRAME[lKeyCount];
						dsAnim.pose[boneIndex].keyFrameNumSZ = lKeyCount;
					}
					dsAnim.pose[boneIndex].keyFrameSZ[lCount].value = lKeyValue;
					dsAnim.pose[boneIndex].keyFrameSZ[lCount].localTimeMs = static_cast<float>(lKeyTime.GetMilliSeconds());
				}
				break;
			default:
				break;
			}
		}
	}
}

void ConvertFBX::_WeightNormalize(const DS_ANIM_MODEL& dsAnimModel, DS_ANIM_BONE& dsAnimBone)
{
	const int vn = dsAnimModel.vn;
	for (int vi = 0; vi < vn; ++vi)
	{
		//この頂点に影響する重み達
		std::vector<float*> weights;

		const int bn = dsAnimBone.bn;
		for (int bi = 0; bi < bn; ++bi)
		{
			DS_BONE& bone = dsAnimBone.pBone[bi];
			const int weightNum = bone.indexNum;
			bool check = false;
			for (int wi = 0; wi < weightNum; ++wi)
			{
				if (vi == bone.pIndex[wi])
				{
					//一つのボーンが２つ以上同じ頂点に作用することはありえない
					if (check)
					{
						assert(0 && "１つのボーン内で頂点インデックスが重複しています");
					}
					weights.push_back(&bone.pWeight[wi]);
					check = true;
				}
			}
		}

		//この頂点に影響する重みを正規化する
		float sum = 0;
		for each(float* w in weights) sum += (*w);
		for each(float* w in weights) (*w) /= sum;
	}
}

namespace
{
	void __FbxCoordTrans(DS_KEY_FRAME* frame, const int keyNum)
	{
		//FBXをエクスポートするときに座標変換がされるが、キーフレームの姿勢はそれが反映されていない
		
	}
}

void ConvertFBX::_FbxCoordTrans(DS_ANIM_BONE& dsAnimBone)
{
	const int an = dsAnimBone.an;
	for (int ai = 0; ai < an; ++ai)
	{
		const int pn = dsAnimBone.pAnim[ai].poseNum;
		for (int pi = 0; pi < pn; ++pi)
		{
			DS_ANIM_POSE& pose = dsAnimBone.pAnim[ai].pose[pi];
			__FbxCoordTrans(pose.keyFrameTX, pose.keyFrameNumTX);
			__FbxCoordTrans(pose.keyFrameTY, pose.keyFrameNumTY);
			__FbxCoordTrans(pose.keyFrameTZ, pose.keyFrameNumTZ);
		}
	}
}

//ボーンの位置にスケールを掛けて、スケール成分を消す
void ConvertFBX::_BorneScaleCalc(DS_ANIM_BONE& dsAnimBone)
{
	//モーションの方にも掛けたのでもういらないから消してしまう
	const int bn = dsAnimBone.bn;
	for (int bi = 0; bi < bn; ++bi)
	{
		DS_MAT& mat = dsAnimBone.pBone[bi].initMat;
		{
			const DS_VERTEX vx = DS_VERTEX::Set(mat.m[0]);
			const float sx = vx.Length3();
			//mat.m[0][3] *= sx;
			const DS_VERTEX vxN = DS_VERTEX::Normalize3(vx);
			mat.m[0][0] = vxN.x; mat.m[0][1] = vxN.y; mat.m[0][2] = vxN.z;
		}
		{
			const DS_VERTEX vy = DS_VERTEX::Set(mat.m[1]);
			const float sy = vy.Length3();
			//mat.m[1][3] *= sy;
			const DS_VERTEX vyN = DS_VERTEX::Normalize3(vy);
			mat.m[1][0] = vyN.x; mat.m[1][1] = vyN.y; mat.m[1][2] = vyN.z;
		}
		{
			const DS_VERTEX vz = DS_VERTEX::Set(mat.m[2]);
			const float sz = vz.Length3();
			//mat.m[2][3] *= sz;
			const DS_VERTEX vzN = DS_VERTEX::Normalize3(vz);
			mat.m[2][0] = vzN.x; mat.m[2][1] = vzN.y; mat.m[2][2] = vzN.z;
		}
	}
}