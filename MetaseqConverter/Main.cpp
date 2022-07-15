#include "MetPch.h"
#include "File/FileRead.h"
#include "Converter/Converter.h"
#include "Converter/ConvertFBX.h"
#ifndef _FILE_WRITE_
#include "FIle/FileWrite.h"
#endif
#ifndef _OUTPUTRES_
#include "Res/OutputRes.h"
#endif
#include "Res/LoadModel.h"

using namespace MetaseqConverter;


int main(int argc, char** argv)
{
#if 1
	if (argc != 3)
	{
		for (int i = 0; i < argc; ++i)
		{
			printf("%s\n", argv[i]);
		}

		printf("-------------\n");

		printf("第一引数：変換元パス\n");
		printf("第二引数：変換先パス\n");
		return 0;
	}

	OutputRes res;
	ConvertFBX c; 
	//c.Convert("C:\\Users\\SIVA2\\Documents\\Blender\\chr\\mononobealice\\alice.fbx", res);
	c.Convert(argv[1], res);
	printf("%s 変換元\n", argv[1]);

	//FileWrite::Write("C:\\work\\data\\asset\\chr\\robo_0.dmdl", res);
	//FileWrite::Check("../SampleData/sample.dmdl", res);
	FileWrite::Write(argv[2], res);
	printf("%s 変換先\n", argv[2]);

	//OutputRes load;
	//LoadModel("../SampleData/sample.dmdl", load);

	return 1;
#else//テスト用
	if (argc != 3)
	{
		for (int i = 0; i < argc; ++i)
		{
			printf("%s\n", argv[i]);
		}

		printf("-------------\n");

		printf("第一引数：変換元パス\n");
		printf("第二引数：変換先パス\n");
		//return 0;
	}

	OutputRes res;
	ConvertFBX c;
	//c.Convert("C:\\Users\\SIVA2\\Documents\\Blender\\chr\\mononobealice\\alice.fbx", res);
	//c.Convert("C:\\Users\\SIVA2\\Documents\\Blender\\chr\\enemy1\\enemy1.fbx", res);
	c.Convert("C:\\Users\\SIVA2\\Documents\\Blender\\asset\\33-cottage_blender\\testObj.fbx", res);

	//c.Convert(argv[1], res);
	//printf("%s 変換元\n", argv[1]);

	//FileWrite::Write("C:\\Users\\SIVA2\\Documents\\Blender\\asset\\33-cottage_blender\\testObj.dmdl", res);
	//FileWrite::Check("../SampleData/sample.dmdl", res);
	//FileWrite::Write(argv[2], res);
	//printf("%s 変換先\n", argv[2]);

	//OutputRes load;
	//LoadModel("../SampleData/sample.dmdl", load);

	return 1;


#endif
}