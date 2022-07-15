#ifndef _DS_FORMAT_
#define _DS_FORMAT_

namespace MetaseqConverter
{
	struct DS_HEADER
	{
		unsigned long long ver;
		unsigned long long vertexOffset;
		unsigned long long faceOffset;
		unsigned long long dmyPolyOffset;
		unsigned long long materialOffset;
		unsigned long long boneOffset;
		unsigned long long animOffset;
		unsigned long long customPropertyRagdollOffset;
		unsigned char pad[64];
	};

	struct DS_VERTEX
	{
		union
		{
			float v[4];
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
		};

		static DS_VERTEX Set(const float m[4])
		{
			DS_VERTEX ret;
			ret.x = m[0]; ret.y = m[1]; ret.z = m[2]; ret.w = m[3];
			return ret;
		}

		static DS_VERTEX Normalize3(const DS_VERTEX& src)
		{
			const float len = src.Length3();
			DS_VERTEX ret;
			ret.x = src.x / len; ret.y = src.y / len; ret.z = src.z / len;
			ret.w = src.w;
			return ret;
		}

		float Length3() const
		{
			return sqrt(x*x + y*y + z*z);
		}

		unsigned long long GetSize() const
		{
			return sizeof(DS_VERTEX);
		}
	};


	struct DS_FACE
	{
		DS_FACE()
			: vn(0)
			, pIndex(0)
			, normal()
		{
		}

		~DS_FACE()
		{
			vn = 0;
			delete[] pIndex;
			pIndex = 0;
		}

		vari_size vn;
		union{
			uint64 indexOffset;
			vari_size* pIndex;
		};
		DS_VERTEX normal;


		unsigned long long GetSize() const
		{
			return (sizeof(vari_size) + normal.GetSize() + (vn*sizeof(vari_size)));
		}

	};

	struct DS_TEXTURE
	{
		struct UV
		{
			UV()
				:vertexIndex(-1)
			{
				uv[0] = 0;
				uv[1] = 0;
			}
			float uv[2];
			int vertexIndex;
		};

		struct UV_FACE
		{
			UV_FACE()
				: index()
				, refGeomFaceIndex(-1)
			{}
			std::vector<int> index;
			int refGeomFaceIndex;
		};

		DS_TEXTURE()
			: albedoTexPath()
			, normalTexPath()
			, specularTexPath()
			, uv()
			, uvFace()
		{	
		}

		std::string albedoTexPath;
		std::string normalTexPath;
		std::string specularTexPath;
		std::vector<UV> uv;
		std::vector<UV_FACE> uvFace;
	};


	struct DS_MATERIAL
	{
		DS_MATERIAL()
			//, ambient{}
			//, diffuse{}
			//, emissive{}
			//, specular{}
			: shininess(0)
			, materialParamId(0)
			, name()
			, texture()
		{
		}
	
		float ambient[3];
		float diffuse[3];
		float emissive[3];
		float specular[3];
		float shininess;
		int materialParamId;
		std::string name;
		std::vector<DS_TEXTURE> texture;
	};
	struct DS_MATERIAL_FORMAT//書き込み用構造体
	{
		DS_MATERIAL_FORMAT()
			//, ambient{}
			//, diffuse{}
			//, emissive{}
			//, specular{}
			: shininess(0)
			, materialParamId(0)
			, nameLen(0)
			, pName(NULL)
			, textureNum(0)
			, pTexture(NULL)
		{
		}
		float ambient[3];
		float diffuse[3];
		float emissive[3];
		float specular[3];
		float shininess;
		int materialParamId;
		int nameLen;
		union{
			uint64 nameOffset;
			char* pName;
		};
		int textureNum;
		union{
			uint64 textureOffset;
			DS_TEXTURE* pTexture;
		};
	};

	struct DS_DMYPOLY
	{
		vari_size vIndex[3];
		vari_size id;
	};

	struct DS_ANIM_MODEL
	{
		DS_ANIM_MODEL()
			: vn(0)
			, pVertex(0)
			, fn(0)
			, pFace(0)
			, mtr()
			, dmyPoly()
			, dataSize(sizeof(vari_size) * 2 + sizeof(vari_size))//vn + fn + mn + size　分
			, vertexSize(0)
			, faceSize(0)
			, meshInfo()
		{
		}
		
		~DS_ANIM_MODEL()
		{
			vn = 0;
			delete[] pVertex;
			pVertex = 0;
			fn = 0;
			delete[] pFace;
			pFace = 0;
		}

