#include "MetPch.h"
#include "LoadModel.h"

using namespace MetaseqConverter;
using namespace std;

bool MetaseqConverter::LoadModel(const char* path, OutputRes& res)
{
	std::ifstream  fs(path, ios::out | ios::binary);
	if (!fs)
	{
		printf("%s\n", path);
		assert(0 && "ファイルオープンに失敗");
		return false;
	}

	{//header
		long ver;
		fs.read((char*)(&ver), sizeof(long));

		unsigned long long vertexOffset;
		fs.read((char*)(&vertexOffset), sizeof(vertexOffset));

		unsigned long long faceOffset;
		fs.read((char*)(&faceOffset), sizeof(faceOffset));
	}

	//ここからモデル
	{//vertex
		vari_size vn;
		fs.read((char*)(&vn), sizeof(vn));
		res.dsAnimModel.vn = vn;

		res.dsAnimModel.pVertex = new DS_VERTEX[vn];
		for (vari_size i = 0; i < vn; ++i)
		{
			float v[4];
			fs.read((char*)(&v[0]), sizeof(v));
			res.dsAnimModel.pVertex[i].x = v[0];
			res.dsAnimModel.pVertex[i].y = v[1];
			res.dsAnimModel.pVertex[i].z = v[2];
			res.dsAnimModel.pVertex[i].w = v[3];
		}
	}
	{//face
		vari_size fn;
		fs.read((char*)(&fn), sizeof(fn));
		res.dsAnimModel.fn = fn;
		
		res.dsAnimModel.pFace = new DS_FACE[fn];
		for (vari_size fIdx = 0; fIdx < fn; ++fIdx)
		{
			vari_size vn;
			fs.read((char*)(&vn), sizeof(vn));
			res.dsAnimModel.pFace[fIdx].vn = vn;

			float normal[4];
			fs.read((char*)(&normal[0]), sizeof(normal));
			res.dsAnimModel.pFace[fIdx].normal.x = normal[0];
			res.dsAnimModel.pFace[fIdx].normal.y = normal[1];
			res.dsAnimModel.pFace[fIdx].normal.z = normal[2];
			res.dsAnimModel.pFace[fIdx].normal.w = normal[3];

			res.dsAnimModel.pFace[fIdx].pIndex = new vari_size[vn];
			for (vari_size vIdx = 0; vIdx < vn; ++vIdx)
			{
				vari_size index;
				fs.read((char*)(&index), sizeof(index));
				res.dsAnimModel.pFace[fIdx].pIndex[vIdx] = index;
			}
		}
	}
	{//material
		vari_size mn;
		fs.read((char*)(&mn), sizeof(mn));
		
		for (vari_size mi = 0; mi < mn; ++mi)
		{
			DS_MATERIAL mtr;

			fs.read((char*)(&mtr.ambient[0]), sizeof(mtr.ambient));
			fs.read((char*)(&mtr.diffuse[0]), sizeof(mtr.diffuse));
			fs.read((char*)(&mtr.emissive[0]), sizeof(mtr.emissive));
			fs.read((char*)(&mtr.specular[0]), sizeof(mtr.specular));
			fs.read((char*)(&mtr.shininess), sizeof(mtr.shininess));

			fs.read((char*)(&mtr.materialParamId), sizeof(mtr.materialParamId));

			vari_size tn;
			fs.read((char*)(&tn), sizeof(tn));

			for (vari_size ti = 0; ti < tn; ++ti)
			{
				DS_TEXTURE texture;
				{
					int nameSize;
					fs.read((char*)(&nameSize), sizeof(nameSize));
					char* tmpBuf = new char[nameSize];
					fs.read(tmpBuf, nameSize);
					texture.albedoTexPath = tmpBuf;
					delete[] tmpBuf;
				}
				{
					int nameSize;
					fs.read((char*)(&nameSize), sizeof(nameSize));
					char* tmpBuf = new char[nameSize];
					fs.read(tmpBuf, nameSize);
					texture.normalTexPath = tmpBuf;
					delete[] tmpBuf;
				}
				{
					int nameSize;
					fs.read((char*)(&nameSize), sizeof(nameSize));
					char* tmpBuf = new char[nameSize];
					fs.read(tmpBuf, nameSize);
					texture.specularTexPath = tmpBuf;
					delete[] tmpBuf;
				}
				
				vari_size uvNum;
				fs.read((char*)(&uvNum), sizeof(uvNum));

				for (vari_size uvi = 0; uvi < uvNum; ++uvi)
				{
					DS_TEXTURE::UV uv;
					float uvCoord[2];
					fs.read((char*)(&uvCoord[0]), sizeof(uvCoord));
					uv.uv[0] = uvCoord[0];
					uv.uv[1] = uvCoord[1];

					vari_size vertexIndex;
					fs.read((char*)(&vertexIndex), sizeof(vertexIndex));
					uv.vertexIndex = vertexIndex;

					texture.uv.push_back(uv);
				}

				vari_size uvFaceNum;
				fs.read((char*)(&uvFaceNum), sizeof(uvFaceNum));
				for (vari_size uvfi = 0; uvfi < uvFaceNum; ++uvfi)
				{
					DS_TEXTURE::UV_FACE uvFace;

					vari_size uvpNum;
					fs.read((char*)(&uvpNum), sizeof(uvpNum));
					vari_size refGeomFaceIndex;
					fs.read((char*)(&refGeomFaceIndex), sizeof(refGeomFaceIndex));
					uvFace.refGeomFaceIndex = refGeomFaceIndex;
					for (vari_size uvpi = 0; uvpi < uvpNum; ++uvpi)
					{
						vari_size uvpIndex;
						fs.read((char*)(&uvpIndex), sizeof(uvpIndex));
						uvFace.index.push_back(uvpIndex);
					}

					texture.uvFace.push_back(uvFace);
				}


				mtr.texture.push_back(texture);
			}

			res.dsAnimModel.mtr.push_back(mtr);
		}
	}


	//ここからキーフレームアニメ
	{//bone
		unsigned long boneNum;
		fs.read((char*)(&boneNum), sizeof(boneNum));
		res.dsAnimBone.bn = boneNum;
		res.dsAnimBone.pBone = new DS_BONE[boneNum];

		for (unsigned long bIdx = 0; bIdx < boneNum; ++bIdx)
		{
			unsigned long nameSize;
			fs.read((char*)(&nameSize), sizeof(nameSize));
			res.dsAnimBone.pBone[bIdx].nameSize = nameSize;
			res.dsAnimBone.pBone[bIdx].name = new char[nameSize];

			fs.read(res.dsAnimBone.pBone[bIdx].name, nameSize);

			fs.read((char*)(&res.dsAnimBone.pBone[bIdx].initMat.m[0][0]), DS_MAT::SIZE);

			long indexNum;
			fs.read((char*)(&indexNum), sizeof(indexNum));
			res.dsAnimBone.pBone[bIdx].indexNum = indexNum;

			//ボーンに紐づく頂点インデックス
			res.dsAnimBone.pBone[bIdx].pIndex = new vari_size[indexNum];
			for (int wIdx = 0; wIdx < indexNum; ++wIdx)
			{
				vari_size vertexIndex;
				fs.read((char*)(&vertexIndex), sizeof(vertexIndex));
				res.dsAnimBone.pBone[bIdx].pIndex[wIdx] = vertexIndex;
			}
			//ボーンに紐づく頂点に対する重み
			res.dsAnimBone.pBone[bIdx].pWeight = new float[indexNum];
			for (int wIdx = 0; wIdx < indexNum; ++wIdx)
			{
				float weight;
				fs.read((char*)(&weight), sizeof(weight));
				res.dsAnimBone.pBone[bIdx].pWeight[wIdx] = weight;
			}

			int parentIdx = -1;
			fs.read((char*)(&parentIdx), sizeof(parentIdx));
			res.dsAnimBone.pBone[bIdx].parentIdx = parentIdx;
			int childNum = 0;
			fs.read((char*)(&childNum), sizeof(childNum));
			res.dsAnimBone.pBone[bIdx].childNum = childNum;
			if (0 < childNum)
			{
				res.dsAnimBone.pBone[bIdx].pChildIdx = new int[childNum];
				fs.read((char*)(res.dsAnimBone.pBone[bIdx].pChildIdx), sizeof(int)*childNum);
			}
		}
	}

	{//anim
		unsigned long animNum;
		fs.read((char*)(&animNum), sizeof(animNum));
		res.dsAnimBone.an=animNum;
		res.dsAnimBone.pAnim = new DS_ANIM[animNum];

		//アニメーション
		for (unsigned long aIdx = 0; aIdx < animNum; ++aIdx)
		{
			unsigned long animNameSize;
			fs.read((char*)(&animNameSize), sizeof(animNameSize));
			res.dsAnimBone.pAnim[aIdx].animNameSize = animNameSize;
			res.dsAnimBone.pAnim[aIdx].animName = new char[animNameSize];

			fs.read(res.dsAnimBone.pAnim[aIdx].animName, animNameSize);

			unsigned long poseNum;
			fs.read((char*)(&poseNum), sizeof(poseNum));
			res.dsAnimBone.pAnim[aIdx].poseNum = poseNum;
			res.dsAnimBone.pAnim[aIdx].pose = new DS_ANIM_POSE[poseNum];
			
			//ポーズ
			for (unsigned long poseIdx = 0; poseIdx < poseNum; ++poseIdx)
			{
				{
					long keyFrameNum;
					fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTX = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTX = new DS_KEY_FRAME[keyFrameNum];

					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						float localTime;
						float value;
						fs.read((char*)(&localTime), sizeof(localTime));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTX[keyFrame].localTimeMs = localTime;
						fs.read((char*)(&value), sizeof(value));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTX[keyFrame].value = value;
					}
				}
				{
					long keyFrameNum;
					fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTY = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTY = new DS_KEY_FRAME[keyFrameNum];

					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						float localTime;
						float value;
						fs.read((char*)(&localTime), sizeof(localTime));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTY[keyFrame].localTimeMs = localTime;
						fs.read((char*)(&value), sizeof(value));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTY[keyFrame].value = value;
					}
				}
				{
					long keyFrameNum;
					fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumTZ = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTZ = new DS_KEY_FRAME[keyFrameNum];

					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						float localTime;
						float value;
						fs.read((char*)(&localTime), sizeof(localTime));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTZ[keyFrame].localTimeMs = localTime;
						fs.read((char*)(&value), sizeof(value));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameTZ[keyFrame].value = value;
					}
				}


				{
					long keyFrameNum;
					fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRX = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRX = new DS_KEY_FRAME[keyFrameNum];

					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						float localTime;
						float value;
						fs.read((char*)(&localTime), sizeof(localTime));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRX[keyFrame].localTimeMs = localTime;
						fs.read((char*)(&value), sizeof(value));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRX[keyFrame].value = value;
					}
				}
				{
					long keyFrameNum;
					fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRY = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRY = new DS_KEY_FRAME[keyFrameNum];

					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						float localTime;
						float value;
						fs.read((char*)(&localTime), sizeof(localTime));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRY[keyFrame].localTimeMs = localTime;
						fs.read((char*)(&value), sizeof(value));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRY[keyFrame].value = value;
					}
				}
				{
					long keyFrameNum;
					fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRZ = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRZ = new DS_KEY_FRAME[keyFrameNum];

					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						float localTime;
						float value;
						fs.read((char*)(&localTime), sizeof(localTime));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRZ[keyFrame].localTimeMs = localTime;
						fs.read((char*)(&value), sizeof(value));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRZ[keyFrame].value = value;
					}
				}
				{
					long keyFrameNum;
					fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumRQ = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRQ = new DS_KEY_FRAME[keyFrameNum];

					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						float localTime;
						float value;
						fs.read((char*)(&localTime), sizeof(localTime));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRQ[keyFrame].localTimeMs = localTime;
						fs.read((char*)(&value), sizeof(value));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameRQ[keyFrame].value = value;
					}
				}


				{
					long keyFrameNum;
					fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSX = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSX = new DS_KEY_FRAME[keyFrameNum];

					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						float localTime;
						float value;
						fs.read((char*)(&localTime), sizeof(localTime));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSX[keyFrame].localTimeMs = localTime;
						fs.read((char*)(&value), sizeof(value));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSX[keyFrame].value = value;
					}
				}
				{
					long keyFrameNum;
					fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSY = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSY = new DS_KEY_FRAME[keyFrameNum];

					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						float localTime;
						float value;
						fs.read((char*)(&localTime), sizeof(localTime));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSY[keyFrame].localTimeMs = localTime;
						fs.read((char*)(&value), sizeof(value));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSY[keyFrame].value = value;
					}
				}
				{
					long keyFrameNum;
					fs.read((char*)(&keyFrameNum), sizeof(keyFrameNum));
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameNumSZ = keyFrameNum;
					res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSZ = new DS_KEY_FRAME[keyFrameNum];

					for (long keyFrame = 0; keyFrame < keyFrameNum; ++keyFrame)
					{
						float localTime;
						float value;
						fs.read((char*)(&localTime), sizeof(localTime));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSZ[keyFrame].localTimeMs = localTime;
						fs.read((char*)(&value), sizeof(value));
						res.dsAnimBone.pAnim[aIdx].pose[poseIdx].keyFrameSZ[keyFrame].value = value;

					}
				}

			}

		}
	}




	return true;
}