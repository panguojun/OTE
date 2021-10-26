/***************************************************************************************************
文件名称:	DymMagicBaseObj.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	特效物件基类
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#include ".\dymmagicbaseobj.h"

CDymMagicBaseObj::CDymMagicBaseObj(CDymRenderSystem *pDevice,MAGICE_OBJ_TYPE type)
{
	m_pDevice = pDevice;
	m_pTex = NULL;
	m_objType = type;
	m_nID = 0;
	m_nID_rotation_with = 0;
	m_pRotationObj = NULL;
	m_pFollowObj = NULL;
	m_nID_follow_with = 0 ;

	m_vBoundingBox1 = D3DXVECTOR3( 0 , 0 , 0 ) ;
	m_vBoundingBox2 = D3DXVECTOR3( 0 , 0 , 0 ) ;
	m_strObjName[0] = 0;
}

CDymMagicBaseObj::~CDymMagicBaseObj(void)
{
	if( m_pTex ) m_pTex->Release ();
}

void CDymMagicBaseObj::SetRotationWithID( CDymMagicBaseObj *p )
{
	if( p )
		m_nID_rotation_with = p->m_nID ;
	else
		m_nID_rotation_with = m_nID;
	m_pRotationObj = p;
}
void CDymMagicBaseObj::SetFollowWithID( CDymMagicBaseObj *p )
{
	if( p )
		m_nID_follow_with = p->m_nID ;
	else
		m_nID_follow_with = 0;
	m_pFollowObj = p;
}
CDymMagicBaseObj::MAGICE_OBJ_TYPE CDymMagicBaseObj::GetObjType()
{
	return m_objType;
}

void CDymMagicBaseObj::SetMaterialType(char* strType)
{
	if( stricmp( strType , "Effect_sceneadd" ) == 0 )
		materialtype = MT_Effect_sceneadd;
	else if( stricmp( strType , "Effect_sceneno" ) == 0 )
		materialtype = MT_Effect_sceneno;
	else if( stricmp( strType , "Effect_sceneab" ) == 0 )
		materialtype = MT_Effect_sceneab;
	else if( stricmp( strType , "Effect_sceneaddalpha" ) == 0 )
		materialtype = MT_Effect_sceneaddalpha;
}

char*  CDymMagicBaseObj::GetMaterialType()
{
	if( materialtype == MT_Effect_sceneadd )
		return "Effect_sceneadd";
	else if( materialtype == MT_Effect_sceneno )
		return "Effect_sceneno";
	else if( materialtype == MT_Effect_sceneab )
		return "Effect_sceneab";
	else if( materialtype == MT_Effect_sceneaddalpha )
		return "Effect_sceneaddalpha";
	return "";
}
D3DXVECTOR3 *CDymMagicBaseObj::GetPos()
{
	CDymMagicKeyFrame *pKeyFrame = m_KeyFrameManager.GetKeyFrameNow();
	CDymMagicKeyFrame keyTmp;
	if( pKeyFrame == NULL )pKeyFrame = &keyTmp;
	return &pKeyFrame->position ;
}
void CDymMagicBaseObj::ComputeWorldMatrix( DYM_WORLD_MATRIX world )
{
	CDymMagicKeyFrame *pKeyFrame = m_KeyFrameManager.GetKeyFrameNow();
	//if( m_pFollowObj )pKeyFrame = m_pFollowObj->m_KeyFrameManager.GetKeyFrameNow();
	CDymMagicKeyFrame keyTmp;
	if( pKeyFrame == NULL )pKeyFrame = &keyTmp;
	D3DXMATRIX matWorld = world.GetWorldMatrix();
	// world
	D3DXMATRIX matKey,matRot,matSca;
	D3DXMatrixTranslation( &matKey , pKeyFrame->position .x , pKeyFrame->position .y , pKeyFrame->position .z );
	D3DXMatrixScaling( &matSca , pKeyFrame->scale .x , pKeyFrame->scale .y , pKeyFrame->scale .z );
	//m_pDevice->SetScaling ( pKeyFrame->scale );
	// rotation
	D3DXMATRIX matOffset1 , matOffset2;
	if( pKeyFrame->nVersion == 1 )
	{
		D3DXVECTOR3 vAxis = pKeyFrame->spinaxis ;
		if( vAxis.x == 0.0f && vAxis.y == 0.0f && vAxis.z == 0.0f ) vAxis.z = 1.0f;
		float fRot = -pKeyFrame->spinangle*D3DX_PI*2.0f/360.0f;
		D3DXMatrixRotationAxis( &matRot , &vAxis , fRot );
		m_pDevice->SetRotation ( fRot );
	}
	else if( pKeyFrame->nVersion == 2 )
	{
		if( pKeyFrame->GetRotationX () != 0.0f || pKeyFrame->GetRotationY () != 0.0f || pKeyFrame->GetRotationZ () != 0.0f 
			|| pKeyFrame->rotation3.x != 0.0f || pKeyFrame->rotation3.y != 0.0f || pKeyFrame->rotation3.z != 0.0f )
		{
			D3DXMATRIX matRotX , matRotY , matRotZ ;
			BOOL bUseOffset = FALSE;
			if( m_pRotationObj )
			{
				D3DXVECTOR3 *pPos = m_pRotationObj->GetPos ();
				if( pPos )
				{
					D3DXMatrixTranslation( &matOffset1 ,-pPos->x+pKeyFrame->position .x ,-pPos->y+pKeyFrame->position .y ,-pPos->z+pKeyFrame->position .z );
					D3DXMatrixTranslation( &matOffset2 , pPos->x-pKeyFrame->position .x , pPos->y-pKeyFrame->position .y , pPos->z-pKeyFrame->position .z );
					bUseOffset = TRUE;
				}
			}
			/*D3DXMatrixRotationX( &matRotX , pKeyFrame->rotation2.x*D3DX_PI*2.0f/360.0f);
			D3DXMatrixRotationY( &matRotY , pKeyFrame->rotation2.y*D3DX_PI*2.0f/360.0f );
			D3DXMatrixRotationZ( &matRotZ , pKeyFrame->rotation2.z*D3DX_PI*2.0f/360.0f );
			m_pDevice->SetRotation ( pKeyFrame->rotation2.z*D3DX_PI*2.0f/360.0f );
			if( bUseOffset )
				matRot = matOffset1 * matRotX * matRotY * matRotZ * matOffset2;
			else
				matRot = matRotX * matRotY * matRotZ;*/

			D3DXMATRIX rotmul;
			D3DXQUATERNION qm;
			//D3DXQuaternionRotationYawPitchRoll( &qm ,pKeyFrame->rotation2.y*D3DX_PI*2.0f/360.0f
			//	, pKeyFrame->rotation2.x*D3DX_PI*2.0f/360.0f
			//	, pKeyFrame->rotation2.z*D3DX_PI*2.0f/360.0f );
			qm = pKeyFrame->GetQuaternion ();
			D3DXMatrixRotationQuaternion( &rotmul , &qm );
			if( bUseOffset )
				matRot = matOffset1 * rotmul * matOffset2;
			else
				matRot = rotmul;
			float fRot = -pKeyFrame->GetRotationZ ()*D3DX_PI*2.0f/360.0f;
			m_pDevice->SetRotation ( fRot );
		}
		else
		{
			D3DXMatrixIdentity( &matRot );
			m_pDevice->SetRotation( 0.0f );
		}
	}
	matKey = matSca * matRot*matKey;
	// top scaling
	D3DXMATRIX topSca,bottomSca;
	if( pKeyFrame->topscale != 1.0f || pKeyFrame->bottomscale != 1.0f)
	{
		D3DXMatrixScaling( &topSca , pKeyFrame->topscale,1,pKeyFrame->topscale );
		m_matWorld2 = topSca*matKey*matWorld ;
	}
	// bottom scaling
	if( pKeyFrame->bottomscale != 1.0f )
	{
		D3DXMatrixScaling( &bottomSca , pKeyFrame->bottomscale,1,pKeyFrame->bottomscale );
		m_matWorld1 = bottomSca*matKey*matWorld;
	}
	else
		m_matWorld1 = matKey*matWorld;
	D3DXVECTOR3 vs = D3DXVECTOR3( world.vSca.x * world.vSca2.x * pKeyFrame->scale .x, 
		world.vSca.y * world.vSca2.y * pKeyFrame->scale .y,
		world.vSca.z * world.vSca2.z * pKeyFrame->scale .z);
	m_pDevice->SetScaling ( vs );
	if( m_pFollowObj && m_pFollowObj!= this )
	{
		//CDymMagicKeyFrame *pKeyFrameTmp = m_pFollowObj->m_KeyFrameManager.GetKeyFrameNow();
		D3DXVECTOR3 *p = m_pFollowObj->GetPos ();
		/*m_matWorld1._41 += p->x;
		m_matWorld1._42 += p->y;
		m_matWorld1._43 += p->z;
		m_matWorld2._41 += p->x;
		m_matWorld2._42 += p->y;
		m_matWorld2._43 += p->z;*/
		if( m_pFollowObj->m_KeyFrameManager.GetKeyFrameNow() )
		{
			D3DXQUATERNION q = m_pFollowObj->m_KeyFrameManager.GetKeyFrameNow()->GetQuaternion ();
			D3DXMATRIX pos,mu,mq,dpos,f1,f2,f3;
			D3DXMatrixTranslation( &pos , p->x , p->y , p->z );
			//D3DXMatrixTranslation( &dpos , -p->x , -p->y , -p->z );
			D3DXMatrixRotationQuaternion( &mq , &q );

			f3 = world.GetWorldMatrix ();
			D3DXMatrixTranslation( &f1 , -f3._41 , -f3._42 , -f3._43 );
			D3DXMatrixTranslation( &f2 , f3._41 , f3._42 , f3._43 );
			m_matWorld1 = m_matWorld1*f1*mq*f2*pos;
			m_matWorld2 = m_matWorld2*f1*mq*f2*pos;
		}
	}
}

