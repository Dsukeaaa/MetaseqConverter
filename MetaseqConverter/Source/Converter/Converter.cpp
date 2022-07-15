#include "MetPch.h"

#ifndef _CONVERTER_
#include "Converter.h"
#endif

#ifndef _FILE_RES_
#include "../File/FileRes.h"
#endif

using namespace MetaseqConverter;

OutputRes Converter::Convert(const FileRes& res)
{
	const char* curDataPtr = res.GetData(); 
	const vari_size dataSize = res.GetSize();
	vari_size sizeCount = 0;

	MQO_HEADER head;
	std::vector<MQO_OBJECT> objects;
	std::vector<MQO_MATERIAL> mtls;

	curDataPtr = _SetHeader(&head, curDataPtr);
	
	const char* cur = curDataPtr;
	while (1)
	{
		char buffer[1024] = "";
		const vari_size len = _GetLine(buffer, cur);

		if (strstr(buffer, "Object"))
		{
			cur = _SetObject(objects, cur);
		}
		else if (strstr(buffer, "Material "))
		{
			cur = _SetMatrial(mtls, cur);
		}
		else
		{
			cur += (len + 1);
		}

		
		sizeCount = curDataPtr - cur;

		if (strstr(buffer, "Eof") || (0 >= sizeCount))
		{
			break;
		}
	}

	return OutputRes();
}

vari_size Converter::_GetLine(char* pBuffer, const char* src) const
{
	const vari_size len = strstr(src, "\n") - src;
	for (vari_size i = 0; i < len; ++i)
	{
		pBuffer[i] = src[i];
	}
	pBuffer[len + 1] = '\0';
	return len;
}

vari_size Converter::_GetLastStr(const char* buffer, const vari_size size, char key) const
{
	vari_size pos = size;
	for (vari_size i = 0; i < size; ++i)
	{
		if (key != buffer[i])
		{
			pos = i;
			break;
		}
	}
	return pos;
}

const char* Converter::_SetHeader(MQO_HEADER* pHeader, const char* data)
{
	//Metasequoia Document
	 const char* doc = strstr(data, "\n") + 1;

	//Format Text Ver 1.1
	 char format[256]="";
	 float ver;
	 sscanf_s(doc, "Format %s Ver %f", format, sizeof(format), &ver);
	 const char* next = strstr(doc, "\n") + 1;

	 pHeader->isText = (0!=strstr(format, "Text"));
	 pHeader->ver = static_cast<int>(ver*10.0f);

	 return next;
}

const char* Converter::_SetObject(std::vector<MQO_OBJECT>& objects, const char* dataPtr)
{
	MQO_OBJECT obj;

	const char* cur = dataPtr;
	while (1)
	{
		char buffer[1024]="";
		const vari_size len = _GetLine(buffer, cur);

		if (strstr(buffer, "}"))
		{
			const vari_size nextLine = strstr(cur, "\n") - cur;
			cur += (nextLine + 1);//}の次の行を返す
			objects.push_back(obj);
			return cur;
		}
		else if (strstr(buffer, "Object "))
		{
			vari_size pos = _GetLastStr(buffer, len, '\t');
			sscanf_s(buffer + pos, "Object %s {", obj.name, sizeof(obj.name));
			cur += (len + 1);
		}
		else if (strstr(buffer, "scale "))
		{
			float scale[3];
			vari_size pos = _GetLastStr(buffer, len, '\t');
			sscanf_s(buffer+pos, "scale %f %f %f", &scale[0], &scale[1], &scale[2]);
			obj.scale[0] = scale[0];
			obj.scale[1] = scale[1];
			obj.scale[2] = scale[2];
			cur += (len + 1);
		}
		else if (strstr(buffer, "rotation "))
		{
			float rot[3];
			vari_size pos = _GetLastStr(buffer, len, '\t');
			sscanf_s(buffer+pos, "rotation %f %f %f", &rot[0], &rot[1], &rot[2]);
			obj.rotation[0] = rot[0];
			obj.rotation[1] = rot[1];
			obj.rotation[2] = rot[2];
			cur += (len + 1);
		}
		else if (strstr(buffer, "translation "))
		{
			float trans[3];
			vari_size pos = _GetLastStr(buffer, len, '\t');
			sscanf_s(buffer + pos, "translation %f %f %f", &trans[0], &trans[1], &trans[2]);
			obj.translation[0] = trans[0];
			obj.translation[1] = trans[1];
			obj.translation[2] = trans[2];
			cur += (len + 1);
		}
		else if (strstr(buffer, "visible "))
		{
			int visible;
			vari_size pos = _GetLastStr(buffer, len, '\t');
			sscanf_s(buffer + pos, "visible %d", &visible);
			obj.visible = visible;
			cur += (len + 1);
		}
		else if (strstr(buffer, "vertex "))
		{
			obj.vertex = NULL;
			cur = _SetVertex(obj, cur);
		}
		else if (strstr(buffer, "face "))
		{
			obj.face = NULL;
			cur = _SetFace(obj, cur);
		}
		else
		{
			cur += (len + 1);
		}
	}

	//括弧閉じがない。フォーマットエラー
	abort();
	return cur;
}

