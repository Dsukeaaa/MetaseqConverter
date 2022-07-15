#ifndef _OUTPUTRES_
#define _OUTPUTRES_

#ifndef _DS_FORMAT_
#include "File/DsFormat.h"
#endif

namespace MetaseqConverter
{
	class OutputRes
	{
	public:
		OutputRes()
			: dsAnimModel()
			, dsAnimBone()
			, dsCustomProperty()
			, lightData()
			, dataSize(0)
		{};
		virtual ~OutputRes(){};

	public:
		DS_ANIM_MODEL dsAnimModel;
		DS_ANIM_BONE dsAnimBone;
		DS_CUSTOM_PROPERTY dsCustomProperty;
		DS_LIGHT_DATA lightData;
		unsigned long long dataSize;
	};
}

#endif