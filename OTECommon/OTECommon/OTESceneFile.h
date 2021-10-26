#pragma once

/** 版本号
*/
#define OTE_VERSION						"1.1"

/** 世界定义
*/
#define ROW_SIZE						5632				//整个场景的尺寸（象素）X方向
#define COL_SIZE						6144                //Z方向

#define TILE_SIZE						8					//一个tile的高度网格格数（象素）
#define WORLD_UNIT_SIZE                 2.0f                //一个高度网格对应尺寸（水平面上）（米）
#define WORLD_TILE_SIZE					16.0f               //一个tile的尺寸（米）

#define TILE_ALPHA_SIZE					16					//一个tile上alpha图的分辨率
#define TILE_VEGMAP_SIZE				16					//一个tile上草分布图（vegmap）的分辨率

#define ROW_TILES						ROW_SIZE/8			//整个场景的尺寸（tile数）	576（tile数）
#define COL_TILES						COL_SIZE/8			//							704（tile数）

#define HEIGHT_MAX						500.0f				//最大高度（米）
#define PIXEL_HEIGHT					2.0f				//一个色阶的高度（垂直）（米）

#define SMALL_TO_BIG					10.0f               //大小地图比例

/** 场景文件格式定义
*/
typedef struct //light
{
	char    name[64];
	float   pos[3];
	float   color[3];
	float   brightness;
	float   range;
	float   constant;
	float   linear;
	float   quadratic;	
}PointLight_t;
typedef struct //entity
{
        char      sNodeName[64];
		float     fMin[3];
		float     fMax[3];
        float     fPos[3];
        float     fRot[4];
		float     fScale;
		int       id;
}TileEntity_t;
typedef struct //magic
{
        char      magTemName[32];
        float     fPos[3];
        float     fRot[4];
		float     fScale;
}TileMagic_t;
struct TileRaw //tile
{        
	float         height[9][9];        //高度数组
	char          texture[3][32];      //三层贴图名称
	unsigned char alpha[2][16][16];    //alpha图
	unsigned int  lightmap[8][8];      //lightmap图
    unsigned char vegmap[16][16];      //草类分布图
    unsigned char collmap[8][8];       //碰撞图  
    TileEntity_t  entities[4];         //实体列表
    PointLight_t  pointlight;           //点光源

	TileMagic_t   magic;               //场景特效只有一种
	float         waterheight;         //水的高度		
	unsigned char texU,texV;           //材质重复频率
	unsigned int  artSectorID;         //美术用区域id
	unsigned int  LogicSectorID;       //策划用区域id
};
