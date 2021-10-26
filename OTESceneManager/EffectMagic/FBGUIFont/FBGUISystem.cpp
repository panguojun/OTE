#include "FBGUISystem.h"

namespace FBGUILogicLayer
{
// ************************************************************ //
// CCommonHandler
// ************************************************************ //

void CCommonHandler::ClosePopup( CControlButton* pButton )
{
	CControlDialog* tpPopup = (CControlDialog*) pButton->GetParent( );
	pButton->GetPage( )->ClosePopup( tpPopup );
}

// ************************************************************ //
// CControlManager
// ************************************************************ //

// 控件管理器构造函数。
CControlManager::CControlManager( CGUIViewport* pViewport, CGUIRender* pRender, CGUITextureFactory* pFactory ) : mLastPage( -1 ), mCurrentPage( -1 ), mNextPage( -1 ), mpViewport( pViewport )
{
	CControl::spRender = new CRenderDevice;
	CControl::spTextureFactory = pFactory;
	CControl::spRender->SetViewport( mpViewport );
	CControl::spRender->SetRender( pRender );
}

// 控件管理器构造函数。
CControlManager::~CControlManager( )
{
	delete CControl::spRender;
	SetCurrentPage( -1 );
	for ( unsigned int i = 0; i < mPages.size( ); i ++ )
		delete mPages[i];
}

CControlPage* CControlManager::GetPage( int vPageIndex ) const
{
	if ( vPageIndex >= int( mPages.size( ) ) )
		return NULL;

	return mPages[ vPageIndex ];
}

int CControlManager::InputProcess( unsigned int vMsg, unsigned int wParam, unsigned int lParam )
{
	CControlPage* tpCurPage = GetCurrentPage( );
	if ( tpCurPage == NULL )
		return 0x0;

    switch ( vMsg )
	{
	case WM_SIZE:
		{
			RECT tRect;	::GetClientRect( ::GetActiveWindow( ), &tRect );
			int tWidth	= tRect.right - tRect.left;
			int tHeight	= tRect.bottom - tRect.top;
			if ( mpViewport != NULL )
				mpViewport->SetViewportExt( tWidth, tHeight );
			break;
		}
	case WM_CHAR:
		tpCurPage->OnChar( wParam, lParam );
		break;
	case WM_KEYDOWN:
		tpCurPage->OnKeyDown( wParam, lParam );
        break;
	case WM_KEYUP:
		tpCurPage->OnKeyUp( wParam, lParam );
		break;
	case WM_LBUTTONDBLCLK:
		tpCurPage->OnMouseLButtonDown( mpViewport->DP2LP( CGUIPoint( LOWORD( lParam ), HIWORD( lParam ) ) ), wParam );
		tpCurPage->OnMouseLButtonDoubleClick( mpViewport->DP2LP( CGUIPoint( LOWORD( lParam ), HIWORD( lParam ) ) ), wParam );
		break;
	case WM_LBUTTONDOWN:
		tpCurPage->OnMouseLButtonDown( mpViewport->DP2LP( CGUIPoint( LOWORD( lParam ), HIWORD( lParam ) ) ), wParam );
		break;
	case WM_LBUTTONUP:
		tpCurPage->OnMouseLButtonUp( mpViewport->DP2LP( CGUIPoint( LOWORD( lParam ), HIWORD( lParam ) ) ), wParam );
		break;
	case WM_MBUTTONDOWN:
		tpCurPage->OnMouseMButtonDown( mpViewport->DP2LP( CGUIPoint( LOWORD( lParam ), HIWORD( lParam ) ) ), wParam );
        break;
	case WM_MBUTTONUP:
		tpCurPage->OnMouseMButtonUp( mpViewport->DP2LP( CGUIPoint( LOWORD( lParam ), HIWORD( lParam ) ) ), wParam );
		break;
	case WM_RBUTTONDOWN:
		tpCurPage->OnMouseRButtonDown( mpViewport->DP2LP( CGUIPoint( LOWORD( lParam ), HIWORD( lParam ) ) ), wParam );
		break;
	case WM_RBUTTONUP:
		tpCurPage->OnMouseRButtonUp( mpViewport->DP2LP( CGUIPoint( LOWORD( lParam ), HIWORD( lParam ) ) ), wParam );
		break;
	case WM_MOUSEMOVE:
		tpCurPage->OnMouseMove( mpViewport->DP2LP( CGUIPoint( LOWORD( lParam ), HIWORD( lParam ) ) ), wParam );
		break;
	case 0x020a: //WM_MOUSEWHEEL
		tpCurPage->OnMouseWheel( mpViewport->DP2LP( CGUIPoint( LOWORD( lParam ), HIWORD( lParam ) ) ), wParam );
		break;
	}

	CControl* tpHitCtl = tpCurPage->GetSubControl( mpViewport->DP2LP( CGUIPoint( LOWORD( lParam ), HIWORD( lParam ) ) ) );

	return ( tpHitCtl == NULL );
}

// 设置当前显示的游戏界面页。
void CControlManager::SetCurrentPage( int vCurrentPage )
{
	if ( mCurrentPage == vCurrentPage )
		return;

	mLastPage = mCurrentPage;
	if ( mCurrentPage >= 0 )
	{
		mPages[ mCurrentPage ]->ShowPage( false );
		mPages[ mCurrentPage ]->InitControl( false );
	}

	if ( vCurrentPage >= 0 )
	{
		mPages[ vCurrentPage ]->ShowPage( true );
		mPages[ vCurrentPage ]->InitControl( true );
	}
	mCurrentPage = vCurrentPage;
}

// 更新当前游戏界面页的控件状态。
void CControlManager::UpdateCurrentPage( )
{
	if ( mCurrentPage >= 0 )
		mPages[ mCurrentPage ]->UpdateStatus( );
}

// 显示当前游戏界面页。
void CControlManager::DrawCurrentPage( )
{
	CControl::spRender->PrepareRender( );

	if ( mCurrentPage >= 0 )
		mPages[ mCurrentPage ]->Draw( );

	CControl::spRender->AfterRender( );
}

void CControlManager::Draw( )
{
	if ( mNextPage != -1 )
	{
		SetCurrentPage( mNextPage );
		mNextPage = -1;
	}

	if ( mCurrentPage == -1 )
		return;

	UpdateCurrentPage( );
	DrawCurrentPage( );
}

// ************************************************************ //
// CControl
// ************************************************************ //
CRenderDevice*		CControl::spRender			= NULL;
CGUITextureFactory*	CControl::spTextureFactory	= NULL;

// 控件构造函数。
CControl::CControl( unsigned int vStyle, const char* tTitle, const CGUIRect& rRect, unsigned int vID ) : mTitle( tTitle ), mRect( rRect ), mID( vID ), mStyle( vStyle ), mpDragFromHandler( NULL ), mpDragToHandler( NULL ), mpParent( NULL ), mFlags( GUI_CTL_ENABLE ), mpTexture( NULL )
{
}

// 控件析构函数。
CControl::~CControl( )
{
}

CControlPage* CControl::GetPage( ) const
{
	if ( mpParent == NULL )
		return NULL;

	CControl* pParentTemp	= mpParent;
	CControl* pParent		= NULL;
	do
	{
		pParent		= pParentTemp;
		pParentTemp = pParent->GetParent( );
	}	while( pParentTemp != NULL );

	return (CControlPage*) pParent;
}

void CControl::OnFocus( const CGUIPoint& rPoint, bool vFocus )
{
	vFocus ? SetFlag( GUI_CTL_FOCUS ) : UnsetFlag( GUI_CTL_FOCUS );
	mStyle & GUI_CSF_TOPMOST ? GetPage( )->BringControlToTop( this ) : NULL;
	if ( mpParent == NULL ) 
		return;
	mpParent->OnFocus( rPoint, vFocus );
}

void CControl::Offset( const CGUIPoint& rPoint, const CGUISize& rSize )
{
	mRect.SetRect( mRect.mTopLeft + rPoint, mRect.Size( ) + rSize ); 
}

// ************************************************************ //
// CControlDialog
// ************************************************************ //

// 控件对话框构造函数。
CControlDialog::CControlDialog( unsigned int vStyle, const char* tTitle, const CGUIRect& rRect, unsigned int vID ) : CControl( GUI_CSF_CONTAINER | vStyle, tTitle, rRect, vID ), mVScrollBarWidth( 16 ), mHScrollBarHeight( 16 ), mCloseWidth( 16 ), mCloseHeight( 16 ), mBorderLength( 4 ), mCaptionHeight( 18 ), mpVScrollBar( NULL ), mpHScrollBar( NULL )
{
	mpVScrollBar = new CControlScrollBar( GUI_SBF_VERT, CGUIRect( ), 105 );
	mpHScrollBar = new CControlScrollBar( GUI_SBF_HORZ, CGUIRect( ), 106 );
	mpVScrollBar->UnsetFlag( GUI_CTL_VISIBLE );
	mpHScrollBar->UnsetFlag( GUI_CTL_VISIBLE );
	mpVScrollBar->SetScrolledHandler( OnVScrollBar );
	mpHScrollBar->SetScrolledHandler( OnHScrollBar );
	InsertControl( mpVScrollBar, GUI_CR_PAGE );
	InsertControl( mpHScrollBar, GUI_CR_PAGE );

	if ( vStyle & GUI_DSF_SYSMENU )
	{
		mpExit = new CControlImageButton( "DialogExit.png", CGUISize( 16, 16 ), GetCloseRect( ), 60008 );
		mpExit->SetClickedHandler( CCommonHandler::ClosePopup );
		InsertControl( mpExit, GUI_CR_PAGE );
	}
}

// 控件对话框析构函数。
CControlDialog::~CControlDialog( )
{
	ClearControl( );
}

void CControlDialog::OnHScrollBar( CControlScrollBar* pScrollBar )
{
	CControlDialog* tpDialog = (CControlDialog*) pScrollBar->GetParent( );
	tpDialog->mCamPoint.mX = pScrollBar->GetPosition( );
}

void CControlDialog::OnVScrollBar( CControlScrollBar* pScrollBar )
{
	CControlDialog* tpDialog = (CControlDialog*) pScrollBar->GetParent( );
	tpDialog->mCamPoint.mY = pScrollBar->GetPosition( );
}

bool CControlDialog::RefreshDialog( int vLogicWidth, int vLogicHeight )
{
	CGUIRect tClientRect = GetClientRect( );
	bool tNeedRefresh = false;
	if ( vLogicHeight > tClientRect.Height( ) )
	{
		if ( mpVScrollBar->IsFlag( GUI_CTL_VISIBLE ) == false && mStyle & GUI_DSF_VSCROLL )
		{
            mpVScrollBar->SetFlag( GUI_CTL_VISIBLE );
			tNeedRefresh = true;
		}
		mpVScrollBar->SetRange( vLogicHeight, tClientRect.Height( ) );
		mpVScrollBar->SetScrollSize( 1, tClientRect.Height( ) );
	}
	else
	{
		if ( mpVScrollBar->IsFlag( GUI_CTL_VISIBLE ) == true && mStyle & GUI_DSF_VSCROLL )
		{
			mpVScrollBar->UnsetFlag( GUI_CTL_VISIBLE );
			tNeedRefresh = true;
		}
		mpVScrollBar->SetPosition( 0 );
	}

	// 如果超过了最大的横向显示区域,就要显示横向滚动条
	if ( vLogicWidth > tClientRect.Width( ) )
	{
		if ( mpHScrollBar->IsFlag( GUI_CTL_VISIBLE ) == false && mStyle & GUI_DSF_HSCROLL )
		{
            mpHScrollBar->SetFlag( GUI_CTL_VISIBLE );
			tNeedRefresh = true;
		}
		mpHScrollBar->SetRange( vLogicWidth, tClientRect.Width( ) );
		mpHScrollBar->SetScrollSize( 1, tClientRect.Width( ) );
	}
	else
	{
		if ( mpHScrollBar->IsFlag( GUI_CTL_VISIBLE ) == true && mStyle & GUI_DSF_HSCROLL )
		{
			mpHScrollBar->UnsetFlag( GUI_CTL_VISIBLE );
			tNeedRefresh = true;
		}
		mpHScrollBar->SetPosition( 0 );
	}

	if ( tNeedRefresh )
	{
		mpHScrollBar->mRect = GetHScrollBarRect( );
		mpVScrollBar->mRect = GetVScrollBarRect( ); 
		mpVScrollBar->SetRange( vLogicHeight, tClientRect.Height( ) );
		mpHScrollBar->SetRange( vLogicWidth, tClientRect.Width( ) );
	}

	return tNeedRefresh;
}

void CControlDialog::OnMouseWheel( const CGUIPoint& rPoint, unsigned int vFlags )
{
	if ( mpVScrollBar != NULL && mpVScrollBar->IsFlag( GUI_CTL_VISIBLE ) )
        mpVScrollBar->OnMouseWheel( rPoint, vFlags );
}

// 相对于Page坐标
CGUIRect CControlDialog::GetClientRect( ) const
{
	CGUIRect tRect = mRect;
	if ( mStyle & GUI_DSF_BORDER )
		tRect.DeflateRect( mBorderLength, mBorderLength );
	
	if ( mStyle & GUI_DSF_CAPTION )
		tRect.mTopLeft.mY += mCaptionHeight;

	int tSizeCX = tRect.Width( );
	int tSizeCY = tRect.Height( );

	if ( mpVScrollBar != NULL && mpVScrollBar->IsFlag( GUI_CTL_VISIBLE ) )
		tSizeCX = tRect.Width( ) - mVScrollBarWidth;

	if ( mpHScrollBar != NULL && mpHScrollBar->IsFlag( GUI_CTL_VISIBLE ) )
		tSizeCY = tRect.Height( ) - mHScrollBarHeight;

	return CGUIRect( tRect.mTopLeft, CGUISize( tSizeCX, tSizeCY ) );
}

// 得到横向滚动条区域，相对于Page坐标
CGUIRect CControlDialog::GetHScrollBarRect( ) const
{
	if ( mpHScrollBar == NULL || mpHScrollBar->IsFlag( GUI_CTL_VISIBLE ) == false )
		return CGUIRect( );

	int tBorder		= ( mStyle & GUI_DSF_BORDER ) ? mBorderLength : 0;
	int tCaption	= ( mStyle & GUI_DSF_CAPTION ) ? mCaptionHeight : 0;
	CGUIRect tHScrollRect;
	tHScrollRect.mTopLeft.mX		= mRect.mTopLeft.mX + tBorder;
	tHScrollRect.mTopLeft.mY		= mRect.mBottomRight.mY - mHScrollBarHeight - tBorder;
	tHScrollRect.mBottomRight.mY	= mRect.mBottomRight.mY - tBorder;

	if ( mpVScrollBar != NULL && mpVScrollBar->IsFlag( GUI_CTL_VISIBLE ) )
		tHScrollRect.mBottomRight.mX = mRect.mBottomRight.mX - tBorder - mVScrollBarWidth;
	else
		tHScrollRect.mBottomRight.mX = mRect.mBottomRight.mX - tBorder;

	return tHScrollRect;
}

// 得到纵向滚动条区域，相对于Page坐标
CGUIRect CControlDialog::GetVScrollBarRect( ) const
{
	if ( mpVScrollBar == NULL || mpVScrollBar->IsFlag( GUI_CTL_VISIBLE ) == false )
		return CGUIRect( );

	int tBorder		= ( mStyle & GUI_DSF_BORDER ) ? mBorderLength : 0;
	int tCaption	= ( mStyle & GUI_DSF_CAPTION ) ? mCaptionHeight : 0;
	CGUIRect tVScrollRect;
	tVScrollRect.mTopLeft.mX		= mRect.mBottomRight.mX - tBorder - mVScrollBarWidth;
	tVScrollRect.mTopLeft.mY		= mRect.mTopLeft.mY + tCaption + tBorder;
	tVScrollRect.mBottomRight.mX	= mRect.mBottomRight.mX - tBorder;

	if ( mpHScrollBar != NULL && mpHScrollBar->IsFlag( GUI_CTL_VISIBLE ) )
        tVScrollRect.mBottomRight.mY = mRect.mBottomRight.mY - tBorder - mHScrollBarHeight;
	else
		tVScrollRect.mBottomRight.mY = mRect.mBottomRight.mY - tBorder;

	return tVScrollRect;
}

bool CControlDialog::CanMove( const CGUIPoint& rPoint ) const
{
	if ( mStyle & GUI_DSF_CANMOVE )
		return GetCaptionRect( ).PtInRect( rPoint );

	return false;
}

// 得到标题栏区域，如果没有标题栏标记返回空矩形，相对于Page坐标
CGUIRect CControlDialog::GetCaptionRect( ) const
{
	if ( !( mStyle & GUI_DSF_CAPTION ) )
		return CGUIRect( );

	CGUIRect tCaptionRect = mRect;
	tCaptionRect.mBottomRight.mY = tCaptionRect.mTopLeft.mY + mCaptionHeight;

	if ( mStyle & GUI_DSF_BORDER )
		tCaptionRect.DeflateRect( mBorderLength, mBorderLength, mBorderLength, -mBorderLength );

	return tCaptionRect;
}

CGUIRect CControlDialog::GetCloseRect( ) const
{
	int tBorder	= ( mStyle & GUI_DSF_BORDER ) ? mBorderLength : 0;
	CGUIRect tCloseRect;
	tCloseRect.mTopLeft.mX		= mRect.mBottomRight.mX - tBorder - mCloseWidth;
	tCloseRect.mTopLeft.mY		= mRect.mTopLeft.mY + tBorder;
	tCloseRect.mBottomRight.mX	= mRect.mBottomRight.mX - tBorder;
	tCloseRect.mBottomRight.mY	= mRect.mTopLeft.mY + tBorder + mCloseHeight;
	return tCloseRect + CGUIPoint( -1, 1 );
}

// 添加一个子控件。
void CControlDialog::InsertControl( CControl* pControl, unsigned int vArchor )
{
	mChildrenList.push_back( pControl );
	pControl->mpParent = this;
	if ( vArchor == GUI_CR_CLIENT )
        pControl->Offset( GetClientRect( ).mTopLeft );
	else if ( vArchor == GUI_CR_CAPTION )
		pControl->Offset( GetCaptionRect( ).mTopLeft );
	else if ( vArchor == GUI_CR_VSCROLL )
		pControl->Offset( GetVScrollBarRect( ).mTopLeft );
	else if ( vArchor == GUI_CR_HSCROLL )
		pControl->Offset( GetHScrollBarRect( ).mTopLeft );
}

// 删除一个子控件。
void CControlDialog::RemoveControl( CControl* pControl )
{
	for ( unsigned int i = 0; i < mChildrenList.size( ); i ++ )
	{
		CControl* tpControl = mChildrenList[i];

		// 根据 ID 找到相应子控件
		if ( tpControl == pControl )
		{
			tpControl->InitControl( false );
			delete tpControl;
			mChildrenList.erase( mChildrenList.begin( ) + i );
			break;
		}
	}
}

void CControlDialog::TabControl( CControl*& pLastFocus )
{
	if ( pLastFocus != NULL )
		pLastFocus->OnFocus( CGUIPoint( 0, 0 ), false );

	int tLastIndex = 0;
	for ( int i = 0; i < int( mChildrenList.size( ) ); i ++ )
	{
		if ( mChildrenList[i] == pLastFocus )
		{
			tLastIndex = i;
			break;
		}
	}

	for ( int i = 0; i < int( mChildrenList.size( ) ); i ++ )
	{
		if ( mChildrenList[ ( tLastIndex + i + 1 ) % int( mChildrenList.size( ) ) ]->mStyle & GUI_CSF_TABSTOP )
		{
			pLastFocus = mChildrenList[ ( tLastIndex + i + 1 ) % int( mChildrenList.size( ) ) ];
			break;
		}
	}

	if ( pLastFocus != NULL )
		pLastFocus->OnFocus( CGUIPoint( 0, 0 ), true );
}

// 删除所有子控件。
void CControlDialog::ClearControl( )
{
	while ( !mChildrenList.empty( ) )
	{
		delete mChildrenList.at( 0 );
		mChildrenList.erase( mChildrenList.begin( ) );
	}
}

// 根据坐标查找子控件。
CControl* CControlDialog::GetSubControl( const CGUIPoint& rPoint ) const
{
	for ( int i = int( mChildrenList.size( ) ) - 1; i >= 0; i -- )
	{
		CControl* tpControl = mChildrenList[i];
		// 根据控件矩形查找
		if ( tpControl->GetRect( ).PtInRect( rPoint ) )
		{
			if ( !tpControl->IsFlag( GUI_CTL_VISIBLE ) || tpControl->IsFlag( GUI_CTL_DISABLE ) )
				continue;

			// 如果还有子控件，递归查找其下一级控件
			if ( tpControl->mStyle & GUI_CSF_CONTAINER )
			{
				CControl* tpSubControl = ( (CControlDialog*) tpControl )->GetSubControl( rPoint );
				if ( tpSubControl != NULL )
					return tpSubControl;
			}
			return tpControl;
		}
	}

	return NULL;
}

// 根据控件 ID 查找子控件。
CControl* CControlDialog::GetSubControl( unsigned int vID ) const
{
	for ( int i = 0; i < int( mChildrenList.size( ) ); i ++ )
	{
		CControl* tpControl = mChildrenList[i];
		// 根据控件 ID 查找
		if ( tpControl->GetID( ) == vID )
			return tpControl;
		// 如果还有子控件，递归查找其下一级控件
		if ( tpControl->mStyle & GUI_CSF_CONTAINER )
		{
			CControl* tpSubControl = ( (CControlDialog*) tpControl )->GetSubControl( vID );
			if ( tpSubControl != NULL )
				return tpSubControl;
		}
	}

	return NULL;
}

int CControlDialog::GetSubControlCount( )
{
	return int( mChildrenList.size( ) );
}

CControl* CControlDialog::GetSubControlByIndex( int vIndex )
{
	return mChildrenList[ vIndex ];
}

void CControlDialog::CenterControl( CControl* pControl ) const
{
	const CGUIRect&		tRect		 = pControl->GetRect( );
	const CGUIPoint&	tCenterPoint = tRect.CenterPoint( );
	pControl->Offset( mRect.CenterPoint( ) - tCenterPoint );
}

void CControlDialog::InitControl( bool vIsInit )
{
	for ( int i = 0; i < GetSubControlCount( ); i ++ )
	{
		CControl* tpControl = GetSubControlByIndex( i );
		tpControl->InitControl( vIsInit );
	}
}

void CControlDialog::UpdateStatus( )
{
	for ( int i = 0; i < GetSubControlCount( ); i ++ )
	{
		CControl* tpControl = GetSubControlByIndex( i );
		tpControl->UpdateStatus( );
	}
}

void CControlDialog::Offset( const CGUIPoint& rPoint )
{
	for ( int i = 0; i < GetSubControlCount( ); i ++ )
	{
		CControl* tpControl = GetSubControlByIndex( i );
		tpControl->Offset( rPoint );
	}
	CControl::Offset( rPoint );
}

void CControlDialog::Offset( const CGUIPoint& rPoint, const CGUISize& rSize )
{
	if ( mpHScrollBar != NULL )
		mpHScrollBar->Offset( CGUIPoint( 0, rSize.mCY ), CGUISize( rSize.mCX, 0 ) );
	if ( mpVScrollBar != NULL )
		mpVScrollBar->Offset( CGUIPoint( rSize.mCX, 0 ), CGUISize( 0, rSize.mCY ) );

	CControl::Offset( rPoint, rSize );
}

void CControlDialog::Draw( )
{
	for ( int i = 0; i < GetSubControlCount( ); i ++ )
	{
		CControl* tpControl = GetSubControlByIndex( i );
		if ( tpControl->IsFlag( GUI_CTL_VISIBLE ) )
            tpControl->Draw( );
	}
}

// ************************************************************ //
// CControlPopup
// ************************************************************ //

// 弹出式对话框构造函数。
CControlPopup::CControlPopup( unsigned int vDialogStyle, const char* tTitle, const CGUIRect& rRect, unsigned int vID ) 
	: CControlDialog( GUI_CSF_TOPMOST | vDialogStyle, tTitle, rRect, vID )
{
}

// 弹出式对话框析构函数。
CControlPopup::~CControlPopup( )
{
}

void CControlPopup::InitControl( bool vIsInit )
{
	vIsInit ? mpTexture = spTextureFactory->LoadTexture( "CommonDialog.png" ) : delete mpTexture;
	CControlDialog::InitControl( vIsInit );
}

void CControlPopup::Draw( )
{
	CGUIRect tCaptionRect = GetCaptionRect( );
	CGUIRect tOldClipRect = spRender->SetClipRect( mRect & GetParent( )->GetRect( ) );
    spRender->DrawCombine( mpTexture, mRect, CGUIPoint( 0, 0 ), CGUISize( 50, 50 ) );
	if ( mStyle & GUI_DSF_CAPTION )
	{
		spRender->FillRect( tCaptionRect, IsFlag( GUI_CTL_FOCUS ) ? 0xFF400000 : 0xFF222222 );
		spRender->DrawRect( tCaptionRect, 0xFF000000, 0xFF808080 );
		spRender->PrintText( mTitle.c_str( ), tCaptionRect );
	}
	CControlDialog::Draw( );
	spRender->SetClipRect( tOldClipRect );
}

// ************************************************************ //
// CControlPage
// ************************************************************ //

// 游戏主窗口构造函数，默认 ID 是 1。
CControlPage::CControlPage( const CGUIRect& rRect ) : CControlDialog( GUI_CSF_NORMAL, "", rRect, 0x00000001 )
{
	mpLastFocus		= NULL;
	mpLastHover		= NULL;
	mpDragSource	= NULL;
	mpCapture		= NULL;
	mpDragImage		= NULL;
	mpPopupMenu		= NULL;
	mDownPoint.mX	= -1;
	mDownPoint.mY	= -1;
	mToolTipTick	= 0xFFFFFFFF;
}

// 游戏主窗口析构函数。
CControlPage::~CControlPage( )
{
}

void CControlPage::TrackPopupMenu( CControlMenu* pMenu )
{
	mpLastFocus == mpPopupMenu ? mpLastFocus = NULL : NULL;
	mpLastHover == mpPopupMenu ? mpLastHover = NULL : NULL;

	if ( mpPopupMenu != NULL )	
		RemoveControl( mpPopupMenu );

	mpPopupMenu = pMenu;
	mpPopupMenu->InitControl( true );
	InsertControl( mpPopupMenu, GUI_CR_CLIENT );

}

void CControlPage::SetDragImage( CGUITexture* pTexture, const CGUIRect& rSrcRect )
{
	mpDragImage = pTexture;
	mImageRect	= rSrcRect;
}

// 弹出一个对话框。
void CControlPage::CreatePopup( CControlDialog* pControlPopup )
{
	pControlPopup->SetParent( this );

	mPopups.push_back( pControlPopup );

	if ( mpLastFocus != NULL)
		mpLastFocus->OnFocus( CGUIPoint( 0, 0 ), false );

	if ( mpLastHover != NULL )
		mpLastHover->OnHover( CGUIPoint( 0, 0 ), false );

	pControlPopup->mpParent = this;

	mpLastFocus = pControlPopup;
	mpLastFocus->OnFocus( CGUIPoint( 0, 0 ), true );
	mpLastHover = NULL;

	pControlPopup->InitControl( true );
	CenterControl( pControlPopup );
}

void CControlPage::SetFocus( CControl* pFocus )
{
	if ( mpLastFocus != NULL )
        mpLastFocus->OnFocus( CGUIPoint( 0, 0), false );

	mpLastFocus = pFocus; 

	if ( pFocus != NULL )
        pFocus->OnFocus( CGUIPoint( 0, 0 ), true ); 
}

// 关闭指定的弹出对话框。如果指定NULL, 删除所有对话框
void CControlPage::ClosePopup( CControlDialog* pPopup )
{
	mpLastHover		= NULL;
	mpDragSource	= NULL;
	mpLastFocus		= NULL;
	if ( pPopup == NULL )
	{
        while( !mPopups.empty( ) )
		{
			mPopups[0]->InitControl( false );
			delete mPopups[ 0 ];
			mPopups.erase( mPopups.begin( ) );
		}
	}
	else 
	{
		pPopup->UnsetFlag( GUI_CTL_VISIBLE );
		for ( size_t i = 0; i < mPopups.size( ); i ++ )
		{
			if ( mPopups[i] == pPopup )
			{
				mPopups[ i ]->InitControl( false );
				delete mPopups[ i ];
				mPopups.erase( mPopups.begin( ) + i );
				break;
			}
		}
	}
}

// 鼠标左键抬起消息处理函数，根据鼠标的位置，发给相应的子控件。
void CControlPage::OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags )
{
	if ( mpPopupMenu != NULL )
	{
		mpPopupMenu->OnMouseLButtonUp( rPoint, vFlags );
		mpLastFocus == mpPopupMenu ? mpLastFocus = NULL : NULL;
		mpLastHover == mpPopupMenu ? mpLastHover = NULL : NULL;
		mpDragSource == mpPopupMenu ? mpDragSource = NULL : NULL;
		RemoveControl( mpPopupMenu );
		mpPopupMenu = NULL;
	}

	mpDragImage = NULL;
	if ( mpCapture != NULL )
	{
		mpCapture->OnMouseLButtonUp( rPoint, vFlags );
		return;
	}

	mDownPoint.mX = -1;
	mDownPoint.mY = -1;			 

	CControl* tpSubControl = GetSubControl( rPoint );

	// 发送拖放消息
	if ( mpDragSource != NULL && mpDragSource->mpDragFromHandler != NULL )
		( *mpDragSource->mpDragFromHandler )( tpSubControl, mpDragSource, rPoint );

	if ( tpSubControl != NULL && tpSubControl->mpDragToHandler != NULL )
		( *tpSubControl->mpDragToHandler )( tpSubControl, mpDragSource, rPoint );

	mpDragSource = NULL;
	if ( tpSubControl != NULL )
        tpSubControl->OnMouseLButtonUp( rPoint, vFlags );
}

