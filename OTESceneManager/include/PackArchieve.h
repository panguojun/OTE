#pragma once
#include <string>
#include <sstream>
#include <map>
#include <hash_map>
#include <windows.h>

namespace OTE
{

// ****************************************************
// 宏
// ****************************************************

#define PACK_FILE_VERSION			0x80000001
#define MAX_FILE_NUM				50000
#define MAX_FILENAME_LENGTH			64

// ****************************************************
// 一些结构体
// ****************************************************

// 源文件信息

struct FileInfo_t
{	
	FileInfo_t(const std::string& rName, const std::string& rPath)
	{
		name = rName;		
		path = rPath;		
	}

	std::string name;
	std::string path;
};

// 包裹文件信息

struct PackFileInfo_t
{		
	PackFileInfo_t(unsigned long file_id, unsigned long file_Pos, unsigned long file_Size)
	{
		filePos		= file_Pos;
		fileSize	= file_Size;
		id			= file_id;
	}
	PackFileInfo_t()
	{
		filePos = 0;
		fileSize = 0;
		id = 0;
		fileBuffer = NULL;
	}

	unsigned long	filePos;
	unsigned int	fileSize;
	unsigned long	id;
	unsigned char*	fileBuffer;

};


// ****************************************************
// CPackArchiveUnpaker
// ****************************************************

class CPackArchiveUnpaker
{
protected:

	// 包裹头

	struct PackHeader_t
	{
		unsigned long	curFileNum;
		unsigned long	update_version;
		unsigned long   version;

		char			type[8];
		unsigned short	second;
		unsigned short	millisecond;
		unsigned char	month;
		unsigned char	day;
		unsigned char	hour;
		unsigned char   minute;
	};

	// 包裹文件头

	struct PackFileHeard_t
	{
		char			fileName[MAX_FILENAME_LENGTH];
		unsigned long	filePos;
		unsigned int	fileSize;
		unsigned long	crc;   //crc
		unsigned long	id;

	};

protected:

	HANDLE										m_hFile;			// 文件句柄

	PackHeader_t								m_PackHead;			// 包裹头

	std::string									m_CurrentPakFile;	// 当前包裹名

public:

	CPackArchiveUnpaker();

	~CPackArchiveUnpaker();

};

// ****************************************************
// COTEPackArchivePaker
// ****************************************************
class COTEPackArchivePaker : public CPackArchiveUnpaker
{
public:
	COTEPackArchivePaker(void);
	~COTEPackArchivePaker(void);

public:
	
	// 初始化

	void Create(const std::string& pakFile);
	
	// 摧毁

	void Destory();

	// 压包

	bool AddFileToPack(const std::string& srcFile, 
		const FileInfo_t& fileInfo, const std::string& pakFile, std::stringstream& logMsg);


protected:

	stdext::hash_map<std::string,	PackFileHeard_t*>	m_FileHeardNameMap; // 包裹文件头MAP	

};

// ****************************************************
// COTEPackArchiveUnpaker
// ****************************************************
class COTEPackArchiveUnpaker : public CPackArchiveUnpaker
{
public:
	COTEPackArchiveUnpaker(void);
	~COTEPackArchiveUnpaker(void);

public:
	
	// 初始化

	void Create(const std::string& pakFile);
	
	// 摧毁

	void Destory();

	// 从包中读取

	bool ReadFileFromPack(const std::string& srcName, 
		const std::string& pakFile, PackFileInfo_t& rPackFileInfo);	

	bool ExportFileFromPack(const std::string& srcName, 
		const std::string& desFile, const std::string& pakFile);

public:

	stdext::hash_map<std::string,	PackFileInfo_t>		m_FileNameMap;		// 包裹文件头MAP

protected:

	HANDLE										m_hMapping;			// 内存映像句柄	

	unsigned char*								m_pBuffer;			// 缓存


};

}