#include "packarchieve.h"
// -----------------------------------------------
using namespace OTE;
// -----------------------------------------------
// 报告错误

void ReportError(int tErrorCode)
{	
	char tBuff[ 8 ];
	::sprintf( tBuff, "打开包裹文件失败 错误号: %d", tErrorCode );
	::MessageBox( NULL, tBuff, 0, 0 );
	throw;
}

void ReportError(const std::string& desp)
{		
	::MessageBox( NULL, desp.c_str(), 0, 0 );
	throw;
}

// -----------------------------------------------
// 内容匹配

unsigned long  xPatchCalcCrc32( unsigned char* lpSrcData, unsigned long dwSize )
{
	unsigned long crc_32_tab[] = {
	0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
	0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
	0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
	0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
	0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
	0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
	0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
	0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
	0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
	0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
	0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
	0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
	0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
	0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
	0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
	0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
	0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
	0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
	0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
	0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
	0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
	0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
	0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
	0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
	0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
	0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
	0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
	0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
	0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
	0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
	0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
	0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
	0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
	0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
	0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
	0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
	0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
	0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
	0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
	0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
	0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
	0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
	0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
	0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
	0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
	0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
	0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
	0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
	0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
	0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
	0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
	0x2d02ef8dL };
	register unsigned long crcsum = 0xffffffff;
	if ( lpSrcData != NULL )
	{
		while( dwSize-- )
		{
			crcsum = crc_32_tab[((int)crcsum ^ (*lpSrcData++)) & 0xff] ^ (crcsum >> 8);
		}
	}
	return crcsum ^ 0xffffffff;
}

// ===============================================
// CPackArchiveUnpaker
// ===============================================
CPackArchiveUnpaker::CPackArchiveUnpaker() :
m_hFile(NULL)
{

}

CPackArchiveUnpaker::~CPackArchiveUnpaker()
{
}

// ===============================================
// COTEPackArchivePaker
// ===============================================

COTEPackArchivePaker::COTEPackArchivePaker(void) :
CPackArchiveUnpaker()
{

}

COTEPackArchivePaker::~COTEPackArchivePaker(void)
{
	Destory();
}

