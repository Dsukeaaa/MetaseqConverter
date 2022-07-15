#include "MetPch.h"
#include "Converter/ConvertFormat.h" 

#ifndef _DS_FORMAT_
#include "File/DsFormat.h"
#endif
#ifndef _OUTPUTRES_
#include "Res/OutputRes.h"
#endif

using namespace MetaseqConverter;

#define RadToDeg(a)(a*static_cast<float>(180.0f/M_PI))
#define DegToRad(a)(a*static_cast<float>(M_PI/180.0f))





Mat33::Mat33(const Quaternion& q)
{
	const float qx = q.qx;
	const float qy = q.qy;
	const float qz = q.qz;
	const float qw = q.qw;

	m[0][0] = 1.0f - 2.0f * qy * qy - 2.0f * qz * qz;
	m[0][1] = 2.0f * qx * qy + 2.0f * qw * qz;
	m[0][2] = 2.0f * qx * qz - 2.0f * qw * qy;

	m[1][0] = 2.0f * qx * qy - 2.0f * qw * qz;
	m[1][1] = 1.0f - 2.0f * qx * qx - 2.0f * qz * qz;
	m[1][2] = 2.0f * qy * qz + 2.0f * qw * qx;

	m[2][0] = 2.0f * qx * qz + 2.0f * qw * qy;
	m[2][1] = 2.0f * qy * qz - 2.0f * qw * qx;
	m[2][2] = 1.0f - 2.0f * qx * qx - 2.0f * qy * qy;
}

Quaternion::Quaternion(const Mat33& mat)
{
	_Conv1(mat);
	//_Conv2(mat);
}

void Quaternion::_Conv1(const Mat33& mat)
{
	// 最大成分を検索
	float elem[4]; // 0:x, 1:y, 2:z, 3:w
	elem[0] = mat.m[0][0] - mat.m[1][1] - mat.m[2][2] + 1.0f;
	elem[1] = -mat.m[0][0] + mat.m[1][1] - mat.m[2][2] + 1.0f;
	elem[2] = -mat.m[0][0] - mat.m[1][1] + mat.m[2][2] + 1.0f;
	elem[3] = mat.m[0][0] + mat.m[1][1] + mat.m[2][2] + 1.0f;

	unsigned biggestIndex = 0;
	for (int i = 1; i < 4; i++) {
		if (elem[i] > elem[biggestIndex])
			biggestIndex = i;
	}

	if (elem[biggestIndex] < 0.0f)
	{
		return; // 引数の行列に間違いあり！
	}

	// 最大要素の値を算出
	float *q[4] = { &qx, &qy, &qz, &qw };
	float v = sqrtf(elem[biggestIndex]) * 0.5f;
	*q[biggestIndex] = v;
	float mult = 0.25f / v;

	switch (biggestIndex) {
	case 0: // x
		*q[1] = (mat.m[0][1] + mat.m[1][0]) * mult;
		*q[2] = (mat.m[2][0] + mat.m[0][2]) * mult;
		*q[3] = (mat.m[1][2] - mat.m[2][1]) * mult;
		break;
	case 1: // y
		*q[0] = (mat.m[0][1] + mat.m[1][0]) * mult;
		*q[2] = (mat.m[1][2] + mat.m[2][1]) * mult;
		*q[3] = (mat.m[2][0] - mat.m[0][2]) * mult;
		break;
	case 2: // z
		*q[0] = (mat.m[2][0] + mat.m[0][2]) * mult;
		*q[1] = (mat.m[1][2] + mat.m[2][1]) * mult;
		*q[3] = (mat.m[0][1] - mat.m[1][0]) * mult;
		break;
	case 3: // w
		*q[0] = (mat.m[1][2] - mat.m[2][1]) * mult;
		*q[1] = (mat.m[2][0] - mat.m[0][2]) * mult;
		*q[2] = (mat.m[0][1] - mat.m[1][0]) * mult;
		break;
	}
}

