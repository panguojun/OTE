/***************************************************************************************************
文件名称:	DymMagicParticles.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	粒子
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#pragma once
#include "DymMagicKeyFrameManager.h"
#include "DymMagicParticlesObj.h"
#include "DymMagicBaseObj.h"

class CDymMagicParticles:public CDymMagicBaseObj
{
	void _ComputeBoundingBox( CDymMagicKeyFrame *pKeyFrame , int addnum , DYM_WORLD_MATRIX world );
	void _ComputeBoundingBox_AddObj( CDymMagicKeyFrame *pKeyFrame , DYM_WORLD_MATRIX world );
	int m_bTrackRand;
	int m_nTrackRandNum;
	int m_nTrackRandNum2;
	float *m_pTrackRandArray;
public:
	char m_strName[32];
	char materialres[32];
	float quota;
	float particle_width;
	float particle_height;
	float particle_width2;
	float particle_height2;
	bool cull_each;
	bool ishav_qua;

	D3DXVECTOR3 force_vector;
	D3DXVECTOR3 ellipsoid_size;
	float time_to_live_min;
	float time_to_live_max;
	D3DXVECTOR4 colour_range_start;
	D3DXVECTOR4 colour_range_end;
	float velocity_min;
	float velocity_max;
	D3DXVECTOR3 direction;
	float angle;
	bool israndangle;
	bool isonlydirection;
	
	D3DXVECTOR4 ColourFader;
	float scale_add_x;
	float scale_add_y;
	float rotate_add;
	bool iswithfather;
	int billboard_type;
	int force_type;
	float accept_power;
	int accept_id;
	CDymMagicBaseObj *m_pAcceptObj;

	float m_fAddTimeLast;
	float m_fPlayTimeForAddObj;
	float m_fAddTime;
	int m_nVersion;
	int force_add_type;
	int particles_type;
	int limit_vector;

	bool m_bUseRate;
	float emission_rate;
	float emission_num;
	float emission_time;


	std::list< CDymMagicParticlesObj* > m_listObj;

	CDymMagicParticles(CDymRenderSystem *pDevice);
	~CDymMagicParticles(void);
	void SetParticlesName(char* strName);
	void FrameMove(float fCurTime,float fElapsedTime,DYM_WORLD_MATRIX world , BOOL bAddObj = TRUE);
	void AddObj(float fCurTime,float fTime , D3DXMATRIX *pworldnow  , CDymMagicKeyFrame *pKey , float LongTime , DYM_WORLD_MATRIX *pDymWorld );
	HRESULT Render(DYM_WORLD_MATRIX world , DWORD flag);
	HRESULT init(void);
	D3DXVECTOR3* GetVertexStreamZeroData(int *pfacenum,int *pStride);
	void CopyParticles( CDymMagicParticles *p );
	void ComputeBoundingBox( DYM_WORLD_MATRIX world );
	void Clear();
	float RandF();
	void GetBoundingBoxRealTime( D3DXVECTOR3 *pMin , D3DXVECTOR3 *pMax );
	void SetBillboardType( int type );
	int GetBillboardType();
	void SetParticlesAcceptID( CDymMagicBaseObj *pObj );
};