		vari_size vn;
		DS_VERTEX* pVertex;

		vari_size fn;
		DS_FACE* pFace;

		std::vector<DS_MATERIAL> mtr;

		std::vector<DS_DMYPOLY> dmyPoly;

		unsigned long long dataSize;		//保存対象の全てのサイズ
		unsigned long long vertexSize;	//vertexのみのサイズ
		unsigned long long faceSize;		//faceのみのサイズ

		struct Mesh
		{
			std::string name;
			int vertexNum;
		};
		std::vector<Mesh> meshInfo;

	};

	struct DS_MAT
	{
		enum{ SIZE = 64 };

		DS_MAT(){}

		DS_MAT& operator =(const double src[4][4])
		{
			for (int col = 0; col < 4; ++col)
			{
				for (int row = 0; row < 4; ++row)
				{
					m[col][row] = static_cast<float>(src[col][row]);
				}
			}
			return (*this);
		}

		DS_MAT operator *(const DS_MAT& src) const
		{
			DS_MAT dst;
			for (int col = 0; col < 4; ++col)
			{
				for (int row = 0; row < 4; ++row)
				{
					dst.m[col][row] = m[col][0] * src.m[0][row] + m[col][1] * src.m[1][row] + m[col][2] * src.m[2][row] + m[col][3] * src.m[3][row];
				}
			}	
			return dst;
		}

		DS_VERTEX operator *(const DS_VERTEX& src) const
		{
			DS_VERTEX dst;
			dst.x = src.x*m[0][0] + src.y*m[0][1] + src.z*m[0][2] + src.w*m[0][3];
			dst.y = src.x*m[1][0] + src.y*m[1][1] + src.z*m[1][2] + src.w*m[1][3];
			dst.z = src.x*m[2][0] + src.y*m[2][1] + src.z*m[2][2] + src.w*m[2][3];
			dst.w = src.x*m[3][0] + src.y*m[3][1] + src.z*m[3][2] + src.w*m[3][3];
			return dst;
		}

		static DS_MAT T(const DS_MAT& src)
		{
			DS_MAT dst;
			for (int col = 0; col < 4; ++col)
			{
				for (int row = 0; row < 4; ++row)
				{
					dst.m[col][row] = src.m[row][col];
				}
			}
			return dst;
		}

		float m[4][4];
	};

	struct DS_BONE
	{
		DS_BONE()
			: indexNum(0)
			, pIndex(0)
			, pWeight(0)
			, initMat()
			, nameSize(0)
			, name(0)
			, parentIdx(-1)
			, childNum(0)
			, pChildIdx(0)
			, isMaster(0)
		{}

		~DS_BONE()
		{
			indexNum = 0;
			delete[] pIndex;
			pIndex = 0;
			delete[] pWeight;
			pWeight = 0;

			delete[] name;
			name = 0;
			nameSize = 0;

			delete[] pChildIdx;
			pChildIdx = 0;
		}

		DS_BONE& operator =(const DS_BONE& src)
		{
			indexNum = src.indexNum;
			delete[] pIndex; pIndex = 0;
			delete[] pWeight; pWeight = 0;
			if(0<indexNum)pIndex = new vari_size[indexNum];
			if(0<indexNum)pWeight = new float[indexNum];
			for (int i = 0; i < src.indexNum; ++i)
			{
				pIndex[i] = src.pIndex[i];
				pWeight[i] = src.pWeight[i];
			}
			
			initMat = src.initMat;
			nameSize = src.nameSize;
			delete[] name; name = 0;
			if(0 < nameSize)name = new char[nameSize];
			for (size_t i = 0; i < nameSize; ++i)
			{
				name[i] = src.name[i];
			}

			parentIdx = src.parentIdx;
			childNum = src.childNum;
			delete[] pChildIdx; pChildIdx = 0;
			if (0<childNum)pChildIdx = new int[childNum];
			for (int i = 0; i < childNum; ++i)
			{
				pChildIdx[i] = src.pChildIdx[i];
			}

			isMaster = src.isMaster;

			return (*this);
		}

		int indexNum;
		vari_size* pIndex;
		float* pWeight;

		DS_MAT initMat;

		size_t nameSize;
		char* name;

		int parentIdx;
		int childNum;
		int* pChildIdx;

		unsigned char isMaster;
	};

	struct DS_KEY_FRAME
	{
		DS_KEY_FRAME()
			:localTimeMs(0)
			,value(0)
		{}
		float localTimeMs;
		float value;
	};