const char* Converter::_SetVertex(MQO_OBJECT& obj, const char* dataPtr)
{
	const char* cur = dataPtr;
	vari_size vIdx = 0;
	
	while (1)
	{
		char buffer[1024] = "";
		const vari_size len = _GetLine(buffer, cur);

		if (strstr(buffer, "}"))
		{
			const vari_size nextLine = strstr(cur, "\n") - cur;
			cur += (nextLine + 1);//}の次の行を返す
			return cur;
		}
		else if (strstr(buffer, "vertex "))
		{
			if (obj.vertex)	abort();
			int vn = 0;
			const char* vertexBuf = strstr(buffer, "vertex");
			sscanf_s(vertexBuf, "vertex %d", &vn);
			obj.vertex = new MQO_VERTEX[vn];
			obj.vn = vn;
		}
		else
		{
			//頂点読み込み
			if ((!obj.vertex) || (static_cast<vari_size>(obj.vn) <= vIdx))
			{
				abort();
			} 
			//タブを消す
			vari_size vertexPos = _GetLastStr(buffer, len, '\t');
			
			float v[3];
			sscanf_s(buffer+vertexPos, "%f %f %f", &v[0], &v[1], &v[2]);
			obj.vertex[vIdx].v[0] = v[0];
			obj.vertex[vIdx].v[1] = v[1];
			obj.vertex[vIdx].v[2] = v[2];
			++vIdx;
		}

		cur += (len + 1);
	}

	//括弧閉じがない。フォーマットエラー
	abort();
	return cur;
}

