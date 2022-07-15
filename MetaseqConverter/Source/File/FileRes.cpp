#include "MetPch.h"

#ifndef _FILE_RES_
#include "FileRes.h"
#endif

using namespace MetaseqConverter;

FileRes::FileRes(char* data, vari_size size)
:m_data(data)
,m_size(size)
{
}

//virtual 
FileRes::~FileRes()
{
}

void FileRes::Finalize()
{
	delete m_data;
	m_data = NULL;
}