	struct DS_ANIM_POSE
	{
		DS_ANIM_POSE()
			: keyFrameNumTX(0)
			, keyFrameTX(0)
			, keyFrameNumTY(0)
			, keyFrameTY(0)
			, keyFrameNumTZ(0)
			, keyFrameTZ(0)

			, keyFrameNumRX(0)
			, keyFrameRX(0)
			, keyFrameNumRY(0)
			, keyFrameRY(0)
			, keyFrameNumRZ(0)
			, keyFrameRZ(0)
			, keyFrameNumRQ(0)
			, keyFrameRQ(0)

			, keyFrameNumSX(0)
			, keyFrameSX(0)
			, keyFrameNumSY(0)
			, keyFrameSY(0)
			, keyFrameNumSZ(0)
			, keyFrameSZ(0)

			//データには残らないけど変換に必要なもの
			, isMaster(false)
		{
		}

		~DS_ANIM_POSE()
		{ 
			delete[] keyFrameTX;
			keyFrameTX = 0;
			delete[] keyFrameTY;
			keyFrameTY = 0;
			delete[] keyFrameTZ;
			keyFrameTZ = 0;

			delete[] keyFrameRX;
			keyFrameRX = 0;
			delete[] keyFrameRY;
			keyFrameRY = 0;
			delete[] keyFrameRZ;
			keyFrameRZ = 0;
			delete[] keyFrameRQ;
			keyFrameRQ = 0;

			delete[] keyFrameSX;
			keyFrameSX = 0;
			delete[] keyFrameSY;
			keyFrameSY = 0;
			delete[] keyFrameSZ;
			keyFrameSZ = 0;
		}

		long keyFrameNumTX;
		DS_KEY_FRAME* keyFrameTX;

		long keyFrameNumTY;
		DS_KEY_FRAME* keyFrameTY;

		long keyFrameNumTZ;
		DS_KEY_FRAME* keyFrameTZ;

		long keyFrameNumRX;
		DS_KEY_FRAME* keyFrameRX;

		long keyFrameNumRY;
		DS_KEY_FRAME* keyFrameRY;

		long keyFrameNumRZ;
		DS_KEY_FRAME* keyFrameRZ;

		long keyFrameNumRQ;
		DS_KEY_FRAME* keyFrameRQ;

		long keyFrameNumSX;
		DS_KEY_FRAME* keyFrameSX;

		long keyFrameNumSY;
		DS_KEY_FRAME* keyFrameSY;

		long keyFrameNumSZ;
		DS_KEY_FRAME* keyFrameSZ;


		//データには残らないけど変換に必要なもの
		bool isMaster;
	};

	struct DS_ANIM
	{
		DS_ANIM()
			: animNameSize(0)
			, animName(0)
			, poseNum(0)
			, pose(0)
			, masterMove()
		{
		}

		void Init(int pn)
		{
			poseNum = pn;
			pose = new DS_ANIM_POSE[poseNum];
		}


		~DS_ANIM()
		{
			delete[] animName;
			animName = 0;
			delete[] pose;
			pose = 0;
		}

		size_t animNameSize;
		char* animName;

		long poseNum;
		DS_ANIM_POSE* pose;

		DS_ANIM_POSE masterMove;
	};

	struct DS_ANIM_BONE
	{
		DS_ANIM_BONE()
			: bn(0)
			, pBone(0)
			, an(0)
			, pAnim(0)
		{}

		~DS_ANIM_BONE()
		{
			bn = 0;
			delete[] pBone;
			pBone = 0;
			an = 0;
			delete[] pAnim;
			pAnim = 0;
		}

		long bn;
		DS_BONE* pBone;

		long an;
		DS_ANIM* pAnim;
	};

	struct DS_RAGDOLL_PARAM_ID
	{
		DS_RAGDOLL_PARAM_ID()
			: id(0)
			, boneIdx(0)
		{}
		int id;
		unsigned long boneIdx;
	};

	struct DS_CUSTOM_PROPERTY
	{
		DS_CUSTOM_PROPERTY()
			: ragdoll()
		{}
		std::vector<DS_RAGDOLL_PARAM_ID> ragdoll;
		unsigned long GetDataSize() const{ return ragdoll.size(); }

	};

	struct DS_LIGHT
	{
		DS_LIGHT()
			: type(0)
			, distance(0)
			, energy(0)
			//, color{0}
		{}
		int type;
		float distance;
		float energy;
		float color[3];
	};

	struct DS_LIGHT_DATA
	{
		std::vector<DS_LIGHT> lights;
	};
}
#endif