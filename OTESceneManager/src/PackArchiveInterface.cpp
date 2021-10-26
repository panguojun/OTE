#include "PackArchiveInterface.h"
#include "OTECommon.h"

using namespace Ogre;
using namespace OTE;

// ===============================================
// PakFileDataStream
// ===============================================
PakFileDataStream::PakFileDataStream(unsigned char* pBuf, int fSize)
: m_pFileBuffer(pBuf)
, m_FilePos(0)
{
	mSize = fSize;	
}

PakFileDataStream::~PakFileDataStream(void)
{
}

// -----------------------------------------------
size_t PakFileDataStream::read(void* buf, size_t count)
{	
	::memcpy( buf, (void*)( m_pFileBuffer + m_FilePos ), count );

	m_FilePos += count;
    return count;
}

// -----------------------------------------------
size_t PakFileDataStream::readLine(char* buf, size_t maxCount, const String& delim)
{
	for ( unsigned int i = m_FilePos; i < mSize; i ++ )
	{
		if ( m_pFileBuffer[i] == '\r' && m_pFileBuffer[i + 1] == '\n' )
		{
			// terminate
			buf[i - m_FilePos] = '\0';

			m_FilePos = i + 2;

			return i - m_FilePos + 2;
		}

		if ( m_pFileBuffer[i] == '\n' )
		{
			// terminate
			buf[i - m_FilePos] = '\0';

			m_FilePos = i + 1;

			return i - m_FilePos + 1;

		}

		buf[i - m_FilePos] = m_pFileBuffer[i];
	}

	int tRead = mSize - m_FilePos;
	m_FilePos = mSize;


    return tRead;
}

// -----------------------------------------------
size_t PakFileDataStream::skipLine(const String& delim)
{
	for ( unsigned int i = m_FilePos; i < mSize; i ++ )
	{
		if ( m_pFileBuffer[i] == '\r' && m_pFileBuffer[i + 1] == '\n' )
		{
			m_FilePos = i + 2;
			return i - m_FilePos + 2;
		}

		if ( m_pFileBuffer[i] == '\n' )
		{
			m_FilePos = i + 1;
			return i - m_FilePos + 1;
		}
	}

	int tRead = mSize - m_FilePos;
	m_FilePos = mSize;

    return tRead;
}

// -----------------------------------------------
void PakFileDataStream::skip(long count)
{
	m_FilePos += count;
}

// -----------------------------------------------
void PakFileDataStream::seek( size_t pos )
{
	m_FilePos = pos;
}

// -----------------------------------------------
size_t PakFileDataStream::tell(void) const
{
	return m_FilePos;
}

// -----------------------------------------------
bool PakFileDataStream::eof(void) const
{
    return m_FilePos >= mSize;
}

// -----------------------------------------------
void PakFileDataStream::close(void)
{

}

// ===============================================
// CPackArchive
// ===============================================
CPackArchive::CPackArchive(const String& name, const String& archType)
: Archive(name, archType),
m_UnPacker(NULL)
{
	m_PakFile = OTEHelper::GetResPath("ResConfig", "PakFile");
}

CPackArchive::~CPackArchive(void)
{
}

// -----------------------------------------------
void CPackArchive::load(void)
{
	if(!m_UnPacker)
	{//return; // temp!
		m_UnPacker = new COTEPackArchiveUnpaker();	
		m_UnPacker->Create(m_PakFile);
	}
}

// -----------------------------------------------
void CPackArchive::unload(void)
{
	if(m_UnPacker)
		delete m_UnPacker;
}

// -----------------------------------------------
Ogre::DataStreamPtr CPackArchive::open(const Ogre::String& filename) const
{	
	static PackFileInfo_t pfi;
	if(!m_UnPacker->ReadFileFromPack(filename, m_PakFile, pfi))
	{		
		throw;
	}
	return DataStreamPtr(new PakFileDataStream(pfi.fileBuffer, pfi.fileSize));
}

// -----------------------------------------------
bool CPackArchive::exists(const Ogre::String& filename)
{//return false; // false
	static PackFileInfo_t pfi;
	return m_UnPacker->ReadFileFromPack(filename, m_PakFile, pfi);
}
// -----------------------------------------------
FileInfoListPtr CPackArchive::listFileInfo(bool recursive)
{
	FileInfoList* fil = new FileInfoList();
    FileInfoList::const_iterator i, iend;
    iend = m_FileList.end();
    for (i = m_FileList.begin(); i != iend; ++i)
    {
        if (recursive || i->path.empty())
        {
            fil->push_back(*i);
        }
    }
    return FileInfoListPtr(fil);	
}

// -----------------------------------------------
StringVectorPtr CPackArchive::list(bool recursive)
{
	StringVectorPtr ret = StringVectorPtr(new StringVector());

    FileInfoList::iterator i, iend;
    iend = m_FileList.end();
    for (i = m_FileList.begin(); i != iend; ++i)
    {
		if (recursive || i->path.empty())
        {
            ret->push_back(i->filename);
        }
    }
    return ret;
}

// -----------------------------------------------
StringVectorPtr CPackArchive::find(const String& pattern, bool recursive)
{
    StringVectorPtr ret = StringVectorPtr(new StringVector());

    FileInfoList::iterator i, iend;
    iend = m_FileList.end();
    for (i = m_FileList.begin(); i != iend; ++i)
    {
		if (recursive || i->path.empty())
        {
            // Check basename matches pattern (zip is case insensitive)
            if (StringUtil::match(i->basename, pattern, false))
            {
                ret->push_back(i->filename);
            }
        }
        else
        {
            // Check full name
            if (StringUtil::match(i->filename, pattern, false))
            {
                ret->push_back(i->filename);
            }

        }
    }
    return ret;
}

// -----------------------------------------------
FileInfoListPtr CPackArchive::findFileInfo(const String& pattern, 
    bool recursive)
{
    FileInfoListPtr ret = FileInfoListPtr(new FileInfoList());

    FileInfoList::iterator i, iend;
    iend = m_FileList.end();
    for (i = m_FileList.begin(); i != iend; ++i)
    {
        if (recursive || i->path.empty())
        {
            // Check name matches pattern (zip is case insensitive)
            if (StringUtil::match(i->basename, pattern, false))
            {
                ret->push_back(*i);
            }
        }
        else
        {
            // Check full name
            if (StringUtil::match(i->filename, pattern, false))
            {
                ret->push_back(*i);
            }

        }
    }
    return ret;
}
// ===============================================
// PakFileArchiveFactory
// ===============================================
PakFileArchiveFactory* PakFileArchiveFactory::Instance = NULL;