// 鼠标左键按下消息处理函数，根据鼠标的位置，发给相应的子控件。
void CControlPage::OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags )
{
	if ( mpCapture != NULL )
	{
		mpCapture->OnMouseLButtonDown( rPoint, vFlags );
		return;
	}

	CControl* tpNewFocus = GetSubControl( rPoint );

	if ( tpNewFocus != NULL && tpNewFocus->mStyle & GUI_CSF_CONTAINER && ( (CControlDialog*) tpNewFocus )->CanMove( rPoint ) )
		mDownPoint = rPoint;

	// 可以自动设置焦点
	if ( mpLastFocus != tpNewFocus )
	{
		if ( mpLastFocus != NULL )
			mpLastFocus->OnFocus( rPoint, false );

		if ( tpNewFocus != NULL )
			tpNewFocus->OnFocus( rPoint, true );

		// 设置新的焦点控件
		mpLastFocus = tpNewFocus;
	}

	// 如果鼠标左键按下，准备拖放
	if ( mpDragSource == NULL )
		mpDragSource = tpNewFocus;

	if ( tpNewFocus != NULL )
		tpNewFocus->OnMouseLButtonDown( rPoint, vFlags );
}

// 鼠标右键抬起消息处理函数，根据鼠标的位置，发给相应的子控件。
void CControlPage::OnMouseRButtonUp( const CGUIPoint& rPoint, unsigned int vFlags )
{
	if ( mpCapture != NULL )
	{
		mpCapture->OnMouseRButtonUp( rPoint, vFlags );
		return;
	}

	CControl* tpSubControl = GetSubControl( rPoint );

	if ( tpSubControl != NULL )
		tpSubControl->OnMouseRButtonUp( rPoint, vFlags );
}

// 鼠标右键按下消息处理函数，根据鼠标的位置，发给相应的子控件。
void CControlPage::OnMouseRButtonDown( const CGUIPoint& rPoint, unsigned int vFlags )
{
	if ( mpPopupMenu != NULL )
	{
		mpPopupMenu->OnMouseRButtonDown( rPoint, vFlags );
		mpLastFocus == mpPopupMenu ? mpLastFocus = NULL : NULL;
		mpLastHover == mpPopupMenu ? mpLastHover = NULL : NULL;
		mpDragSource == mpPopupMenu ? mpDragSource = NULL : NULL;
		RemoveControl( mpPopupMenu );
		mpPopupMenu = NULL;
	}

	if ( mpCapture != NULL )
	{
		mpCapture->OnMouseRButtonDown( rPoint, vFlags );
		return;
	}

	CControl* tpSubControl = GetSubControl( rPoint );

	if ( tpSubControl != NULL )
		tpSubControl->OnMouseRButtonDown( rPoint, vFlags );
}

// 鼠标中键抬起消息处理函数，根据鼠标的位置，发给相应的子控件。
void CControlPage::OnMouseMButtonUp( const CGUIPoint& rPoint, unsigned int vFlags )
{
	if ( mpCapture != NULL )
	{
		mpCapture->OnMouseMButtonUp( rPoint, vFlags );
		return;
	}

	CControl* tpSubControl = GetSubControl( rPoint );

	if ( tpSubControl != NULL )
		tpSubControl->OnMouseMButtonUp( rPoint, vFlags );
}

// 鼠标中键按下消息处理函数，根据鼠标的位置，发给相应的子控件。
void CControlPage::OnMouseMButtonDown( const CGUIPoint& rPoint, unsigned int vFlags )
{
	if ( mpPopupMenu != NULL )
	{
		mpPopupMenu->OnMouseMButtonDown( rPoint, vFlags );
		mpLastFocus == mpPopupMenu ? mpLastFocus = NULL : NULL;
		mpLastHover == mpPopupMenu ? mpLastHover = NULL : NULL;
		mpDragSource == mpPopupMenu ? mpDragSource = NULL : NULL;
		RemoveControl( mpPopupMenu );
		mpPopupMenu = NULL;
	}

	if ( mpCapture != NULL )
	{
		mpCapture->OnMouseMButtonDown( rPoint, vFlags );
		return;
	}

	CControl* tpSubControl = GetSubControl( rPoint );

	if ( tpSubControl != NULL )
		tpSubControl->OnMouseMButtonDown( rPoint, vFlags );
}

// 鼠标左键双击消息处理函数，根据鼠标的位置，发给相应的子控件。
void CControlPage::OnMouseLButtonDoubleClick( const CGUIPoint& rPoint, unsigned int vFlags )
{
	if ( mpCapture != NULL )
	{
		mpCapture->OnMouseLButtonDoubleClick( rPoint, vFlags );
		return;
	}

	CControl* tpSubControl = GetSubControl( rPoint );

	if ( tpSubControl != NULL )
		tpSubControl->OnMouseLButtonDoubleClick( rPoint, vFlags );
}

// 鼠标右键双击消息处理函数，根据鼠标的位置，发给相应的子控件。
void CControlPage::OnMouseRButtonDoubleClick( const CGUIPoint& rPoint, unsigned int vFlags )
{
	if ( mpCapture != NULL )
	{
		mpCapture->OnMouseRButtonDoubleClick( rPoint, vFlags );
		return;
	}

	CControl* tpSubControl = GetSubControl( rPoint );

	if ( tpSubControl != NULL )
		tpSubControl->OnMouseRButtonDoubleClick( rPoint, vFlags );
}

// 鼠标滚动消息处理函数，根据鼠标的位置，发给焦点控件。
void CControlPage::OnMouseWheel( const CGUIPoint& rPoint, unsigned int vFlags )
{
	if ( mpLastFocus != NULL )
		mpLastFocus->OnMouseWheel( rPoint, vFlags );
}

// 鼠标移动消息处理函数，根据鼠标的位置，发给相应的子控件。
void CControlPage::OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags )
{
	mToolTipTick	= ::GetTickCount( );
	mMousePoint		= rPoint;
	if ( mpCapture != NULL )
	{
		mpCapture->OnMouseMove( rPoint, vFlags );
		return;
	}

	if ( mpLastHover != NULL && mpLastHover->mStyle & GUI_DSF_CANMOVE && mDownPoint.mX != -1 && mDownPoint.mY != - 1 )
	{
		mpLastHover->Offset( rPoint - mDownPoint );
        mDownPoint = rPoint;
		return;
	}

	CControl* tpNewHover = GetSubControl( rPoint );

	if ( mpLastHover != tpNewHover )
	{
		if ( mpLastHover != NULL )
			mpLastHover->OnHover( rPoint, false );

		if ( tpNewHover != NULL )
			tpNewHover->OnHover( rPoint, true );

		// 设置新的悬停控件
		mpLastHover = tpNewHover;
	}

	// 发送鼠标移动的消息
	if ( tpNewHover != NULL )
		tpNewHover->OnMouseMove( rPoint, vFlags );
}

// 键盘按键抬起消息处理函数，发给焦点控件。
void CControlPage::OnKeyUp( unsigned int vKeyCode, unsigned int vFlags )
{
	if ( mpLastFocus != NULL )
		mpLastFocus->OnKeyUp( vKeyCode, vFlags );
}

// 键盘按键按下消息处理函数，发给焦点控件。
void CControlPage::OnKeyDown( unsigned int vKeyCode, unsigned int vFlags )
{
	if ( vKeyCode == VK_TAB )
	{
		int tPopupsCount = int( mPopups.size( ) );
		if ( tPopupsCount == 0 )
			TabControl( mpLastFocus );
		else
			mPopups[ tPopupsCount - 1 ]->TabControl( mpLastFocus );
	}

	if ( mpLastFocus != NULL )
        mpLastFocus->OnKeyDown( vKeyCode, vFlags );
}

void CControlPage::OnSysKeyDown( unsigned int vKeyCode, unsigned int vFlags )
{
	if ( mpLastFocus != NULL )
		mpLastFocus->OnSysKeyDown( vKeyCode, vFlags );
}

// 键盘输入字符消息处理函数，发给焦点控件。
void CControlPage::OnChar( unsigned int vCharCode, unsigned int vFlags )
{
	if ( mpLastFocus != NULL )
		mpLastFocus->OnChar( vCharCode, vFlags );
}

// 根据坐标查找子控件。
CControl* CControlPage::GetSubControl( const CGUIPoint& rPoint )
{
	// 如果有弹出式对话框，优先查找其对应控件
	if ( mPopups.size( ) > 0 )
	{
		CControlDialog* tpPopup = mPopups[ mPopups.size( ) - 1 ];

		if ( !tpPopup->GetRect( ).PtInRect( rPoint ) || !tpPopup->IsFlag( GUI_CTL_VISIBLE ) || tpPopup->IsFlag( GUI_CTL_DISABLE ) )
			return NULL;

		CControl* tpSubControl = tpPopup->GetSubControl( rPoint );

		if ( tpSubControl == NULL )
			return tpPopup;
		else
			return tpSubControl;
	}
	else
	{
		return CControlDialog::GetSubControl( rPoint );
	}
}

// 根据 ID 查找子控件。
CControl* CControlPage::GetSubControl( unsigned int vID )
{
	for ( unsigned int i = 0; i < mPopups.size( ); i ++ )
	{
		if ( mPopups[i]->GetID( ) == vID )
			return mPopups[i];
	}

	return CControlDialog::GetSubControl( vID );
}

// 显示或者关闭页面。
void CControlPage::ShowPage( bool vIsShow )
{
	if ( vIsShow )
	{
		if ( mpLastHover != NULL )
			mpLastHover->OnHover( CGUIPoint( 0, 0 ), false );

		mpLastFocus		= NULL;
		mpLastHover		= NULL;
		mpDragSource	= NULL;
	}
	else
		ClosePopup( NULL );
}

// 更新子控件的状态。
void CControlPage::UpdateStatus( )
{
	CControlDialog::UpdateStatus( );

	for ( unsigned int i = 0; i < mPopups.size( ); i ++ )
		mPopups[i]->UpdateStatus( );
}

void CControlPage::BringControlToTop( CControl* pControl )
{
	for ( unsigned int i = 0; i < mChildrenList.size( ); i ++ )
	{
		CControl* tpControl = mChildrenList[i];

		// 根据 ID 找到相应子控件
		if ( tpControl == pControl )
		{
			mChildrenList.erase( mChildrenList.begin( ) + i );
			mChildrenList.push_back( pControl );
			return;
		}
	}
}

void CControlPage::BringControlToBottom( CControl* pControl )
{
	for ( unsigned int i = 0; i < mChildrenList.size( ); i ++ )
	{
		CControl* tpControl = mChildrenList[i];

		// 根据 ID 找到相应子控件
		if ( tpControl == pControl )
		{
			mChildrenList.erase( mChildrenList.begin( ) + i );
			mChildrenList.insert( mChildrenList.begin( ), pControl );
			return;
		}
	}
}

// 渲染控件。
void CControlPage::Draw( )
{
	CControlDialog::Draw( );

	for ( size_t i = 0; i < mPopups.size( ); i ++ )
		mPopups[i]->Draw( );

	// 渲染拖放图标
	if ( mpDragImage != NULL )
	{
		CGUIRect tDestRect;
		tDestRect.mTopLeft.mX = mMousePoint.mX - mImageRect.Size( ).mCX / 2;
		tDestRect.mTopLeft.mY = mMousePoint.mY - mImageRect.Size( ).mCY / 2;
		tDestRect.mBottomRight.mX = mMousePoint.mX + mImageRect.Size( ).mCX / 2;
		tDestRect.mBottomRight.mY = mMousePoint.mY + mImageRect.Size( ).mCY / 2;
		spRender->BitBlt( mpDragImage, mImageRect, tDestRect, 0xFFFFFFFF );
	}

	// 渲染悬停控件提示信息
	if ( ::GetTickCount( ) - mToolTipTick > 1000 && mpLastHover != NULL )
		mpLastHover->OnDrawToolTip( mMousePoint );
}

// ************************************************************ //
// CControlButton
// ************************************************************ //

// 按钮控件构造函数。
CControlButton::CControlButton( char* tTitle, const CGUIRect& rRect, unsigned int vID )
	: CControl( GUI_CSF_NORMAL, tTitle, rRect, vID )
{
	mpClickedHandler = NULL;
}

// 按钮控件析构函数。
CControlButton::~CControlButton( )
{
}

