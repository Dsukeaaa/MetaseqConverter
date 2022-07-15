#include "MetPch.h"
#include "Config.h"

using namespace MetaseqConverter;


static vector<string> s_resList;


void Config::Initialize()
{
	const char* list[] =
	{
		"../data/chr",
		"../data/map",
	};

}

const vector<string>& Config::RefResList()
{
	return s_resList;
}