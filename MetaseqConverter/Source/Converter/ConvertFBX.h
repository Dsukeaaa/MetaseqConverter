#ifndef _CONVERT_FBX_
#define _CONVERT_FBX_
#include <fbxsdk.h>

#ifndef _DS_FORMAT_
#include "File/DsFormat.h"
#endif

namespace MetaseqConverter
{
	class OutputRes;
}

namespace MetaseqConverter
{
	class ConvertFBX
	{
	private:
		enum CurvePoseType
		{
			POS_X,
			POS_Y,
			POS_Z,
			
			ROT_X,
			ROT_Y,
			ROT_Z,

			SCL_X,
			SCL_Y,
			SCL_Z,
		};

	public:
		ConvertFBX();
		virtual ~ConvertFBX();

	public:
		void Convert(const char* path, OutputRes& res);

	private:
		void _AccessNode(FbxNode* pNode, DS_ANIM_MODEL& model, DS_ANIM_BONE& dsAnimBone, DS_LIGHT_DATA& light);
		void _AccessMesh(FbxMesh *pMesh, DS_ANIM_MODEL& dsAnimModel, DS_MAT offset);
		void _AccessBone(FbxMesh *pMesh, DS_ANIM_BONE& dsAnimBone, DS_ANIM_MODEL& dsAnimModel, DS_MAT offset);
		void _SetChildBone(FbxNode *pNode, DS_ANIM_BONE& dsAnimBone, int parentIdx, DS_BONE* pParentBone, DS_CUSTOM_PROPERTY& ragdollParam);

	private:
		void _AceessAnim(FbxScene* pScene, DS_ANIM_BONE& dsAnimBone);
		void _AceessAnimLayer(FbxAnimLayer* pAnimLayer, FbxNode* pNode, DS_ANIM& dsAnim, DS_ANIM_BONE& dsAnimBone);
		void _AceessAnimCurve(FbxAnimCurve* pCurve, FbxNode* pNode, DS_ANIM& dsAnim, DS_ANIM_BONE& dsAnimBone, CurvePoseType type);
		bool _IsBoneAnimLayer(FbxAnimLayer* pAnimLayer, FbxNode* pNode) const;

	private:
		void _WeightNormalize(const DS_ANIM_MODEL& dsAnimModel, DS_ANIM_BONE& dsAnimBone);
		void _FbxCoordTrans(DS_ANIM_BONE& dsAnimBone);
		void _BorneScaleCalc(DS_ANIM_BONE& dsAnimBone);
	};
}

#endif

