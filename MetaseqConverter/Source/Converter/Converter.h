#ifndef _CONVERTER_
#define _CONVERTER_

#ifndef _OUTPUTRES_
#include "../Res/OutputRes.h"
#endif

#ifndef _MQO_FORMAT_
#include "../File/MqoFormat.h"
#endif

namespace MetaseqConverter
{
	class FileRes;
}

namespace MetaseqConverter
{
	class Converter
	{
	public:
		Converter(){};
		virtual ~Converter(){};

	public:
		OutputRes Convert(const FileRes& res);

	private:
		vari_size _GetLine(char* pBuffer, const char* src) const;

	private:
		vari_size _GetLastStr(const char* buffer, const vari_size size, char key) const;

	private:
		const char* _SetHeader(MQO_HEADER* pHeader, const char* data);
		const char* _SetObject(std::vector<MQO_OBJECT>& objects, const char* dataPtr);
		const char* _SetVertex(MQO_OBJECT& obj, const char* dataPtr);
		const char* _SetFace(MQO_OBJECT& obj, const char* dataPtr);
		const char* _SetMatrial(std::vector<MQO_MATERIAL>& mtls, const char* dataPtr);
	};
}


#endif