void CDymMagicBaseObj::SetupRenderState( DYM_WORLD_MATRIX world )
{
	CDymMagicKeyFrame *pKeyFrame = m_KeyFrameManager.GetKeyFrameNow();
	CDymMagicKeyFrame keyTmp;
	if( pKeyFrame == NULL )pKeyFrame = &keyTmp;

	/*static CDymMagicKeyFrame stKeyLast;
	if( CDymMagicKeyFrame::isSameKey ( &stKeyLast , pKeyFrame ) )
	{
		ComputeWorldMatrix( world );
		// texture
		CDymD3DTexture *pTex = m_pTex ;
		if( pKeyFrame->m_pTexture )
			pTex = pKeyFrame->m_pTexture;
		m_pDevice->SetTexture ( 0 , 0 );
		return ;
	}
	memcpy( &stKeyLast , pKeyFrame , sizeof( CDymMagicKeyFrame ) );*/
	if( pKeyFrame->nVersion == 1 )
	{
		// 循环贴图
		if( pKeyFrame->scroll_anim .x != 0.0f || pKeyFrame->rotate_anim != 0.0f || pKeyFrame->matUVtile.x < 1.0f )
			m_pDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
		else
			m_pDevice->SetSamplerState(0,D3DSAMP_ADDRESSU,D3DTADDRESS_CLAMP);
		if( pKeyFrame->scroll_anim .y != 0.0f || pKeyFrame->rotate_anim != 0.0f || pKeyFrame->matUVtile.y < 1.0f )
			m_pDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
		else
			m_pDevice->SetSamplerState(0,D3DSAMP_ADDRESSV,D3DTADDRESS_CLAMP);
	}
	else if( pKeyFrame->nVersion == 2 )
	{
		m_pDevice->SetSamplerState(0,D3DSAMP_ADDRESSU, pKeyFrame->bUwrap2 ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP );
		m_pDevice->SetSamplerState(0,D3DSAMP_ADDRESSV, pKeyFrame->bVwrap2 ? D3DTADDRESS_WRAP : D3DTADDRESS_CLAMP );
	}
	ComputeWorldMatrix( world );
	// texture
	CDymD3DTexture *pTex = m_pTex ;
	if( pKeyFrame->m_pTexture )
		pTex = pKeyFrame->m_pTexture;
	m_pDevice->SetTexture ( 0 , pTex );
	// color
	m_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR,	D3DCOLOR_COLORVALUE( pKeyFrame->colour.x*world.vColor.x , 
		pKeyFrame->colour.y*world.vColor.y , pKeyFrame->colour.z*world.vColor.z , pKeyFrame->colour.w*world.vColor.w) );
	m_pDevice->SetRenderState( D3DRS_FILLMODE,	D3DFILL_SOLID );

	if( materialtype == MT_Effect_sceneadd )
	{
		m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE);
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_ONE);
		m_pDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE);
		m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE ,	FALSE);
		m_pDevice->SetTextureStageState (0,D3DTSS_ALPHAOP,D3DTOP_DISABLE);
		m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE);
	}
	if( materialtype == MT_Effect_sceneaddalpha )
	{
		m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE);
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE);
		m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE ,	FALSE);
		m_pDevice->SetTextureStageState (0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
		m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE);
		m_pDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW );
	}
	else if( materialtype == MT_Effect_sceneno )
	{
		m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	FALSE);
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA);
		m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE ,	TRUE);
		m_pDevice->SetTextureStageState (0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
		m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	TRUE);
		m_pDevice->SetRenderState( D3DRS_ALPHAREF,			0x0000000A);
		m_pDevice->SetRenderState( D3DRS_ALPHAFUNC,		D3DCMP_GREATER);
	}
	else if( materialtype == MT_Effect_sceneab )
	{
		m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE);
		m_pDevice->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCALPHA);
		m_pDevice->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA);
		m_pDevice->SetRenderState( D3DRS_ZWRITEENABLE ,	FALSE);
		m_pDevice->SetTextureStageState (0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
		m_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE,	FALSE);
		m_pDevice->SetRenderState( D3DRS_CULLMODE , D3DCULL_CCW );
	}
	
	if( pTex == NULL )
	{
		m_pDevice->SetTextureStageState (0,D3DTSS_COLOROP,D3DTOP_SELECTARG1);
		m_pDevice->SetTextureStageState (0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	}
	else
	{
		m_pDevice->SetTextureStageState (0,D3DTSS_COLOROP,D3DTOP_MODULATE);
		m_pDevice->SetTextureStageState (0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
	}
	m_pDevice->SetTextureStageState (0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
	m_pDevice->SetTextureStageState (0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState (0,D3DTSS_ALPHAARG1,D3DTA_TFACTOR);
	m_pDevice->SetTextureStageState (0,D3DTSS_ALPHAARG2,D3DTA_TEXTURE);

	D3DXMATRIX matUV;
	if( pKeyFrame->nVersion == 2 )
	{
		D3DXMATRIX Rot,Pos,Sca,RotOf1,RotOf2;
		D3DXMatrixRotationZ( &Rot , pKeyFrame->uvRotationAngle2 *D3DX_PI*2.0f/360.0f );
		D3DXMatrixTranslation( &Pos , -pKeyFrame->uvOffset2 .x, -pKeyFrame->uvOffset2 .y , 0.0f );
		D3DXMatrixScaling( &Sca , 1.0f/pKeyFrame->matUVtile .x , 1.0f/pKeyFrame->matUVtile .y , 1.0f );

		D3DXMatrixTranslation( &RotOf1 , -0.5f , -0.5f , 0 );
		D3DXMatrixTranslation( &RotOf2 , 0.5f , 0.5f , 0 );
		Rot = RotOf1*Rot*RotOf2;

		matUV = Sca * Rot * Pos ;
	}
	else
	{
		matUV = m_KeyFrameManager.GetUVAniMatrix ();
	}
	matUV._31 = matUV._41;
	matUV._32 = matUV._42;
	matUV._33 = matUV._43;
	m_pDevice->SetTransform ( D3DTS_TEXTURE0 , &matUV );
	m_pDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2 );
	// top scaling
	D3DXMATRIX topSca,bottomSca;
	if( pKeyFrame->topscale != 1.0f || pKeyFrame->bottomscale != 1.0f)
	{
		m_pDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_1WEIGHTS );
	}
	else
	{
		m_pDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
	}

	// grass 
	if( world.bGrassMethod )
	{
		//float b ;
		//LARGE_INTEGER litmp; 
		//double dfFreq;
		//QueryPerformanceFrequency(&litmp);
		//LONGLONG d = litmp.LowPart;
		//dfFreq = (double) d;// 获得计数器的时钟频率
		//QueryPerformanceCounter(&litmp);

		D3DXMATRIX rot;
		m_pDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_1WEIGHTS );
		//float a = sinf( timeGetTime()/100.0f + (m_matWorld1._41+1.4f)*(m_matWorld1._42+0.36f)*m_matWorld1._43 );

		float a = sinf( timeGetTime()/500.0f + 0.504  );


		//float time=timeGetTime()/300.0f;

		/*static float aa=0;
		if(aa>10000000.f)aa = 0;
		float a=sinf( aa + 0.504);
		aa+= 0.0007;*/
		float oXF=m_matWorld1._41;
		float oZF=m_matWorld1._43;
		float oX=oXF-(int)oXF;
		float oZ=oZF-(int)oZF;
		D3DXMatrixRotationAxis( &rot, &D3DXVECTOR3(oX+0.01,0,oZ), a/5.0f );

		//D3DXMatrixRotationAxis( &rot, &D3DXVECTOR3(0.707,0,-0.012), a/5.0f );

		D3DXMATRIX m1,m2;
		D3DXMatrixTranslation( &m1 , -m_matWorld1._41 , -m_matWorld1._42 , -m_matWorld1._43 );
		D3DXMatrixTranslation( &m2 ,  m_matWorld1._41 ,  m_matWorld1._42 ,  m_matWorld1._43 );
   		m_matWorld2= m_matWorld1*m1*rot*m2;

		//D3DXMatrixMultiply( &m_matWorld2,&rot,&m_matWorld1);
	}

	m_pDevice->SetTransform ( D3DTS_WORLD1 , &m_matWorld2 );
	m_pDevice->SetTransform ( D3DTS_WORLD  , &m_matWorld1 );
}
void CDymMagicBaseObj::FrameMoveBase(float fCurTime , float fElapsedTime)
{
	m_KeyFrameManager.FrameMove ( fCurTime , fElapsedTime );
}

HRESULT CDymMagicBaseObj::Render(DYM_WORLD_MATRIX world , DWORD flag)
{
	return S_OK;
}

void CDymMagicBaseObj::CopyBaseObj( CDymMagicBaseObj *p )
{
	m_matWorld1 = p->m_matWorld1 ;
	m_matWorld2 = p->m_matWorld2 ;
	m_objType = p->m_objType ;
	materialtype = p->materialtype ;
	m_vBoundingBox1 = p->m_vBoundingBox1 ;
	m_vBoundingBox2 = p->m_vBoundingBox2 ;
	_tcscpy( materialres , p->materialres) ;
	this->m_nID_rotation_with = p->m_nID_rotation_with ;
	this->m_pRotationObj = p->m_pRotationObj ;
	m_pFollowObj = p->m_pFollowObj ;
	if( p == p->m_pRotationObj )
		this->m_pRotationObj = this;
	if( p == p->m_pFollowObj )
		m_pFollowObj = this;
	m_KeyFrameManager.CopyKeyFrame ( &p->m_KeyFrameManager , m_pDevice , TRUE ) ;
	_tcscpy( m_strObjName , p->m_strObjName );
}