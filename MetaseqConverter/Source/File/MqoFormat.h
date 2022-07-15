#ifndef _MQO_FORMAT_
#define _MQO_FORMAT_

namespace MetaseqConverter
{
	struct MQO_HEADER
	{
		bool isText;
		int ver;
	};

	struct MQO_MATERIAL
	{
		MQO_MATERIAL()
		{
			name[0] = '\0';
			texPath[0] = '\0';
			aplanePath[0] = '\0';
			bunpPath[0] = '\0';
		}
		char name[32];
		float color[4];
		float dif;
		float amb;
		float emi;
		float spc;
		float power;
		char texPath[1024];
		char aplanePath[1024];
		char bunpPath[1024];
	};

	struct MQO_VERTEX
	{
		float v[3];
	};

	struct MQO_FACE
	{
		int vn;
		int vIdx[4];
		int mIdx;
		float uv[8];
		int color[3];
	};

	struct MQO_OBJECT
	{
		MQO_OBJECT():vertex(NULL), face(NULL)
		{}
		char name[64];
		float scale[3];
		float rotation[3];
		float translation[3];
		int visible;
		float color[3];
		int vn;
		int fn;
		MQO_VERTEX* vertex;
		MQO_FACE* face;
	};

	
}

#endif