void CControlButton::InitControl( bool vIsInit )
{
	if ( vIsInit )
		mpTexture = spTextureFactory->LoadTexture( "CommonButton.png" );
	else
		delete mpTexture;

	CControl::InitControl( vIsInit );
}

void CControlButton::Draw( )
{
	CGUIRect tDestRect = mRect;
	if ( IsFlag( GUI_CTL_CLICKED ) )
	{
		spRender->DrawCombine( mpTexture, mRect, CGUIPoint( 0, 90 ), CGUISize( 15, 15 ) );
		tDestRect += CGUIPoint( 1, 1 );
	}
	else if ( IsFlag( GUI_CTL_HOVER ) )
		spRender->DrawCombine( mpTexture, mRect, CGUIPoint( 0, 45 ), CGUISize( 15, 15 ) );
	else if ( IsFlag( GUI_CTL_ENABLE ) )
		spRender->DrawCombine( mpTexture, mRect, CGUIPoint( 0, 0 ), CGUISize( 15, 15 ) );
	else
		spRender->DrawCombine( mpTexture, mRect, CGUIPoint( 0, 135 ), CGUISize( 15, 15 ) );

	spRender->PrintText( mTitle.c_str( ), tDestRect, GUI_DT_VCENTER | GUI_DT_HCENTER );
}

// 鼠标左键抬起消息处理函数。
void CControlButton::OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags )
{
	// 取消按下状态
	UnsetFlag( GUI_CTL_CLICKED );

	// 触发点击事件
	if ( mpClickedHandler != NULL )
		( *mpClickedHandler)( this );
}

// 鼠标左键按下消息处理函数。
void CControlButton::OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags )
{
	// 设置按下状态
	SetFlag( GUI_CTL_CLICKED );
}

// 鼠标移动消息处理函数。
void CControlButton::OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags )
{
	// 如果鼠标左键按下移动到此按钮，设置按下状态
	if ( vFlags & 1 )
		SetFlag( GUI_CTL_CLICKED );
}

// 重载控件获得或失去悬停时的事件处理，及时将控件弹起。
void CControlButton::OnHover( const CGUIPoint& rPoint, bool vHover )
{
	CControl::OnHover( rPoint, vHover );

	if ( !vHover )
	{
		UnsetFlag( GUI_CTL_CLICKED );
	}
}

// ************************************************************ //
// CControlCheck
// ************************************************************ //

// 复选框控件构造函数。
CControlCheck::CControlCheck( char* pTitle, const CGUIRect& rRect, unsigned int vID ) : CControl( GUI_CSF_NORMAL, pTitle, rRect, vID ), mBtnSize( 16, 16 ), mIsChecked( false )
{
	mpCheckedHandler = NULL;
}

// 复选框控件析构函数。
CControlCheck::~CControlCheck( )
{
}

void CControlCheck::SetCheck( bool vCheck )
{
	mIsChecked = vCheck;
}

void CControlCheck::InitControl( bool vIsInit )
{
	GUI_INITEXTURE( vIsInit, mpTexture, "CommonCheck.png" );
	CControl::InitControl( vIsInit );
}

void CControlCheck::Draw( )
{
	CGUIRect tSrcRect;
	if ( IsFlag( GUI_CTL_DISABLE ) )
	{
		tSrcRect.mTopLeft.mX		= 0;
		tSrcRect.mBottomRight.mX	= mBtnSize.mCX;
		tSrcRect.mTopLeft.mY		= mBtnSize.mCY * 6;
		tSrcRect.mBottomRight.mY	= mBtnSize.mCY * 7;
	}
	else if ( IsFlag( GUI_CTL_CLICKED ) )
	{
		if ( mIsChecked )
		{
			tSrcRect.mTopLeft.mX		= 0;
			tSrcRect.mBottomRight.mX	= mBtnSize.mCX;
			tSrcRect.mTopLeft.mY		= mBtnSize.mCY * 5;
			tSrcRect.mBottomRight.mY	= mBtnSize.mCY * 6;
		}
		else
		{
			tSrcRect.mTopLeft.mX		= 0;
			tSrcRect.mBottomRight.mX	= mBtnSize.mCX;
			tSrcRect.mTopLeft.mY		= mBtnSize.mCY * 4;
			tSrcRect.mBottomRight.mY	= mBtnSize.mCY * 5;
		}
	}
	else if ( IsFlag( GUI_CTL_HOVER ) )
	{
		if ( mIsChecked )
		{
			tSrcRect.mTopLeft.mX		= 0;
			tSrcRect.mBottomRight.mX	= mBtnSize.mCX;
			tSrcRect.mTopLeft.mY		= mBtnSize.mCY * 3;
			tSrcRect.mBottomRight.mY	= mBtnSize.mCY * 4;
		}
		else
		{
			tSrcRect.mTopLeft.mX		= 0;
			tSrcRect.mBottomRight.mX	= mBtnSize.mCX;
			tSrcRect.mTopLeft.mY		= mBtnSize.mCY * 1;
			tSrcRect.mBottomRight.mY	= mBtnSize.mCY * 2;
		}
	}
	else
	{
		if ( mIsChecked )
		{
			tSrcRect.mTopLeft.mX		= 0;
			tSrcRect.mBottomRight.mX	= mBtnSize.mCX;
			tSrcRect.mTopLeft.mY		= mBtnSize.mCY * 2;
			tSrcRect.mBottomRight.mY	= mBtnSize.mCY * 3;
		}
		else
		{
			tSrcRect.mTopLeft.mX		= 0;
			tSrcRect.mBottomRight.mX	= mBtnSize.mCX;
			tSrcRect.mTopLeft.mY		= 0;
			tSrcRect.mBottomRight.mY	= mBtnSize.mCY;
		}
	}

	CGUIRect tDestRect = mRect;
	tDestRect.SetRect( mRect.mTopLeft, mBtnSize );
	spRender->BitBlt( mpTexture, tSrcRect, tDestRect, 0xFFFFFFFF );

	CGUIRect tTextRect( CGUIPoint( mRect.mTopLeft + CGUISize( mBtnSize.mCX, 0 ) ), mRect.mBottomRight );
	spRender->PrintText( mTitle.c_str( ), tTextRect );
}

// 鼠标左键抬起消息处理函数。
void CControlCheck::OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags )
{
	// 取消按下状态
	UnsetFlag( GUI_CTL_CLICKED );

	mIsChecked == true ? mIsChecked = false : mIsChecked = true;

	// 触发选择事件
	if ( mpCheckedHandler != NULL )
		( *mpCheckedHandler )( this, mIsChecked );
}

// 鼠标左键按下消息处理函数。
void CControlCheck::OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags )
{
	// 设置按下状态
	SetFlag( GUI_CTL_CLICKED );
}

// 鼠标移动消息处理函数。
void CControlCheck::OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags )
{
	// 如果鼠标左键按下移动到此按钮，设置按下状态
	if ( vFlags & 1 )
		SetFlag( GUI_CTL_CLICKED );
}

// 重载控件获得或失去悬停时的事件处理，及时将控件弹起。
void CControlCheck::OnHover( const CGUIPoint& rPoint, bool vHover )
{
	CControl::OnHover( rPoint, vHover );

	if ( !vHover )
		UnsetFlag( GUI_CTL_CLICKED );
}

// ************************************************************ //
// CCharObject
// ************************************************************ //

CTextObject::CTextObject( )
{
	::ZeroMemory( mText, sizeof( mText ) );
	mCount		= 0;
}

CTextObject::~CTextObject( )
{
}

CGUIRect CTextObject::GetCursorRect( ) const 
{
	return CGUIRect( mRect.mBottomRight.mX - 2, mRect.mTopLeft.mY, mRect.mBottomRight.mX, mRect.mBottomRight.mY ); 
}

int CTextObject::GetTextWidth( ) const
{
	return CControl::spRender->GetTextWidth( mText, int( ::strlen( mText ) ) );
}

// ************************************************************ //
// CControlEdit
// ************************************************************ //

// 输入框控件构造函数。
CControlEdit::CControlEdit( unsigned int vEditStyle, const CGUIRect& rRect, unsigned int vID ) : CControlDialog( GUI_CSF_TABSTOP | GUI_DSF_BORDER | vEditStyle, "", rRect, vID )
{
	mpChangedHandler	= NULL;
	mpKeyDownHandler	= NULL;
	mCursorIndex		= 0;
	mSelectionBegin		= -1;
	mSelectionEnd		= -1;
	mLastTickCount		= 0;
	mTurn				= 0;
	mInputChinese		= false;
}

// 输入框控件析构函数。
CControlEdit::~CControlEdit( )
{
}

void CControlEdit::OnDrawShade( )
{
	spRender->DrawCombine( mpTexture, mRect, CGUIPoint( 0, 0 ), CGUISize( 15, 15 ) );
}

int CControlEdit::GetTextIndex( const CGUIPoint& rPoint ) const
{
	CGUIRect tClientRect = GetClientRect( );
	for ( int i = 0; i < int( mTexts.size( ) ); i ++ )
	{
		CGUIRect tTextRect = mTexts[i].mRect + tClientRect.mTopLeft - mCamPoint;
		if ( tTextRect.PtInRect( rPoint ) )
			return i;
	}

	return -1;
}

void CControlEdit::OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags )
{
	GetPage( )->SetCapture( this );
	mLastTickCount		= 0;
	mTurn				= 0;

	mCursorIndex = GetTextIndex( rPoint );
	if ( mCursorIndex == -1 )
		mCursorIndex = int( mTexts.size( ) );

	UnSelection( );
}

void CControlEdit::OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags )
{
	if ( mTexts.size( ) == 0 )
		return;

	if ( vFlags & MK_LBUTTON )
	{
		int tCurIndex = GetTextIndex( rPoint );
		if ( tCurIndex == -1 )
			return;

		if( tCurIndex < mCursorIndex )
		{
			while( mCursorIndex != tCurIndex )
				CursorLeft( true );
		}
		else if ( tCurIndex > mCursorIndex )
		{
			while( mCursorIndex != tCurIndex )
                CursorRight( true );
		}
	}
}

void CControlEdit::OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags )
{
	GetPage( )->SetCapture( NULL );
}

void CControlEdit::SelectionLeft( int vOffset, int vCursorIndex )
{
	// 如果没有偏移，就返回
	if ( vOffset == 0 )
		return;

	if ( mSelectionBegin == -1 && mSelectionEnd == -1 )
	{
		mSelectionBegin = vCursorIndex + vOffset;
		mSelectionEnd = vCursorIndex + vOffset;
	}

	int tOldCursorIndex = vCursorIndex + vOffset;

	if ( vCursorIndex >= mSelectionBegin )
		mSelectionEnd = vCursorIndex;
	else
	{
		if ( vCursorIndex == 0 && tOldCursorIndex > mSelectionBegin )
		{
			mSelectionEnd	= mSelectionBegin;
			mSelectionBegin = 0;
		}
		else
		{
			mSelectionBegin = vCursorIndex;
		}
	}
}

void CControlEdit::SelectionRight( int vOffset, int vCursorIndex )
{
	// 如果没有偏移，就返回
	if ( vOffset == 0 )
		return;

	if ( mSelectionBegin == -1 && mSelectionEnd == -1 )
	{
		mSelectionBegin = vCursorIndex - vOffset;
		mSelectionEnd = vCursorIndex - vOffset;
	}

	int tOldCursorIndex = vCursorIndex - vOffset;
	if ( vCursorIndex > mSelectionEnd )
	{
		if ( vCursorIndex == mTexts.size( ) && tOldCursorIndex < mSelectionEnd )
		{
			mSelectionBegin = mSelectionEnd;
			mSelectionEnd	= vCursorIndex;
		}
		else
		{
            mSelectionEnd = vCursorIndex;
		}
	}
	else
	{
		mSelectionBegin = vCursorIndex;
	}
}

void CControlEdit::UnSelection( )
{
	mSelectionBegin = -1;
	mSelectionEnd	= -1;
}

void CControlEdit::DeleteSelection( )
{
	for ( int i = mSelectionBegin; i < mSelectionEnd; i ++ )
		mTexts.erase( mTexts.begin( ) + mSelectionBegin );

	if ( mSelectionBegin != mSelectionEnd )
	{
		mCursorIndex = mSelectionBegin;
		mpVScrollBar->SetPosition( 0 );
		mpHScrollBar->SetPosition( 0 );
	}
}

void CControlEdit::VisibleCursor( )
{
	CGUIRect tClientRect = GetClientRect( );
	CGUIRect tCursorRect = GetCursorRect( );
	tCursorRect += tClientRect.mTopLeft - mCamPoint;
	int tOffsetY = tCursorRect.mBottomRight.mY - tClientRect.mBottomRight.mY;
	if ( tOffsetY > 0 ) 
		mpVScrollBar->SetPosition( mpVScrollBar->GetPosition( ) + tOffsetY );

	tOffsetY = tCursorRect.mTopLeft.mY - tClientRect.mTopLeft.mY;
	if ( tOffsetY < 0 )
		mpVScrollBar->SetPosition( mpVScrollBar->GetPosition( ) + tOffsetY );

	int tOffsetX = tCursorRect.mBottomRight.mX - tClientRect.mBottomRight.mX;
	if ( tOffsetX > 0 ) 
		mpHScrollBar->SetPosition( mpHScrollBar->GetPosition( ) + tOffsetX );

	tOffsetX = tCursorRect.mTopLeft.mX - tClientRect.mTopLeft.mX;
	if ( tOffsetX < 0 )
		mpHScrollBar->SetPosition( mpHScrollBar->GetPosition( ) + tOffsetX );
}

void CControlEdit::CalcTextRect( )
{
	int	tLeft			= 0;
	int	tTop			= 0;
	int	tLogicWidth		= 0;
	int	tLogicHeight	= 0;
	CGUIRect tClientRect = GetClientRect( );
	for ( int i = 0; i < int( mTexts.size( ) ); i ++ )
	{
		int tWidth = ( mStyle & GUI_ESF_PASSWORD ? spRender->GetTextWidth( "*", 1 ) : mTexts[i].GetTextWidth( ) );
		if ( tLeft + tWidth > tClientRect.Width( ) && mStyle & GUI_ESF_MUTILINE && !( mStyle & GUI_ESF_AUTOHSCROLL ) )
		{
			tLeft	 = 0;
			tTop	+= spRender->GetFontHeight( );
		}
		else if ( mTexts[i].mText[0] == '\r' && mStyle & GUI_ESF_MUTILINE )
		{
			tWidth	= 2;
			tLeft	= 0;
			tTop	+= spRender->GetFontHeight( );
		}

		mTexts[i].mRect.SetRect( tLeft, tTop, tLeft + tWidth, tTop + spRender->GetFontHeight( ) );
		tLeft		+= ( tWidth == 2 ? 0 : tWidth );
		tLogicWidth = max( tLeft, tLogicWidth );
		tLogicHeight = tTop + spRender->GetFontHeight( );
	}

	RefreshDialog( tLogicWidth, tLogicHeight );
	VisibleCursor( );
}

void CControlEdit::ResetContext( )
{
	mTexts.clear( );
	mCursorIndex		= 0;
	mSelectionBegin		= -1;
	mSelectionEnd		= -1;
	mLastTickCount		= 0;
	mTurn				= 0;
	mInputChinese		= false;
}

// 设置输入框中的字符串。
void CControlEdit::SetEditText( const char* pText )
{
	ResetContext( );
	for ( int i = 0; pText[i] != '\0'; i ++ )
		InsertChar( pText[i] );

	mCursorIndex = int( mTexts.size( ) );
	CalcTextRect( );
}

void CControlEdit::SetSel( int vStartChar, int vEndChar )
{
	vStartChar == -1 ? mSelectionBegin = 0 : mSelectionBegin = vStartChar;
	vEndChar == - 1 ? mSelectionEnd = int( mTexts.size( ) ) : mSelectionEnd = vEndChar;
}

void CControlEdit::Clear( )
{
	DeleteSelection( );
}

void CControlEdit::Copy( )
{
	HWND tWnd = ::GetActiveWindow( );
	int tStrLen = 0;
	for ( int i = mSelectionBegin; i < mSelectionEnd; i ++ )
		tStrLen += mTexts[ i ].mCount;
	
	HANDLE tHandleMemory = ::GlobalAlloc( GHND , tStrLen + 1 );
	BYTE* tpMemory = ( LPBYTE ) GlobalLock( tHandleMemory );
    
	//得到文字
	char tSelectText[ 2048 ] = { 0 };
	int tTextIndex = 0;
	for ( int i = mSelectionBegin; i < mSelectionEnd; i ++ )
	{
		::memcpy( &tSelectText[ tTextIndex ], mTexts[ i ].mText, mTexts[ i ].mCount );
		tTextIndex += mTexts[i].mCount;
	}

	::strcpy( (char*) tpMemory, tSelectText );
	::GlobalUnlock( tHandleMemory );

	::OpenClipboard( tWnd );
	::EmptyClipboard( );
	::SetClipboardData( CF_TEXT, tpMemory );
	::CloseClipboard( );
}

void CControlEdit::Paste( )
{
	DeleteSelection( );

	HWND tWnd = ::GetActiveWindow( );
	// 得到内容
	::OpenClipboard( tWnd );
	HANDLE tHandleMemory = ::GetClipboardData( CF_TEXT );
	if ( tHandleMemory == NULL )
		return;

	size_t tSize = GlobalSize( tHandleMemory );
	BYTE* tpMemory = ( LPBYTE ) GlobalLock( tHandleMemory );

	// 加入字符
	for ( DWORD i = 0; i < tSize - 1; i ++ )
	{
		if ( InsertChar( char( *tpMemory ) ) == false )
			break;

		tpMemory ++;
	}

	::GlobalUnlock( tHandleMemory );
	::CloseClipboard( );

 	// 触发输入内容改变事件
	if ( mpChangedHandler != NULL )
		( *mpChangedHandler )( this );

	UnSelection( );
	CalcTextRect( );
}

void CControlEdit::Cut( )
{
	int tStrLen = 0;
	for ( int i = mSelectionBegin; i < mSelectionEnd; i ++ )
		tStrLen += mTexts[ i ].mCount;

	HWND tWnd			 = ::GetActiveWindow( );
	HANDLE tHandleMemory = ::GlobalAlloc( GHND , tStrLen + 1 );
	BYTE* tpMemory = ( LPBYTE ) GlobalLock( tHandleMemory );
    
	//得到文字
	char tSelectText[2048] = { 0 };
	int tTextIndex = 0;
	for ( int i = mSelectionBegin; i < mSelectionEnd; i ++ )
	{
		::memcpy( &tSelectText[ tTextIndex ], mTexts[ i ].mText, mTexts[ i ].mCount );
		tTextIndex += mTexts[i].mCount;
	}

	::strcpy( (char*) tpMemory, tSelectText );
	::GlobalUnlock( tHandleMemory );

	::OpenClipboard( tWnd );
	::EmptyClipboard( );
	::SetClipboardData( CF_TEXT, tpMemory );
	::CloseClipboard( );	

	DeleteSelection( );
	UnSelection( );
	CalcTextRect( );
}

void CControlEdit::LineScroll( int vLine )
{
	mpVScrollBar->SetPosition( vLine * spRender->GetFontHeight( ) );
}

// 得到输入框中的字符串。
std::string CControlEdit::GetEditText( )
{
	char tText[ 2048 ] = { 0 };
	int tTextIndex = 0;
	for ( int i = 0; i < int( mTexts.size( ) ); i ++ )
	{
		::memcpy( &tText[tTextIndex], mTexts[i].mText, mTexts[i].mCount );
		tTextIndex += mTexts[i].mCount;
	}

	return std::string( tText );
}

void CControlEdit::OnFocus( const CGUIPoint& rPoint, bool vFocus )
{
	vFocus == false ? UnSelection( ) : SetSel( -1, -1 );
	CControl::OnFocus( rPoint, vFocus );
}

void CControlEdit::CursorUp( int vShiftDown )
{
	if ( mCursorIndex == 0 )
		return;

	std::vector<CGUIRect> tRects;
	std::vector<int> tIndexs;
	CGUIRect tHitRect = GetCursorRect( ) - CGUIPoint( 0, spRender->GetFontHeight( ) );

	for ( int i = 1; i < mCursorIndex; i ++ )
	{
		if ( mTexts[i - 1].GetCursorRect( ).CenterPoint( ).mY == tHitRect.CenterPoint( ).mY )
		{
			tRects.push_back( mTexts[i - 1].GetCursorRect( ) );
			tIndexs.push_back( i );
		}
	}
	
	int tMinLength = 0x0FFFFFFF;
	int tCursorIndex = 0;
	for ( int i = 0; i < int( tRects.size( ) ); i ++ )
	{
		if ( abs( tRects[i].CenterPoint( ).mX - tHitRect.CenterPoint( ).mX ) < tMinLength )
		{
			tCursorIndex = tIndexs[i];
			tMinLength = abs( tRects[i].CenterPoint( ).mX - tHitRect.CenterPoint( ).mX );
		}
	}

	if ( tHitRect.Width( ) == 2 && tCursorIndex == 1 )
		tCursorIndex = 0;
	
	while( mCursorIndex > tCursorIndex )
		CursorLeft( vShiftDown );
}

