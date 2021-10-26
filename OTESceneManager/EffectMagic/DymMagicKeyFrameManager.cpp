/***************************************************************************************************
文件名称:	DymMagicKeyFrameManager.cpp
系统名称:	纵横时空
模块名称:	特效模块
功能说明:	关键帧管理
相关文档:	
作    者:	董夜明
建立日期:	2006-7
审    核:	左强
软件版权:	卓智时代
***************************************************************************************************/
#include ".\dymmagickeyframemanager.h"

CDymMagicKeyFrameManager::CDymMagicKeyFrameManager(void)
{
	m_pLastKeyFrame = NULL;
	m_pKeyMin = NULL;
	m_pKeyMax = NULL;
	m_pKeyFrameNow = NULL;
	D3DXMatrixTranslation( &m_matUVPos , 0 , 0 , 0 );
	D3DXMatrixRotationX( &m_matUVRot , 0 );
	D3DXMatrixScaling( &m_matUVSca , 1 , 1 , 1 );
	m_fMaxTime = 2000.0f;
	m_fLastFindKeyMinMax = -1.0f;
	m_fCurTime = 0.0f;
	m_fElapsedTime = 0.0f;
}

CDymMagicKeyFrameManager::~CDymMagicKeyFrameManager(void)
{
	ClearKey();
	m_keyFrameForLerp.m_pTexture = NULL;
}

void CDymMagicKeyFrameManager::ClearKey()
{
	for( std::list< CDymMagicKeyFrame* >::iterator it = m_listKeyFrame.begin () ; it != m_listKeyFrame.end () ; it++ )
	{
		CDymMagicKeyFrame *p = *it;
		delete p;
	}
	m_listKeyFrame.clear ();
}

_stdcall bool _SortKey(CDymMagicKeyFrame* p1,CDymMagicKeyFrame* p2)
{
	if(p1->m_fTime <p2->m_fTime )return true;
	return false;
}

void CDymMagicKeyFrameManager::SortKey()
{
	m_listKeyFrame.sort ( _SortKey );
	if( m_listKeyFrame.size () > 0 )
	{
		std::list<CDymMagicKeyFrame*>::iterator it = m_listKeyFrame.end ();
		it--;
		m_pLastKeyFrame = *it;
	}
	else
		m_pLastKeyFrame = NULL;
}
void CDymMagicKeyFrameManager::AddKeyFrame(CDymMagicKeyFrame * pKeyFrame)
{
	m_listKeyFrame.push_back ( pKeyFrame );
	SortKey();
}

CDymMagicKeyFrame* CDymMagicKeyFrameManager::GetKeyFrame(float fTime)
{
	//CDymMagicKeyFrame *pLastKeyFrame = GetLastKeyFrame ();
	//if(fTime>pLastKeyFrame->m_fTime )return NULL;

	//if( m_fLastFindKeyMinMax != fTime )
	{
		FindKeyFrameMinMax( &m_pKeyMin , &m_pKeyMax , fTime );
		m_fLastFindKeyMinMax = fTime ;
	}
	if( m_pKeyMin == NULL || m_pKeyMax == NULL )
		return NULL;
	if( m_pKeyMin == m_pKeyMax )return m_pKeyMin;
	LerpKeyFrame( m_pKeyMin , m_pKeyMax , fTime );
	return &m_keyFrameForLerp;
}

CDymMagicKeyFrame* CDymMagicKeyFrameManager::GetKeyFrame2(float fTime , float fOffset)
{
	for( std::list< CDymMagicKeyFrame* >::iterator it = m_listKeyFrame.begin () ; it != m_listKeyFrame.end () ; it++ )
	{
		CDymMagicKeyFrame *p = *it;
		if( fabsf( p->m_fTime - fTime ) <= fOffset )
			return p;
	}
	return NULL;
}

CDymMagicKeyFrame* CDymMagicKeyFrameManager::CopyKey( CDymMagicKeyFrame *pKey , CDymRenderSystem *pDevice )
{
	CDymMagicKeyFrame * pNewKey = new CDymMagicKeyFrame;
	*pNewKey = *pKey;
	pNewKey->SetMatresName ( pNewKey->matresname , pDevice );
	pNewKey->SetSound ( NULL );
	this->AddKeyFrame ( pNewKey );
	SortKey();
	return pNewKey;
}
void CDymMagicKeyFrameManager::CopyKeyFrame( CDymMagicKeyFrameManager *p , CDymRenderSystem *pDevice , BOOL bCopySound )
{
	for( std::list< CDymMagicKeyFrame* >::iterator it = p->m_listKeyFrame.begin () ; it != p->m_listKeyFrame.end () ; it++ )
	{
		CDymMagicKeyFrame * pKey = *it;
		CDymMagicKeyFrame * pNewKey = new CDymMagicKeyFrame;
		*pNewKey = *pKey;
		pNewKey->SetMatresName ( pNewKey->matresname , pDevice );
		if( !bCopySound )
			pNewKey->SetSound ( NULL );
		this->AddKeyFrame ( pNewKey );
	}
	SortKey();

	m_keyFrameForLerp = p->m_keyFrameForLerp ;
	m_fCurTime = p->m_fCurTime ;
	m_fElapsedTime = p->m_fElapsedTime ;
	m_matUVPos = p->m_matUVPos;
	m_matUVRot = p->m_matUVRot;
	m_matUVSca = p->m_matUVSca;
	m_fMaxTime = p->m_fMaxTime;
	m_fLastFindKeyMinMax = p->m_fLastFindKeyMinMax;

	FrameMove( m_fCurTime , m_fElapsedTime );
}

