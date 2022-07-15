#ifndef _FILE_READ_
#define _FILE_READ_

#ifndef _FILE_RES_
#include "FileRes.h"
#endif

namespace MetaseqConverter
{
	class FileRead
	{
	public:
		FileRead(){}
		virtual ~FileRead(){}

	public:
		FileRes TestRead();

	private:
		FileRes _Read(const char* path) const;
	};
}


#endif