void CControlEdit::CursorDown( int vShiftDown )
{
	std::vector< CGUIRect > tRects;
	std::vector< int >		tIndexs;
	CGUIRect tHitRect = GetCursorRect( ) + CGUIPoint( 0, spRender->GetFontHeight( ) );
	
	for ( int i = int( mTexts.size( ) ); i > mCursorIndex; i -- )
	{
		if ( mTexts[i - 1].GetCursorRect( ).CenterPoint( ).mY == tHitRect.CenterPoint( ).mY )
		{
			tRects.push_back( mTexts[i - 1].GetCursorRect( ) );
			tIndexs.push_back( i );
		}
	}
	
	int tMinLength = 0x0FFFFFFF;
	int tCursorIndex = int( mTexts.size( ) );
	for ( int i = 0; i < int( tRects.size( ) ); i ++ )
	{
		if ( abs( tRects[i].CenterPoint( ).mX - tHitRect.CenterPoint( ).mX ) < tMinLength )
		{
			tCursorIndex = tIndexs[i];
			tMinLength = abs( tRects[i].CenterPoint( ).mX - tHitRect.CenterPoint( ).mX );
		}
	}

	while( mCursorIndex < tCursorIndex )
		CursorRight( vShiftDown );
}

void CControlEdit::CursorRight( int vShiftDown )
{
	if ( mCursorIndex < int( mTexts.size( ) ) )
	{
		mCursorIndex += 1;
		if ( vShiftDown )
			SelectionRight( 1, mCursorIndex );					
	}

	if ( vShiftDown == false )
		UnSelection( );
}

void CControlEdit::CursorLeft( int vShiftDown )
{
	if ( mCursorIndex > 0 )
	{
		mCursorIndex -= 1;
		if ( vShiftDown )
			SelectionLeft( 1, mCursorIndex );
	}

	if ( vShiftDown == false )
		UnSelection( );
}

bool CControlEdit::InsertChar( char vCharCode )
{
	if ( vCharCode & 0x80 && mInputChinese == false )
	{
		mCurrentInput.mCount	= 2;
		mCurrentInput.mText[0]	= vCharCode;
		mInputChinese = true;
		return true;
	}
	else if ( mInputChinese == true )
	{
		mCurrentInput.mText[1] = vCharCode;
		mInputChinese = false;
	}
	// 如果是新行，忽略
	else if ( vCharCode == '\n' )
		return false;
	// 单行输入框忽略回车
	else if ( vCharCode == VK_RETURN && mStyle & GUI_ESF_SINGLELINE )
		return false;
	else
	{
		int tCount = int( mTexts.size( ) );
		CGUIRect tClientRect = GetClientRect( );
		if ( tCount > 0 )
		{
			CGUIRect tLastRect = mTexts[tCount - 1].mRect + tClientRect.mTopLeft - mCamPoint;
			// 如果是多行输入框，而且没有设置自动纵向滚动，满屏后忽略回车
			if ( mStyle & GUI_ESF_MUTILINE && !( mStyle & GUI_ESF_AUTOVSCROLL ) )
			{
				if ( tLastRect.mBottomRight.mY + spRender->GetFontHeight( ) >= tClientRect.mBottomRight.mY )
				{
					if ( vCharCode == VK_RETURN )
						return false;
				}
			}

			if ( ( !( mStyle & GUI_ESF_AUTOHSCROLL ) ) && tLastRect.mBottomRight.mX + spRender->GetMaxTextWidth( ) >= tClientRect.mBottomRight.mX )
				return false;
		}

		mCurrentInput.mCount	= 1;
		mCurrentInput.mText[0]	= vCharCode;
		mCurrentInput.mText[1]	= 0;
	}
	
	mTexts.insert( mTexts.begin( ) + ( mCursorIndex ++ ), mCurrentInput );
	return true;
}

// 键盘按键按下消息处理函数，负责处理控制光标移动的几个按键的消息。
void CControlEdit::OnKeyDown( unsigned int vKeyCode, unsigned int vFlags )
{
	if ( mStyle & GUI_ESF_READONLY )
		return;

	int tShiftDown		= ::GetKeyState( VK_SHIFT ) & 0x80;
	int tControlDown	= ::GetKeyState( VK_CONTROL ) & 0x80;
	mLastTickCount		= 0;
	mTurn				= 0;

	switch ( vKeyCode )
	{
		case 'X':
		{
			if ( tControlDown )
				Cut( );
			break;
		}
		case 'C':
		{
			if ( tControlDown )
				Copy( );

			break;
		}
		case 'V':
		{
			if ( tControlDown )
				Paste( );
			break;
		}
		case VK_LEFT:		// 光标左移，左移需要特殊处理
		{
			CursorLeft( tShiftDown );
			VisibleCursor( );
			break;
		}
		case VK_UP:
		{
			CursorUp( tShiftDown );
			VisibleCursor( );
			break;
		}
		case VK_DOWN:
		{
			CursorDown( tShiftDown );
			VisibleCursor( );
			break;
		}
		case VK_RIGHT:		// 光标右移
		{
			CursorRight( tShiftDown );
			VisibleCursor( );
			break;
		}
		case VK_HOME:		// 光标移到开始
		{
			int tOldCursorIndex = mCursorIndex;
			mCursorIndex = 0;
			if ( tShiftDown )
				SelectionLeft( tOldCursorIndex - mCursorIndex, mCursorIndex );
			else
				UnSelection( );
			
			VisibleCursor( );
			break;
		}
		case VK_END:		// 光标移到最后
		{
			CGUIRect tTextRect = GetClientRect( );
			int tOldCursorIndex = mCursorIndex;
			mCursorIndex = int( mTexts.size( ) );
			if ( tShiftDown )
				SelectionRight( mCursorIndex - tOldCursorIndex, mCursorIndex );
			else
				UnSelection( );

			VisibleCursor( );
			break;
		}
		case VK_DELETE:		// 删除光标所指字符
		{
			if ( mSelectionBegin == mSelectionEnd && mTexts.size( ) != mCursorIndex )
				mTexts.erase( mTexts.begin( ) + mCursorIndex );
			else if ( mSelectionBegin != mSelectionEnd )
			{
				DeleteSelection( );
				UnSelection( );
			}
			CalcTextRect( );

			// 触发输入内容改变事件
			if ( mpChangedHandler != NULL )
				( *mpChangedHandler )( this );

			break;
		}
	}

	if ( mpKeyDownHandler != NULL )
		mpKeyDownHandler( this, vKeyCode, vFlags );

}

// 键盘输入字符消息处理函数，改变输入框的内容。
void CControlEdit::OnChar( unsigned int vCharCode, unsigned int vFlags )
{
	if ( GetKeyState( VK_CONTROL ) & 0x80 || mStyle & GUI_ESF_READONLY )
		return;

	if ( vCharCode == VK_ESCAPE || vCharCode == VK_TAB )
		return;

	mLastTickCount		= 0;
	mTurn				= 0;
	if ( vCharCode == VK_BACK )
	{
		if ( mSelectionBegin == mSelectionEnd )
		{
			// 退格键，删掉前一个字符				  
			if ( mCursorIndex > 0 )
				mTexts.erase( mTexts.begin( ) + ( -- mCursorIndex ) );
		}
		else
		{
			DeleteSelection( );
            UnSelection( );
		}
	}
	else				 
	{
		DeleteSelection( );
		UnSelection( );
		if ( InsertChar( vCharCode ) && vCharCode == VK_RETURN )
			mpHScrollBar->SetPosition( 0 );
	}

	CalcTextRect( );
	// 触发输入内容改变事件
	if ( mpChangedHandler != NULL )
		( *mpChangedHandler )( this );
}

CGUIRect CControlEdit::GetCursorRect( ) const 
{
	CGUIRect tClientRect = GetClientRect( );
	CGUIRect tCursorRect;
	if ( mCursorIndex == 0 )
		tCursorRect.SetRect( CGUIPoint( 0, 0 ), CGUISize( 2, spRender->GetFontHeight( ) ) );
	else
		tCursorRect = mTexts[ mCursorIndex - 1 ].GetCursorRect( );
	
	return tCursorRect;
}

void CControlEdit::Draw( )
{
	OnDrawShade( );
	CGUIRect tClientRect = GetClientRect( );
	CGUIRect tLastClipRect = spRender->SetClipRect( tClientRect );
	// 渲染文本
	for ( size_t i = 0; i < mTexts.size( ); i ++ )
	{
		CTextObject* tpTextObj = &mTexts[i];
		CGUIRect tCharRect = tpTextObj->mRect + tClientRect.mTopLeft - mCamPoint;
		if ( tpTextObj->mText[0] == '\r' )
			continue;

		if ( i >= size_t( mSelectionBegin ) && i < size_t( mSelectionEnd ) )
			spRender->FillRect( tCharRect, 0xAF808080 );

		if ( mStyle & GUI_ESF_PASSWORD )
            spRender->PrintText( "*", tCharRect, GUI_DT_HCENTER );
		else
            spRender->PrintText( tpTextObj->mText, tCharRect, GUI_DT_BOTTOM );
	}

	// 渲染鼠标光标
	unsigned int tThisTickCount = ::GetTickCount( );
	if ( tThisTickCount - mLastTickCount > 500 )
	{
		mTurn ++;
		mLastTickCount	= tThisTickCount;
	}
	
	if ( mTurn % 2 == 1 && IsFlag( GUI_CTL_FOCUS ) )
	{
		CGUIRect tCursorRect = GetCursorRect( );
		spRender->FillRect( tCursorRect + tClientRect.mTopLeft - mCamPoint, 0xFFFFFFFF );
	}

	spRender->SetClipRect( tLastClipRect );
	CControlDialog::Draw( );
}

void CControlEdit::InitControl( bool vIsInit )
{
	vIsInit ? mpTexture = spTextureFactory->LoadTexture( "CommonEdit.png" ) : delete mpTexture;
	CControlDialog::InitControl( vIsInit );
}

// ************************************************************ //
// CControlComboBox
// ************************************************************ //

CControlComboBox::CControlComboBox( const CGUIRect& rRect, unsigned int vID ) : CControlDialog( GUI_CSF_TOPMOST, "", rRect, vID ), mComboHeight( 20 ), mListHeight( 0 )
{
	CControlListBox* tpListBox = new CControlListBox( GUI_LCS_MOUSEOVER, GetListRect( ) - CGUIRect( 3, 3, 3, 3 ), IDC_COMBODIALOG_LIST_ITEMLIST );
	tpListBox->SetClickedHandler( OnComboItemClicked );
	InsertControl( tpListBox, GUI_CR_PAGE );
	ComboExpand( false );
}

CControlComboBox::~CControlComboBox( )
{
}

void CControlComboBox::ComboExpand( bool vExpand )
{
	CControlListBox* tpListBox = (CControlListBox*) GetSubControl( IDC_COMBODIALOG_LIST_ITEMLIST );
	CGUIRect tListRect = tpListBox->GetRect( ) + CGUIRect( 3, 3, 3, 3 );
	if ( vExpand == false )
	{
		tpListBox->UnsetFlag( GUI_CTL_VISIBLE );
		mRect = GetComboRect( );
	}
	else
	{
		tpListBox->SetFlag( GUI_CTL_VISIBLE );
		mRect.UnionRect( mRect, tListRect );
	}
}

void CControlComboBox::ComboExpand( )
{
	CControlListBox* tpListBox = (CControlListBox*) GetSubControl( IDC_COMBODIALOG_LIST_ITEMLIST );
	CGUIRect tListRect = tpListBox->GetRect( ) + CGUIRect( 3, 3, 3, 3 );
	if ( IsComboExpanded( ) )
	{
		tpListBox->UnsetFlag( GUI_CTL_VISIBLE );
		mRect = GetComboRect( );
	}
	else
	{
		tpListBox->SetFlag( GUI_CTL_VISIBLE );
		mRect.UnionRect( mRect, tListRect );
	}
}

void CControlComboBox::OnFocus( const CGUIPoint& rPoint, bool vFocus )
{
	if ( mRect.PtInRect( rPoint ) == false )
		ComboExpand( false );

	CControlDialog::OnFocus( rPoint, vFocus );
}

void CControlComboBox::OnComboItemClicked( CControlListBox* pListBox, int vItemIndex )
{
	if ( vItemIndex == -1 )
		return;

	std::string tItemText = pListBox->GetItemText( vItemIndex );
	CControlComboBox* tpComboBox = (CControlComboBox*) pListBox->GetParent( );
	tpComboBox->SetTitle( tItemText.c_str( ) );
	tpComboBox->ComboExpand( false );
}

bool CControlComboBox::IsComboExpanded( ) const
{
	CControlListBox* tpListBox = (CControlListBox*) GetSubControl( IDC_COMBODIALOG_LIST_ITEMLIST );
	return tpListBox->IsFlag( GUI_CTL_VISIBLE );
}

void CControlComboBox::OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags )
{
	CControlListBox* tpListBox = (CControlListBox*) GetSubControl( IDC_COMBODIALOG_LIST_ITEMLIST );
	if ( GetComboRect( ).PtInRect( rPoint ) == true )
		ComboExpand( );

	CControlDialog::OnMouseLButtonDown( rPoint, vFlags );
}

CGUIRect CControlComboBox::GetListRect( ) const
{
	CGUIRect tClientRect	= GetClientRect( );
	CGUIRect tListRect		= tClientRect; tListRect.mTopLeft.mY = tClientRect.mTopLeft.mY + mComboHeight;
	return tListRect;
}

CGUIRect CControlComboBox::GetComboRect( ) const
{
	CGUIRect tClientRect	= GetClientRect( );
	CGUIRect tComboRect		= tClientRect; tComboRect.mBottomRight.mY = tClientRect.mTopLeft.mY + mComboHeight;
	return tComboRect;
}

void CControlComboBox::AppendItem( const char* pText )
{
	CControlListBox* pListBox = (CControlListBox*) GetSubControl( IDC_COMBODIALOG_LIST_ITEMLIST );
	pListBox->AppendItem( pText );
}

void CControlComboBox::Draw( )
{
	CGUIRect tComboRect = GetComboRect( );
	CGUIRect tListRect	= GetListRect( );
	spRender->DrawCombine( mpTexture, tComboRect, CGUIPoint( 0, 0 ), CGUISize( 15, 15 ) );
	spRender->DrawRect( tComboRect - CGUIRect( 2, 2, 2, 2 ), 0xFF000000, 0xFF808080 );
	CGUIRect tOldClipRect = spRender->SetClipRect( tComboRect - CGUIRect( 3, 3, 3, 3 ) );
	spRender->PrintText( mTitle.c_str( ), tComboRect - CGUIRect( 3, 3, 3, 3 ) );
	spRender->SetClipRect( tOldClipRect );

	if ( IsComboExpanded( ) )
	{
        spRender->DrawCombine( mpTexture, tListRect, CGUIPoint( 0, 0 ), CGUISize( 15, 15 ) );
		spRender->DrawRect( tListRect - CGUIRect( 2, 2, 2, 2 ), 0xFF000000, 0xFF808080 );
	}
	CControlDialog::Draw( );
}

void CControlComboBox::InitControl( bool vIsInit )
{
	vIsInit ? mpTexture = spTextureFactory->LoadTexture( "CommonMenu.png" ) : delete mpTexture;
	CControlDialog::InitControl( vIsInit );
}

// ************************************************************ //
// CControlScrollBar
// ************************************************************ //

// 滚动条控件构造函数。
CControlScrollBar::CControlScrollBar( unsigned int vScrollStyle, const CGUIRect& rRect, unsigned int vID )
	: CControl( vScrollStyle, "", rRect, vID ), mMouseCursor( -1, -1 )
{
	mpScrolledHandler	= NULL;
	mRangeMax			= 1;
	mRangeMin			= 1;
	mLineSize			= 0;
	mPageSize			= 0;
	mPosition			= 0;
	mClickArea			= GUI_SBR_NONE;
	mScrollPoint		= 0;
	mLastTickCount		= 0;
	SetPosition( 0 );

	// 设置两个滚动键和一个滚动光标的标志位
	mPageUpFlags	= GUI_CTL_ENABLE;
	mPageDownFlags	= GUI_CTL_ENABLE;
	mCursorFlags	= GUI_CTL_ENABLE;

	if ( mStyle == GUI_SBF_VERT )
	{
		mPageBtnSize		= CGUISize( 16, 16 );
		mCursorPrefixSize	= CGUISize( 16, 11 );
		mScrollSliderSize	= CGUISize( 16, 60 );
		mCursorSize			= CGUISize( 16, 20 );
	}
	else
	{
		mPageBtnSize		= CGUISize( 16, 16 );
		mCursorPrefixSize	= CGUISize( 11, 16 );
		mScrollSliderSize	= CGUISize( 60, 16 );
		mCursorSize			= CGUISize( 20, 16 );
	}
}

// 滚动条控件析构函数。
CControlScrollBar::~CControlScrollBar( )
{
}

CGUIRect CControlScrollBar::GetPageUpRect( ) const
{
	CGUISize tBtnSize;
	mStyle == GUI_SBF_VERT ? tBtnSize = CGUISize( mRect.Width( ), 16 ) : tBtnSize = CGUISize( 16, mRect.Height( ) );
	return CGUIRect( mRect.mTopLeft, tBtnSize );
}

CGUIRect CControlScrollBar::GetPageDownRect( ) const
{
	CGUISize tBtnSize;
	mStyle == GUI_SBF_VERT ? tBtnSize = CGUISize( mRect.Width( ), 16 ) : tBtnSize = CGUISize( 16, mRect.Height( ) );
	return CGUIRect( CGUIPoint( mRect.mBottomRight.mX - tBtnSize.mCX, mRect.mBottomRight.mY - tBtnSize.mCY ), tBtnSize );
}

int CControlScrollBar::GetMinCursorLength( ) const
{
	if ( mStyle == GUI_SBF_HORZ )
        return mCursorPrefixSize.mCX * 2;
	else 
		return mCursorPrefixSize.mCY * 2;
}