void CDymMagicKeyFrameManager::FindKeyFrameMinMax(CDymMagicKeyFrame** ppMin, CDymMagicKeyFrame** ppMax,float fTime)
{
	*ppMin = NULL;
	*ppMax = NULL;
	for( std::list< CDymMagicKeyFrame* >::iterator it = m_listKeyFrame.begin () ; it != m_listKeyFrame.end () ; it++ )
	{
		CDymMagicKeyFrame *p = *it;
		if( p->m_fTime <= fTime )
		{
			*ppMin = p;
			*ppMax = p;
			if( p->m_fTime == fTime )break;
		}
		else 
		{
			if( (*ppMin)==NULL ) *ppMin = p;
			*ppMax = p;
			break;
		}
	}
}

void CDymMagicKeyFrameManager::LerpKeyFrame(CDymMagicKeyFrame* p1, CDymMagicKeyFrame* p2, float fLerpTime)
{
	CDymMagicKeyFrame::Lerp ( p1 , p2 , &m_keyFrameForLerp , fLerpTime );
}
CDymMagicKeyFrame *CDymMagicKeyFrameManager::GetLastKeyFrame()
{
	return m_pLastKeyFrame;
}

void CDymMagicKeyFrameManager::FrameMove(float fCurTime , float fElapsedTime)
{
	m_fCurTime = fCurTime;
	m_fElapsedTime = fElapsedTime;
	m_pKeyFrameNow = GetKeyFrame ( fCurTime );
	if(m_pKeyFrameNow)
	{
		// move scale and rotation uv.
		//if(m_pKeyFrameNow->animate)
		{
			D3DXMATRIX Rot,Pos;
			D3DXMatrixRotationZ( &Rot , m_pKeyFrameNow->rotate_anim*fElapsedTime/1000.0f );
			m_matUVRot *= Rot;
			D3DXMatrixTranslation( &Pos , m_pKeyFrameNow->scroll_anim .x*fElapsedTime/1000.0f  , m_pKeyFrameNow->scroll_anim .y*fElapsedTime/1000.0f  , 0.0f );
			m_matUVPos *= Pos;
			D3DXMatrixScaling( &m_matUVSca , 1.0f/m_pKeyFrameNow->matUVtile .x , 1.0f/m_pKeyFrameNow->matUVtile .y , 1.0f );
		}
	}

	/*if( m_fMaxTime == -1.0f )
	{
		CDymMagicKeyFrame *pLastKeyFrame = GetLastKeyFrame ();
		if(pLastKeyFrame)
			if( pLastKeyFrame->m_fTime > 0.0f )
				m_fCurTime = fmodf( m_fCurTime , pLastKeyFrame->m_fTime ); 
	}
	else if( m_fMaxTime == 0.0f )m_fCurTime = 0.0f;
	else m_fCurTime = fmodf( m_fCurTime , m_fMaxTime );*/
}

CDymMagicKeyFrame* CDymMagicKeyFrameManager::GetKeyFrameNow(void)
{
	if( m_pKeyFrameNow == NULL )
		this->FrameMove ( 0 , 0 );
	return m_pKeyFrameNow;
}

D3DXMATRIX CDymMagicKeyFrameManager::GetUVAniMatrix(void)
{
	D3DXMATRIX RotOf1,RotOf2,Rot;
	D3DXMatrixTranslation( &RotOf1 , -0.5f , -0.5f , 0 );
	D3DXMatrixTranslation( &RotOf2 , 0.5f , 0.5f , 0 );
	Rot = RotOf1*m_matUVRot*RotOf2;
	return m_matUVSca * Rot * m_matUVPos;
}

void CDymMagicKeyFrameManager::SetKeyMaxTime(float fMaxTime)
{
	m_fMaxTime = fMaxTime;
}
float CDymMagicKeyFrameManager::GetKeyMaxTime()
{
	return m_fMaxTime;
}

void CDymMagicKeyFrameManager::DeleteKey( CDymMagicKeyFrame *pKey )
{
	if( pKey )
	{
		this->m_listKeyFrame.remove ( pKey );
		delete pKey;
		FrameMove( m_fCurTime , m_fElapsedTime );
	}
}