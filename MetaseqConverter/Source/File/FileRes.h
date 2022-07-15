#ifndef _FILE_RES_
#define _FILE_RES_

namespace MetaseqConverter
{
	class FileRes
	{
	public:
		FileRes(char* data, vari_size size);
		virtual ~FileRes();

	public:
		const char* GetData() const { return m_data; }
		vari_size GetSize() const { return m_size; }

	public:
		void Finalize();

	private:
		char* m_data;
		vari_size m_size;
	};


}


#endif