void CControlScrollBar::Draw( )
{
	CGUIRect tNormalPageUp( CGUIPoint( 0, 0 ), mPageBtnSize );
	CGUIRect tHoverPageUp( CGUIPoint( 0, mPageBtnSize.mCY ), mPageBtnSize );
	CGUIRect tClickPageUp( CGUIPoint( 0, mPageBtnSize.mCY * 2 ), mPageBtnSize );

	CGUIRect tNormalPageDown( CGUIPoint( mPageBtnSize.mCX, 0 ), mPageBtnSize );
	CGUIRect tHoverPageDown( CGUIPoint( mPageBtnSize.mCX, mPageBtnSize.mCY ), mPageBtnSize );
	CGUIRect tClickPageDown( CGUIPoint( mPageBtnSize.mCX, mPageBtnSize.mCY * 2 ), mPageBtnSize );

	CGUIRect tNormalCursorUp( CGUIPoint( mPageBtnSize.mCX * 2, 0 ), mCursorPrefixSize );
	CGUIRect tHoverCursorUp( CGUIPoint( mPageBtnSize.mCX * 2, mCursorPrefixSize.mCY ), mCursorPrefixSize );
	CGUIRect tClickCursorUp( CGUIPoint( mPageBtnSize.mCX * 2, mCursorPrefixSize.mCY * 2 ), mCursorPrefixSize );

	CGUIRect tNormalCursorDown( CGUIPoint( mPageBtnSize.mCX * 2 + mCursorPrefixSize.mCX, 0 ), mCursorPrefixSize );
	CGUIRect tHoverCursorDown( CGUIPoint( mPageBtnSize.mCX * 2 + mCursorPrefixSize.mCX, mCursorPrefixSize.mCY ), mCursorPrefixSize );
	CGUIRect tClickCursorDown( CGUIPoint( mPageBtnSize.mCX * 2 + mCursorPrefixSize.mCX, mCursorPrefixSize.mCY * 2 ), mCursorPrefixSize );

	CGUIRect tNormalCursor( CGUIPoint( mPageBtnSize.mCX * 2 + mCursorPrefixSize.mCX * 2 + mScrollSliderSize.mCX, 0 ), mCursorSize );
	CGUIRect tHoverCursor( CGUIPoint( mPageBtnSize.mCX * 2 + mCursorPrefixSize.mCX * 2 + mScrollSliderSize.mCX, mCursorSize.mCY ), mCursorSize );
	CGUIRect tClickCursor( CGUIPoint( mPageBtnSize.mCX * 2 + mCursorPrefixSize.mCX * 2 + mScrollSliderSize.mCX, mCursorSize.mCY * 2 ), mCursorSize );

	CGUIRect tSrcSlider( CGUIPoint( mPageBtnSize.mCX * 2 + mCursorPrefixSize.mCX * 2, 0 ), mScrollSliderSize );

	if ( mStyle == GUI_SBF_HORZ )
	{
		tNormalPageUp.SetRect( CGUIPoint( 0, 0 ), mPageBtnSize );
		tHoverPageUp.SetRect( CGUIPoint( mPageBtnSize.mCX, 0 ), mPageBtnSize );
		tClickPageUp.SetRect( CGUIPoint( mPageBtnSize.mCX * 2, 0 ), mPageBtnSize );

		tNormalPageDown.SetRect( CGUIPoint( 0, mPageBtnSize.mCY ), mPageBtnSize );
		tHoverPageDown.SetRect( CGUIPoint( mPageBtnSize.mCX, mPageBtnSize.mCY ), mPageBtnSize );
		tClickPageDown.SetRect( CGUIPoint( mPageBtnSize.mCX * 2, mPageBtnSize.mCY ), mPageBtnSize );

		tNormalCursorUp.SetRect( CGUIPoint( 0, mPageBtnSize.mCX * 2 ), mCursorPrefixSize );
		tHoverCursorUp.SetRect( CGUIPoint( mCursorPrefixSize.mCX, mPageBtnSize.mCY * 2 ), mCursorPrefixSize );
		tClickCursorUp.SetRect( CGUIPoint( mCursorPrefixSize.mCX * 2, mPageBtnSize.mCY * 2 ), mCursorPrefixSize );

		tNormalCursorDown.SetRect( CGUIPoint( 0, mPageBtnSize.mCY * 2 + mCursorPrefixSize.mCY ), mCursorPrefixSize );
		tHoverCursorDown.SetRect( CGUIPoint( mCursorPrefixSize.mCX, mPageBtnSize.mCY * 2 + mCursorPrefixSize.mCY ), mCursorPrefixSize );
		tClickCursorDown.SetRect( CGUIPoint( mCursorPrefixSize.mCX * 2, mPageBtnSize.mCY * 2 + mCursorPrefixSize.mCY ), mCursorPrefixSize );

		tNormalCursor.SetRect( CGUIPoint( 0, mPageBtnSize.mCY * 2 + mCursorPrefixSize.mCY * 2 + mScrollSliderSize.mCY ), mCursorSize );
		tHoverCursor.SetRect( CGUIPoint( mCursorSize.mCX, mPageBtnSize.mCY * 2 + mCursorPrefixSize.mCY * 2 + mScrollSliderSize.mCY ), mCursorSize );
		tClickCursor.SetRect( CGUIPoint( mCursorSize.mCX * 2, mPageBtnSize.mCY * 2 + mCursorPrefixSize.mCY * 2 + mScrollSliderSize.mCY ), mCursorSize );

		tSrcSlider.SetRect( CGUIPoint( 0, mPageBtnSize.mCY * 2 + mCursorPrefixSize.mCY * 2 ), mScrollSliderSize );
	}

	CGUIRect tSrcPageUp;
	CGUIRect tSrcPageDown;
	CGUIRect tSrcCursorUp;
	CGUIRect tSrcCursorDown;
	CGUIRect tSrcCursor;
	CGUIRect tCursorUp;
	CGUIRect tCursorDown;
	CGUIRect tCursor;
	CGUIRect tSlider;
	CGUIRect& tPageUpRect = GetPageUpRect( );
	CGUIRect& tPageDownRect = GetPageDownRect( );

	if ( IsSubFlag( mPageUpFlags, GUI_CTL_CLICKED ) )
		tSrcPageUp = tClickPageUp;
	else if ( IsSubFlag( mPageUpFlags, GUI_CTL_HOVER ) )
		tSrcPageUp = tHoverPageUp;
	else
		tSrcPageUp = tNormalPageUp;

	if ( IsSubFlag( mPageDownFlags, GUI_CTL_CLICKED ) )
		tSrcPageDown = tClickPageDown;
	else if ( IsSubFlag( mPageDownFlags, GUI_CTL_HOVER ) )
		tSrcPageDown = tHoverPageDown;
	else
		tSrcPageDown = tNormalPageDown;

	if ( IsSubFlag( mCursorFlags, GUI_CTL_CLICKED ) )
	{
		tSrcCursorUp	= tClickCursorUp;
		tSrcCursorDown	= tClickCursorDown;
		tSrcCursor		= tClickCursor;
	}
	else if ( IsSubFlag( mCursorFlags, GUI_CTL_HOVER ) )
	{
		tSrcCursorUp	= tHoverCursorUp;
		tSrcCursorDown	= tHoverCursorDown;
		tSrcCursor		= tHoverCursor;
	}
	else
	{
		tSrcCursorUp	= tNormalCursorUp;
		tSrcCursorDown	= tNormalCursorDown;
		tSrcCursor		= tNormalCursor;
	}

	if ( mStyle == GUI_SBF_HORZ )
	{
		tCursorUp.SetRect( mCursorRect.mTopLeft.mX, mRect.mTopLeft.mY, mCursorRect.mTopLeft.mX + mCursorPrefixSize.mCX, mRect.mBottomRight.mY );
		tCursorDown.SetRect( mCursorRect.mBottomRight.mX - mCursorPrefixSize.mCX, mRect.mTopLeft.mY, mCursorRect.mBottomRight.mX, mRect.mBottomRight.mY );
		tCursor.SetRect( tCursorUp.mBottomRight.mX, mRect.mTopLeft.mY, tCursorUp.mBottomRight.mX + mCursorSize.mCX, mRect.mBottomRight.mY );
		tSlider.SetRect( tPageUpRect.mBottomRight.mX, mRect.mTopLeft.mY, tPageUpRect.mBottomRight.mX + mScrollSliderSize.mCX, mRect.mBottomRight.mY );
	}
	else
	{
		tCursorUp.SetRect( mRect.mTopLeft.mX, mCursorRect.mTopLeft.mY, mRect.mBottomRight.mX, mCursorRect.mTopLeft.mY + mCursorPrefixSize.mCY );
		tCursorDown.SetRect( mRect.mTopLeft.mX, mCursorRect.mBottomRight.mY - mCursorPrefixSize.mCY, mRect.mBottomRight.mX, mCursorRect.mBottomRight.mY );
		tCursor.SetRect( mRect.mTopLeft.mX, tCursorUp.mBottomRight.mY, mRect.mBottomRight.mX, tCursorUp.mBottomRight.mY + mCursorSize.mCY );
		tSlider.SetRect( mRect.mTopLeft.mX, tPageUpRect.mBottomRight.mY, mRect.mBottomRight.mX, tPageUpRect.mBottomRight.mY + mScrollSliderSize.mCY );
	}

	// 渲染滑道
	if ( mStyle == GUI_SBF_HORZ )
	{
		for ( ; tSlider.mTopLeft.mX < tPageDownRect.mTopLeft.mX; tSlider.mTopLeft.mX += mScrollSliderSize.mCX )
		{
			tSrcSlider.mBottomRight.mX	= tSrcSlider.mTopLeft.mX + min( mScrollSliderSize.mCX, tPageDownRect.mTopLeft.mX - tSlider.mTopLeft.mX );
			tSlider.mBottomRight.mX		= tSlider.mTopLeft.mX + min( mScrollSliderSize.mCX, tPageDownRect.mTopLeft.mX - tSlider.mTopLeft.mX );
			spRender->BitBlt( mpTexture, tSrcSlider, tSlider, 0xFFFFFFFF );
		}
	}
	else
	{
		for ( ; tSlider.mTopLeft.mY < tPageDownRect.mTopLeft.mY; tSlider.mTopLeft.mY += mScrollSliderSize.mCY )
		{
			tSrcSlider.mBottomRight.mY	= tSrcSlider.mTopLeft.mY + min( mScrollSliderSize.mCY, tPageDownRect.mTopLeft.mY - tSlider.mTopLeft.mY );
			tSlider.mBottomRight.mY		= tSlider.mTopLeft.mY + min( mScrollSliderSize.mCY, tPageDownRect.mTopLeft.mY - tSlider.mTopLeft.mY );
			spRender->BitBlt( mpTexture, tSrcSlider, tSlider, 0xFFFFFFFF );
		}
	}

	// 渲染上滚按钮
	spRender->BitBlt( mpTexture, tSrcPageUp, tPageUpRect, 0xFFFFFFFF );

	// 渲染下滚按钮
	spRender->BitBlt( mpTexture, tSrcPageDown, tPageDownRect, 0xFFFFFFFF );

	// 渲染滑块按钮上部份
	spRender->BitBlt( mpTexture, tSrcCursorUp, tCursorUp, 0xFFFFFFFF );

	// 渲染滑块按钮下部份
	spRender->BitBlt( mpTexture, tSrcCursorDown, tCursorDown, 0xFFFFFFFF );

	// 渲染滑块按钮中间部份
	if ( mStyle == GUI_SBF_HORZ )
	{
		for ( ; tCursor.mTopLeft.mX < tCursorDown.mTopLeft.mX; tCursor.mTopLeft.mX += mCursorSize.mCX )
		{
			tSrcCursor.mBottomRight.mX	= tSrcCursor.mTopLeft.mX + min( mCursorSize.mCX, tCursorDown.mTopLeft.mX - tCursor.mTopLeft.mX );
			tCursor.mBottomRight.mX		= tCursor.mTopLeft.mX + min( mCursorSize.mCX, tCursorDown.mTopLeft.mX - tCursor.mTopLeft.mX );
			spRender->BitBlt( mpTexture, tSrcCursor, tCursor, 0xFFFFFFFF );
		}
	}
	else
	{
		for ( ; tCursor.mTopLeft.mY < tCursorDown.mTopLeft.mY; tCursor.mTopLeft.mY += mCursorSize.mCY )
		{
			tSrcCursor.mBottomRight.mY	= tSrcCursor.mTopLeft.mY + min( mCursorSize.mCY, tCursorDown.mTopLeft.mY - tCursor.mTopLeft.mY );
			tCursor.mBottomRight.mY		= tCursor.mTopLeft.mY + min( mCursorSize.mCY, tCursorDown.mTopLeft.mY - tCursor.mTopLeft.mY );
			spRender->BitBlt( mpTexture, tSrcCursor, tCursor, 0xFFFFFFFF );
		}
	}
}

void CControlScrollBar::InitControl( bool vIsInit )
{
	GUI_INITEXTURE( vIsInit, mpTexture, mStyle == GUI_SBF_HORZ ? "CommonScrollH.png" : "CommonScrollV.png" );
	CControl::InitControl( vIsInit );
}

// 移动滚动光标。
void CControlScrollBar::MoveCursor( const CGUIPoint& rPoint )
{
	CGUIRect& tPageUpRect = GetPageUpRect( );
	CGUIRect& tPageDownRect = GetPageDownRect( );
	// 自由移动滚动光标
	if ( mStyle == GUI_SBF_HORZ )	// 横向滚动条
	{
		CGUIPoint tCursorPoint( rPoint.mX - mScrollPoint, mCursorRect.mTopLeft.mY );
		mCursorRect.SetRect( tCursorPoint, mCursorRect.Size( ) );

		// 防止左滚动出界
		if ( mCursorRect.mTopLeft.mX <= tPageUpRect.mBottomRight.mX )
			mCursorRect.MoveToX( tPageUpRect.mBottomRight.mX );

		// 防止右滚动出界
		if ( mCursorRect.mBottomRight.mX >= tPageDownRect.mTopLeft.mX )
			mCursorRect.MoveToX( tPageDownRect.mTopLeft.mX - mCursorRect.Width( ) );

		mPosition = int( ( mCursorRect.mTopLeft.mX - tPageUpRect.mBottomRight.mX ) * ( mRangeMax - mRangeMin ) / 
			( mRect.Width( ) - mCursorRect.Width( ) - tPageUpRect.Width( ) * 2.0f ) );

        mMouseCursor.mX = rPoint.mX;
	}
	else									// 纵向滚动条
	{
		CGUIPoint tCursorPoint( mCursorRect.mTopLeft.mX, rPoint.mY - mScrollPoint );
		mCursorRect.SetRect( tCursorPoint, mCursorRect.Size( ) );

		// 防止上滚动出界
		if ( mCursorRect.mTopLeft.mY <= tPageUpRect.mBottomRight.mY )
			mCursorRect.MoveToY( tPageUpRect.mBottomRight.mY );

		// 防止下滚动出界
		if ( mCursorRect.mBottomRight.mY >= tPageDownRect.mTopLeft.mY )
			mCursorRect.MoveToY( tPageDownRect.mTopLeft.mY - mCursorRect.Height( ) );

		mPosition = int( ( mCursorRect.mTopLeft.mY - tPageUpRect.mBottomRight.mY ) * ( mRangeMax - mRangeMin ) / 
			( mRect.Height( ) - mCursorRect.Height( ) - tPageUpRect.Height( ) * 2.0f ) );
		mMouseCursor.mY = rPoint.mY;
	}

	// 触发滚动事件
	if ( mpScrolledHandler != NULL )
		( *mpScrolledHandler )( this );
}

// 设置移动光标偏移量。
void CControlScrollBar::SetPosition( int vPosition )
{
	CGUIRect& tPageUpRect = GetPageUpRect( );
	CGUIRect& tPageDownRect = GetPageDownRect( );
	mPosition = vPosition;

	if ( mPosition > mRangeMax - mRangeMin )
		mPosition = mRangeMax - mRangeMin;

	if ( mPosition < 0 )
		mPosition = 0;

	if ( mRangeMax == 0 )
		return;

	if ( mStyle == GUI_SBF_HORZ )	// 横向滚动条
	{
		// 滚动光标的长度
		int tLength = int( ( mRect.Width( ) - tPageUpRect.Width( ) * 2.0f ) * mRangeMin / mRangeMax );

		// 保证滚动光标的最小长度
		tLength = max( tLength, GetMinCursorLength( ) );

		// 滚动部分的实际长度
		int tLeft	= 0;
		int tRight	= 0;

		if ( mRangeMax == mRangeMin )
		{
			tLeft	= tPageUpRect.mBottomRight.mX;
			tRight	= tPageDownRect.mTopLeft.mX;
			// 设置滚动光标的位置
		}
		else
		{
			// 设置滚动光标的位置
			tLeft	= tPageUpRect.mBottomRight.mX + int( mRect.Width( ) - tPageUpRect.Width( ) * 2.0f - tLength ) * mPosition / ( mRangeMax - mRangeMin );
			tRight	= tLeft + tLength;
		}
		mCursorRect.SetRect( tLeft, mRect.mTopLeft.mY, tRight, mRect.mBottomRight.mY );
	}
	else									// 纵向滚动条
	{
		// 滚动光标的长度
		int tLength = int( ( mRect.Height( ) - tPageUpRect.Height( ) * 2.0f ) * mRangeMin / mRangeMax );

		// 保证滚动光标的最小长度
		tLength = max( tLength, GetMinCursorLength( ) );

		// 滚动部分的实际长度
		int tTop	= 0;
		int tBottom	= 0;

		if ( mRangeMax == mRangeMin )
		{
			// 设置滚动光标的位置
			tTop	= tPageUpRect.mBottomRight.mY;
			tBottom	= tPageDownRect.mTopLeft.mY;
		}
		else
		{
            // 设置滚动光标的位置
			tTop = tPageUpRect.mBottomRight.mY + int( mRect.Height( ) - tPageUpRect.Height( ) * 2.0f - tLength ) * mPosition / ( mRangeMax - mRangeMin );
			tBottom	= tTop + tLength;
		}
		mCursorRect.SetRect( mRect.mTopLeft.mX, tTop, mRect.mBottomRight.mX, tBottom );
	}
}

// 设置滚动条所能显示的最大范围和滚动光标所代表的范围。
void CControlScrollBar::SetRange( int vRangeMax, int vRangeMin )
{
	if ( vRangeMax >= vRangeMin )
	{
		mRangeMax = vRangeMax;
		mRangeMin = vRangeMin;
	}
	else
	{
		mRangeMax = vRangeMin;
		mRangeMin = vRangeMin;
	}

	if ( mPosition > mRangeMax )
		mPosition = mRangeMax;

	SetPosition( mPosition );
}

// 鼠标左键抬起消息处理函数。
void CControlScrollBar::OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags )
{
	UnsetSubFlag( mCursorFlags, GUI_CTL_CLICKED );
	UnsetSubFlag( mPageUpFlags, GUI_CTL_CLICKED );
	UnsetSubFlag( mPageDownFlags, GUI_CTL_CLICKED );
	mClickArea = GUI_SBR_NONE;

	GetPage( )->SetCapture( NULL );
}

// 鼠标左键按下消息处理函数。
void CControlScrollBar::OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags )
{
	CGUIRect& tPageUpRect = GetPageUpRect( );
	CGUIRect& tPageDownRect = GetPageDownRect( );
	CGUIRect tScrollUp;
	CGUIRect tScrollDown;
	mClickArea = GUI_SBR_NONE;
	if ( mCursorRect.PtInRect( rPoint ) )
	{
		mClickArea = GUI_SBR_CURSOR;
		GetPage( )->SetCapture( this );

		// 设置滚动光标为按下状态
		SetSubFlag( mCursorFlags, GUI_CTL_CLICKED );

		mMouseCursor = rPoint;
		if ( mStyle == GUI_SBF_HORZ )	// 横向滚动条
			mScrollPoint = rPoint.mX - mCursorRect.mTopLeft.mX;
		else
            mScrollPoint = rPoint.mY - mCursorRect.mTopLeft.mY;
	}
	else if ( tPageUpRect.PtInRect( rPoint ) )
	{
		mClickArea = GUI_SBR_PAGEUP;
		SetSubFlag( mPageUpFlags, GUI_CTL_CLICKED );
	}
	else if ( tPageDownRect.PtInRect( rPoint ) )
	{
		mClickArea = GUI_SBR_PAGEDOWN;
		SetSubFlag( mPageDownFlags, GUI_CTL_CLICKED );
	}
	else
	{
		if ( mStyle == GUI_SBF_HORZ )	// 横向滚动条
		{
			tScrollUp.SetRect( tPageUpRect.mBottomRight.mX, mRect.mTopLeft.mY, mCursorRect.mTopLeft.mX, mRect.mBottomRight.mY );
			tScrollDown.SetRect( mCursorRect.mBottomRight.mX, mRect.mTopLeft.mY, tPageDownRect.mTopLeft.mX, mRect.mBottomRight.mY );

			if ( tScrollUp.PtInRect( rPoint ) )
				mClickArea = GUI_SBR_BLANKUP;
			else if ( tScrollDown.PtInRect( rPoint ) )
				mClickArea = GUI_SBR_BLANKDOWN;
		}
		else
		{
			tScrollUp.SetRect( mRect.mTopLeft.mX, tPageUpRect.mBottomRight.mY, mRect.mBottomRight.mX, mCursorRect.mTopLeft.mY );
			tScrollDown.SetRect( mRect.mTopLeft.mX, mCursorRect.mBottomRight.mY, mRect.mBottomRight.mX, tPageDownRect.mTopLeft.mY );

			if ( tScrollUp.PtInRect( rPoint ) )
				mClickArea = GUI_SBR_BLANKUP;
			else if ( tScrollDown.PtInRect( rPoint ) )
				mClickArea = GUI_SBR_BLANKDOWN;
		}

		UnsetSubFlag( mCursorFlags, GUI_CTL_HOVER );
		UnsetSubFlag( mCursorFlags, GUI_CTL_CLICKED );
	}

	GetPage( )->SetCapture( this );
}

// 鼠标滚动消息处理函数。
void CControlScrollBar::OnMouseWheel( const CGUIPoint& rPoint, unsigned int vFlags )
{
	short vDelta = HIWORD( vFlags );

	if ( vDelta < 0 )
	{
		for ( vDelta += WHEEL_DELTA; vDelta <= 0; vDelta += WHEEL_DELTA )
			PageDown( );
	}
	else
	{
		for ( vDelta -= WHEEL_DELTA; vDelta >= 0; vDelta -= WHEEL_DELTA )
			PageUp( );
	}

	UnsetSubFlag( mCursorFlags, GUI_CTL_HOVER );
	UnsetSubFlag( mCursorFlags, GUI_CTL_CLICKED );

	// 触发滚动事件
	if ( mpScrolledHandler != NULL )
		( *mpScrolledHandler )( this );
}

// 鼠标移动消息处理函数。
void CControlScrollBar::OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags )
{
	CGUIRect& tPageUpRect = GetPageUpRect( );
	CGUIRect& tPageDownRect = GetPageDownRect( );
	mCursorFlags	= GUI_CTL_ENABLE;
	mPageUpFlags	= GUI_CTL_ENABLE;
	mPageDownFlags	= GUI_CTL_ENABLE;

	// 鼠标在上滚按键范围内，设置上滚按键属性
	if ( tPageUpRect.PtInRect( rPoint ) )
	{
		if ( mClickArea == GUI_SBR_NONE )
			SetSubFlag( mPageUpFlags, GUI_CTL_HOVER );

		if ( vFlags & 1 && mClickArea == GUI_SBR_PAGEUP ) 
			SetSubFlag( mPageUpFlags, GUI_CTL_CLICKED );
	}
	// 鼠标在下滚按键范围内，设置下滚按键属性
	else if ( tPageDownRect.PtInRect( rPoint ) )
	{
		if ( mClickArea == GUI_SBR_NONE )
			SetSubFlag( mPageDownFlags, GUI_CTL_HOVER );

		if ( vFlags & 1 && mClickArea == GUI_SBR_PAGEDOWN ) 
			SetSubFlag( mPageDownFlags, GUI_CTL_CLICKED );
	}
	else if ( mCursorRect.PtInRect( rPoint ) )
	{
		if ( mClickArea == GUI_SBR_NONE )
			SetSubFlag( mCursorFlags, GUI_CTL_HOVER );
	}

	if ( mClickArea == GUI_SBR_CURSOR )
	{
		// 不可以滚动，直接返回
		if ( mRangeMax == mRangeMin )
			return;

		MoveCursor( rPoint );
	}
}

