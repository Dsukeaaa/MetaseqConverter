#ifndef _LOAD_MODEL_
#define _LOAD_MODEL_

/*
	リソース読み込みを行うプログラムのサンプル
*/

#ifndef _OUTPUTRES_
#include "Res/OutputRes.h"
#endif 

namespace MetaseqConverter
{
	bool LoadModel(const char* path, OutputRes& res);
}





#endif