void Quaternion::_Conv2(const Mat33& mat)
{
	float q0 = (mat.m[0][0] + mat.m[1][1] + mat.m[2][3] + 1.0f) / 4.0f;
	float q1 = (mat.m[0][0] - mat.m[1][1] - mat.m[2][3] + 1.0f) / 4.0f;
	float q2 = (-mat.m[0][0] + mat.m[1][1] - mat.m[2][3] + 1.0f) / 4.0f;
	float q3 = (-mat.m[0][0] - mat.m[1][1] + mat.m[2][3] + 1.0f) / 4.0f;
	if (q0 < 0.0f) q0 = 0.0f;
	if (q1 < 0.0f) q1 = 0.0f;
	if (q2 < 0.0f) q2 = 0.0f;
	if (q3 < 0.0f) q3 = 0.0f;
	q0 = sqrt(q0);
	q1 = sqrt(q1);
	q2 = sqrt(q2);
	q3 = sqrt(q3);
	if (q0 >= q1 && q0 >= q2 && q0 >= q3) {
		q0 *= +1.0f;
		q1 *= SIGN(mat.m[2][1] - mat.m[1][2]);
		q2 *= SIGN(mat.m[0][2] - mat.m[2][0]);
		q3 *= SIGN(mat.m[1][0] - mat.m[0][1]);
	}
	else if (q1 >= q0 && q1 >= q2 && q1 >= q3) {
		q0 *= SIGN(mat.m[2][1] - mat.m[1][2]);
		q1 *= +1.0f;
		q2 *= SIGN(mat.m[1][0] + mat.m[0][1]);
		q3 *= SIGN(mat.m[0][2] + mat.m[2][0]);
	}
	else if (q2 >= q0 && q2 >= q1 && q2 >= q3) {
		q0 *= SIGN(mat.m[0][2] - mat.m[2][0]);
		q1 *= SIGN(mat.m[1][0] + mat.m[0][1]);
		q2 *= +1.0f;
		q3 *= SIGN(mat.m[2][1] + mat.m[1][2]);
	}
	else if (q3 >= q0 && q3 >= q1 && q3 >= q2) {
		q0 *= SIGN(mat.m[1][0] - mat.m[0][1]);
		q1 *= SIGN(mat.m[2][0] + mat.m[0][2]);
		q2 *= SIGN(mat.m[2][1] + mat.m[1][2]);
		q3 *= +1.0f;
	}
	else {
		printf("coding error\n");
	}
	float r = NORM(q0, q1, q2, q3);
	q0 /= r;
	q1 /= r;
	q2 /= r;
	q3 /= r;

	qw = q0;
	qx = q1;
	qy = q2;
	qz = q3;
}

namespace
{
	DS_KEY_FRAME* _Reallocate(int index, DS_KEY_FRAME val, const DS_KEY_FRAME* src, int srcNum)
	{
		int newNum = srcNum + 1;
		DS_KEY_FRAME* newKeyFrame = new DS_KEY_FRAME[newNum];

		for (int i = 0, oldIdx = 0; i < newNum; ++i)
		{
			if (i == index)
			{
				newKeyFrame[i] = val;
			}
			else
			{
				newKeyFrame[i] = src[oldIdx];
				++oldIdx;
			}
		}

		delete[] src;
		return newKeyFrame;
	}


}

namespace
{
	struct RearrangeResult
	{
		RearrangeResult() :pX(NULL), xn(0), pY(NULL), yn(0), pZ(NULL), zn(0)
		{}
		DS_KEY_FRAME *pX;
		int xn;
		DS_KEY_FRAME* pY;
		int yn;
		DS_KEY_FRAME* pZ;
		int zn;
	};

