// ========================================================================================
//  COTETilePage class
//  By Romeo Jan 10/06
// ========================================================================================

#pragma once
#include "OTEStaticInclude.h"
#include "OTETile.h"
#include "OgreDataStream.h"
#include "OgreRenderQueue.h"
#include "Ogre.h"
#include "OgreMovableObject.h"
#include "OgreRenderable.h"
#include "OgreSceneManager.h"
#include "OgreHardwareVertexBuffer.h"
#include "DymProperty.h"
#include "EventObjBase.h"
#include <windows.h>

// -------------------------------------------------------
namespace OTE
{	

// ********************************************************
// COTEPage
// NOTE: ���ݴ�ŷ�ʽ ������ ��Ϊת��
// ********************************************************
template <class  T> 
class COTEPage
{
public:
    typedef std::vector < T* > TileRow;
    typedef std::vector < TileRow > Tile2D;

    Tile2D							m_Tiles;
	size_t							m_TilesPerPageX;
	size_t							m_TilesPerPageZ;

protected:
   
public:

	//-------------------------------------------------------------------------
	COTEPage(unsigned short numTilesX,unsigned short numTilesZ)
	{
		m_TilesPerPageX = numTilesX;
        m_TilesPerPageZ = numTilesZ;
        // Set up an empty array of COgreTile pointers
        size_t i, j;
        for ( i = 0; i < m_TilesPerPageX; i++ )
        {
            m_Tiles.push_back( TileRow() );

            for ( j = 0; j < m_TilesPerPageZ; j++ )
            {
                m_Tiles[ i ].push_back( 0 );
            }
        }        
	}

    //-------------------------------------------------------------------------
    ~COTEPage()
    {
        Tile2D::iterator i, iend;
        iend = m_Tiles.end();
        for (i = m_Tiles.begin(); i != iend; ++i)
        {
            TileRow::iterator j, jend;
            jend = i->end();
            for (j = i->begin(); j != jend; ++j)
            {
                delete *j; // �Զ�ɾ��
                *j = 0;
            }
        }
    }	
};

// ********************************************************
// RenderPatch_t
// ********************************************************

// һ����Ⱦ��Ԫ( �����ͬ���ʵ�tile��� )

struct RenderUnit_t
{			
	RenderUnit_t() :
	m_RenderState(eRenderfinished),
	m_TileCount(0)
	{
	}

	Ogre::IndexData*					m_IndexData;
	Ogre::VertexData*					m_Terrain;
	Ogre::HardwareVertexBufferSharedPtr	m_VertBuffer;
	eRenderState						m_RenderState;

	Ogre::Vector2						m_Position;
	int									m_TileCount;
};

struct RenderPatch_t
{
	std::map<Ogre::Material*, RenderUnit_t >		m_RenderMap;	
	int												m_PatchMapX;
	int												m_PatchMapZ;

};

// ********************************************************
// COTETilePage
//  ������ָĳ�COTETerrainPage ��ȷ��һ��
// ********************************************************

class _OTEExport COTETilePage : public Ogre::MovableObject, public CEventObjBase, public CDymProperty
{
friend class COTETile;
friend class COTETilePageRQListener;
friend class COTESceneXmlLoader;

public:

	COTETilePage(Ogre::SceneManager* sm);
	virtual ~COTETilePage(void);

public: 
	
	// �ݻ����е�ҳ

	static void DestroyAllPages();

	// ���ش���ɽ��

	static void CreateTerrain(const std::string& szPageName, const std::string& szFileName);

	static void AddTerrainPage(const std::string& name, COTETilePage* page);

	static COTETilePage* CreatePage();
	
	static COTETilePage* GetCurrentPage();

	static void SetCurrentPage(COTETilePage* page);

	static void ImportTerrain(const std::string& szPageName, const std::string& szFileName);

	static COTETilePage* SwitchTerrainPage(const char* szPageName);

	static int GetLightmapScale();

	// �ر�

	void Shutdown(void);

	void RemoveTileMaterials();

	// �õ�ҳ�ĳ����ڵ�

	Ogre::SceneNode* GetPageSceneNode()
	{
		return	m_pSceneNode;
	}
	
	/// �õ�������

	COTETile* GetTileAt(float x, float z)
	{
		return GetTileAt((int)(x / m_Scale.x) / (m_TileSize), (int)(z / m_Scale.z) / (m_TileSize));
	}
	
	COTETile* GetTileAt(int x, int z)
	{		
		int maxSizeX = (m_PageSizeX) / (m_TileSize);
		int maxSizeZ = (m_PageSizeZ) / (m_TileSize);
		if(x >= 0 && x < maxSizeX && z >= 0 && z < maxSizeZ)
			return m_pPage->m_Tiles[x][z];	
		return NULL;
	}

	// ����

	void BuildPage(TileCreateData_t* pData);

	// ��������

	void ExportData(TileCreateData_t* pData);

	// ��ʾɽ�屣����

	void ShowTerrainBoundingBox(bool bVisible);

	// ���û�׼�߶�(����������ü�)

	void SetBaseHeight(float hgt)
	{
		m_BaseTerrainHeight = hgt;
	}

	// �߶�

	float GetHeightAt(float x, float y);

	COTETile* TerrainHitTest(const Ogre::Ray& r, Ogre::Vector3& result);

	// ���ɼ���Χ

	float  GetMaxDistance()
	{
		return m_MaxDistance;
	}

	void  SetMaxDistance(float dis)
	{
		m_MaxDistance = dis;
	}
	
	// �ɼ���

	void ShowTerrain(bool visible)
	{
		m_ShowTerrain = visible;
	}

	void EnableShowAll(bool isShowAll = true)
	{
		m_ShowAll = isShowAll;
	}

