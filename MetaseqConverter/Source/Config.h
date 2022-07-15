#ifndef _CONFIG_H_
#define _CONFIG_H_


namespace MetaseqConverter
{
	class Config
	{
	public:
		static void Initialize();
		static const vector<string>& RefResList();
	};
}

#endif