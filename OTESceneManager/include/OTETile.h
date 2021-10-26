// ======================================================================
//  COTETile class
//  By Romeo Jan 10/06
// ======================================================================
#pragma once
#include "OTEStaticInclude.h"
#include "OgreVertexIndexData.h"
#include "OgreMovableObject.h"
#include "OgreRenderable.h"
#include "OgreNode.h"
#include "OgreCamera.h"
#include "OgreHardwarePixelBuffer.h"
#include "OTECommon.h"
#include "OgreSceneManager.h"
#include "OTEEntityRender.h"

// *******************************************
namespace OTE
{
#define		ALPHA_LAYER_COUNT			2		// 地表ALPHA层数
#define		TEXTURE_LAYER_COUNT			3		// 地表贴图层数

typedef std::map <unsigned int, Ogre::IndexData* >	IndexMap;
typedef std::vector < IndexMap* >					LevelArray;
typedef std::map<std::string, std::string>			TileTextureMap_t;

// *******************************************
// CTile
// *******************************************
class CTile
{
public:

	enum Neighbor{NORTH = 0,SOUTH = 1,EAST = 2,WEST = 3,HERE = 4};

	enum Corner{LEFT = 0, RIGHT= 1, LEFTBOTTOM = 2, RIGHTBOTTOM = 3};

public:
	// int m_Size;
	CTile*	m_Neighbors[4];

    void SetNeighbor( Neighbor n, CTile *t )
    {
        m_Neighbors[ n ] = t;
    };

    CTile* GetNeighbor( Neighbor n)
    {
        return m_Neighbors[ n ];
    };
};

// *******************************************
// CTileMap
// NOTE: 数据存放方式 与坐标 互为转置
// *******************************************

class CTileMap
{
public:
    typedef std::vector < CTile* > TileRow_t;
    typedef std::vector < TileRow_t > TileMap_t;

public:
	TileMap_t			m_Tiles;

public:	
	// ------------------------------------------
	CTileMap(){}
	// ------------------------------------------
	CTileMap(int mapSizeX, int mapSizeZ)
	{
		SetSize(mapSizeX, mapSizeZ);
	}
	// ------------------------------------------
	int GetHeight()
	{
		if(m_Tiles.empty())
			return 0;
		return int(m_Tiles[0].size());
	}
	// ------------------------------------------
	int GetWidth()
	{
		return int(m_Tiles.size());
	}

	// ------------------------------------------
	void SetSize(int mapSizeX, int mapSizeZ)
	{
		for ( int i = 0; i < mapSizeX; ++ i )
		{
			m_Tiles.push_back( TileRow_t() );

			for ( int j = 0; j < mapSizeZ; ++ j )
			{
				m_Tiles[ i ].push_back( 0 );
			}
		}			

	}
	// ------------------------------------------
	void LinkNeighbours()
	{
		//setup the neighbor links.

		int sizeX = GetWidth();
		int sizeZ = GetHeight();

        for ( int j = 0; j < sizeZ; ++ j )
        {
            for ( int i = 0; i < sizeX; ++ i )
            {
                if ( j != sizeZ - 1 )
                {
                    m_Tiles[ i ][ j ]	  -> SetNeighbor( CTile::SOUTH, m_Tiles[ i ][ j + 1 ] );
                    m_Tiles[ i ][ j + 1 ] -> SetNeighbor( CTile::NORTH, m_Tiles[ i ][ j ] );
                }

                if ( i != sizeX - 1 )
                {
					m_Tiles[ i ][ j ]     -> SetNeighbor( CTile::EAST, m_Tiles[ i + 1 ][ j ] );
                    m_Tiles[ i + 1 ][ j ] -> SetNeighbor( CTile::WEST, m_Tiles[ i ][ j ] );
                }
            }
        }
	}
	// ------------------------------------------
	 ~CTileMap()
    {
       /* TileMap_t::iterator i, iend;
        iend = m_Tiles.end();
        for ( i = m_Tiles.begin(); i != iend; ++ i )
        {
            TileRow_t::iterator j, jend;
            jend = i->end();
            for ( j = i->begin(); j != jend; ++ j )
            {
                delete *j;
                *j = 0;
            }
        }*/
    }	
	
};


//	*********************************************************************
//	TerrainUnitData
//  可以用于读取/存储的数据结构
//	*********************************************************************
class CTerrainUnitData
{	

public:

	// 索引

	unsigned char			m_LayerUIndex[TEXTURE_LAYER_COUNT];	
	unsigned char			m_LayerVIndex[TEXTURE_LAYER_COUNT];	

public:

	void operator = ( const CTerrainUnitData& rUnitData )
	{
		for(int i = 0; i < TEXTURE_LAYER_COUNT; i ++)	//lyh三层,
		{
			m_LayerUIndex[i] = rUnitData.m_LayerUIndex[i];
			m_LayerVIndex[i] = rUnitData.m_LayerVIndex[i];
		}
	}

};

//	*********************************************************************
//	CTerrainUnit 
//  用于确定地表贴图UV坐标的最小单元
//	*********************************************************************

class CTerrainUnit : public CTile , public CTerrainUnitData
{
};

//	*********************************************************************
//	TileCreateData_t
//  场景创建数据,  Page共享
//	*********************************************************************

struct TileCreateData_t
{
	// 静态数据

