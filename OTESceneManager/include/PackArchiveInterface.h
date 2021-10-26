#pragma once
#include "OTEStaticInclude.h"
#include "OgreArchive.h"
#include "OgreArchiveFactory.h"
#include "packarchieve.h"

namespace OTE
{
// ************************************************
// PakFileDataStream
// ************************************************

class  PakFileDataStream : public Ogre::DataStream
{
protected:
	unsigned int	m_FilePos;
	unsigned char*	m_pFileBuffer;

public:
	PakFileDataStream(unsigned char* pBuf, int fSize);

	~PakFileDataStream();
	/** @copydoc DataStream::read
	*/
	size_t read(void* buf, size_t count);
	/** @copydoc DataStream::readLine
	*/
	size_t readLine(char* buf, size_t maxCount, const Ogre::String& delim = "\n");
	
	/** @copydoc DataStream::skipLine
	*/
	size_t skipLine(const Ogre::String& delim = "\n");

	/** @copydoc DataStream::skip
	*/
	void skip(long count);

	/** @copydoc DataStream::seek
	*/
	void seek( size_t pos );

	/** @copydoc DataStream::tell
	*/
	size_t tell(void) const;

	/** @copydoc DataStream::eof
	*/
	bool eof(void) const;

	void* GetBuffer( ){ return NULL; }
	/** @copydoc DataStream::close
	*/
	void close(void);
};


// ************************************************
// CPackArchive
// ************************************************
class CPackArchive : public Ogre::Archive 
{
public:
	CPackArchive( const Ogre::String& name,const Ogre::String& archType );
	~CPackArchive(void);

public:

	/// @copydoc Archive::isCaseSensitive
	bool isCaseSensitive(void) const
	{
		#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			return false;
		#else
			return true;
		#endif
	}

	/// @copydoc Archive::load
	void load();

	/// @copydoc Archive::unload
	void unload();

	/// @copydoc Archive::open
	Ogre::DataStreamPtr open(const Ogre::String& filename) const;

	/// @copydoc Archive::exists
	bool exists(const Ogre::String& filename);

	/// @copydoc Archive::list
    Ogre::StringVectorPtr list(bool recursive = true );

    /// @copydoc Archive::find
    Ogre::StringVectorPtr find(const Ogre::String& pattern, bool recursive = true);

    /// @copydoc Archive::listFileInfo
    Ogre::FileInfoListPtr listFileInfo(bool recursive = true );

    /// @copydoc Archive::findFileInfo
    Ogre::FileInfoListPtr findFileInfo(const Ogre::String& pattern, bool recursive = true);

protected:
	
	/// 文件列表
	Ogre::FileInfoList		m_FileList;
	
	/// 解包裹器
	COTEPackArchiveUnpaker*	m_UnPacker;

	/// 包文件
	Ogre::String			m_PakFile;

};

// ************************************************
// PakFileArchiveFactory
// ************************************************
class _OTEExport PakFileArchiveFactory : public Ogre::ArchiveFactory
{
public:
	static PakFileArchiveFactory* Instance;
	PakFileArchiveFactory(){  Instance = this; }
	virtual ~PakFileArchiveFactory() {	Instance = NULL;	}
	/// @copydoc FactoryObj::getType
	const Ogre::String& getType(void) const
	{        
		static Ogre::String name = "PakFileSystem";
        return name;	
	}
	/// @copydoc FactoryObj::createInstance
	Ogre::Archive *createInstance( const Ogre::String& name ) 
	{
		return new CPackArchive(name, "PakFileSystem");		
	}
	/// @copydoc FactoryObj::destroyInstance
	void destroyInstance( Ogre::Archive* arch) { delete arch; }
};


}
