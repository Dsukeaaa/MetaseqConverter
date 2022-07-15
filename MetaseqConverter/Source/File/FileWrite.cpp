#include "MetPch.h"

#ifndef _FILE_WRITE_
#include "FileWrite.h"
#endif

#ifndef _OUTPUTRES_
#include "Res/OutputRes.h"
#endif

using namespace MetaseqConverter;
using namespace std;

FileWrite::FileWrite()
{
}

//virtual 
FileWrite::~FileWrite()
{
}

#if 0//オフセット形式途中実装
//static 
bool FileWrite::Write(const char* path, const OutputRes& res)
{
	std::ofstream  fs(path, ios::out | ios::binary);
	if (!fs)
	{
		printf("%s\n", path);
		assert(0 && "ファイルオープンに失敗");
		return false;
	}

	uint64 currentWriteSize = 0;
	auto _write = [&fs, &currentWriteSize](const void* data, size_t size){
		fs.write((char*)(data), size);
		currentWriteSize += size;
	};

	/*
	header
	vertex
	index
	face
	dmyPoly
	*/

	{//header
		DS_HEADER header;
		header.ver = 0;	
		header.vertexOffset = sizeof(DS_HEADER);
		header.faceOffset = sizeof(DS_HEADER) + res.dsAnimModel.vertexSize;
		_write(&header, sizeof(header));
	}

	{//vertex
		const vari_size vn = res.dsAnimModel.vn;
		_write(&vn, sizeof(vn));
		_write(res.dsAnimModel.pVertex, vn*sizeof(DS_VERTEX));
	}

	{//face
		const vari_size fn = res.dsAnimModel.fn;

		//indexポインタをオフセット値に書き換えるためのコピー作成
		DS_FACE* pWriteFace = new DS_FACE[fn];
		std::memcpy(pWriteFace, res.dsAnimModel.pFace, sizeof(DS_FACE)*fn);

		{//index
			for (vari_size fIdx = 0; fIdx < fn; ++fIdx)
			{
				DS_FACE& face = pWriteFace[fIdx];
				const uint64 indexSize = sizeof(vari_size)*face.vn;
				//ポインタをファイルオフセット値に置き換え
				face.indexOffset = currentWriteSize;
				_write(face.pIndex, indexSize);
				
			}
		}

		{//face
			_write(&fn, sizeof(fn));
			_write(pWriteFace, sizeof(DS_FACE)*fn);
		}
		delete[] pWriteFace;
	}

	{//dmyPoly
		const vari_size dn = res.dsAnimModel.dmyPoly.size();
		const size_t dmyPolySize = dn*sizeof(DS_DMYPOLY);
		_write(&dn, sizeof(dn));
		_write(res.dsAnimModel.dmyPoly.data(), dmyPolySize);
	}

	{//material
		const vari_size mn = static_cast<vari_size>(res.dsAnimModel.mtr.size());
		DS_MATERIAL_FORMAT *pWrinteMtr = new DS_MATERIAL_FORMAT[mn];
		//オフセット書き込みのためのコピー
		for (int mtrIdx = 0; mtrIdx < mn; ++mtrIdx)
		{
			pWrinteMtr[mtrIdx].ambient[0] = res.dsAnimModel.mtr[mtrIdx].ambient[0];
			pWrinteMtr[mtrIdx].ambient[1] = res.dsAnimModel.mtr[mtrIdx].ambient[1];
			pWrinteMtr[mtrIdx].ambient[2] = res.dsAnimModel.mtr[mtrIdx].ambient[2];
			pWrinteMtr[mtrIdx].diffuse[0] = res.dsAnimModel.mtr[mtrIdx].diffuse[0];
			pWrinteMtr[mtrIdx].diffuse[1] = res.dsAnimModel.mtr[mtrIdx].diffuse[1];
			pWrinteMtr[mtrIdx].diffuse[2] = res.dsAnimModel.mtr[mtrIdx].diffuse[2];
			pWrinteMtr[mtrIdx].emissive[0] = res.dsAnimModel.mtr[mtrIdx].emissive[0];
			pWrinteMtr[mtrIdx].emissive[1] = res.dsAnimModel.mtr[mtrIdx].emissive[1];
			pWrinteMtr[mtrIdx].emissive[2] = res.dsAnimModel.mtr[mtrIdx].emissive[2];
			pWrinteMtr[mtrIdx].specular[0] = res.dsAnimModel.mtr[mtrIdx].specular[0];
			pWrinteMtr[mtrIdx].specular[1] = res.dsAnimModel.mtr[mtrIdx].specular[1];
			pWrinteMtr[mtrIdx].specular[2] = res.dsAnimModel.mtr[mtrIdx].specular[2];
			pWrinteMtr[mtrIdx].shininess = res.dsAnimModel.mtr[mtrIdx].shininess;
			pWrinteMtr[mtrIdx].materialParamId = res.dsAnimModel.mtr[mtrIdx].materialParamId;
			pWrinteMtr[mtrIdx].nameLen = res.dsAnimModel.mtr[mtrIdx].name.length()+1;
			pWrinteMtr[mtrIdx].nameOffset = 0;//あとで書き込む
			pWrinteMtr[mtrIdx].textureNum = res.dsAnimModel.mtr[mtrIdx].texture.size();
			pWrinteMtr[mtrIdx].textureOffset = 0;//あとで書き込む
		}
	
		{//name
			for (int mtrIdx = 0; mtrIdx < mn; ++mtrIdx)
			{
				pWrinteMtr[mtrIdx].nameOffset = currentWriteSize;
				_write(res.dsAnimModel.mtr[mtrIdx].name.data(), res.dsAnimModel.mtr[mtrIdx].name.size()*sizeof(char));
			}
		}

		{//uv
			for (const DS_MATERIAL& mtr : res.dsAnimModel.mtr)
			{
				for (const DS_TEXTURE& texture : mtr.texture)
				{
					const vari_size uvNum = static_cast<vari_size>(texture.uv.size());
				
				
					for(const DS_TEXTURE::UV& uv : texture.uv)
					{
						float uvCoord[2] =
						{
							uv.uv[0],
							uv.uv[1],
						};
						fs.write((char*)(&uvCoord[0]), sizeof(uvCoord));

						const vari_size vertexIndex = static_cast<vari_size>(uv.vertexIndex);
						fs.write((char*)(&vertexIndex), sizeof(vertexIndex));
					}
				}
			}
		}

		{//face
			
		}
	}


	{//material
		const vari_size mn = static_cast<vari_size>(res.dsAnimModel.mtr.size());
		fs.write((char*)(&mn), sizeof(mn));

		for(const DS_MATERIAL& mtr : res.dsAnimModel.mtr)
		{
			fs.write((char*)(&mtr.ambient[0]), sizeof(mtr.ambient));
			fs.write((char*)(&mtr.diffuse[0]), sizeof(mtr.diffuse));
			fs.write((char*)(&mtr.emissive[0]), sizeof(mtr.emissive));
			fs.write((char*)(&mtr.specular[0]), sizeof(mtr.specular));
			fs.write((char*)(&mtr.shininess), sizeof(mtr.shininess));

			fs.write((char*)(&mtr.materialParamId), sizeof(mtr.materialParamId));

			const vari_size tn = static_cast<vari_size>(mtr.texture.size());
			fs.write((char*)(&tn), sizeof(tn));

			for each(const DS_TEXTURE& texture in mtr.texture)
			{
				{
					const int nameSize = static_cast<int>(texture.albedoTexPath.size());
					fs.write((char*)(&nameSize), sizeof(nameSize));
					fs.write(texture.albedoTexPath.data(), texture.albedoTexPath.size());
				}
				{
					const int nameSize = static_cast<int>(texture.normalTexPath.size());
					fs.write((char*)(&nameSize), sizeof(nameSize));
					fs.write(texture.normalTexPath.data(), texture.normalTexPath.size());
				}
				{
					const int nameSize = static_cast<int>(texture.specularTexPath.size());
					fs.write((char*)(&nameSize), sizeof(nameSize));
					fs.write(texture.specularTexPath.data(), texture.specularTexPath.size());
				}

				const vari_size uvNum = static_cast<vari_size>( texture.uv.size() );
				fs.write((char*)(&uvNum), sizeof(uvNum));
				
				for each(const DS_TEXTURE::UV& uv in texture.uv)
				{
					float uvCoord[2] =
					{
						uv.uv[0],
						uv.uv[1],
					};
					fs.write((char*)(&uvCoord[0]), sizeof(uvCoord));

					const vari_size vertexIndex = static_cast<vari_size>(uv.vertexIndex);
					fs.write((char*)(&vertexIndex), sizeof(vertexIndex));
				}

				const vari_size uvFaceNum = static_cast<vari_size>(texture.uvFace.size());
				fs.write((char*)(&uvFaceNum), sizeof(uvFaceNum));
				for each(const DS_TEXTURE::UV_FACE& uvFace in texture.uvFace)
				{
					const vari_size uvpNum = static_cast<vari_size>(uvFace.index.size());
					fs.write((char*)(&uvpNum), sizeof(uvpNum));
					const vari_size refGeomFaceIndex = uvFace.refGeomFaceIndex;
					fs.write((char*)(&refGeomFaceIndex), sizeof(refGeomFaceIndex));
					for each(int uvpIndex in uvFace.index)
					{
						fs.write((char*)(&uvpIndex), sizeof(uvpIndex));
					}
				}
			}
		}
	}


	/*
	-----animation
	boneNum  4byte
	-----bone
		nameSize 4byte
		name	nameSize byte
		initMat  64byte
		indexNum 4byte
		------index
			index 4byte
			weight 4byte
			:
		-----------
		parentIdx　4byte
		childNum 4byte
		pChildIdx 4byte * childNum
		isMaster 1byte
	---------
	:

	animNum 4byte
	-----anim
		animNameSize 4byte
		animName animNameSize 
		poseNum 4byte
		------pose
			keyFrameNumTX  4byte
			----keyFrameTX
				localTimeMs    4byte
				value 4byte
				:
			-----------
			keyFrameNumTY  4byte
			----keyFrameTY
				localTimeMs    4byte
				value 4byte
				:
			-----------
			keyFrameNumTZ  4byte
			----keyFrameTZ
				localTimeMs    4byte
				value 4byte
				:
			-----------
			keyFrameNumRX  4byte
			----keyFrameRX
				localTimeMs    4byte
				value 4byte
				:
			-----------
			keyFrameNumRY  4byte
			----keyFrameRY
				localTimeMs    4byte
				value 4byte
				:
			-----------
			keyFrameNumRZ  4byte
			----keyFrameRZ
				localTimeMs    4byte
				value 4byte
				:
			-----------
			keyFrameNumRQ  4byte
			----keyFrameRQ
				localTimeMs    4byte
				value 4byte
				:
			-----------
			keyFrameNumSX  4byte
			----keyFrameSX
				localTimeMs    4byte
				value 4byte
				:
			-----------
			keyFrameNumSY  4byte
			----keyFrameSY
				localTimeMs    4byte
				value 4byte
				:
			-----------
			keyFrameNumSZ  4byte
			----keyFrameSZ
				localTimeMs    4byte
				value 4byte
				:
			-----------
		//マスター移動量
		---------
		keyFrameNumTX  4byte
		----keyFrameTX
		localTimeMs    4byte
		value 4byte
		:
		-----------
		keyFrameNumTY  4byte
		----keyFrameTY
		localTimeMs    4byte
		value 4byte
		:
		-----------
		keyFrameNumTZ  4byte
		----keyFrameTZ
		localTimeMs    4byte
		value 4byte
		:
		-----------
		keyFrameNumRX  4byte
		----keyFrameRX
		localTimeMs    4byte
		value 4byte
		:
		-----------
		keyFrameNumRY  4byte
		----keyFrameRY
		localTimeMs    4byte
		value 4byte
		:
		-----------
		keyFrameNumRZ  4byte
		----keyFrameRZ
		localTimeMs    4byte
		value 4byte
		:
	--------
	:
	--------
	*/
	
	{//bone
		const unsigned long boneNum = res.dsAnimBone.bn;
		fs.write((char*)(&boneNum), sizeof(boneNum));
		for (unsigned long bIdx = 0; bIdx < boneNum; ++bIdx)
		{
			const unsigned long nameSize = res.dsAnimBone.pBone[bIdx].nameSize;
			fs.write((char*)(&nameSize), sizeof(nameSize));
			fs.write(res.dsAnimBone.pBone[bIdx].name, nameSize);
			fs.write((char*)(&res.dsAnimBone.pBone[bIdx].initMat.m[0][0]), DS_MAT::SIZE);
			
			const long indexNum = res.dsAnimBone.pBone[bIdx].indexNum;
			fs.write((char*)(&indexNum), sizeof(indexNum));
			//ボーンに紐づく頂点インデックス
			for (int wIdx = 0; wIdx < indexNum; ++wIdx)
			{
				vari_size vertexIndex = res.dsAnimBone.pBone[bIdx].pIndex[wIdx];
				fs.write((char*)(&vertexIndex), sizeof(vertexIndex));
			}
			//ボーンに紐づく頂点に対する重み
			for (int wIdx = 0; wIdx < indexNum; ++wIdx)
			{
				float weight = res.dsAnimBone.pBone[bIdx].pWeight[wIdx];
				fs.write((char*)(&weight), sizeof(weight));
			}
			const int parentIdx = res.dsAnimBone.pBone[bIdx].parentIdx;
			fs.write((char*)(&parentIdx), sizeof(parentIdx));
			const int childNum = res.dsAnimBone.pBone[bIdx].childNum;
			fs.write((char*)(&childNum), sizeof(childNum));
			if (0 < childNum)
			{
				fs.write((char*)(res.dsAnimBone.pBone[bIdx].pChildIdx), sizeof(int)*childNum);
			}
			const unsigned char isMaster = res.dsAnimBone.pBone[bIdx].isMaster;
			fs.write((char*)(&isMaster), sizeof(isMaster));
		}
	}

	{//anim
		const unsigned long animNum = res.dsAnimBone.an;
		fs.write((char*)(&animNum), sizeof(animNum));
		//アニメーション
		for (unsigned long aIdx = 0; aIdx < animNum; ++aIdx)
		{
			const unsigned long animNameSize = res.dsAnimBone.pAnim[aIdx].animNameSize;
			fs.write((char*)(&animNameSize), sizeof(animNameSize));
			fs.write(res.dsAnimBone.pAnim[aIdx].animName, animNameSize);
			
			const unsigned long poseNum = res.dsAnimBone.pAnim[aIdx].poseNum;
			fs.write((char*)(&poseNum), sizeof(poseNum));
			//ポーズ
			for (unsigned long poseIdx = 0; poseIdx < poseNum; ++poseIdx)
			{
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTX;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTX[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTX[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTY;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTY[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTY[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTZ;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTZ[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTZ[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}


				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRX;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRX[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRX[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRY;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRY[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRY[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRZ;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRZ[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRZ[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRQ;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRQ[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRQ[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}


				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSX;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSX[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSX[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSY;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSY[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSY[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSZ;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSZ[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSZ[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
			}

			//マスター移動量
			{
				const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumTX;
				fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
				for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
				{
					const float localTime = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTX[keyFrame].localTimeMs;
					const float value = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTX[keyFrame].value;
					fs.write((char*)(&localTime), sizeof(localTime));
					fs.write((char*)(&value), sizeof(value));
				}
			}
			{
				const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumTY;
				fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
				for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
				{
					const float localTime = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTY[keyFrame].localTimeMs;
					const float value = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTY[keyFrame].value;
					fs.write((char*)(&localTime), sizeof(localTime));
					fs.write((char*)(&value), sizeof(value));
				}
			}
			{
				const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumTZ;
				fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
				for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
				{
					const float localTime = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTZ[keyFrame].localTimeMs;
					const float value = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTZ[keyFrame].value;
					fs.write((char*)(&localTime), sizeof(localTime));
					fs.write((char*)(&value), sizeof(value));
				}
			}

			{
				const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumRX;
				fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
				for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
				{
					const float localTime = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRX[keyFrame].localTimeMs;
					const float value = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRX[keyFrame].value;
					fs.write((char*)(&localTime), sizeof(localTime));
					fs.write((char*)(&value), sizeof(value));
				}
			}
			{
				const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumRY;
				fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
				for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
				{
					const float localTime = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRY[keyFrame].localTimeMs;
					const float value = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRY[keyFrame].value;
					fs.write((char*)(&localTime), sizeof(localTime));
					fs.write((char*)(&value), sizeof(value));
				}
			}
			{
				const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumRZ;
				fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
				for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
				{
					const float localTime = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRZ[keyFrame].localTimeMs;
					const float value = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRZ[keyFrame].value;
					fs.write((char*)(&localTime), sizeof(localTime));
					fs.write((char*)(&value), sizeof(value));
				}
			}
			{
				const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumRQ;
				fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
				for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
				{
					const float localTime = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRQ[keyFrame].localTimeMs;
					const float value = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRQ[keyFrame].value;
					fs.write((char*)(&localTime), sizeof(localTime));
					fs.write((char*)(&value), sizeof(value));
				}
			}
		
		}
	}

	/*
	ragdollParamNum 4byte
	-----ragdollParam
		paramId 4byte
		boneIdx 4byte
		:
	----------------
	*/
	{//custom property ragdoll
		const std::vector<DS_RAGDOLL_PARAM_ID>& ragdoll = res.dsCustomProperty.ragdoll;
		unsigned long ragdollParamNum = ragdoll.size();
		fs.write((char*)(&ragdollParamNum), sizeof(ragdollParamNum));
		if (0 < ragdollParamNum){
			fs.write((char*)ragdoll.data(), sizeof(DS_RAGDOLL_PARAM_ID)*ragdollParamNum);
		}
	}
	return true;
}
#endif

//static 
bool FileWrite::Write(const char* path, const OutputRes& res)
{
	/*

	32bit ver
	頂点数とか4byte

	----header
	ver %d  4byte
	vertex_offset 8byte			先頭からのオフセット
	face_offset   8byte			先頭からのオフセット

	-----vertex
	vertex_num 4byte
	-----vertex
	vertex x 4byte     インデックスの０から
	vertex y 4byte
	vertex z 4byte
	vertex w 4byte
	---------
	:
	----------

	-----face
	face_num 4byte
	----------face     インデックス０から
	fn 4byte
	normal x 4byte
	normal y 4byte
	normal z 4byte
	index 4byte    インデックス０から
	---------------
	:
	---------

	-----dmyPoly
	dmyPoly_num 4byte
	----------dmyPoly
	index0 4byte
	index1 4byte
	index2 4byte
	id     4byte
	---------------
	:
	---------


	material num 4byte
	-----material
	ambient 12byte
	diffuse 12byte
	specular 12byte
	shininess 4byte
	materialParamId 4byte
	texture num 4byte
	----------texture
	name sizeAlbedo 4byte
	texture pathAlbedo    n * namae size byte
	name sizeNormal 4byte
	texture pathNormal    n * namae size byte
	uv num 4byte
	---------uv
	u 4byte
	v 4byte
	vertexIndex 4byte
	----------
	:
	face num 4byte
	---------uvFace
	uvpNum 4byte
	refGeomFaceIndex 4byte
	--------uvpIndex
	index 4byte
	---------------
	:
	--------------
	:
	----------------
	:
	--------
	:

	*/


	std::ofstream  fs(path, ios::out | ios::binary);
	if (!fs)
	{
		printf("%s\n", path);
		assert(0 && "ファイルオープンに失敗");
		return false;
	}

	{//header
		long ver = FORMAT_VER;
		fs.write((char*)(&ver), sizeof(long));

		const unsigned long long vertexOffset = 20;
		fs.write((char*)(&vertexOffset), sizeof(vertexOffset));

		const unsigned long long faceOffset = vertexOffset + sizeof(unsigned long long) + res.dsAnimModel.vertexSize;
		fs.write((char*)(&faceOffset), sizeof(faceOffset));
	}
	{//vertex
		const vari_size vn = res.dsAnimModel.vn;
		fs.write((char*)(&vn), sizeof(vn));

		for (vari_size i = 0; i < vn; ++i)
		{
			const float v[4] =
			{
				res.dsAnimModel.pVertex[i].x,
				res.dsAnimModel.pVertex[i].y,
				res.dsAnimModel.pVertex[i].z,
				res.dsAnimModel.pVertex[i].w,
			};
			fs.write((char*)(&v[0]), sizeof(v));
		}
	}
	{//face
		const vari_size fn = res.dsAnimModel.fn;
		fs.write((char*)(&fn), sizeof(fn));

		for (vari_size fIdx = 0; fIdx < fn; ++fIdx)
		{
			const vari_size vn = res.dsAnimModel.pFace[fIdx].vn;
			fs.write((char*)(&vn), sizeof(vn));

			const float normal[4] =
			{
				res.dsAnimModel.pFace[fIdx].normal.x,
				res.dsAnimModel.pFace[fIdx].normal.y,
				res.dsAnimModel.pFace[fIdx].normal.z,
				res.dsAnimModel.pFace[fIdx].normal.w,
			};
			fs.write((char*)(&normal[0]), sizeof(normal));

			for (vari_size vIdx = 0; vIdx < vn; ++vIdx)
			{
				const vari_size index = res.dsAnimModel.pFace[fIdx].pIndex[vIdx];
				fs.write((char*)(&index), sizeof(index));
			}
		}
	}
	{//dmyPoly
		const vari_size dn = res.dsAnimModel.dmyPoly.size();
		fs.write((char*)(&dn), sizeof(dn));
		const size_t dmyPolySize = dn*sizeof(DS_DMYPOLY);
		fs.write((char*)(res.dsAnimModel.dmyPoly.data()), dmyPolySize);
	}
	{//material
		const vari_size mn = static_cast<vari_size>(res.dsAnimModel.mtr.size());
		fs.write((char*)(&mn), sizeof(mn));

		for each(const DS_MATERIAL& mtr in res.dsAnimModel.mtr)
		{
			fs.write((char*)(&mtr.ambient[0]), sizeof(mtr.ambient));
			fs.write((char*)(&mtr.diffuse[0]), sizeof(mtr.diffuse));
			fs.write((char*)(&mtr.emissive[0]), sizeof(mtr.emissive));
			fs.write((char*)(&mtr.specular[0]), sizeof(mtr.specular));
			fs.write((char*)(&mtr.shininess), sizeof(mtr.shininess));

			fs.write((char*)(&mtr.materialParamId), sizeof(mtr.materialParamId));

			const vari_size tn = static_cast<vari_size>(mtr.texture.size());
			fs.write((char*)(&tn), sizeof(tn));

			for each(const DS_TEXTURE& texture in mtr.texture)
			{
				{
					const int nameSize = static_cast<int>(texture.albedoTexPath.size());
					fs.write((char*)(&nameSize), sizeof(nameSize));
					fs.write(texture.albedoTexPath.data(), texture.albedoTexPath.size());
				}
				{
					const int nameSize = static_cast<int>(texture.normalTexPath.size());
					fs.write((char*)(&nameSize), sizeof(nameSize));
					fs.write(texture.normalTexPath.data(), texture.normalTexPath.size());
				}
				{
					const int nameSize = static_cast<int>(texture.specularTexPath.size());
					fs.write((char*)(&nameSize), sizeof(nameSize));
					fs.write(texture.specularTexPath.data(), texture.specularTexPath.size());
				}

				const vari_size uvNum = static_cast<vari_size>( texture.uv.size() );
				fs.write((char*)(&uvNum), sizeof(uvNum));

				for each(const DS_TEXTURE::UV& uv in texture.uv)
				{
					float uvCoord[2] =
					{
						uv.uv[0],
						uv.uv[1],
					};
					fs.write((char*)(&uvCoord[0]), sizeof(uvCoord));

					const vari_size vertexIndex = static_cast<vari_size>(uv.vertexIndex);
					fs.write((char*)(&vertexIndex), sizeof(vertexIndex));
				}

				const vari_size uvFaceNum = static_cast<vari_size>(texture.uvFace.size());
				fs.write((char*)(&uvFaceNum), sizeof(uvFaceNum));
				for each(const DS_TEXTURE::UV_FACE& uvFace in texture.uvFace)
				{
					const vari_size uvpNum = static_cast<vari_size>(uvFace.index.size());
					fs.write((char*)(&uvpNum), sizeof(uvpNum));
					const vari_size refGeomFaceIndex = uvFace.refGeomFaceIndex;
					fs.write((char*)(&refGeomFaceIndex), sizeof(refGeomFaceIndex));
					for each(int uvpIndex in uvFace.index)
					{
						fs.write((char*)(&uvpIndex), sizeof(uvpIndex));
					}
				}
			}
		}
	}


	/*
	-----animation
	boneNum  4byte
	-----bone
	nameSize 4byte
	name	nameSize byte
	initMat  64byte
	indexNum 4byte
	------index
	index 4byte
	weight 4byte
	:
	-----------
	parentIdx　4byte
	childNum 4byte
	pChildIdx 4byte * childNum
	isMaster 1byte
	---------
	:

	animNum 4byte
	-----anim
	animNameSize 4byte
	animName animNameSize
	poseNum 4byte
	------pose
	keyFrameNumTX  4byte
	----keyFrameTX
	localTimeMs    4byte
	value 4byte
	:
	-----------
	keyFrameNumTY  4byte
	----keyFrameTY
	localTimeMs    4byte
	value 4byte
	:
	-----------
	keyFrameNumTZ  4byte
	----keyFrameTZ
	localTimeMs    4byte
	value 4byte
	:
	-----------
	keyFrameNumRX  4byte
	----keyFrameRX
	localTimeMs    4byte
	value 4byte
	:
	-----------
	keyFrameNumRY  4byte
	----keyFrameRY
	localTimeMs    4byte
	value 4byte
	:
	-----------
	keyFrameNumRZ  4byte
	----keyFrameRZ
	localTimeMs    4byte
	value 4byte
	:
	-----------
	keyFrameNumRQ  4byte
	----keyFrameRQ
	localTimeMs    4byte
	value 4byte
	:
	-----------
	keyFrameNumSX  4byte
	----keyFrameSX
	localTimeMs    4byte
	value 4byte
	:
	-----------
	keyFrameNumSY  4byte
	----keyFrameSY
	localTimeMs    4byte
	value 4byte
	:
	-----------
	keyFrameNumSZ  4byte
	----keyFrameSZ
	localTimeMs    4byte
	value 4byte
	:
	-----------
	//マスター移動量
	---------
	keyFrameNumTX  4byte
	----keyFrameTX
	localTimeMs    4byte
	value 4byte
	:
	-----------
	keyFrameNumTY  4byte
	----keyFrameTY
	localTimeMs    4byte
	value 4byte
	:
	-----------
	keyFrameNumTZ  4byte
	----keyFrameTZ
	localTimeMs    4byte
	value 4byte
	:
	-----------
	keyFrameNumRX  4byte
	----keyFrameRX
	localTimeMs    4byte
	value 4byte
	:
	-----------
	keyFrameNumRY  4byte
	----keyFrameRY
	localTimeMs    4byte
	value 4byte
	:
	-----------
	keyFrameNumRZ  4byte
	----keyFrameRZ
	localTimeMs    4byte
	value 4byte
	:
	--------
	:
	--------
	*/

	{//bone
		const unsigned long boneNum = res.dsAnimBone.bn;
		fs.write((char*)(&boneNum), sizeof(boneNum));
		for (unsigned long bIdx = 0; bIdx < boneNum; ++bIdx)
		{
			const unsigned long nameSize = res.dsAnimBone.pBone[bIdx].nameSize;
			fs.write((char*)(&nameSize), sizeof(nameSize));
			fs.write(res.dsAnimBone.pBone[bIdx].name, nameSize);
			fs.write((char*)(&res.dsAnimBone.pBone[bIdx].initMat.m[0][0]), DS_MAT::SIZE);

			const long indexNum = res.dsAnimBone.pBone[bIdx].indexNum;
			fs.write((char*)(&indexNum), sizeof(indexNum));
			//ボーンに紐づく頂点インデックス
			for (int wIdx = 0; wIdx < indexNum; ++wIdx)
			{
				vari_size vertexIndex = res.dsAnimBone.pBone[bIdx].pIndex[wIdx];
				fs.write((char*)(&vertexIndex), sizeof(vertexIndex));
			}
			//ボーンに紐づく頂点に対する重み
			for (int wIdx = 0; wIdx < indexNum; ++wIdx)
			{
				float weight = res.dsAnimBone.pBone[bIdx].pWeight[wIdx];
				fs.write((char*)(&weight), sizeof(weight));
			}
			const int parentIdx = res.dsAnimBone.pBone[bIdx].parentIdx;
			fs.write((char*)(&parentIdx), sizeof(parentIdx));
			const int childNum = res.dsAnimBone.pBone[bIdx].childNum;
			fs.write((char*)(&childNum), sizeof(childNum));
			if (0 < childNum)
			{
				fs.write((char*)(res.dsAnimBone.pBone[bIdx].pChildIdx), sizeof(int)*childNum);
			}
			const unsigned char isMaster = res.dsAnimBone.pBone[bIdx].isMaster;
			fs.write((char*)(&isMaster), sizeof(isMaster));
		}
	}

	{//anim
		const unsigned long animNum = res.dsAnimBone.an;
		fs.write((char*)(&animNum), sizeof(animNum));
		//アニメーション
		for (unsigned long aIdx = 0; aIdx < animNum; ++aIdx)
		{
			const unsigned long animNameSize = res.dsAnimBone.pAnim[aIdx].animNameSize;
			fs.write((char*)(&animNameSize), sizeof(animNameSize));
			fs.write(res.dsAnimBone.pAnim[aIdx].animName, animNameSize);

			const unsigned long poseNum = res.dsAnimBone.pAnim[aIdx].poseNum;
			fs.write((char*)(&poseNum), sizeof(poseNum));
			//ポーズ
			for (unsigned long poseIdx = 0; poseIdx < poseNum; ++poseIdx)
			{
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTX;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTX[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTX[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTY;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTY[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTY[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTZ;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTZ[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTZ[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}


				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRX;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRX[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRX[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRY;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRY[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRY[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRZ;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRZ[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRZ[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRQ;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRQ[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRQ[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}


				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSX;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSX[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSX[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSY;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSY[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSY[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
				{
					const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSZ;
					fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						const float localTime = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSZ[keyFrame].localTimeMs;
						const float value = res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSZ[keyFrame].value;
						fs.write((char*)(&localTime), sizeof(localTime));
						fs.write((char*)(&value), sizeof(value));
					}
				}
			}

			//マスター移動量
			{
				const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumTX;
				fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
				for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
				{
					const float localTime = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTX[keyFrame].localTimeMs;
					const float value = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTX[keyFrame].value;
					fs.write((char*)(&localTime), sizeof(localTime));
					fs.write((char*)(&value), sizeof(value));
				}
			}
			{
				const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumTY;
				fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
				for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
				{
					const float localTime = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTY[keyFrame].localTimeMs;
					const float value = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTY[keyFrame].value;
					fs.write((char*)(&localTime), sizeof(localTime));
					fs.write((char*)(&value), sizeof(value));
				}
			}
			{
				const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumTZ;
				fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
				for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
				{
					const float localTime = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTZ[keyFrame].localTimeMs;
					const float value = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameTZ[keyFrame].value;
					fs.write((char*)(&localTime), sizeof(localTime));
					fs.write((char*)(&value), sizeof(value));
				}
			}

			{
				const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumRX;
				fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
				for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
				{
					const float localTime = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRX[keyFrame].localTimeMs;
					const float value = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRX[keyFrame].value;
					fs.write((char*)(&localTime), sizeof(localTime));
					fs.write((char*)(&value), sizeof(value));
				}
			}
			{
				const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumRY;
				fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
				for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
				{
					const float localTime = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRY[keyFrame].localTimeMs;
					const float value = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRY[keyFrame].value;
					fs.write((char*)(&localTime), sizeof(localTime));
					fs.write((char*)(&value), sizeof(value));
				}
			}
			{
				const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumRZ;
				fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
				for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
				{
					const float localTime = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRZ[keyFrame].localTimeMs;
					const float value = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRZ[keyFrame].value;
					fs.write((char*)(&localTime), sizeof(localTime));
					fs.write((char*)(&value), sizeof(value));
				}
			}
			{
				const long keyFrameNum = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameNumRQ;
				fs.write((char*)(&keyFrameNum), sizeof(keyFrameNum));
				for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
				{
					const float localTime = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRQ[keyFrame].localTimeMs;
					const float value = res.dsAnimBone.pAnim[aIdx].masterMove.keyFrameRQ[keyFrame].value;
					fs.write((char*)(&localTime), sizeof(localTime));
					fs.write((char*)(&value), sizeof(value));
				}
			}

		}
	}

	/*
	ragdollParamNum 4byte
	-----ragdollParam
	paramId 4byte
	boneIdx 4byte
	:
	----------------
	*/
	{//custom property ragdoll
		const std::vector<DS_RAGDOLL_PARAM_ID>& ragdoll = res.dsCustomProperty.ragdoll;
		unsigned long ragdollParamNum = ragdoll.size();
		fs.write((char*)(&ragdollParamNum), sizeof(ragdollParamNum));
		if (0 < ragdollParamNum){
			fs.write((char*)ragdoll.data(), sizeof(DS_RAGDOLL_PARAM_ID)*ragdollParamNum);
		}
	}


	{//lightData
		const std::vector<DS_LIGHT>& lights = res.lightData.lights;
		unsigned int lightNum = lights.size();
		fs.write((char*)(&lightNum), sizeof(lightNum));
		if (0 < lightNum){
			fs.write((char*)lights.data(), sizeof(DS_LIGHT)*lightNum);
		}
	}

	return true;

}


//static 
bool FileWrite::Check(const char* path, const OutputRes& res)
{
	std::ifstream  fs(path, ios::out | ios::binary);
	if (!fs)
	{
		assert(0 && "ファイルオープンに失敗");
		return false;
	}


	{//header
		long ver;
		fs.read((char*)(&ver), sizeof(long));
		if (ver != 0)
		{
			assert(0&&"file error");
		}

		unsigned long long vertexOffset;
		fs.read((char*)(&vertexOffset), sizeof(vertexOffset));
		if (vertexOffset != 20)
		{ 
			assert(0 && "file error");
		}

		unsigned long long faceOffset;
		fs.read((char*)(&faceOffset), sizeof(faceOffset));
		if (faceOffset != (vertexOffset + sizeof(unsigned long long) + res.dsAnimModel.vertexSize))
		{
			assert(0 && "file error");
		}
	}
	{//vertex
		vari_size vn;
		fs.read((char*)(&vn), sizeof(vn));
		if (vn != res.dsAnimModel.vn)
		{
			assert(0 && "file error");
		}

		for (vari_size i = 0; i < vn; ++i)
		{
			float v[4];
			fs.read((char*)(&v[0]), sizeof(v));
			if ((v[0] != res.dsAnimModel.pVertex[i].x) ||
				(v[1] != res.dsAnimModel.pVertex[i].y) ||
				(v[2] != res.dsAnimModel.pVertex[i].z) ||
				(v[3] != res.dsAnimModel.pVertex[i].w)  )
			{
				assert(0 && "file error");
			}
		}
	}
	{//face
		vari_size fn;;
		fs.read((char*)(&fn), sizeof(fn));
		if (fn != res.dsAnimModel.fn)
		{
			assert(0 && "file error");
		}

		for (vari_size fIdx = 0; fIdx < fn; ++fIdx)
		{
			vari_size vn;
			fs.read((char*)(&vn), sizeof(vn));
			if (vn != res.dsAnimModel.pFace[fIdx].vn)
			{
				assert(0 && "file error");
			}

			float normal[4];
			fs.read((char*)(&normal[0]), sizeof(normal));
			if ((normal[0] != res.dsAnimModel.pFace[fIdx].normal.x) ||
				(normal[1] != res.dsAnimModel.pFace[fIdx].normal.y) ||
				(normal[2] != res.dsAnimModel.pFace[fIdx].normal.z) ||
				(normal[3] != res.dsAnimModel.pFace[fIdx].normal.w))
			{
				assert(0 && "file error");
			}


			for (vari_size vIdx = 0; vIdx < vn; ++vIdx)
			{
				vari_size index;
				fs.read((char*)(&index), sizeof(index));
				if (index != res.dsAnimModel.pFace[fIdx].pIndex[vIdx])
				{
					assert(0 && "file error");
				}

			}
		}
	}

	return true;
}