	std::string	name;			// 场景名称	
	std::string layerTexes[TEXTURE_LAYER_COUNT];	// 默认TEXTURE_LAYER_COUNT层贴图名称
	std::string	hgtmapName;		// 高度图名称	
	std::string	normalName;		// 法线文件名称	
	std::string	uvInfoName;		// UV文件名称	
	std::string	lgtmapName;		// 光照图
	std::string	alphamapName;	// Alpha
	std::string	defaultTexs[TEXTURE_LAYER_COUNT];	// 默认地表贴图
	int         pageSizeX;		// 场景尺寸X
	int         pageSizeZ;		// 场景尺寸Z

	// 动态数据

	float*		   fHeightData;	// 浮点高度 
	unsigned char* heightData;	// 高度图
	Ogre::Vector3* normal;		// 法线图
	CTerrainUnitData* uvData;	// uv数据

	typedef std::map<std::string, std::string>	TileTextureMap_t;

	TileTextureMap_t* tileTexes[TEXTURE_LAYER_COUNT];	//记录每个需要单独配置的纹理

	TileCreateData_t() : 
	fHeightData(NULL),
	heightData(NULL),
	normal(NULL),
	uvData(NULL)
	{
	}

};

//	*********************************************************************
//	class COTETile
//  这个名字改成COTETerrainTile 更确切一点
//	*********************************************************************

//#define STITCH_NORTH_SHIFT 0
//#define STITCH_SOUTH_SHIFT 8
//#define STITCH_WEST_SHIFT  16
//#define STITCH_EAST_SHIFT  24
//
//#define STITCH_NORTH  128 << STITCH_NORTH_SHIFT
//#define STITCH_SOUTH  128 << STITCH_SOUTH_SHIFT
//#define STITCH_WEST   128 << STITCH_WEST_SHIFT
//#define STITCH_EAST   128 << STITCH_EAST_SHIFT

#define UV_OFFSET			0.004f
#define DEFAULT_TILESIZE	8		// tile 尺寸
#define DEFAULT_PAGESIZE	256		// page 尺寸

#define MORPH_CUSTOM_PARAM_ID 77

class COTETilePage;
struct RenderUnit_t;
struct RenderPatch_t;

enum eRenderState	{eRenderPrepared, eRenderfinished};

//	*********************************************************************

class _OTEExport COTETile : public Ogre::Renderable, public Ogre::MovableObject , public CTile
{
friend class COTEQTSceneManager;
friend class COTETerrainHeightMgr;
friend class COTETerrainLightmapMgr;
friend class COTETerrainMaterialMgr;
friend class COTEVegManager;	
friend class COTELiquid;
friend class CWaterFan;
friend class COTEPrjTexture;
friend class COTETilePage;
friend class CTerrainRQListener;
friend class COTETerrainObjectListMgr;
friend class COTESceneXmlLoader;
friend class COTEShadowMap;
friend class COTEAutoEntityMgr;	
friend class COTEAutoEntity;

public:

	COTETile(const std::string& tileName, COTETilePage* parent);
	virtual ~COTETile(void);

public:

	void Initialise(int startx, int startz, TileCreateData_t* pData,
					int pageIndex, RenderPatch_t* renderPatch);

	// 材质贴图

	bool SetMaterialTexture(const std::string& fileName, int layer);

	 /** Returns the type of the movable. */
    virtual const Ogre::String& getMovableType( void ) const
    {
		static std::string sType = "TerrainTile";
        return sType;
    }

    /** Returns the bounding box of this TerrainRenderable */
	const Ogre::AxisAlignedBox& getBoundingBox( void ) const
    {
        return m_Bounds;
    }

	Ogre::Real getBoundingRadius(void) const { return 1000.0f; } // TEMP!

	bool UpdateRenderQueue( Ogre::RenderQueue* queue );

	virtual void _notifyCurrentCamera(Ogre::Camera* cam){}

	virtual void _updateRenderQueue(Ogre::RenderQueue* rq){}

    /**
    Constructs a RenderOperation to render the TerrainRenderable.
    @remarks
    Each TerrainRenderable has a block of vertices that represent the terrain.  Index arrays are dynamically
    created for mipmap level, and then cached.
    */
	virtual void getRenderOperation( Ogre::RenderOperation& rend );

    virtual const Ogre::MaterialPtr& getMaterial( void ) const
    {
        return m_Material;
    };

	void SetMaterial(const Ogre::MaterialPtr& m )
    {
        m_Material = m;
    };

	virtual void getWorldTransforms( Ogre::Matrix4* xform ) const;

	virtual const Ogre::Quaternion& getWorldOrientation(void) const;

	virtual const Ogre::Vector3& getWorldPosition(void) const
	{
		return m_Center;
	}

