#include "MetPch.h"

#ifndef _FILE_READ_
#include "FileRead.h"
#endif

using namespace MetaseqConverter;
using namespace std;

static const char* s_testDataPath = "C:\\Users\\SIVA2\\Documents\\hito-mmd.mqo";


FileRes FileRead::_Read(const char* path) const
{
	ifstream ifs(path);
	if (ifs.fail())
	{
		return FileRes(NULL, 0);
	}
	
	istreambuf_iterator<char> it(ifs);
	istreambuf_iterator<char> last;
	string str(it, last);
	
	char* data = new char[str.size()];
	vari_size size = str.size();
	memcpy(data, str.data(), str.size());

	FileRes ret(data, size);
	return ret;
}

FileRes FileRead::TestRead()
{
	return _Read(s_testDataPath);
}