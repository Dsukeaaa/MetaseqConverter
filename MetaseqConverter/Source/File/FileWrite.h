#ifndef _FILE_WRITE_
#define _FILE_WRITE_

namespace MetaseqConverter
{
	class OutputRes;
}

namespace MetaseqConverter
{
	class FileWrite
	{
	public:
		FileWrite();
		virtual ~FileWrite();

	public:
		static bool Write(const char* path, const OutputRes& res);

		static bool Check(const char* path, const OutputRes& res);

	};
}


#endif