	RearrangeResult _Rearrange(DS_KEY_FRAME* pX, int xn, DS_KEY_FRAME* pY, int yn, DS_KEY_FRAME* pZ, int zn, int MaxFrameNum)
	{
		//キーフレーム数を一致させる
		for (int keyFrameIdx = 0; keyFrameIdx < MaxFrameNum; ++keyFrameIdx)
		{
			DS_KEY_FRAME msx;
			if (keyFrameIdx < xn)
			{
				msx = pX[keyFrameIdx];
			}
			else
			{
				const int index = xn - 1;
				msx = pX[index];
			}

			DS_KEY_FRAME msy;
			if (keyFrameIdx < yn)
			{
				msy = pY[keyFrameIdx];
			}
			else
			{
				const int index = yn - 1;
				msy = pY[index];
			}

			DS_KEY_FRAME msz;
			if (keyFrameIdx < zn)
			{
				msz = pZ[keyFrameIdx];
			}
			else
			{
				const int index = zn - 1;
				msz = pZ[index];
			}

			bool isInsert = true;
			//成分間で時間に差がないか調べる
			float diff = msx.localTimeMs - msy.localTimeMs;
			if (fabs(diff) < 0.00001f)
			{
				diff = msx.localTimeMs - msz.localTimeMs;
				if (fabs(diff) < 0.00001f)
				{
					isInsert = false;
				}
			}

			//一番最初のフレームは時間が揃ってる前提なので揃える処理はしない
			if (keyFrameIdx == 0)
			{
				isInsert = false;
			}

			//差があったので、一番時間が小さい成分を探して、他の２つを１つ前の成分でコピーする

			/*
			0 0 0
			1 1 1
			1 2 2
			2
			↓

			0 0 0
			1 1 1
			1 2 2
			2 2 2

			//データを見てみると、キーフレームが欠けてる成分がちょいちょいある模様

			*/

			if (isInsert)
			{
				if (msx.localTimeMs < msy.localTimeMs)
				{
					if (msx.localTimeMs < msz.localTimeMs)
					{
						//xが一番小さい

						if (0.00001f < fabs(msx.localTimeMs - msy.localTimeMs))
						{
							DS_KEY_FRAME insertY;
							insertY.localTimeMs = msx.localTimeMs;
							insertY.value = pY[keyFrameIdx - 1].value;
							pY =
								_Reallocate(keyFrameIdx, insertY, pY, yn);
							++yn;
						}

						if (0.00001f < fabs(msx.localTimeMs - msz.localTimeMs))
						{
							DS_KEY_FRAME insertZ;
							insertZ.localTimeMs = msx.localTimeMs;
							insertZ.value = pZ[keyFrameIdx - 1].value;
							pZ =
								_Reallocate(keyFrameIdx, insertZ, pZ, zn);
							++zn;
						}
					}
					else
					{
						//ｚが一番小さい

						if (0.00001f < fabs(msz.localTimeMs - msx.localTimeMs))
						{
							DS_KEY_FRAME insertX;
							insertX.localTimeMs = msz.localTimeMs;
							insertX.value = pX[keyFrameIdx - 1].value;
							pX =
								_Reallocate(keyFrameIdx, insertX, pX, xn);
							++xn;
						}

						if (0.00001f < fabs(msz.localTimeMs - msy.localTimeMs))
						{
							DS_KEY_FRAME insertY;
							insertY.localTimeMs = msz.localTimeMs;
							insertY.value = pY[keyFrameIdx - 1].value;
							pY =
								_Reallocate(keyFrameIdx, insertY, pY, yn);
							++yn;
						}
					}
				}
				else
				{
					if (msy.localTimeMs < msz.localTimeMs)
					{
						//yが一番小さい

						if (0.00001f < fabs(msy.localTimeMs - msx.localTimeMs))
						{
							DS_KEY_FRAME insertX;
							insertX.localTimeMs = msy.localTimeMs;
							insertX.value = pX[keyFrameIdx - 1].value;
							pX =
								_Reallocate(keyFrameIdx, insertX, pX, xn);
							++xn;
						}

						if (0.00001f < fabs(msy.localTimeMs - msz.localTimeMs))
						{
							DS_KEY_FRAME insertZ;
							insertZ.localTimeMs = msy.localTimeMs;
							insertZ.value = pZ[keyFrameIdx - 1].value;
							pZ =
								_Reallocate(keyFrameIdx, insertZ, pZ, zn);
							++zn;
						}
					}
					else
					{
						//zが一番小さい

						if (0.00001f < fabs(msz.localTimeMs - msx.localTimeMs))
						{
							DS_KEY_FRAME insertX;
							insertX.localTimeMs = msz.localTimeMs;
							insertX.value = pX[keyFrameIdx - 1].value;
							pX =
								_Reallocate(keyFrameIdx, insertX, pX, xn);
							++xn;
						}

						if (0.00001f < fabs(msz.localTimeMs - msy.localTimeMs))
						{
							DS_KEY_FRAME insertY;
							insertY.localTimeMs = msz.localTimeMs;
							insertY.value = pY[keyFrameIdx - 1].value;
							pY =
								_Reallocate(keyFrameIdx, insertY, pY, yn);
							++yn;
						}
					}
				}
			}

			MaxFrameNum = std::max(std::max(xn, yn), zn);
		}

		if ((xn != yn) ||
			(yn != zn))
		{
			printf("キーフレーム数が一致してない");
			abort();
		}

		RearrangeResult ret;
		ret.pX = pX;
		ret.xn = xn;
		ret.pY = pY;
		ret.yn = yn;
		ret.pZ = pZ;
		ret.zn = zn;
		return ret;
	}
}