// -----------------------------------------------
void COTEPackArchivePaker::Create(const std::string& pakFile)
{
	Destory();
		
	// 包裹头

	m_PackHead.version		= PACK_FILE_VERSION;  // 版本
	m_PackHead.curFileNum	= 0;

	SYSTEMTIME sysTime;
	::GetSystemTime(&sysTime);

	m_PackHead.month		= sysTime.wMonth;
	m_PackHead.day			= sysTime.wDay;
	m_PackHead.hour			= sysTime.wHour;
	m_PackHead.minute		= sysTime.wMinute;
	m_PackHead.second		= sysTime.wSecond;
	m_PackHead.millisecond	= sysTime.wMilliseconds;

	m_hFile		= ::CreateFile( pakFile.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	
	DWORD writeSize;

	if (m_hFile == INVALID_HANDLE_VALUE) 
	{
		// 创建包裹

		m_hFile		= ::CreateFile( pakFile.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL );
	
		::WriteFile(m_hFile, &m_PackHead, sizeof(PackHeader_t), &writeSize,		NULL );				  // version 	
		
		// 包裹文件头缓存

		char* buffer = new char[sizeof(PackFileHeard_t) * MAX_FILE_NUM];	
		::WriteFile(m_hFile, buffer, sizeof(PackFileHeard_t) * MAX_FILE_NUM, &writeSize, NULL);   // all file header space  
		delete[] buffer;

		::OutputDebugString("创建包裹完成！\n");
	}
	else
	{		
		// 包裹头

		::ReadFile(m_hFile, &m_PackHead, sizeof(PackHeader_t), &writeSize, NULL);				  // version 	
		
	}

	// 获得所有pak包中已经存在的文件信息	

	for(int i = 0; i < m_PackHead.curFileNum; i ++)
	{			
		PackFileHeard_t* pfh = new PackFileHeard_t();

		::ReadFile(m_hFile, pfh, sizeof(PackFileHeard_t), &writeSize,NULL );					
		
		m_FileHeardNameMap[ pfh->fileName ]	= pfh;		

	}

	m_CurrentPakFile = pakFile;

}

// -----------------------------------------------
void COTEPackArchivePaker::Destory()
{
	// 关闭文件

	if(m_hFile)	
	{
		::CloseHandle( m_hFile );
		m_hFile = 0;
	}	

	// 清理
	
	stdext::hash_map<std::string,	PackFileHeard_t*>::iterator it = m_FileHeardNameMap.begin();
	while(it != m_FileHeardNameMap.end())
	{
		delete it->second;

		++ it;
	}
	m_FileHeardNameMap.clear();		
	
	
}

// ------------------------------------------
// 包内 名字与文件头的映射

bool COTEPackArchivePaker::AddFileToPack(const std::string& srcFile, const FileInfo_t& fileInfo, 
											const std::string& pakFile, std::stringstream& logMsg)
{
	// 包裹文件

	if(m_CurrentPakFile.empty() ||	m_CurrentPakFile != pakFile)
	{
		Create(pakFile);
	}	

	// 读取原文件

	unsigned long readSize;

	HANDLE hFile = ::CreateFile( srcFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile == INVALID_HANDLE_VALUE )
	{
		::OutputDebugString("读取原文件失败！\n");

		ReportError(::GetLastError());
		return false;
	}

	unsigned long	srcFileSize		= ::GetFileSize(hFile, NULL );
	if(srcFileSize == 0)
	{
		char buff[256];
		sprintf(buff, "发现0字节文件 %s", srcFile);
		::MessageBox (NULL, buff, "错误", MB_OK);
		throw;
	}

	unsigned char*  pSrcFileBuffer	= new unsigned char[srcFileSize];

	::memset(pSrcFileBuffer, 0, sizeof(unsigned char) * srcFileSize);

	::ReadFile( hFile, pSrcFileBuffer, srcFileSize, &readSize, NULL );	
	
	::CloseHandle(hFile);

	// 组织新包裹文件

	unsigned long writeSize;

	PackFileHeard_t  newFileHeard;

	newFileHeard.fileSize			= srcFileSize;
	if(fileInfo.name.length() >= MAX_FILENAME_LENGTH)
	{
		::MessageBox( NULL, "包裹文件名不得超过64!", 0, 0 );
		throw;
	}

	::strcpy(newFileHeard.fileName,	fileInfo.name.c_str());								// 用fileInfo结构的name成员
	newFileHeard.filePos			= ::GetFileSize(m_hFile, NULL);						// 加在尾部
	newFileHeard.crc				= xPatchCalcCrc32(pSrcFileBuffer, srcFileSize);		// 计算文件crc
	newFileHeard.fileSize			= srcFileSize;
	newFileHeard.id					= m_PackHead.curFileNum;

	// 查询文件是否已经存在

	unsigned int curPos		= m_PackHead.curFileNum * sizeof( PackFileHeard_t ) + sizeof(PackHeader_t);
	bool isChanged = false;
	stdext::hash_map<std::string, PackFileHeard_t*>::iterator it = m_FileHeardNameMap.find(fileInfo.name);
	if(it != m_FileHeardNameMap.end())
	{
		// 对于已经存在的文件进行更新

		PackFileHeard_t* pft = it->second;		

		if(pft->crc != newFileHeard.crc)
		{
			int headPos = pft->id * sizeof(PackFileHeard_t) + sizeof(PackHeader_t);

			// 替换文件头

			::SetFilePointer(	m_hFile,	headPos,				NULL,					FILE_BEGIN		);
			::WriteFile(		m_hFile,	&newFileHeard,		sizeof(newFileHeard),	&writeSize, NULL	);                     //write file header
			
			// 替换文件内容

			::SetFilePointer(	m_hFile,	pft->filePos,		NULL,					FILE_BEGIN		);
			::WriteFile(		m_hFile,	pSrcFileBuffer,		srcFileSize,			&writeSize, NULL	); 
					
			isChanged = true;			

			::OutputDebugString("替换包裹文件完成！\r\n");
			logMsg << "替换包裹文件： " << newFileHeard.fileName << "\r\n";
		}		
	}
	else
	{
		/// 创建新文件

		// 文件头
		
		::SetFilePointer( m_hFile,	curPos,				NULL,					FILE_BEGIN		);
		::WriteFile(	  m_hFile,	&newFileHeard,		sizeof(newFileHeard),	&writeSize, NULL	);                     // write file header
	
		// 将文件内容写到最后

		::SetFilePointer( m_hFile,	0,					NULL,					FILE_END		);
		::WriteFile(	  m_hFile,	pSrcFileBuffer,		srcFileSize,			&writeSize, NULL	); 

		PackFileHeard_t* pfh = new PackFileHeard_t();
		memcpy( pfh, &newFileHeard, sizeof(PackFileHeard_t) );
		m_FileHeardNameMap[fileInfo.name] = pfh;

		m_PackHead.curFileNum ++;
		isChanged = true;

		::OutputDebugString(newFileHeard.fileName);
		::OutputDebugString("创建新包裹文件完成！\n");
		logMsg << "创建新包裹文件： " << newFileHeard.fileName << "\r\n";

	}


	// 更新包裹头
		
	if(isChanged)
	{
		::SetFilePointer(	m_hFile,	0,		NULL,	FILE_BEGIN					);
		::WriteFile( m_hFile, &m_PackHead, sizeof(PackHeader_t), &writeSize, NULL	);	
	}
	else
	{
		::OutputDebugString("无更新包裹操作！\n");
		logMsg << "更新包裹文件： " << newFileHeard.fileName << "已经存在且内容一致 被忽略！\r\n";
	}

	// 清理

	if(pSrcFileBuffer)
		delete []pSrcFileBuffer;

	return true;

}

// ===============================================
// COTEPackArchiveUnpaker
// ===============================================

COTEPackArchiveUnpaker::COTEPackArchiveUnpaker(void) : 
CPackArchiveUnpaker(),
m_hMapping(NULL),
m_pBuffer(NULL)
{

}

COTEPackArchiveUnpaker::~COTEPackArchiveUnpaker(void)
{
	Destory();
}

// ------------------------------------------
bool COTEPackArchiveUnpaker::ReadFileFromPack(const std::string& srcName, const std::string& pakFile, PackFileInfo_t& rPackFileInfo)
{
	// 包裹文件

	if(m_CurrentPakFile.empty() ||	m_CurrentPakFile != pakFile)
	{
		Create(pakFile);
	}	

	stdext::hash_map<std::string, PackFileInfo_t>::iterator it = m_FileNameMap.find(srcName);
	if(it != m_FileNameMap.end())
	{
		rPackFileInfo = it->second;	

		rPackFileInfo.fileBuffer = m_pBuffer + rPackFileInfo.filePos;

		return true;
	}

	return false;
}

// ------------------------------------------
bool COTEPackArchiveUnpaker::ExportFileFromPack(const std::string& srcName, const std::string& desFile, const std::string& pakFile)
{
	// 包裹文件

	if(m_CurrentPakFile.empty() ||	m_CurrentPakFile != pakFile)
	{
		Create(pakFile);
	}

	PackFileInfo_t pfi;
	ReadFileFromPack(srcName, pakFile, pfi);

	// 输出文件

	if(pfi.fileBuffer)
	{
		unsigned long readSize;

		HANDLE hFile = ::CreateFile( desFile.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
		if ( hFile == INVALID_HANDLE_VALUE )
		{
			::OutputDebugString("文件打开失败！\n");

			ReportError(::GetLastError());
			return false;
		}

		unsigned long	srcFileSize		= pfi.fileSize;
		unsigned char*  pSrcFileBuffer	= pfi.fileBuffer;

		::WriteFile( hFile, pSrcFileBuffer, srcFileSize, &readSize, NULL );

		::CloseHandle(hFile);
	}

	return true;
}

// -----------------------------------------------
void COTEPackArchiveUnpaker::Create(const std::string& pakFile)
{
	Destory();

	DWORD writeSize;

	m_hFile		= ::CreateFile( pakFile.c_str(), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	
	if ( m_hFile == INVALID_HANDLE_VALUE )
	{
		::OutputDebugString("文件打开失败！\n");

		ReportError(::GetLastError());

	}

	m_hMapping	= ::CreateFileMapping( m_hFile, 0, PAGE_READONLY | SEC_COMMIT, 0, 0, 0 );
	m_pBuffer	= (unsigned char*)::MapViewOfFile( m_hMapping, FILE_MAP_READ, 0, 0, 0 );

	// 包裹头

	::ReadFile(m_hFile, &m_PackHead, sizeof(PackHeader_t), &writeSize, NULL);				  // version 	
		
	unsigned int maxFilePos = 0;
	for(int i = 0; i < m_PackHead.curFileNum; i ++)
	{			
		PackFileHeard_t pfh;

		::ReadFile(m_hFile, &pfh, sizeof(PackFileHeard_t), &writeSize,NULL );			

		//::OutputDebugString(pfh.fileName);
		//::OutputDebugString("\n");

		if(pfh.fileSize == 0)
		{
			::MessageBox (NULL, "发现0字节文件！", "错误", MB_OK);
			continue;
		}

		m_FileNameMap[ pfh.fileName ] = PackFileInfo_t(i, pfh.filePos, pfh.fileSize);

		if(maxFilePos < pfh.filePos)
			maxFilePos = pfh.filePos;
	}

	m_CurrentPakFile = pakFile;

}

// -----------------------------------------------
void COTEPackArchiveUnpaker::Destory()
{
	// 关闭文件

	if(m_hFile)	
	{
		::CloseHandle( m_hFile );
		m_hFile = 0;
	}	

	if(m_pBuffer)	
	{
		::UnmapViewOfFile( m_pBuffer ); 
		m_pBuffer = 0;
	}

	if(m_hMapping)
	{
		::CloseHandle( m_hMapping ); 
		m_hMapping = 0;
	}
	
}
