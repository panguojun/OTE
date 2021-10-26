#pragma once
#include <string>
#include <sstream>
#include <map>
#include <hash_map>
#include <windows.h>

namespace OTE
{

// ****************************************************
// ��
// ****************************************************

#define PACK_FILE_VERSION			0x80000001
#define MAX_FILE_NUM				50000
#define MAX_FILENAME_LENGTH			64

// ****************************************************
// һЩ�ṹ��
// ****************************************************

// Դ�ļ���Ϣ

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

// �����ļ���Ϣ

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

	// ����ͷ

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

	// �����ļ�ͷ

	struct PackFileHeard_t
	{
		char			fileName[MAX_FILENAME_LENGTH];
		unsigned long	filePos;
		unsigned int	fileSize;
		unsigned long	crc;   //crc
		unsigned long	id;

	};

protected:

	HANDLE										m_hFile;			// �ļ����

	PackHeader_t								m_PackHead;			// ����ͷ

	std::string									m_CurrentPakFile;	// ��ǰ������

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
	
	// ��ʼ��

	void Create(const std::string& pakFile);
	
	// �ݻ�

	void Destory();

	// ѹ��

	bool AddFileToPack(const std::string& srcFile, 
		const FileInfo_t& fileInfo, const std::string& pakFile, std::stringstream& logMsg);


protected:

	stdext::hash_map<std::string,	PackFileHeard_t*>	m_FileHeardNameMap; // �����ļ�ͷMAP	

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
	
	// ��ʼ��

	void Create(const std::string& pakFile);
	
	// �ݻ�

	void Destory();

	// �Ӱ��ж�ȡ

	bool ReadFileFromPack(const std::string& srcName, 
		const std::string& pakFile, PackFileInfo_t& rPackFileInfo);	

	bool ExportFileFromPack(const std::string& srcName, 
		const std::string& desFile, const std::string& pakFile);

public:

	stdext::hash_map<std::string,	PackFileInfo_t>		m_FileNameMap;		// �����ļ�ͷMAP

protected:

	HANDLE										m_hMapping;			// �ڴ�ӳ����	

	unsigned char*								m_pBuffer;			// ����


};

}