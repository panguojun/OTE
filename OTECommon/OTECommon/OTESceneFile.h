#pragma once

/** �汾��
*/
#define OTE_VERSION						"1.1"

/** ���綨��
*/
#define ROW_SIZE						5632				//���������ĳߴ磨���أ�X����
#define COL_SIZE						6144                //Z����

#define TILE_SIZE						8					//һ��tile�ĸ߶�������������أ�
#define WORLD_UNIT_SIZE                 2.0f                //һ���߶������Ӧ�ߴ磨ˮƽ���ϣ����ף�
#define WORLD_TILE_SIZE					16.0f               //һ��tile�ĳߴ磨�ף�

#define TILE_ALPHA_SIZE					16					//һ��tile��alphaͼ�ķֱ���
#define TILE_VEGMAP_SIZE				16					//һ��tile�ϲݷֲ�ͼ��vegmap���ķֱ���

#define ROW_TILES						ROW_SIZE/8			//���������ĳߴ磨tile����	576��tile����
#define COL_TILES						COL_SIZE/8			//							704��tile����

#define HEIGHT_MAX						500.0f				//���߶ȣ��ף�
#define PIXEL_HEIGHT					2.0f				//һ��ɫ�׵ĸ߶ȣ���ֱ�����ף�

#define SMALL_TO_BIG					10.0f               //��С��ͼ����

/** �����ļ���ʽ����
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
	float         height[9][9];        //�߶�����
	char          texture[3][32];      //������ͼ����
	unsigned char alpha[2][16][16];    //alphaͼ
	unsigned int  lightmap[8][8];      //lightmapͼ
    unsigned char vegmap[16][16];      //����ֲ�ͼ
    unsigned char collmap[8][8];       //��ײͼ  
    TileEntity_t  entities[4];         //ʵ���б�
    PointLight_t  pointlight;           //���Դ

	TileMagic_t   magic;               //������Чֻ��һ��
	float         waterheight;         //ˮ�ĸ߶�		
	unsigned char texU,texV;           //�����ظ�Ƶ��
	unsigned int  artSectorID;         //����������id
	unsigned int  LogicSectorID;       //�߻�������id
};