// 键盘按键按下消息处理函数，处理 PageUp，PageDown, Home，End。
void CControlScrollBar::OnKeyDown( unsigned int vKeyCode, unsigned int vFlags )
{
	int tOldPosition = mPosition;

	switch ( vKeyCode )
	{
	case VK_PRIOR:	PageUp( ); break;
	case VK_NEXT:	PageDown( ); break;
	case VK_HOME:	SetPosition( 0 ); break;
	case VK_END:	SetPosition( mRangeMax ); break;
	}

	if ( mStyle == GUI_SBF_HORZ )	// 横向滚动条
	{
		if ( vKeyCode == VK_LEFT )
			LineUp( );
		else if ( vKeyCode == VK_RIGHT )
			LineDown( );
	}
	else									// 纵向滚动条
	{
		if ( vKeyCode == VK_UP )
			LineUp( );
		else if ( vKeyCode == VK_DOWN )
			LineDown( );
	}

	// 如果滚动光标的位置发生变化，取消滚动光标的悬停和点击属性
	if ( tOldPosition != mPosition )
	{
		UnsetSubFlag( mCursorFlags, GUI_CTL_HOVER );
		UnsetSubFlag( mCursorFlags, GUI_CTL_CLICKED );

		// 触发滚动事件
		if ( mpScrolledHandler != NULL )
			( *mpScrolledHandler )( this );
	}
}

// 重载获得悬停时的消息处理。
void CControlScrollBar::OnHover( const CGUIPoint& rPoint, bool vHover )
{
	CControl::OnHover( rPoint, vHover );

	// 如果是失去悬停，取消所有子区域的属性
	if ( !vHover )
	{
		mPageUpFlags	= GUI_CTL_ENABLE;
		mPageDownFlags	= GUI_CTL_ENABLE;
		mCursorFlags	= GUI_CTL_ENABLE;
	}
}

void CControlScrollBar::UpdateStatus( )
{
	CControl::UpdateStatus( );

	DWORD tThisTickCount = ::GetTickCount( );
	if ( tThisTickCount - mLastTickCount < 100 )
		return;

	mLastTickCount = tThisTickCount;
	switch( mClickArea )
	{
	case GUI_SBR_PAGEUP:
		{
			LineUp( );
			break;
		}
	case GUI_SBR_PAGEDOWN:
		{
			LineDown( );
			break;
		}
	case GUI_SBR_BLANKUP:
		{
			PageUp( );
			break;
		}
	case GUI_SBR_BLANKDOWN:
		{
			PageDown( );
			break;
		}
	case GUI_SBR_NONE:
		break;
	}

	// 触发滚动事件
	if ( mpScrolledHandler != NULL )
		( *mpScrolledHandler )( this );
}

// 重载移动控件的方法，同时移动子区域的位置。
void CControlScrollBar::Offset( const CGUIPoint& rPoint )
{
	CControl::Offset( rPoint );
	SetPosition( mPosition );
}

void CControlScrollBar::Offset( const CGUIPoint& rPoint, const CGUISize& rSize )
{
	CControl::Offset( rPoint, rSize );
	SetPosition( mPosition );
}

// ************************************************************ //
// CTreeNode
// ************************************************************ //

CTreeNode::CTreeNode( const char* pText ) : mText( pText ), mIsExpanded( true ), mIsChecked( false ), mIsSelected( false )
{
}

CTreeNode::CTreeNode( ) : mIsExpanded( true ), mIsChecked( false ), mIsSelected( false )
{
}

CTreeNode::~CTreeNode( )
{
	while ( !mNodes.empty( ) )
	{
		delete mNodes.at( 0 );
		mNodes.erase( mNodes.begin( ) );
	}
}

void CTreeNode::Expand( )
{
	if ( mNodes.size( ) == 0 )
		return;

	mIsExpanded = ( mIsExpanded ? false : true );
}

CTreeNode* CTreeNode::HitTest( const CGUIPoint& rPoint )
{
	if ( mIsExpanded == false )
		return NULL;

	CTreeNode* tpNode = NULL;
	for ( int i = 0; i < int( mNodes.size( ) ); i ++ )
	{
		if ( mNodes[i]->mRect.PtInRect( rPoint ) )
			tpNode = mNodes[i];
		else
			tpNode = mNodes[i]->HitTest( rPoint );

		if ( tpNode != NULL )
			break;
	}

	return tpNode;
}

void CTreeNode::DrawItems( CControlTree* pTree )
{
	CGUIRect tClientRect = pTree->GetClientRect( );
	CGUIRect tDestRect = mRect + tClientRect.mTopLeft - pTree->mCamPoint; 
	if ( mRect.IsRectEmpty( ) == false )
        pTree->OnDrawItem( mText.c_str( ), tDestRect, mIsExpanded, mIsChecked, mIsSelected, ( mNodes.size( ) == 0 ) );

	if ( mIsExpanded == true )
	{
		for ( int i = 0; i < int( mNodes.size( ) ); i ++ )
			mNodes[i]->DrawItems( pTree );
	}
}

int CTreeNode::RefreshItems( int vLeft, int vTop, int vHeight, int& rWidth )
{
	int tLeft	= vLeft;
	int tTop	= vTop;
	int tHeight = vHeight;
	int tWidth	= 0;
	if ( mIsExpanded == false )
		return tTop;

	for ( int i = 0; i < int( mNodes.size( ) ); i ++ )
	{
		int tTextCount = int( ::strlen( mNodes[i]->mText.c_str( ) ) );
		tWidth = CControl::spRender->GetTextWidth( mNodes[i]->mText.c_str( ), tTextCount ) + 15;
		mNodes[i]->mRect.SetRect( tLeft, tTop, tLeft + tWidth, tTop + tHeight );
		rWidth = max( rWidth, tLeft + tWidth );
		tTop = mNodes[i]->RefreshItems( tLeft + 20, tTop + tHeight, vHeight, rWidth );
	}

	return tTop;
}

// ************************************************************ //
// CControlTree
// ************************************************************ //

CControlTree::CControlTree( const CGUIRect& rRect, unsigned int vID ) : CControlDialog( GUI_CSF_NORMAL, "", rRect, vID ), mItemHeight( 18 )
{
	mpRoot	= new CTreeNode;
	mpFocus = NULL;
}

CControlTree::~CControlTree( )
{
	delete mpRoot;
}

void CControlTree::InitControl( bool vIsInit )
{
	GUI_INITEXTURE( vIsInit, mpTexture, "CommonTree.png" );

	CControlDialog::InitControl( vIsInit );
}

void CControlTree::OnDrawItem( const char* pText, const CGUIRect& rRect, bool vIsExpanded, bool vIsChecked, bool vIsSelected, bool vIsLeafNode )
{
	unsigned int tTextColor = 0xFFFFFFFF;

	if ( vIsLeafNode == false )
	{
		if ( vIsSelected == true )
		{
			CGUIRect tBackRect = rRect;
			tBackRect.DeflateRect( 9, 0, 0, 0 );
			spRender->DrawCombine( mpTexture, tBackRect, CGUIPoint( 9, 0 ), CGUISize( 15, 15 ) );
		}

		if ( vIsExpanded == false )
			spRender->BitBlt( mpTexture, CGUIRect( 0, 0, 9, 9 ), CGUIRect( rRect.mTopLeft + CGUIPoint( 0, 5 ), CGUISize( 9, 9 ) ), 0xFFFFFFFF );
		else
			spRender->BitBlt( mpTexture, CGUIRect( 0, 18, 9, 27 ), CGUIRect( rRect.mTopLeft + CGUIPoint( 0, 5 ), CGUISize( 9, 9 ) ), 0xFFFFFFFF );

		spRender->PrintText( pText, rRect + CGUIPoint( 12, 0 ) );
	}
	else
	{
		if ( vIsSelected == true )
			spRender->DrawCombine( mpTexture, rRect, CGUIPoint( 9, 0 ), CGUISize( 15, 15 ) );

		spRender->PrintText( pText, rRect + CGUIPoint( 3, 0 ) );
	}
}

void CControlTree::Draw( )
{
	CGUIRect tClientRect = GetClientRect( );
	CGUIRect tLastClipRect = spRender->SetClipRect( tClientRect );
	mpRoot->DrawItems( this );
	spRender->SetClipRect( tLastClipRect );
	CControlDialog::Draw( );
}

void CControlTree::Offset( const CGUIPoint& rPoint )
{
	CControlDialog::Offset( rPoint );
}

void CControlTree::Offset( const CGUIPoint& rPoint, const CGUISize& rSize )
{
	CControlDialog::Offset( rPoint, rSize );
	RefreshItems( );
}

void CControlTree::RefreshItems( )
{
	int tWidth = 0;
	int tHeight = mpRoot->RefreshItems( 0, 0, mItemHeight, tWidth );
	// 如果超过了最大的显示行数,就要显示纵向滚动条
	bool tNeedRefresh = RefreshDialog( tWidth, tHeight );

	if ( tNeedRefresh )
        RefreshItems( );
}

void CControlTree::OnMouseLButtonDoubleClick( const CGUIPoint& rPoint, unsigned int vFlags )
{
	CTreeNode* tpNode = mpRoot->HitTest( rPoint + mCamPoint - GetClientRect( ).mTopLeft );
	if ( tpNode != NULL )
        tpNode->Expand( );

	RefreshItems( );
	CControlDialog::OnMouseLButtonDoubleClick( rPoint, vFlags );
}

void CControlTree::OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags )
{
	CTreeNode* tpNode = mpRoot->HitTest( rPoint + mCamPoint - GetClientRect( ).mTopLeft );
	if ( mpFocus != NULL )
		mpFocus->mIsSelected = false;

	if ( tpNode != NULL )
        tpNode->mIsSelected = true;

	mpFocus = tpNode;
	CControlDialog::OnMouseLButtonDown( rPoint, vFlags );
}

void* CControlTree::InsertItem( const char* pText, void* vParent, void* vInsertAfter )
{
	CTreeNode* tpNewNode = new CTreeNode( pText );
	CTreeNode* tpParent	= (CTreeNode*) vParent;
	CTreeNode* tpAfter	= (CTreeNode*) vInsertAfter;
	if ( vParent == NULL )
		tpParent = mpRoot;

	if ( vInsertAfter == NULL )
	{
		if ( tpParent->mNodes.size( ) == 0 )
			tpAfter = NULL;
		else
            tpAfter = tpParent->mNodes[tpParent->mNodes.size( ) - 1];
	}

	for ( int i = 0; i < int( tpParent->mNodes.size( ) ); i ++ )
	{
		if ( tpAfter == tpParent->mNodes[i] )
		{
			tpParent->mNodes.insert( tpParent->mNodes.begin( ) + i + 1, tpNewNode );
			break;
		}
	}

	if ( tpAfter == NULL )
		tpParent->mNodes.push_back( tpNewNode );

	RefreshItems( );
	return tpNewNode;
}

// ************************************************************ //
// CColumnItem
// ************************************************************ //

CColumnItem::CColumnItem( const char* pText ) : mText( pText ), mIsClicked( false ), mSortType( 0 )
{
}

CColumnItem::~CColumnItem( )
{
}

// ************************************************************ //
// CGridItem
// ************************************************************ //

CGridItem::CGridItem( const char* pText ) : mText( pText )
{
}

CGridItem::~CGridItem( )
{
}

// ************************************************************ //
// CRowItem
// ************************************************************ //

CRowItem::CRowItem( )
{
	mIsSelected = false;
	mItemData	= 0;
}

CRowItem::~CRowItem( )
{
	while ( !mItems.empty( ) )
	{
		delete mItems.at( 0 );
		mItems.erase( mItems.begin( ) );
	}
}

// ************************************************************ //
// CControlList
// ************************************************************ //

CControlList::CControlList( const CGUIRect& rRect, unsigned int vID ) : CControlDialog( GUI_CSF_NORMAL | GUI_DSF_VSCROLL | GUI_DSF_HSCROLL, "", rRect, vID ), mItemHeight( 18 ), mColumnHeight( 18 ), mpDblClkItem( NULL ), mpClickItem( NULL )
{
}

CControlList::~CControlList( )
{
	RemoveAllItems( );

	while ( !mColumns.empty( ) )
	{
		delete mColumns.at( 0 );
		mColumns.erase( mColumns.begin( ) );
	}
}

void CControlList::RemoveItem( int vItem )
{
	delete mItems.at( vItem );
	mItems.erase( mItems.begin( ) + vItem );
}

void CControlList::RemoveAllItems( )
{
	while ( !mItems.empty( ) )
	{
		delete mItems.at( 0 );
		mItems.erase( mItems.begin( ) );
	}

	mSlotRects.clear( );
}

std::string CControlList::GetItemText( int vItemIndex )
{
	return mItems[vItemIndex]->mItems[0]->mText;
}

int CControlList::GetCurSel( )
{
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
	{
		if ( mItems[i]->mIsSelected )
			return i;
	}

	return -1;
}

void CControlList::SetCurSel( int vItem )
{
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
		vItem != -1 ? mItems[i]->mIsSelected = false : NULL;

	vItem != -1 ? mItems[ vItem ]->mIsSelected = true : NULL;
}

int CControlList::HitTest( const CGUIPoint& rPoint )
{
	for ( int i = 0; i < int( mSlotRects.size( ) ); i ++ )
	{
		if ( mSlotRects[i].PtInRect( rPoint + mCamPoint - GetClientRect( ).mTopLeft ) )
			return i;
	}

	return -1;
}

void CControlList::AppendColumn( const char* pText, int vWidth, int vFormat )
{
	CColumnItem* tpColumn = new CColumnItem( pText );
	int tColumnCount = int( mColumns.size( ) );
	int tRowCount	 = int( mItems.size( ) );
	if ( tColumnCount == 0 )
		tpColumn->mRect.SetRect( CGUIPoint( 0, 0 ), CGUISize( vWidth, mColumnHeight ) );
	else
        tpColumn->mRect.SetRect( mColumns[tColumnCount - 1]->mRect.TopRight( ), CGUISize( vWidth, mColumnHeight ) );

	tpColumn->mFormat = vFormat;
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
	{
		mSlotRects[i] = CGUIRect( mColumns[0]->mRect.mTopLeft, tpColumn->mRect.mBottomRight ) + mSlotRects[i].mTopLeft;
		mItems[i]->mItems.push_back( new CGridItem( "" ) );
	}

	mColumns.push_back( tpColumn );
	tColumnCount = int( mColumns.size( ) );
	int tWidth = mColumns[tColumnCount - 1]->mRect.mBottomRight.mX;
	int tHeight = tRowCount * mItemHeight + mColumnHeight;
	RefreshDialog( tWidth, tHeight );
}

int CControlList::AppendItem( const char* pText )
{
	int tColumnCount = int( mColumns.size( ) );
	int tRowCount	 = int( mItems.size( ) );
	if ( tColumnCount == 0 )
		return -1;

	int tWidth = mColumns[tColumnCount - 1]->mRect.mBottomRight.mX;
	CGUIRect tItemRect( CGUIPoint( 0, tRowCount * mItemHeight + mColumnHeight ), CGUISize( tWidth, mItemHeight ) );
	mSlotRects.push_back( tItemRect );

	CRowItem* tpRow = new CRowItem;
	for ( int i = 0; i < tColumnCount; i ++ )
	{
		CGridItem* tpGrid = new CGridItem( pText );
		i == 0 ? tpGrid->mText = pText : tpGrid->mText = "";
		tpRow->mItems.push_back( tpGrid );
	}

	mItems.push_back( tpRow );
	tRowCount = int( mItems.size( ) );
	int tHeight = tRowCount * mItemHeight + mColumnHeight;
	RefreshDialog( tWidth, tHeight );
	return tRowCount - 1;
}

void CControlList::SetItemData( int vItem, unsigned int vUserData )
{
	int tRowCount = int( mItems.size( ) );
	if ( vItem >= tRowCount )
		return;

	mItems[vItem]->mItemData = vUserData;
}

unsigned int CControlList::GetItemData( int vItem )
{
	int tRowCount = int( mItems.size( ) );
	if ( vItem >= tRowCount )
		return 0;

	return mItems[vItem]->mItemData;
}

void CControlList::SetItem( int vItem, int vSubItem, const char* pText )
{
	int tRowCount	 = int( mItems.size( ) );
	int tColumnCount = int( mColumns.size( ) );
	if ( vSubItem >= tColumnCount || vItem >= tRowCount )
		return;
	
	mItems[vItem]->mItems[vSubItem]->mText = pText;
}

void CControlList::InitControl( bool vIsInit )
{
	GUI_INITEXTURE( vIsInit, mpTexture, "CommonList.png" );

	CControlDialog::InitControl( vIsInit );
}

void CControlList::OnDrawColumn( const char* pText, const CGUIRect& rRect, bool vIsClicked, int vSortType, int vFormat )
{
	int tTextStyle = GUI_DT_VCENTER;
	if ( vFormat == GUI_LCSFMT_LEFT ) tTextStyle |= GUI_DT_LEFT;
	else if ( vFormat == GUI_LCSFMT_CENTER ) tTextStyle |= GUI_DT_HCENTER;
	else if ( vFormat == GUI_LCSFMT_RIGHT ) tTextStyle |= GUI_DT_RIGHT;

	CGUIRect tTextRect = rRect;
	tTextRect.DeflateRect( 2, 2 );
	if ( vIsClicked )
	{
		spRender->DrawCombine( mpTexture, rRect, CGUIPoint( 0, 45 ), CGUISize( 15, 15 ) );
		spRender->PrintText( pText, tTextRect + CGUIPoint( 1, 1 ), tTextStyle );
	}
	else
	{
		spRender->DrawCombine( mpTexture, rRect, CGUIPoint( 0, 0 ), CGUISize( 15, 15 ) );
        spRender->PrintText( pText, tTextRect, tTextStyle );
	}

	CGUIRect tSortRect;
	if ( vFormat == GUI_LCSFMT_LEFT )
	{
		tSortRect.mTopLeft = rRect.TopRight( ) - CGUIPoint( 18, -1 );
		tSortRect.mBottomRight = tSortRect.mTopLeft + CGUIPoint( 16, 16 );
	}
	else
	{
		tSortRect.mTopLeft = rRect.mTopLeft + CGUIPoint( 2, 1 );
		tSortRect.mBottomRight = tSortRect.mTopLeft + CGUIPoint( 16, 16 );
	}
	if ( vIsClicked )
		tSortRect += CGUIPoint( 1, 1 );

	if ( vSortType == 1 )
		spRender->BitBlt( mpTexture, CGUIRect( CGUIPoint( 45, 0 ), CGUISize( 16, 16 ) ), tSortRect, 0xFFFFFFFF );
	else if ( vSortType == 2 )
		spRender->BitBlt( mpTexture, CGUIRect( CGUIPoint( 45, 16 ), CGUISize( 16, 16 ) ), tSortRect, 0xFFFFFFFF );
}

void CControlList::OnDrawGrid( const char* pText, const CGUIRect& rRect, int vFormat )
{
	int tTextStyle = GUI_DT_VCENTER;
	if ( vFormat == GUI_LCSFMT_LEFT ) tTextStyle |= GUI_DT_LEFT;
	else if ( vFormat == GUI_LCSFMT_CENTER ) tTextStyle |= GUI_DT_HCENTER;
	else if ( vFormat == GUI_LCSFMT_RIGHT ) tTextStyle |= GUI_DT_RIGHT;

	spRender->PrintText( pText, rRect - CGUIPoint( 1, 0 ), tTextStyle );
}

void CControlList::OnDrawItem( const CGUIRect& rRect, bool vIsSelected )
{
	if ( vIsSelected )
		spRender->DrawCombine( mpTexture, rRect, CGUIPoint( 0, 90 ), CGUISize( 15, 15 ) );
}

void CControlList::OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags )
{
	for ( int i = 0; i < int( mColumns.size( ) ); i ++ )
	{
		CColumnItem* tpItem = mColumns[i];
		if ( tpItem->mIsClicked == true )
		{
			tpItem->mIsClicked = false;
			Sort( i, tpItem->mSortType == 1 );
			break;
		}
	}
}

void CControlList::OnMouseLButtonDoubleClick( const CGUIPoint& rPoint, unsigned int vFlags )
{
	if ( mpDblClkItem != NULL )
		mpDblClkItem( this, HitTest( rPoint ) );
}

void CControlList::Sort( int vColumnIndex, bool vIsAscending )
{
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
	{
		for ( int j = i + 1; j < int( mItems.size( ) ); j ++ )
		{
			CGridItem* tpItem1 = mItems[i]->mItems[vColumnIndex];
			CGridItem* tpItem2 = mItems[j]->mItems[vColumnIndex];
			if ( vIsAscending )
			{
				if ( strcmp( tpItem1->mText.c_str( ), tpItem2->mText.c_str( ) ) > 0 )
				{
					CRowItem* tpTmp = mItems[j];
					mItems[j] = mItems[i];
					mItems[i] = tpTmp;
				}
			}
			else
			{
				if ( strcmp( tpItem1->mText.c_str( ), tpItem2->mText.c_str( ) ) < 0 )
				{
					CRowItem* tpTmp = mItems[j];
					mItems[j] = mItems[i];
					mItems[i] = tpTmp;
				}
			}
		}
	}
}

