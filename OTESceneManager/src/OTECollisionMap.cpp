#include "OTECollisionMap.h"
#include "OTEQTSceneManager.h"
// ---------------------------------------------
using namespace Ogre;
using namespace OTE;

// =============================================
// COTECollisionMap
// =============================================

COTECollisionMap::COTECollisionMap(void) :
m_pData(NULL),
m_SizeX(0),
m_SizeZ(0)
{
}

COTECollisionMap::~COTECollisionMap(void)
{
	Clear();
}

// ---------------------------------------------	
void COTECollisionMap::Clear()
{
	SAFE_DELETE(m_pData)
}

// ---------------------------------------------
bool COTECollisionMap::ReadFromBmp(const std::string& rFileName)
{
	Clear();

	m_ColliFile = rFileName;

	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(rFileName);		

	BITMAPFILEHEADER FileHeader;

	stream->read( &FileHeader, sizeof(BITMAPFILEHEADER));

	BITMAPINFOHEADER tHeader;		
	stream->read( &tHeader, sizeof(BITMAPINFOHEADER));

	if( tHeader.biBitCount != 24/* 24位 */)
	{
		MessageBox(NULL,"图片格式不对!","错误",0);

		return false;
	}

	m_SizeX = tHeader.biWidth;

	m_SizeZ = tHeader.biHeight;

	m_pData = new unsigned int [m_SizeX * m_SizeZ];

	// 读数据

	for(int j = 0; j < m_SizeZ; j ++) 
	for(int i = 0; i < m_SizeX;  i ++)
	{
		unsigned int color;
		stream->read(&color,3);	   
		unsigned int tcolor=((color & 0x000000FF)<<16)|((color & 0x00FF0000)>>16)|(color & 0x0000FF00);
		tcolor = tcolor << 8;
		m_pData[( m_SizeZ - j - 1 ) * m_SizeX + i] = tcolor;
	}	
	return true;
}

// ---------------------------------------------
unsigned int COTECollisionMap::GetPixel(int x, int y)
{	
	int ix = x;
	int iy = y;	

	if(!m_pData) // 没初始化是可达
		return 1;

	if(ix >= 0 && iy >= 0 && ix < m_SizeX && iy < m_SizeZ)
	{
		return m_pData[ix + iy * m_SizeX];
	}
	else
	{	
		return 0;
	}

}

// ---------------------------------------------
// 更新
void COTECollisionMap::Refresh()
{		
	OTE_ASSERT(!m_ColliFile.empty());

	ReadFromBmp(m_ColliFile);
}


// =============================================
// CCollision2D
// =============================================
bool CCollision2D::HitTest(COTECollisionMap* gmap, int x, int y)
{
	if(!gmap)
		return false;

	unsigned int cor = gmap->GetPixel(x, y);
	return (cor << 8) == 0x00000000;	
}

// ---------------------------------------------
// true 是不可达
bool CCollision2D::HitTest(COTECollisionMap* gmap, int sx, int sy, int ex, int ey, float step, int hitCount)
{	
	if(!gmap)
		return false;

	if( HitTest(gmap, ex, ey) || HitTest(gmap, sx, sy))
		return true;	
	
	Ogre::Vector2 sp(sx, sy);
	Ogre::Vector2 ep(ex, ey);
	Ogre::Vector2 n = (ep - sp).normalisedCopy();
	
	while((ep - sp).length() > step)
	{
		if(gmap->GetPixel(sp.x, sp.y) == 0)
		{
			hitCount --;

			if(hitCount <= 0)
			  return true;
		}
		
		sp += n * step;
	}

	return false;

}