	bool IsVisible()
	{
		return m_ShowTerrain;
	}
	
	int GetPageTileNum()
	{
		int sizeX = (m_PageSizeX) / (m_TileSize);
		int sizeZ = (m_PageSizeZ) / (m_TileSize);
		return sizeX * sizeZ;
	}

	std::string GetTileName(float x, float z)
	{		
		COTETile* t = GetTileAt(x, z);
		if(t)
		{
			return t->getName();
		}

		return "";
	}	


public:

	/// �༭���ӿ�

	// ���¹������εķ���

	void AverageVertexNormal();

	void AverageVertexNormal(float x, float z, float halfSize);
	
	//�߶�ͼȥ��

	void heightMapDeleteDirt(int Sensitivity);

	// ����ͼ

	bool LoadLightmapData(std::string strFileName = "");	

	Ogre::ColourValue GetLightMapColorAt(float fx, float fz);

	bool LoadAlphamapData(std::string strFileName = "");

	Ogre::ColourValue GetAlphaMapColorAt(int x, int y);

	void GetAlphamapChannels(unsigned char* pData[]);

public:

	/// ��ӿ�ʵ��

	const Ogre::AxisAlignedBox& GetBoundingBox()
	{
		return m_BoundingBox;
	}


	/** Returns the name of this object. */
    virtual const Ogre::String& getName(void) const
	{
		return m_TerrainName;
	}

    /** Returns the type name of this object. */
	virtual const Ogre::String& getMovableType(void) const
	{
		static std::string sType = "TerrainMipMap";
        return sType;     
	}

	virtual void _notifyCurrentCamera(Ogre::Camera* cam)
	{		
	}

	virtual const Ogre::AxisAlignedBox& getBoundingBox(void) const
	{
		
		return m_BoundingBox;
	}

	virtual Ogre::Real getBoundingRadius(void) const
	{
		return (m_BoundingBox.getMaximum() - m_BoundingBox.getMinimum()).length() / 2.0f;
	}

	virtual void _updateRenderQueue(Ogre::RenderQueue* queue);

	virtual std::string	GetName(){return getName();}

protected:		

	void BuildNeighbers();

public:

	static const int					c_LightmapScale;

	static const int					c_AlphamapScale;

	static const int					c_PatchTileSize;		// patch��һ��tile������

	static COTETilePage* 				s_CurrentPage;	

	// ������

	static Ogre::SceneManager*			s_pCreator;

    // ��ͼ��

	TileCreateData_t::TileTextureMap_t	m_TileTexes[TEXTURE_LAYER_COUNT];	// Ĭ����ͼ

	std::string							m_strDefaultTexs[TEXTURE_LAYER_COUNT];

	COTEPage<COTETile>*					m_pPage;	

	int									m_PageSizeX;	// ҳ��X�ߴ�

	int									m_PageSizeZ;	// ҳ��Z�ߴ�		

	int									m_TileSize;		// tile�ߴ�

	int									m_UnitSize;		// ��ͼ��Ԫ�ߴ�

	Ogre::Vector3						m_Scale;		// ������
	
	// �󶨽��

	Ogre::SceneNode*					m_pSceneNode;

	Ogre::Rect							m_ActiveRect;

	// bounding box
	
	Ogre::AxisAlignedBox				m_BoundingBox;
	
	// page name

	std::string							m_PageName;	

	// terrain name

	std::string							m_TerrainName;	

	// hardware buffers

	Ogre::HardwareVertexBufferSharedPtr	m_MainBuffer;

	Ogre::HardwareIndexBufferSharedPtr	m_IndexBuffer;	

	// ����Lod

	struct LodLevel_t
	{
		Ogre::HardwareIndexBufferSharedPtr	IndexBuffer;
		int									IndexCount;
	};

	LodLevel_t							m_LodLevel[3]; // [0] ��Ӧ��ȫ��ƽ��, [1] ��Ӧһ��Lod [2] ��Ӧ����Lod

	// ����ͼ

	Ogre::TexturePtr					m_LightmapPtr;

	unsigned char*						m_LightmapData;
	
	std::string							m_LightmapPath;

	std::string							m_LightmapName;

	BITMAPFILEHEADER					m_LightmapBmfh;

	BITMAPINFOHEADER					m_LightmapBmih;

	// Alphaͼ

	Ogre::TexturePtr					m_AlphamapPtr;

	unsigned char*						m_AlphaBmpdata;
	
	std::string							m_AlphamapPath;

	std::string							m_AlphamapName;

	BITMAPFILEHEADER					m_Alphabmfh;

	BITMAPINFOHEADER					m_Alphabmih;

	Ogre::TexturePtr					m_AlphaTextures[ALPHA_LAYER_COUNT];		// һ��Alpha�� ��Ӧһ����ͼ

private:

	bool								m_ShowTerrain;

	bool								m_ShowAll;

	// ��Զ�Ӿ�

	float								m_MaxDistance;
	
	// ����ҳ(��ҳ�ṹ)

	static HashMap<std::string, COTETilePage*>	s_TerrainPageMap;	
	
	// �ü���߶�

	float								m_BaseTerrainHeight;

	// render patches

	COTEPage<RenderPatch_t>*			m_pRenderPatchPage;	

	// tile buffer poors

	Ogre::VertexData*					m_pTileVertexDataPool;

	Ogre::IndexData*					m_pTileIndexDataPool;

	float*								m_pTilePositionBufferPool;

	Ogre::Vector3*						m_pTileNormalBufferPool;

	CTileMap*							m_pTileMapBufferPool;

	CTerrainUnit*						m_pTerrainUnitPool;

	static COTETile*					s_LastTile;

};


}