void CControlList::OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags )
{
	int tColumnCount = int( mColumns.size( ) );
	if ( tColumnCount == 0 )
		return;

	CGUIRect tColumnRect( mColumns[0]->mRect.mTopLeft, mColumns[tColumnCount - 1]->mRect.mBottomRight );
	for ( int i = 0; i < int( mColumns.size( ) ); i ++ )
	{
		CColumnItem* tpItem = mColumns[i];
		if ( tpItem->mRect.PtInRect( rPoint + mCamPoint - GetClientRect( ).mTopLeft ) )
		{
			tpItem->mIsClicked = true;
			if ( tpItem->mSortType == 0 )
				tpItem->mSortType = 1;
			else if ( tpItem->mSortType == 1 )
				tpItem->mSortType = 2;
			else if ( tpItem->mSortType == 2 )
				tpItem->mSortType = 1;
		}
		else if ( tColumnRect.PtInRect( rPoint + mCamPoint - GetClientRect( ).mTopLeft ) )
			tpItem->mSortType = 0;
	}

	if ( i == int( mColumns.size( ) ) )
	{
		SetCurSel( HitTest( rPoint ) );
		if ( mpClickItem != NULL )
            mpClickItem( this, GetCurSel( ) );
	}
	CControlDialog::OnMouseLButtonDown( rPoint, vFlags );
}

void CControlList::Draw( )
{
	CGUIRect tClientRect = GetClientRect( );
	CGUIRect tOldClipRect = spRender->SetClipRect( tClientRect );

	for ( int i = 0; i < int( mColumns.size( ) ); i ++ )
	{
		CColumnItem* tpItem = mColumns[i];
		OnDrawColumn( tpItem->mText.c_str( ), tpItem->mRect + tClientRect.mTopLeft - mCamPoint, tpItem->mIsClicked, tpItem->mSortType, tpItem->mFormat );
	}
	
	for ( int i = 0; i < int( mSlotRects.size( ) ); i ++ )
		OnDrawItem( mSlotRects[i] + tClientRect.mTopLeft - mCamPoint, mItems[i]->mIsSelected );

	for ( int i = 0; i < int( mColumns.size( ) ); i ++ )
	{
		CColumnItem* tpColoumn = mColumns[i];
		for ( int j = 0; j < int( mSlotRects.size( ) ); j ++ )
		{
			CRowItem* tpRow = mItems[j];
			CGUIRect tGridRect = tpColoumn->mRect + mSlotRects[j].mTopLeft + tClientRect.mTopLeft - mCamPoint;
			CGUIRect tOldClipRect = spRender->SetClipRect( tGridRect & tClientRect );
			OnDrawGrid( tpRow->mItems[i]->mText.c_str( ), tGridRect, tpColoumn->mFormat );
			spRender->SetClipRect( tOldClipRect );
		}
	}

	spRender->SetClipRect( tOldClipRect );
	CControlDialog::Draw( );
}

// ************************************************************ //
// CListItem
// ************************************************************ //

CListItem::CListItem( const char* pText )
{
	mText		= pText;
	mIsSelected = false;
	mIsHover	= false;
	mItemData	= 0;
}

CListItem::~CListItem( )
{
}

// ************************************************************ //
// CControlListBox
// ************************************************************ //

CControlListBox::CControlListBox( int vStyle, const CGUIRect& rRect, unsigned int vID ) : CControlDialog( GUI_CSF_NORMAL | GUI_DSF_VSCROLL | GUI_DSF_HSCROLL | vStyle, "", rRect, vID ), mItemHeight( 18 ), mpDblClkItem( NULL ), mpClickItem( NULL )
{
}

CControlListBox::~CControlListBox( )
{
	RemoveAllItems( );
}

void CControlListBox::RemoveAllItems( )
{
	while ( !mItems.empty( ) )
	{
		delete mItems.at( 0 );
		mItems.erase( mItems.begin( ) );
	}
}

int CControlListBox::GetCurSel( ) const
{
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
	{
		if ( mItems[i]->mIsSelected )
			return i;
	}

	return -1;
}

void CControlListBox::SetCurSel( int vItem )
{
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
		vItem != -1 ? mItems[ i ]->mIsSelected = false : NULL;

	vItem != -1 ? mItems[ vItem ]->mIsSelected = true : NULL;
}

int	CControlListBox::HitTest( const CGUIPoint& rPoint )
{
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
	{
		CGUIRect tItemRect( mItems[i]->mPoint, CGUISize( 4096, mItemHeight ) );
		if ( tItemRect.PtInRect( rPoint + mCamPoint - GetClientRect( ).mTopLeft ) ) 
			return i;
	}

	return -1;
}

void CControlListBox::RefreshItems( )
{
	CGUIRect tClientRect = GetClientRect( );
	int tHeight, tWidth	= 0;
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
	{
		CListItem* tpItem = mItems[i];
		tpItem->mPoint.mY = i * mItemHeight;
		int tTextCount = int( ::strlen( tpItem->mText.c_str( ) ) );
		tWidth = max( tWidth, spRender->GetTextWidth( tpItem->mText.c_str( ), tTextCount ) );
	}
	
	tHeight = int( mItems.size( ) ) * mItemHeight;
	if ( RefreshDialog( tWidth, tHeight ) == true )
		RefreshItems( );
}

int CControlListBox::AppendItem( const char* pText )
{
	mItems.push_back( new CListItem( pText ) );
	RefreshItems( );
	return int( mItems.size( ) ) - 1;
}

void CControlListBox::OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags )
{
	int tItemIndex = HitTest( rPoint );
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
		tItemIndex != -1 ? mItems[ i ]->mIsHover = false : NULL;

	tItemIndex != -1 ? mItems[ tItemIndex ]->mIsHover = true : NULL;
	CControlDialog::OnMouseMove( rPoint, vFlags );
}

void CControlListBox::OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags )
{
	SetCurSel( HitTest( rPoint ) );
	if ( mpClickItem != NULL )
		mpClickItem( this, GetCurSel( ) );
	CControlDialog::OnMouseLButtonDown( rPoint, vFlags );
}

void CControlListBox::OnMouseLButtonDoubleClick( const CGUIPoint& rPoint, unsigned int vFlags )
{
	SetCurSel( HitTest( rPoint ) );
	if ( mpDblClkItem != NULL )
        mpDblClkItem( this, GetCurSel( ) );
	CControlDialog::OnMouseLButtonDoubleClick( rPoint, vFlags );
}

void CControlListBox::InitControl( bool vIsInit )
{
	GUI_INITEXTURE( vIsInit, mpTexture, "CommonListBox.png" );
	CControlDialog::InitControl( vIsInit );
}

void CControlListBox::Draw( )
{
	CGUIRect tClientRect = GetClientRect( );
	CGUIRect tOldClipRect = spRender->SetClipRect( tClientRect );
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
	{
		int tTextCount = int( ::strlen( mItems[i]->mText.c_str( ) ) );
		int tWidth = spRender->GetTextWidth( mItems[i]->mText.c_str( ), tTextCount );
		CGUIRect tTextRect( mItems[i]->mPoint, CGUISize( tWidth, mItemHeight ) );
		CGUIRect tItemRect = tClientRect;
		tItemRect.mTopLeft.mY		= mItems[i]->mPoint.mY + tClientRect.mTopLeft.mY - mCamPoint.mY;
		tItemRect.mBottomRight.mY	= mItems[i]->mPoint.mY + mItemHeight + tClientRect.mTopLeft.mY - mCamPoint.mY;

		OnDrawItem( mItems[i]->mText.c_str( ), tTextRect + GetClientRect( ).mTopLeft - mCamPoint, tItemRect, mItems[i]->mIsSelected, mItems[i]->mIsHover );
	}
	spRender->SetClipRect( tOldClipRect );
	CControlDialog::Draw( );
}

void CControlListBox::OnDrawItem( const char* pText, const CGUIRect& rTextRect, const CGUIRect& rItemRect, bool vIsSelected, bool vIsHover )
{
	if ( vIsSelected )
		spRender->DrawCombine( mpTexture, rItemRect, CGUIPoint( 0, 0 ), CGUISize( 15, 15 ) );

	if ( mStyle & GUI_LCS_MOUSEOVER && vIsHover )
		spRender->DrawCombine( mpTexture, rItemRect, CGUIPoint( 0, 0 ), CGUISize( 15, 15 ) );

	spRender->PrintText( pText, rTextRect );
}

// ************************************************************ //
// CMenuItem
// ************************************************************ //

CMenuItem::CMenuItem( const char* pText, unsigned int vMenuID, unsigned int vStyle )
{
	this->mText = pText;
	this->mMenuID = vMenuID;
	this->mStyle = vStyle;
}

CMenuItem::~CMenuItem( )
{
}

void CMenuItem::UnExpand( )
{
	mpParent->mpExpand	= NULL;
	mIsExpanded			= false;
	if ( mpExpand != NULL && mStyle == GUI_MSF_POPUPMENU )
		mpExpand->UnExpand( );
}

bool CMenuItem::Expand( )
{
	bool tNeedRecalc = false;
	if ( mpParent != NULL )
	{
		if ( mpParent->mpExpand != NULL && mpParent->mpExpand != this )
		{
			mpParent->mpExpand->UnExpand( );
			tNeedRecalc	= true;
		}
			
		if ( mpParent->mpExpand != this && mStyle == GUI_MSF_POPUPMENU )
		{
			mIsExpanded			= true;
			mpParent->mpExpand	= this;
			tNeedRecalc = true;
		}
	}

	return tNeedRecalc;
}

void CMenuItem::DrawItems( CControlMenu* pMenu )
{
	pMenu->OnDrawItem( mText.c_str( ), mRect, mStyle, mIsHover );

	if ( mStyle == GUI_MSF_POPUPMENU && mIsExpanded == true )
	{
		pMenu->OnDrawMenu( mPopupRect );
		for ( int i = 0; i < int( mItems.size( ) ); i ++ )
			mItems[i]->DrawItems( pMenu );
	}
}

CMenuItem* CMenuItem::HitTest( const CGUIPoint& rPoint )
{
	if ( mIsExpanded == false )
		return NULL;

	CMenuItem* tpItem = NULL;
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
	{
		CMenuItem* tpMenuItem = mItems[i];
		if ( tpMenuItem->mRect.PtInRect( rPoint ) )
			tpItem = tpMenuItem;
		else if ( tpMenuItem->mStyle == GUI_MSF_POPUPMENU )
			tpItem = tpMenuItem->HitTest( rPoint );

		if ( tpItem != NULL )
			break;
	}

	return tpItem;
}

void CMenuItem::GetRectRange( CGUIRect& rRect )
{
	if ( mStyle == GUI_MSF_POPUPMENU && mIsExpanded == true )
	{
		rRect.UnionRect( rRect, mPopupRect );

		for ( int i = 0; i < int( mItems.size( ) ); i ++ )
			mItems[i]->GetRectRange( rRect );
	}
}

// ************************************************************ //
// CControlMenu
// ************************************************************ //

CControlMenu::CControlMenu( const CGUIPoint& rPoint, unsigned int vID ) : CControl( GUI_CSF_NORMAL, "", CGUIRect( rPoint, rPoint + CGUISize( 1, 1 ) ), vID ), mItemHeight( 16 ), mHorzPrefix( 5 ), mVertPrefix( 5 ), mpHover( NULL )
{
	mpRoot	= new CMenuItem( "", 0, GUI_MSF_POPUPMENU );
	mpRoot->mIsExpanded = true;
}

CControlMenu::~CControlMenu( )
{
	delete mpRoot;
}

void CControlMenu::InitControl( bool vIsInit )
{
	GUI_INITEXTURE( vIsInit, mpTexture, "CommonMenu.png" );

	CControl::InitControl( vIsInit );
}

void CControlMenu::OnDrawItem( const char* pText, const CGUIRect& rRect, unsigned int vStyle, bool vIsHover )
{
	if ( vIsHover )
		spRender->BitBlt( mpTexture, CGUIRect( CGUIPoint( 0, 45 ), CGUISize( 64, 16 ) ), rRect, 0xFFFFFFFF );

	if ( vStyle == GUI_MSF_POPUPMENU )
	{
		CGUIRect tExpandRect = rRect;
		tExpandRect.mTopLeft.mX = rRect.mBottomRight.mX - mItemHeight;
		tExpandRect.DeflateRect( 1, 1 );

		if ( vIsHover )
            spRender->BitBlt( mpTexture, CGUIRect( CGUIPoint( 45, 15 ), CGUISize( 16, 16 ) ), tExpandRect, 0xFFFFFFFF );
		else
			spRender->BitBlt( mpTexture, CGUIRect( CGUIPoint( 45, 0 ), CGUISize( 16, 16 ) ), tExpandRect, 0xFFFFFFFF );
	}

	spRender->PrintText( pText, rRect );
}

void CControlMenu::OnDrawMenu( const CGUIRect& rRect )
{
	spRender->DrawCombine( mpTexture, rRect, CGUIPoint( 0, 0 ), CGUISize( 15, 15 ) );
}

void* CControlMenu::AppendItem( void* vParentMenu, const char* pText, unsigned int vMenuID, unsigned int vStyle )
{
	CMenuItem* tpParent = (CMenuItem*) vParentMenu;
	if ( tpParent == NULL )
		tpParent = mpRoot;
	
	if ( tpParent->mStyle != GUI_MSF_POPUPMENU )
		return NULL;

	CMenuItem* tpNewItem = new CMenuItem( pText, vMenuID, vStyle );
	tpNewItem->mpParent = tpParent;

	tpParent->mItems.push_back( tpNewItem );
	int tItemWidth	= 0;
	CGUIPoint tPoint;
	tpParent == mpRoot ? tPoint = mRect.mTopLeft : tPoint = tpParent->mRect.TopRight( ) + CGUISize( mHorzPrefix, 0 );
	for ( int i = 0; i < int( tpParent->mItems.size( ) ); i ++ )
	{
		int tTextCount = int( ::strlen( tpParent->mItems[i]->mText.c_str( ) ) );
		int tFontWidth = spRender->GetTextWidth( tpParent->mItems[i]->mText.c_str( ), tTextCount );
		tItemWidth = max( tItemWidth, tFontWidth + 20 );
	}

	for ( int i = 0; i < int( tpParent->mItems.size( ) ); i ++ )
	{
		tpParent->mItems[i]->mRect.mTopLeft.mX = tPoint.mX;
		tpParent->mItems[i]->mRect.mTopLeft.mY = tPoint.mY + i * mItemHeight;
		tpParent->mItems[i]->mRect.mBottomRight.mX = tPoint.mX + tItemWidth;
		tpParent->mItems[i]->mRect.mBottomRight.mY = tPoint.mY + ( i + 1 ) * mItemHeight;
		tpParent->mItems[i]->mRect += CGUIPoint( mHorzPrefix, mVertPrefix );
	}

	tpParent->mPopupRect.SetRect( tPoint, CGUISize( tItemWidth + mHorzPrefix * 2, int( tpParent->mItems.size( ) ) * mItemHeight + mVertPrefix * 2 ) );
	mpRoot->GetRectRange( mRect );
	return tpNewItem;
}

void CControlMenu::Draw( )
{
	mpRoot->DrawItems( this );
}

void CControlMenu::OnMouseLButtonUp( const CGUIPoint& rPoint, unsigned int vFlags )
{
	CMenuItem* tpItem = mpRoot->HitTest( rPoint );
	if ( tpItem != NULL )
        mpItemClickedHandler( this, tpItem->mMenuID );

	CControl::OnMouseLButtonUp( rPoint, vFlags );
}

void CControlMenu::OnMouseMove( const CGUIPoint& rPoint, unsigned int vFlags )
{
	CMenuItem* tpItem = mpRoot->HitTest( rPoint );

	if ( mpHover != NULL )
		mpHover->mIsHover = false;

	if ( tpItem != NULL )
		tpItem->mIsHover = true;

	mpHover = tpItem;
	if ( mpHover != NULL && mpHover->Expand( ) )
	{
		mRect.mBottomRight = mRect.mTopLeft + CGUISize( 1, 1 );
		mpRoot->GetRectRange( mRect );
	}

	return CControl::OnMouseMove( rPoint, vFlags );
}

void CControlMenu::OnHover( const CGUIPoint& rPoint, bool vHover )
{
	if ( vHover == false )
	{
		if ( mpHover != NULL )
			mpHover->mIsHover = false;

		mpHover = NULL;
	}
	return CControl::OnHover( rPoint, vHover );
}

// ************************************************************ //
// CControlCommonScrollBar
// ************************************************************ //

CControlSlider::CControlSlider( unsigned int vStyle, const CGUIRect& rRect, unsigned int vID ) : CControlScrollBar( vStyle, rRect, vID )
{
}

CControlSlider::~CControlSlider( )
{
}

int CControlSlider::GetMinCursorLength( )
{
	return 13;
}

CGUIRect CControlSlider::GetPageUpRect( ) const
{
	return CGUIRect( mRect.mTopLeft, CGUISize( 0, 0 ) );
}

CGUIRect CControlSlider::GetPageDownRect( ) const
{
	return CGUIRect( CGUIPoint( mRect.mBottomRight.mX, mRect.mBottomRight.mY ), CGUISize( 0, 0 ) );
}

// ************************************************************ //
// CControlImageButton
// ************************************************************ //

CControlImageButton::CControlImageButton( const char* pResName, const CGUISize& rImageSize, const CGUIRect& rRect, unsigned int vID ) : CControlButton( "", rRect, vID ), mResName( pResName ), mImageSize( rImageSize )
{
}

CControlImageButton::~CControlImageButton( )
{
}

void CControlImageButton::InitControl( bool vIsInit )
{
	GUI_INITEXTURE( vIsInit, mpTexture, mResName.c_str( ) );
}

void CControlImageButton::Draw( )
{
	if ( IsFlag( GUI_CTL_CLICKED ) )
		spRender->BitBlt( mpTexture, CGUIRect( CGUIPoint( 0, mImageSize.mCY * 2 ), mImageSize ), mRect, 0xFFFFFFFF );
	else if ( IsFlag( GUI_CTL_HOVER ) )
		spRender->BitBlt( mpTexture, CGUIRect( CGUIPoint( 0, mImageSize.mCY ), mImageSize ), mRect, 0xFFFFFFFF );
	else if ( IsFlag( GUI_CTL_ENABLE ) )
		spRender->BitBlt( mpTexture, CGUIRect( CGUIPoint( 0, 0 ), mImageSize ), mRect, 0xFFFFFFFF );
	else
		spRender->BitBlt( mpTexture, CGUIRect( CGUIPoint( 0, mImageSize.mCY * 3 ), mImageSize ), mRect, 0xFFFFFFFF );
}

// ************************************************************ //
// CControlImageCheck
// ************************************************************ //

CControlImageCheck::CControlImageCheck( const char* pResName, const CGUISize& rImageSize, const CGUIRect& rRect, unsigned int vID ) : CControlCheck( "", rRect, vID ), mResName( pResName ), mImageSize( rImageSize )
{
}

CControlImageCheck::~CControlImageCheck( )
{
}

void CControlImageCheck::InitControl( bool vIsInit )
{
	GUI_INITEXTURE( vIsInit, mpTexture, mResName.c_str( ) );
}

void CControlImageCheck::Draw( )
{
	CGUIRect tSrcRect;
	if ( IsFlag( GUI_CTL_DISABLE ) )
	{
		tSrcRect.mTopLeft.mX		= 0;
		tSrcRect.mBottomRight.mX	= mImageSize.mCX;
		tSrcRect.mTopLeft.mY		= mImageSize.mCY * 6;
		tSrcRect.mBottomRight.mY	= mImageSize.mCY * 7;
	}
	else if ( IsFlag( GUI_CTL_CLICKED ) )
	{
		if ( mIsChecked )
		{
			tSrcRect.mTopLeft.mX		= 0;
			tSrcRect.mBottomRight.mX	= mImageSize.mCX;
			tSrcRect.mTopLeft.mY		= mImageSize.mCY * 5;
			tSrcRect.mBottomRight.mY	= mImageSize.mCY * 6;
		}
		else
		{
			tSrcRect.mTopLeft.mX		= 0;
			tSrcRect.mBottomRight.mX	= mImageSize.mCX;
			tSrcRect.mTopLeft.mY		= mImageSize.mCY * 4;
			tSrcRect.mBottomRight.mY	= mImageSize.mCY * 5;
		}
	}
	else if ( IsFlag( GUI_CTL_HOVER ) )
	{
		if ( mIsChecked )
		{
			tSrcRect.mTopLeft.mX		= 0;
			tSrcRect.mBottomRight.mX	= mImageSize.mCX;
			tSrcRect.mTopLeft.mY		= mImageSize.mCY * 3;
			tSrcRect.mBottomRight.mY	= mImageSize.mCY * 4;
		}
		else
		{
			tSrcRect.mTopLeft.mX		= 0;
			tSrcRect.mBottomRight.mX	= mImageSize.mCX;
			tSrcRect.mTopLeft.mY		= mImageSize.mCY * 1;
			tSrcRect.mBottomRight.mY	= mImageSize.mCY * 2;
		}
	}
	else
	{
		if ( mIsChecked )
		{
			tSrcRect.mTopLeft.mX		= 0;
			tSrcRect.mBottomRight.mX	= mImageSize.mCX;
			tSrcRect.mTopLeft.mY		= mImageSize.mCY * 2;
			tSrcRect.mBottomRight.mY	= mImageSize.mCY * 3;
		}
		else
		{
			tSrcRect.mTopLeft.mX		= 0;
			tSrcRect.mBottomRight.mX	= mImageSize.mCX;
			tSrcRect.mTopLeft.mY		= 0;
			tSrcRect.mBottomRight.mY	= mImageSize.mCY;
		}
	}

	CGUIRect tDestRect = mRect;
	tDestRect.SetRect( mRect.mTopLeft, mImageSize );
	spRender->BitBlt( mpTexture, tSrcRect, tDestRect, 0xFFFFFFFF );
}