	/** Overridden, see Renderable */
	Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const
	{
		Ogre::Vector3 diff = m_Center - cam->getDerivedPosition();
        
        return diff.squaredLength();
	}

	/** @copydoc Renderable::getLights */
	const Ogre::LightList& getLights(void) const;

	virtual const Ogre::String& getName(void) const
	{
		return m_Name;
	}

    COTETile * GetNeighbor( Neighbor n )
    {
        return m_Neighbors[ n ];
    }

    void SetNeighbor( Neighbor n, COTETile *t )
    {
        m_Neighbors[ n ] = t;
    };

	const Ogre::Vector3& GetCenter()
	{
		return m_Center;
	}
	
	// 高度

	float GetHeightAt(float x, float z);

	// 法线

	void GetNormalAt(float x, float z, Ogre::Vector3* result);

	void _GetNormalAt(float x, float z, Ogre::Vector3* result); // 根据高度计算

	const Ogre::Vector3& GetAveNormal();

	// 碰撞测试

	bool intersectSegment( const Ogre::Vector3 & start, 
		const Ogre::Vector3 & end, Ogre::Vector3 * result );

	
	// 手工设置顶点高度

	void ManualSetVertexbufferHgtAt(int x, int z, float y);

	// 输出数据

	void ExportData(int startx, int startz, TileCreateData_t* pData);

	// 更新山体特效

	void UpdateTileEffect();

protected:

	void DeleteGeometry();

    /** Returns the index into the height array for the given coords. */
    inline int _index( int x, int z ) const;

    /** Returns the  vertex coord for the given coordinates */
    inline float _vertex( int x, int z, int n );	
	
	inline bool isInRect(const CRectA<float>& rRect);

	bool isInRect(float x, float z, float sx, float sz, float ex, float ez)
	{
		if(x>sx && z>sz && x<ex && z<ez)
			return 1;
		else 
			return 0;
	
	}

	/** Get index in the picture **/
	inline void GetPctIndex(float x,float z,int& x_index,int& z_index);

	// Height

	void SetPosBufHeightAt(int x, int z, float y, float* buff);	

	void UpdateVertexHgtbuffer(int x, int z);

	void UpdateVertexHgtbuffer(const CRectA<int>& rect);

	void UpdateVertexHgtbuffer(int x, int z, const Ogre::VertexElement* poselem, unsigned char* pBase0);

	inline void SetVertexHgtBufferAt(const Ogre::VertexElement* poselem, unsigned char* pBase0, int totalOffset);

	// Material

	void SetupMaterial(std::string texNames[]);
	
	// LightMap / alphaMap

	void SetLightMap(const std::string& lightmapName, int layer);

	void AddAlphaMap(const std::string& alphaName, int layer);

	virtual void _updateCustomGpuParameter(
		const Ogre::GpuProgramParameters::AutoConstantEntry& constantEntry,
            Ogre::GpuProgramParameters* params) const;

	void UpdateLodLevel(Ogre::IndexData* indexData, const Ogre::Camera* cam);

public :

	/// 法线相关

	void AverageVertexNormal();

	void ClearVertexNormal();

	// 水　

	void SetWater(int waterValue);

	bool GetTextureNames(std::string texNames[TEXTURE_LAYER_COUNT]);

	inline int  GetTileVertexCount();

	inline int  GetTileIndexCount();

public :

	void*							m_pGrassList;   //  草列表	

	COTETilePage*					m_pParentPage;  //  父页
	
	short							m_IndexInPage;  //  页中索引	

	bool							m_isVisible;	// 是否可见

	Ogre::Vector3					m_Center;		// 中心

protected:	

	Ogre::IndexData*				m_IndexData;

	Ogre::VertexData*				m_Terrain;	

	COTETile*						m_Neighbors [ 4 ];

	std::string						m_Name;

	Ogre::AxisAlignedBox			m_Bounds;

	Ogre::MaterialPtr				m_Material;

	Ogre::TextureUnitState*			m_LightmapUnit;

	bool							m_UseSharedMaterial; // tile上的材质已经在别的tile创建时创建完毕 直接引用之！

	CTileMap*						m_pTerrainUnitMap;	

	float*							m_pPositionBuffer;

	Ogre::Vector3*					m_pNormalBuffer;	

	eRenderState					m_RenderState;

	bool							m_GrassNeedUpdate;		
	
	std::string						m_TexNames[TEXTURE_LAYER_COUNT];

	// 悬崖在山体上标记

	unsigned int					m_CliffMark;

	// 水面

	int								m_WaterValue;

	Ogre::Vector3					m_AlphaMapping;

	// 地表上的静态小物件
	
	std::vector<void*>				m_ObjectList;


#ifndef GAME_DEBUG1

	// 渲染单元

	RenderUnit_t*					m_ParentRenderUnit;	

	int								m_VertexStart;

#endif

	// 是否更新过法线

	bool							m_NormalUpdated;

};

// 自连接贴图

extern inline float GetULeft(short uIndex);

extern inline float GetURight(short uIndex);

extern inline float GetVTop(short vIndex);

extern inline float GetVBottom(short vIndex);

}