/***************************************************************************************************
文件名称:	DymMagic.h
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	特效主模块
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#pragma once
#include "DymScriptLoader.h"
#include "DymMagicModel.h"
#include "DymMagicParticles.h"
#include "DymFontFactory.h"
#include "DymSoundManager.h"

/// 编辑器创建子物体类型
enum MAGIC_CREATE_TYPE
{
	CREATE_BILLBOARD ,
	CREATE_SPIRE ,
	CREATE_MESH ,
	CREATE_SHADOW ,
	CREATE_SPIRE_SHADOW ,
};
class __declspec( dllexport ) CDymMagic
{
	/// 检测两个关键帧里的某个值是否相同
	template<class Type>inline
	BOOL Check_PrevKey( Type v1 , Type v2 )
	{
		return v1!=v2;
	}
	/// 检测两个关键帧里的某个字符串是否相同
	BOOL Check_PrevKey_Str( TCHAR* v1 , TCHAR* v2 )
	{
		if( _tcsicmp( v1 , v2 ) == 0 )return FALSE;
		return TRUE;
	}

	/// 特效是否循环播放
	bool isloop;
	/// 旧版本使用，已无用
	bool issametime;
	/// 模型物体列表
	std::list<CDymMagicModel*> m_listModel;
	/// 粒子发射器列表
	std::list<CDymMagicParticles*> m_listParticles;
	/// 关键帧列表
	std::list<CDymMagicKeyFrame*> m_listSoundKey;
	/// 声音对象列表，创建特效时，从关键帧中读入所有声音数据，加入到此列表中
	struct SOUND_OBJ
	{
		CDymSoundObj* pSound;
		CDymMagicBaseObj *pObj;
		BOOL bLoop;
		float radio;
	};
	std::list<SOUND_OBJ*> m_listSoundObj;
	/// 渲染设备
	CDymRenderSystem * m_pDevice;
	/// 音乐管理设备
	CDymSoundManager * m_pSoundManager;
	/// 保存世界坐标
	DYM_WORLD_MATRIX m_world;
	/// 播放最大时间
	float m_fKeyMaxTime;
	/// 播放当前时间
	float m_fKeyAddTime;
	/// 是否播放完毕
	BOOL m_bOverTime;
	/// 特效版本号
	int m_nVersion;
	/// 所有子物体全局缩放
	D3DXVECTOR3 m_vGlobleScaling;
	/// 是否快速渲染，快速渲染将自动关闭Z-BUFFER
	BOOL m_bFastRender;
	/// 是否是第一次播放，第一次播放时需要载入声音，当第二次以后播放时，循环播放的声音将不再载入
	BOOL m_bFirstPlay;
	
	
	/// 绑定盒
	D3DXVECTOR3 m_vBoundingBox1;
	D3DXVECTOR3 m_vBoundingBox2;

	/// 从脚本中读取模型
	BOOL LoadModel(CDymMagicModel *pModel,CDymScriptLoader* pLoader);
	/// 从脚本中读取粒子发射器
	BOOL LoadParticles(CDymMagicParticles *pParticles,CDymScriptLoader* pLoader);
	BOOL LoadParticles2(CDymMagicParticles *pParticles,CDymScriptLoader* pLoader);
	/// 从脚本中读取关键帧
	BOOL LoadKeyFrame(CDymMagicKeyFrame *p,CDymScriptLoader* pLoader);

	void _DelObj( CDymMagicBaseObj *p );
	void ComputeRotationID();
	void ResetParticlesPlayTime();
public:
	/// 保存其它数据为外部使用，特效本身不使用该数据
	int m_nExData;
	/// 屏蔽旋转，已无用
	BOOL m_bDisableRotation;

	CDymMagic( CDymRenderSystem *pDevice , CDymSoundManager *pSoundManager );
	~CDymMagic(void);
	/// 从文件中创建
	BOOL LoadFromFile(LPCTSTR fileName);
	/// 从内存中创建
	BOOL LoadFromMemory( void *pData , int datalen );
	/// 从脚本中创建
	BOOL LoadFromLoader(CDymScriptLoader* pLoader);
	/// 导入另一个特效（融合）
	BOOL Import( CDymMagic *pMagic );
	/// 帧循环
	BOOL FrameMove(float fElapsedTime);
	/// 渲染
	HRESULT Render( DWORD flag );
	/// 释放
	void Clear(void);
	/// 设置为快速渲染
	void SetFastRender( BOOL bFast );

	// 关于世界矩阵
	void SetPosition(D3DXVECTOR3 &v);
	void SetScaling(D3DXVECTOR3 v);
	void SetRotation(float a);
	void SetRotationAxis(D3DXVECTOR3 &v);
	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetScaling();
	D3DXVECTOR3 GetRotationAxis();
	float GetRotation();
	void SetMagicColor( float r , float g , float b , float a );

	// 关于声音
	std::list<CDymMagicKeyFrame*>::iterator m_itSoundKey;
	float m_fLastFrameTime;

	void ComputeSound();
	void _AddSound( CDymMagicKeyFrameManager *pManager );
	void FrameMoveForSound( float skiptime );
	void StopSound();
	void RenderSoundRadio();
	void _RenderSoundRadio( CDymMagicKeyFrameManager *pManager );

private:
	void ComputeKeyMaxTime(void);

	// 关于碰撞检测
	BOOL CDymMagic::Pick(const D3DXVECTOR3* pVertexStreamZeroData,UINT VertexStreamZeroStride,int facenum,POINT *pmousePt,D3DXMATRIX *pMul,float *pDisOut,D3DXVECTOR3 *pPosOut);
	BOOL CDymMagic::M3D_IntersectTriangle( const D3DXVECTOR3& orig,
                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2,
                                       FLOAT* t, FLOAT* u, FLOAT* v ,D3DXVECTOR3 *pOut);
	HRESULT CDymMagic::_D3DXIntersect3(CONST void *pDataStream,CONST int facenum,CONST int Stride,CONST D3DXVECTOR3 * pRayPos,CONST D3DXVECTOR3 * pRayDir,
		BOOL * pHit,DWORD * pFaceIndex,FLOAT * pU,FLOAT * pV,FLOAT * pDist,LPD3DXBUFFER * ppAllHits,DWORD * pCountOfHits ,
		D3DXMATRIX *pmatWorld1 = NULL ,D3DXMATRIX *pmatWorld2 = NULL ,int StrideForBlend = 0);

	int NewID();
	float _GetSoundArea( CDymMagicKeyFrameManager *pManager );
public:
	/// 创建模型
	int CreateModel( MAGIC_CREATE_TYPE type , void *pUnknow );
	/// 创建粒子发射器
	int CreateParticles( void *pUnknow );
	/// 考贝创建（复制一个对象）
	int CopyCreate( int id , void *pUnknow );
	int CopyCreate( CDymMagicBaseObj *pObj , void *pUnkonw );
	/// 考贝帧（复制帧）
	CDymMagicKeyFrame* CopyKeyFrame( CDymMagicKeyFrame *pKeyFrame );
	/// 碰撞检测，传入鼠标屏幕坐标，反回对象ID，未碰撞反回零
	int Collision(int moustx,int mousty);
	/// 删除对象
	BOOL DelObj(int id );
	/// 获取对象
	CDymMagicBaseObj *GetObj(int id);
	CDymMagicBaseObj *GetObj ( CDymMagicKeyFrame *pKey );
	/// 获取对象数量
	int GetObjNum();
	/// 获取所有对象ID，传入数组指针和大小
	void GetAllObj(int*idBuffer,int buffersize);
	/// 渲染选取线
	void RenderSelection(int* ObjIDindex, int indexSize);
	/// 开始播放
	BOOL Play();
	/// 停止播放
	BOOL Stop();
	/// 是否正在播放
	BOOL isPlaying( float fElapsedTime );
	/// 获取当前播放时间 
	float GetCurTime();
	/// 设置当前播放时间 
	void SetCurTime(float t);
	/// 通过关键帧对象获取该关键帧的管理者
	CDymMagicKeyFrameManager *FindKeyManager( CDymMagicKeyFrame *pKey );
	/// 设置最大播放时间
	void SetKeyMaxTime(float t);
	/// 获得最大播放时间
	float GetKeyMaxTime();
	/// 获得是否重复播放
	BOOL isRepeat();
	/// 设置是否重复播放
	void SetRepeat( BOOL bRepeat );
	/// 给关键帧设置一张帖图
	void SetTexture( CDymMagicKeyFrame *pKey , LPCTSTR filename );
	/// 给关键帧设置一个声音
	void SetSound( CDymMagicKeyFrame *pKey , LPCTSTR filename );
	/// 保存特效脚本
	BOOL Save( LPCTSTR fullpathfilename , void **ppSaveBuffer , int *pBufferSize );
	/// 保存关键帧
	void SaveKeyFrame( CDymScriptLoader *pLoader , CDymMagicKeyFrameManager *pKeyMana );
	/// 改变对象ID
	BOOL ChangeID( int oldid , int newid );
	/// 设置对象绕另一个对象旋转
	void SetRotationWithID( int objid , int rotationid );
	/// 设置对象跟随另一个对象
	void SetFollowWithID( int objid , int followid );
	/// 计算绑定盒
	void ComputeBoundingBox();
	/// 获得脚本里的静态绑定盒
	void GetBoundingBox( D3DXVECTOR3 *pMin , D3DXVECTOR3 *pMax );
	/// 获得实时运算绑定盒
	void GetBoundingBoxRealTime( D3DXVECTOR3 *pMin , D3DXVECTOR3 *pMax );
	/// 设置残像两个点的坐标
	void SetDefaultShadowPos( D3DXVECTOR3 v1 , D3DXVECTOR3 v2 );
	/// 设置默认公告牌的文字贴图
	void SetDefaultBillboardString( LPCTSTR str , DWORD color , void *pUnknow );
	/// 设置全局缩放
	void SetGlobleScaling( D3DXVECTOR3 vSca );
	/// 获得全局缩放
	D3DXVECTOR3 GetGlobleScaling();
	/// 设置粒子发射器发出的粒子被哪个对象吸收
	void SetParticlesAcceptID( int particlesobjid );
	/// 设置所有对象的所有关键帧贴图
	void SetAllKeyTexture( LPCTSTR texFileName );
	/// 设置是否循环播放特效
	void SetLoop( BOOL bLoop );
	/// 获得是否特环播放特效
	BOOL GetLoop();
	/// 获得声音范围
	float GetSoundArea();
};