namespace
{
	Quaternion _GetQuaternion(const DS_ANIM_POSE& pose, int keyFrameIdx)
	{
		const float rx = pose.keyFrameRX[keyFrameIdx].value;
		const float ry = pose.keyFrameRY[keyFrameIdx].value;
		const float rz = pose.keyFrameRZ[keyFrameIdx].value;

		Mat33 r = Mat33::RotX(DegToRad(rx));
		r = Mat33::RotY(DegToRad(ry))*r;
		r = Mat33::RotZ(DegToRad(rz))*r;

		const Quaternion q(r);

		{//デバッグ
			const Mat33 check = Mat33(q);
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					float e = fabs(r.m[i][j] - check.m[i][j]);
					if (e > 0.0001f)
					{
						printf("クォータニオン→回転行列変換失敗");
						assert(false);
					}
				}
			}
		}

		return q;
	}
}

void MetaseqConverter::ConvertFormat( OutputRes& src)
{
	for (int animIdx = 0; animIdx < src.dsAnimBone.an; ++animIdx)
	{
		const char* animName = src.dsAnimBone.pAnim[animIdx].animName;

		auto func = [](DS_ANIM_POSE& pose)
		{
			//xyz成分のキーフレーム数を揃える
			{
				int MaxFrameNum = std::max(std::max(pose.keyFrameNumTX, pose.keyFrameNumTY), pose.keyFrameNumTZ);
				RearrangeResult T = _Rearrange(pose.keyFrameTX, pose.keyFrameNumTX, pose.keyFrameTY, pose.keyFrameNumTY, pose.keyFrameTZ, pose.keyFrameNumTZ, MaxFrameNum);
				pose.keyFrameTX = T.pX;
				pose.keyFrameNumTX = T.xn;
				pose.keyFrameTY = T.pY;
				pose.keyFrameNumTY = T.yn;
				pose.keyFrameTZ = T.pZ;
				pose.keyFrameNumTZ = T.zn;
			}
			{
				int MaxFrameNum = std::max(std::max(pose.keyFrameNumRX, pose.keyFrameNumRY), pose.keyFrameNumRZ);
				RearrangeResult R = _Rearrange(pose.keyFrameRX, pose.keyFrameNumRX, pose.keyFrameRY, pose.keyFrameNumRY, pose.keyFrameRZ, pose.keyFrameNumRZ, MaxFrameNum);
				pose.keyFrameRX = R.pX;
				pose.keyFrameNumRX = R.xn;
				pose.keyFrameRY = R.pY;
				pose.keyFrameNumRY = R.yn;
				pose.keyFrameRZ = R.pZ;
				pose.keyFrameNumRZ = R.zn;


				//クォータニオンに変換
				pose.keyFrameRQ = new DS_KEY_FRAME[R.zn];
				pose.keyFrameNumRQ = R.zn;
				for (int keyFrameIdx = 0; keyFrameIdx < R.zn; ++keyFrameIdx)
				{
					const Quaternion q = _GetQuaternion(pose, keyFrameIdx);

					pose.keyFrameRX[keyFrameIdx].value = q.qx;
					pose.keyFrameRY[keyFrameIdx].value = q.qy;
					pose.keyFrameRZ[keyFrameIdx].value = q.qz;
					pose.keyFrameRQ[keyFrameIdx].value = q.qw;
					pose.keyFrameRQ[keyFrameIdx].localTimeMs = pose.keyFrameRX[keyFrameIdx].localTimeMs;
				}
			}
			{
				int MaxFrameNum = std::max(std::max(pose.keyFrameNumSX, pose.keyFrameNumSY), pose.keyFrameNumSZ);
				RearrangeResult S = _Rearrange(pose.keyFrameSX, pose.keyFrameNumSX, pose.keyFrameSY, pose.keyFrameNumSY, pose.keyFrameSZ, pose.keyFrameNumSZ, MaxFrameNum);
				pose.keyFrameSX = S.pX;
				pose.keyFrameNumSX = S.xn;
				pose.keyFrameSY = S.pY;
				pose.keyFrameNumSY = S.yn;
				pose.keyFrameSZ = S.pZ;
				pose.keyFrameNumSZ = S.zn;
			}
		};

		for (int boneIdx = 0; boneIdx < src.dsAnimBone.pAnim[animIdx].poseNum; ++boneIdx)
		{			
			DS_ANIM_POSE& pose = src.dsAnimBone.pAnim[animIdx].pose[boneIdx];
			func(pose);
		}

		//マスター移動量
		DS_ANIM_POSE& masterPose = src.dsAnimBone.pAnim[animIdx].masterMove;
		func(masterPose);



		//マスターボーン座標系に変換
		{
			//指定時間から位置を取得
			auto getTranslate = [](const DS_ANIM_POSE& pose, double time) -> DS_VERTEX
			{
				DS_VERTEX ret;
				const int kn = pose.keyFrameNumTX;
				for (int i = 0; i < kn; ++i)//キーフレーム数は上記で成分全部一致している前提
				{
					if (pose.keyFrameTX[i].localTimeMs <= time)
					{
						ret.x = pose.keyFrameTX[i].value;
						ret.y = pose.keyFrameTY[i].value;
						ret.z = pose.keyFrameTZ[i].value;
					}
					else
					{
						return ret;//時間外
					}
				}
				return ret;
			};


			//指定時間から回転を取得
			auto getRotation = [](const DS_ANIM_POSE& pose, double time) -> DS_VERTEX
			{
				DS_VERTEX ret;
				const int kn = pose.keyFrameNumRX;
				for (int i = 0; i < kn; ++i)//キーフレーム数は上記で成分全部一致している前提
				{
					if (pose.keyFrameTX[i].localTimeMs <= time)
					{
						ret.x = pose.keyFrameRX[i].value;
						ret.y = pose.keyFrameRY[i].value;
						ret.z = pose.keyFrameRZ[i].value;
						ret.w = pose.keyFrameRQ[i].value;
					}
					else
					{
						return ret;//時間外
					}
				}
				return ret;
			};

			const DS_ANIM_POSE* pMaster = NULL;
			for (int boneIdx = 0; boneIdx < src.dsAnimBone.pAnim[animIdx].poseNum; ++boneIdx)
			{
				const DS_ANIM_POSE& pose = src.dsAnimBone.pAnim[animIdx].pose[boneIdx];
				if (pose.isMaster)
				{
					pMaster = &pose;
				}
			}

			for (int boneIdx = 0; boneIdx < src.dsAnimBone.pAnim[animIdx].poseNum; ++boneIdx)
			{
				DS_ANIM_POSE& pose = src.dsAnimBone.pAnim[animIdx].pose[boneIdx];
				const DS_BONE& bone = src.dsAnimBone.pBone[boneIdx];

				if ((!bone.isMaster) && pMaster)
				{
					//関節の相対座標になっているのでルートだけマスター座標に変換すればいい
					if (-1 == bone.parentIdx)
					{
						if (0 < pMaster->keyFrameNumTX)//マスター移動量があるときだけ
						{
							const int tn = pose.keyFrameNumTX;
							for (int i = 0; i < tn; ++i)
							{
								const float curTime = pose.keyFrameTX[i].localTimeMs;
								const DS_VERTEX pos = getTranslate(*pMaster, curTime);
								pose.keyFrameTX[i].value -= pos.x;
								pose.keyFrameTY[i].value -= pos.y;
								pose.keyFrameTZ[i].value -= pos.z;
							}
						}

						//ブレブレ。まだ実装中だけど、使わないので放置中
						//if (0 < pMaster->keyFrameNumRX)//マスター移動量があるときだけ
						//{
						//	const int rn = pose.keyFrameNumRX;
						//	for (int i = 0; i < rn; ++i)
						//	{
						//		const float curTime = pose.keyFrameRX[i].localTimeMs;
						//		const DS_VERTEX v = getRotation(*pMaster, curTime);
						//		const Quaternion masterQ(v);
						//		Quaternion q;
						//		q.qx = pose.keyFrameRX[i].value;
						//		q.qy = pose.keyFrameRY[i].value;
						//		q.qz = pose.keyFrameRZ[i].value;
						//		q.qw = pose.keyFrameRQ[i].value;
						//		q = Quaternion::Inverse(masterQ)*q;
						//		pose.keyFrameRX[i].value = q.qx;
						//		pose.keyFrameRY[i].value = q.qy;
						//		pose.keyFrameRZ[i].value = q.qz;
						//		pose.keyFrameRQ[i].value = q.qw;
						//	}
						//}
					}
				}
			}
		}
	}
}
	