const char* Converter::_SetFace(MQO_OBJECT& obj, const char* dataPtr)
{
	const char* cur = dataPtr;
	vari_size fIdx = 0;

	while (1)
	{
		char buffer[1024] = "";
		const vari_size len = _GetLine(buffer, cur);

		if (strstr(buffer, "}"))
		{
			const vari_size nextLine = strstr(cur, "\n") - cur;
			cur += (nextLine + 1);//}の次の行を返す
			return cur;
		}
		else if (strstr(buffer, "face "))
		{
			if (obj.face)	abort();
			int fn = 0;
			const char* faceBuf = strstr(buffer, "face");
			sscanf_s(faceBuf, "face %d", &fn);
			obj.face = new MQO_FACE[fn];
			obj.fn = fn;
		}
		else
		{
			//面読み込み
			if ((!obj.face) || (static_cast<vari_size>(obj.fn) <= fIdx))
			{
				abort();
			}
			//タブを消す
			vari_size facePos = _GetLastStr(buffer, len, '\t');;
			
			//4 V(93 23 25 94) M(0) UV(0.202141 0.790004 0.208003 0.85388 0.181559 0.850519 0.171732 0.783247)
			int vn = 0;
			int vidx[4] = { -1, -1, -1, -1 };
			int midx;
			float uv[8] = { -1, -1, -1, -1, -1, -1, -1, -1};
			sscanf_s(buffer+facePos, "%d V", &vn);
			facePos = strstr(buffer, "V") - buffer; //'V'まで飛ばす
			if (4 == vn)
			{
				sscanf_s(buffer + facePos, "V(%d %d %d %d) M(%d) UV(%f %f %f %f %f %f %f %f)", &vidx[0], &vidx[1], &vidx[2], &vidx[3], &midx,
					&uv[0], &uv[1], &uv[2], &uv[3], &uv[4], &uv[5], &uv[6], &uv[7]);
			}
			else if (3 == vn)
			{
				sscanf_s(buffer + facePos, "V(%d %d %d) M(%d) UV(%f %f %f %f %f %f)", &vidx[0], &vidx[1], &vidx[2], &midx,
					&uv[0], &uv[1], &uv[2], &uv[3], &uv[4], &uv[5], &uv[6], &uv[7]);
			}
			
			obj.face[fIdx].vn = vn;
			obj.face[fIdx].vIdx[0] = vidx[0];
			obj.face[fIdx].vIdx[1] = vidx[1];
			obj.face[fIdx].vIdx[2] = vidx[2];
			obj.face[fIdx].vIdx[3] = vidx[3];
			obj.face[fIdx].mIdx = midx;
			obj.face[fIdx].uv[0] = uv[0];
			obj.face[fIdx].uv[1] = uv[1];
			obj.face[fIdx].uv[2] = uv[2];
			obj.face[fIdx].uv[3] = uv[3];
			obj.face[fIdx].uv[4] = uv[4];
			obj.face[fIdx].uv[5] = uv[5];
			obj.face[fIdx].uv[6] = uv[6];
			obj.face[fIdx].uv[7] = uv[7];
			obj.face[fIdx].color[0] = 0;
			obj.face[fIdx].color[1] = 0;
			obj.face[fIdx].color[2] = 0;

			++fIdx;
		}

		cur += (len + 1);
	}

	//括弧閉じがない。フォーマットエラー
	abort();
	return cur;
}

const char* Converter::_SetMatrial(std::vector<MQO_MATERIAL>& mtls, const char* dataPtr)
{
	const char* cur = dataPtr;
	while (1)
	{
		char buffer[1024] = "";
		const vari_size len = _GetLine(buffer, cur);

		if (strstr(buffer, "}"))
		{
			const vari_size nextLine = strstr(cur, "\n") - cur;
			cur += (nextLine + 1);//}の次の行を返す
			return cur;
		}
		else if (strstr(buffer, "col("))
		{
			MQO_MATERIAL mtl;
			const char* mtlBuf = buffer + _GetLastStr(buffer, len, '\t');

			//"hito" col(1.000 1.000 1.000 1.000) dif(0.800) amb(0.600) emi(0.600) spc(0.000) power(5.00) tex("hito-mmd.jpg")
			{
				sscanf_s(mtlBuf, "%s ", mtl.name, sizeof(mtl.name));
			}
			{
				const char* offset = strstr(mtlBuf, "col");
				float col[3];
				sscanf_s(offset, "col(%f %f %f) ", &col[0], &col[1], &col[2] );
				mtl.color[0] = col[0];
				mtl.color[1] = col[1];
				mtl.color[2] = col[2];
			}
			{
				const char* offset = strstr(mtlBuf, "dif(");
				float dif;
				sscanf_s(offset, "dif(%f) ", &dif);
				mtl.dif = dif;
			}
			{
				const char* offset = strstr(mtlBuf, "amb(");
				float amb;
				sscanf_s(offset, "amb(%f) ", &amb);
				mtl.amb = amb;
			}
			{
				const char* offset = strstr(mtlBuf, "emi(");
				float emi;
				sscanf_s(offset, "emi(%f) ", &emi);
				mtl.emi = emi;
			}
			{
				const char* offset = strstr(mtlBuf, "spc(");
				float spc;
				sscanf_s(offset, "spc(%f) ", &spc);
				mtl.spc = spc;
			}
			{
				const char* offset = strstr(mtlBuf, "power(");
				float power;
				sscanf_s(offset, "power(%f) ", &power);
				mtl.power = power;
			}
			{
				const char* offset = strstr(mtlBuf, "tex(");
				sscanf_s(offset, "tex(%s) ", mtl.texPath, sizeof(mtl.texPath));
			}
			mtls.push_back(mtl);
		}

		cur += (len + 1);
	}

	//括弧閉じがない。フォーマットエラー
	abort();
	return cur;
}