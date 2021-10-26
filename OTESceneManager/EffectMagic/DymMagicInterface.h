#pragma once

def_RegisterIGUID( IID_IDymMagicInterface, 0, 0, 1, 0 );
#define IsBoneRotation(p) (p->GetMagicExData()&0x00000001)
#define IsFollowWithHero(p) (p->GetMagicExData()&0x00000002)

class IDymMagicInterface
{
public:
	IDymMagicInterface( ) { };
	virtual ~IDymMagicInterface( ) { };
	virtual void SetDefaultShadowPos( float directionX , float directionY , float directionXZ ,float centerX , float centerY , float centerZ ) = 0;
	virtual void SetDefaultBillboardString( LPCTSTR str , DWORD color , void *pUnknow ) = 0;
	virtual int GetMagicExData() = 0;
	virtual void SetFastRender( BOOL bFast , BOOL bAll ) = 0;
	virtual void SetAllKeyTexture( LPCTSTR texFileName ) = 0;
	virtual void SetLoop( BOOL bLoop ) = 0;
	virtual BOOL GetLoop() = 0;
	virtual void CreateShadow() = 0;
};