// ************************************************************ //
// CTabItem
// ************************************************************ //

CTabItem::CTabItem( const char* pText ) : mText( pText ), mIsSelected( false )
{
}

CTabItem::~CTabItem( )
{
}

// ************************************************************ //
// CControlTab
// ************************************************************ //

CControlTab::CControlTab( const CGUIRect& rRect, unsigned int vID ) : CControlDialog( GUI_CSF_NORMAL, "", rRect, vID ), 
	mTabActiveHeight( 20 ), mTabUnactiveHeight( 16 ), mBorderLength( 4 ), mTabInterval( 4 ), mHorzPrefix( 5 ), mTabBtnPrefix( 10 )
{
}

CControlTab::~CControlTab( )
{
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
		delete mItems[i];
}

void CControlTab::OnMouseLButtonDown( const CGUIPoint& rPoint, unsigned int vFlags )
{
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
	{
		if ( mItems[i]->mRect.PtInRect( rPoint - mRect.mTopLeft + mCamPoint ) )
		{
			SetSel( i );
			break;
		}
	}

	CControlDialog::OnMouseLButtonDown( rPoint, vFlags );
}

void CControlTab::SetSel( int vIndex )
{
	unsigned int tOldIndex = 0xFFFFFFFF;
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
	{
		if ( mItems[i]->mIsSelected == true )
		{
			mItems[i]->mIsSelected = false;
			tOldIndex = i;
			break;
		}
	}
	mItems[vIndex]->mIsSelected = true;

	if ( mpTabChangedHandler != NULL )
        mpTabChangedHandler( this, vIndex, tOldIndex );
}

void CControlTab::AppendItem( const char* pText )
{
	CTabItem* tpItem = new CTabItem( pText );
	int tTabCount = int( mItems.size( ) );
	int tTabBtnWidth = mTabBtnPrefix * 2 + spRender->GetTextWidth( pText, int( ::strlen( pText ) ) );
	int tTabLeft = 0;
	if ( tTabCount == 0 )
		tTabLeft = mHorzPrefix;
	else
		tTabLeft = mItems[tTabCount - 1]->mRect.mBottomRight.mX + mTabInterval;

	tpItem->mRect.SetRect( tTabLeft, mRect.Height( ) - mTabActiveHeight, tTabLeft + tTabBtnWidth, mRect.Height( ) );
	mItems.push_back( tpItem );
}

CGUIRect CControlTab::GetTabClientRect( ) const
{
	CGUIRect tClientRect = mRect;
	tClientRect.DeflateRect( mBorderLength, mBorderLength, mBorderLength, mTabActiveHeight + 2 );
	return tClientRect;
}

void CControlTab::OnDrawTab( const CGUIRect& rRect )
{
	spRender->DrawCombine( mpTexture, rRect, CGUIPoint( 0, 0 ), CGUISize( 15, 15 ) );
}

void CControlTab::OnDrawItem( const char* pText, const CGUIRect& rRect, bool vIsSelected )
{
	unsigned int tTextStyle = 0;
	CGUIPoint tPoint;
	if ( vIsSelected == true )
	{
		tPoint		= CGUIPoint( 0, 45 );
		tTextStyle	= GUI_DT_VCENTER | GUI_DT_HCENTER;
	}
	else
	{
		tPoint		= CGUIPoint( 0, 90 );
		tTextStyle	= GUI_DT_BOTTOM | GUI_DT_HCENTER;
	}

	spRender->DrawCombine( mpTexture, rRect, tPoint, CGUISize( 15, 15 ) );
	spRender->PrintText( pText, rRect, tTextStyle );
}

void CControlTab::Draw( ) 
{
	CGUIRect tOldClipRect = spRender->SetClipRect( mRect );
	CGUIRect tTabRect = mRect;
	tTabRect.DeflateRect( 0, 0, 0, mTabUnactiveHeight );
	CTabItem* tpSelItem = NULL;
	for ( int i = 0; i < int( mItems.size( ) ); i ++ )
	{
		if ( mItems[i]->mIsSelected == true )
			tpSelItem = mItems[i];
		else
			OnDrawItem( mItems[i]->mText.c_str( ), mItems[i]->mRect + mRect.mTopLeft - mCamPoint, mItems[i]->mIsSelected );
	}

	OnDrawTab( tTabRect );
	if ( tpSelItem != NULL )
		OnDrawItem( tpSelItem->mText.c_str( ), tpSelItem->mRect + mRect.mTopLeft - mCamPoint, tpSelItem->mIsSelected );
	
	spRender->SetClipRect( tOldClipRect );
	CControlDialog::Draw( );
}

void CControlTab::InitControl( bool vIsInit )
{
	GUI_INITEXTURE( vIsInit, mpTexture, "CommonTab.png" );

	CControlDialog::InitControl( vIsInit );
}

// ************************************************************ //
// CMessageBox
// ************************************************************ //

CMessageBox::CMessageBox( unsigned int vStyle, const char* pTitle, const char* pText, const CGUIRect& rRect, unsigned int vID )
	: CControlPopup( vStyle | GUI_DSF_CANMOVE | GUI_DSF_BORDER | GUI_DSF_CAPTION | GUI_DSF_SYSMENU, pTitle, rRect, vID ), mpYes( NULL ), mpNo( NULL ), mText( pText )
{
	CGUIRect tClientRect = GetClientRect( );
	CGUIPoint tBottomCenter;
	tBottomCenter.mX = tClientRect.CenterPoint( ).mX;
	tBottomCenter.mY = tClientRect.BottomRight( ).mY;
	if ( mStyle & GUI_MBS_OK && mStyle & GUI_MBS_CANCEL )
	{
		mpYes = new CControlButton( "确定", CGUIRect( tBottomCenter.mX - 80, tBottomCenter.mY - 25, tBottomCenter.mX - 20, tBottomCenter.mY - 5 ), IDC_MESSAGEBOX_BUTTON_OK );
		mpNo  = new CControlButton( "取消", CGUIRect( tBottomCenter.mX + 20, tBottomCenter.mY - 25, tBottomCenter.mX + 80, tBottomCenter.mY - 5 ), IDC_MESSAGEBOX_BUTTON_CANCEL );
		InsertControl( mpYes, GUI_CR_PAGE );
		InsertControl( mpNo, GUI_CR_PAGE );
	}
	else if ( mStyle & GUI_MBS_OK )
	{
		mpYes = new CControlButton( "确定", CGUIRect( tBottomCenter.mX - 30, tBottomCenter.mY - 25, tBottomCenter.mX + 30, tBottomCenter.mY - 5 ), IDC_MESSAGEBOX_BUTTON_OK );
		InsertControl( mpYes, GUI_CR_PAGE );
	}
	else if ( mStyle & GUI_MBS_CANCEL )
	{
		mpNo  = new CControlButton( "取消", CGUIRect( tBottomCenter.mX - 30, tBottomCenter.mY - 25, tBottomCenter.mX + 30, tBottomCenter.mY - 5 ), IDC_MESSAGEBOX_BUTTON_CANCEL );
		InsertControl( mpNo, GUI_CR_PAGE );
	}
}

CMessageBox::~CMessageBox( )
{
}

void CMessageBox::Offset( const CGUIPoint& rPoint )
{
	CControlPopup::Offset( rPoint );
}

void CMessageBox::Offset( const CGUIPoint& rPoint, const CGUISize& rSize )
{
	CControlPopup::Offset( rPoint, rSize );

	CGUIRect tClientRect = GetClientRect( );
	CGUIPoint tBottomCenter;
	tBottomCenter.mX = tClientRect.CenterPoint( ).mX;
	tBottomCenter.mY = tClientRect.BottomRight( ).mY;
	if ( mStyle & GUI_MBS_OK && mStyle & GUI_MBS_CANCEL )
	{
		mpYes->SetRect( CGUIRect( tBottomCenter.mX - 80, tBottomCenter.mY - 25, tBottomCenter.mX - 20, tBottomCenter.mY - 5 ) );
		mpNo->SetRect( CGUIRect( tBottomCenter.mX + 20, tBottomCenter.mY - 25, tBottomCenter.mX + 80, tBottomCenter.mY - 5 ) );
	}
	else if ( mStyle & GUI_MBS_OK )
		mpYes->SetRect( CGUIRect( tBottomCenter.mX - 30, tBottomCenter.mY - 25, tBottomCenter.mX + 30, tBottomCenter.mY - 5 ) );
	else if ( mStyle & GUI_MBS_CANCEL )
		mpNo->SetRect( CGUIRect( tBottomCenter.mX - 30, tBottomCenter.mY - 25, tBottomCenter.mX + 30, tBottomCenter.mY - 5 ) );
}

void CMessageBox::GetMinMaxInfo( CGUISize& rSizeMin, CGUISize& rSizeMax )
{
	rSizeMin.mCX = 200;	rSizeMin.mCY = 150;
	rSizeMax.mCX = 650;	rSizeMax.mCY = 650;
}

void CMessageBox::Draw( )
{
	CControlPopup::Draw( );
	CGUIRect tClientRect = GetClientRect( );
	spRender->PrintText( mText.c_str( ), tClientRect - CGUIPoint( 0, 10 ), GUI_DT_VCENTER | GUI_DT_HCENTER );
}

// ************************************************************ //
// CControlProgress
// ************************************************************ //

CControlProgress::CControlProgress( const CGUIRect& rRect, unsigned int vID  ) : CControl( GUI_CSF_NORMAL, "", rRect, vID ), mRangeMax( 100 ), mRangeMin( 0 ), mPos( 0 )
{
}

CControlProgress::~CControlProgress( )
{
}

void CControlProgress::Draw( )
{
	CGUIRect tInner = mRect;
	tInner.DeflateRect( 2, 2 );
	tInner.mBottomRight.mX = tInner.mTopLeft.mX + int( ( mPos - mRangeMin ) / ( (float) mRangeMax - mRangeMin ) * tInner.Width( ) );

	OnDrawShade( );
	OnDrawSlider( tInner );
}

void CControlProgress::InitControl( bool vIsInit )
{
	GUI_INITEXTURE( vIsInit, mpTexture, "CommonProgress.bmp" );
	CControl::InitControl( vIsInit );
}

void CControlProgress::OnDrawShade( )
{
	spRender->DrawCombine( mpTexture, mRect, CGUIPoint( 0, 0 ), CGUISize( 80, 5 ) );
}

void CControlProgress::OnDrawSlider( const CGUIRect& rRect )
{
	CGUIRect tInner = mRect; tInner.DeflateRect( 2, 2, 1, 1 );
	spRender->FillRect( rRect, 0xFF800000 );
	spRender->DrawLine( rRect.TopRight( ), rRect.BottomRight( ), 0x80000000 );
	spRender->DrawRect( tInner, 0xFF000000, 0xFF808080 );
}

// ************************************************************ //
// CPopupFileSystem
// ************************************************************ //

CPopupFileSystem::CPopupFileSystem( unsigned int vID ) : CControlPopup( GUI_DSF_SYSMENU | GUI_DSF_BORDER | GUI_DSF_CAPTION | GUI_DSF_CANMOVE, "文件打开", CGUIRect( 0, 0, 600, 400 ), vID )
{
	CGUIRect tClientRect = GetClientRect( );
	CControlList* tpFileList = new CControlList( CGUIRect( tClientRect.mTopLeft.mX + 5, tClientRect.mTopLeft.mY + 37, tClientRect.mBottomRight.mX - 5, tClientRect.mBottomRight.mY - 5 ), IDC_FILEPOPUP_LIST_FILELIST );
	CControlEdit* tpFileName = new CControlEdit( GUI_ESF_SINGLELINE | GUI_ESF_AUTOHSCROLL, CGUIRect( tClientRect.mTopLeft.mX + 75, tClientRect.mTopLeft.mY + 5, tClientRect.mTopLeft.mX + 250, tClientRect.mTopLeft.mY + 32 ), IDC_FILEPOPUP_EDIT_FILENAME );
	CControlButton* tpUpper	 = new CControlButton( "<-", CGUIRect( tClientRect.mTopLeft.mX + 260, tClientRect.mTopLeft.mY + 9, tClientRect.mTopLeft.mX + 300, tClientRect.mTopLeft.mY + 30 ), IDC_FILEPOPUP_BUTTON_UPPER );
	CControlButton* tpOk	 = new CControlButton( "确定", CGUIRect( tClientRect.mBottomRight.mX - 65, tClientRect.mTopLeft.mY + 9, tClientRect.mBottomRight.mX - 5, tClientRect.mTopLeft.mY + 30 ), IDC_FILEPOPUP_BUTTON_OK );
	tpFileList->AppendColumn( "名称", 300, GUI_LCSFMT_LEFT );
	tpFileList->AppendColumn( "大小", 50, GUI_LCSFMT_LEFT );
	tpFileList->AppendColumn( "类型", 80, GUI_LCSFMT_LEFT );
	tpFileList->AppendColumn( "修改时间", 152, GUI_LCSFMT_LEFT );

	tpUpper->SetClickedHandler( OnUpDirectory );
	tpFileList->SetDblClkHandler( OnFileListDblClk );
	tpFileList->SetClickHandler( OnFileListClick );
	tpOk->SetClickedHandler( OnClickOK );
	InsertControl( tpFileList, GUI_CR_PAGE );
	InsertControl( tpFileName, GUI_CR_PAGE );
	InsertControl( tpUpper, GUI_CR_PAGE );
	InsertControl( tpOk, GUI_CR_PAGE );

	char tCurPath[ MAX_PATH ] = { 0 };
	::GetCurrentDirectory( MAX_PATH, tCurPath );
	mCurrentPath = tCurPath;
	RefreshFileList( );
}

CPopupFileSystem::~CPopupFileSystem( )
{
}

void CPopupFileSystem::OnClickOK( CControlButton* pButton )
{
	CPopupFileSystem* tpFileSystem = (CPopupFileSystem*) pButton->GetParent( );
	std::string tFileName = tpFileSystem->GetFileName( );
	if ( GetFileAttributes( tFileName.c_str( ) ) & FILE_ATTRIBUTE_DIRECTORY )
	{
		tpFileSystem->mCurrentPath = tFileName;
		tpFileSystem->RefreshFileList( );
	}
	else
	{
		if ( tpFileSystem->mpFileSelected != NULL )
            tpFileSystem->mpFileSelected( tpFileSystem, tFileName.c_str( ) );
		pButton->GetPage( )->ClosePopup( tpFileSystem );
	}
}

std::string CPopupFileSystem::GetFileName( ) const
{
	CControlEdit* tpEdit = (CControlEdit*) GetSubControl( IDC_FILEPOPUP_EDIT_FILENAME );
	std::string tText;
	if ( mCurrentPath.empty( ) )
		tText += tpEdit->GetEditText( );
	else
	{
		tText = mCurrentPath;
		tText += "\\";
		tText += tpEdit->GetEditText( );
	}

	return tText;
}

void CPopupFileSystem::OnFileListClick( CControlList* pList, int vItemIndex )
{
	if ( vItemIndex == -1 )
		return;

	CPopupFileSystem* tpFileSystem = (CPopupFileSystem*) pList->GetParent( );
	CControlEdit* tpEdit = (CControlEdit*) tpFileSystem->GetSubControl( IDC_FILEPOPUP_EDIT_FILENAME );
	tpEdit->SetEditText( tpFileSystem->mFileList[ pList->GetItemData( vItemIndex ) ].c_str( ) );
}

void CPopupFileSystem::OnFileListDblClk( CControlList* pList, int vItemIndex )
{
	if ( vItemIndex == -1 )
		return;

	CPopupFileSystem* tpFileSystem = (CPopupFileSystem*) pList->GetParent( );
	std::string tFilePath = tpFileSystem->GetFileName( );
	if ( GetFileAttributes( tFilePath.c_str( ) ) & FILE_ATTRIBUTE_DIRECTORY )
	{
		tpFileSystem->mCurrentPath = tFilePath;
		tpFileSystem->RefreshFileList( );
	}
}

void CPopupFileSystem::OnUpDirectory( CControlButton* pButton )
{
	CPopupFileSystem* tpFileSystem = (CPopupFileSystem*) pButton->GetParent( );
	for ( int i = int( tpFileSystem->mCurrentPath.size( ) ) - 1; i >= 0; i -- )
	{
		if ( tpFileSystem->mCurrentPath[ i ] == '\\' )
		{
			tpFileSystem->mCurrentPath.erase( tpFileSystem->mCurrentPath.begin( ) + i );
			break;
		}
        
		tpFileSystem->mCurrentPath.erase( tpFileSystem->mCurrentPath.begin( ) + i );
	}
	tpFileSystem->RefreshFileList( );
}

void CPopupFileSystem::RefreshFileList( )
{
	CControlList* pFileList = (CControlList*) GetSubControl( IDC_FILEPOPUP_LIST_FILELIST );
	CControlEdit* pFileName = (CControlEdit*) GetSubControl( IDC_FILEPOPUP_EDIT_FILENAME );
	char tFilePath[ MAX_PATH ] = { 0 };
	::strcat( tFilePath, mCurrentPath.c_str( ) );
	::strcat( tFilePath, "\\*.*" );
	pFileName->ResetContext( );
	pFileList->RemoveAllItems( );
	mFileList.clear( );

	int tItem = 0;
	char tDriverName[3] = { 0, ':', 0 };
	if ( mCurrentPath.empty( ) )
	{
		DWORD tMaskBit = ::GetLogicalDrives( ) ;
		for ( int i = 0; i < 26; i ++ )
		{
			if ( tMaskBit & ( int( pow( 2, i ) ) ) )
			{
				tDriverName[0] = 'A' + i;
				mFileList.push_back( tDriverName );
				tItem = pFileList->AppendItem( tDriverName );
				pFileList->SetItemData( tItem, int( mFileList.size( ) ) - 1 );
				pFileList->SetItem( tItem, 2, "本地磁盘" );
			}
		}
	}
	else
	{
		WIN32_FIND_DATA tFfd;
		HANDLE tFind = ::FindFirstFile( tFilePath, &tFfd );
		if ( tFind == INVALID_HANDLE_VALUE )
			return;

		for ( BOOL tFindNext = TRUE; tFindNext == TRUE; tFindNext = ::FindNextFile( tFind, &tFfd ) )
		{
			if ( strcmp( tFfd.cFileName, "." ) == 0 || strcmp( tFfd.cFileName, ".." ) == 0 )
				continue;

			if ( !( tFfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
			{
				tItem = pFileList->AppendItem( tFfd.cFileName );
				__int64 tFileSize = ( tFfd.nFileSizeHigh << 32 ) + tFfd.nFileSizeLow;
				pFileList->SetItem( tItem, 1, CGUIText( "%dK", tFileSize / 1024 ) );
				pFileList->SetItem( tItem, 2, "文件" );
			}
			else
			{
				std::string tFileName = "["; tFileName += tFfd.cFileName; tFileName += "]";
				tItem = pFileList->AppendItem( tFileName.c_str( ) );
				pFileList->SetItem( tItem, 2, "目录" );
			}

			mFileList.push_back( tFfd.cFileName );
			pFileList->SetItemData( tItem, int( mFileList.size( ) ) - 1 );
			SYSTEMTIME tSystemTime; ::FileTimeToSystemTime( &tFfd.ftLastWriteTime, &tSystemTime );
			pFileList->SetItem( tItem, 3, CGUIText( "%d-%d-%d %d:%d:%d", tSystemTime.wYear, tSystemTime.wMonth, tSystemTime.wDay, tSystemTime.wHour,
				tSystemTime.wMinute, tSystemTime.wSecond ) );
		}

		::FindClose( tFind );
	}
}

void CPopupFileSystem::InitControl( bool vIsInit )
{
	CControlPopup::InitControl( vIsInit );
}

void CPopupFileSystem::Draw( )
{
	CControlPopup::Draw( );

	CGUIRect tClientRect = GetClientRect( );
	spRender->PrintText( "文件名:", tClientRect + CGUIPoint( 5, 10 ), GUI_DT_LEFT | GUI_DT_TOP );

	CGUIRect tListRect = GetSubControl( IDC_FILEPOPUP_LIST_FILELIST )->GetRect( ); tListRect.InflateRect( 1, 1 );
	spRender->DrawRect( tListRect, 0xFF000000, 0xFF808080 );
}

}