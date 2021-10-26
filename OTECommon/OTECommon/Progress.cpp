// Progress.cpp : 实现文件
//
#include "stdafx.h"
#include "Progress.h"

#define IDC_CANCEL   10
#define IDC_TEXT     11
#define IDC_PROGRESS 12

LPCTSTR szSection = _T( "设置" );   
LPCTSTR szEntryX  = _T( "X" );
LPCTSTR szEntryY  = _T( "Y" );

Progress::tagInitCommonControls Progress::m_InitCommonControls;

Progress::tagInitCommonControls::tagInitCommonControls()
{
	INITCOMMONCONTROLSEX icce;

	icce.dwSize = sizeof( INITCOMMONCONTROLSEX );
	icce.dwICC	= PROGRESS_CONTROL_CLASSES;

	InitCommonControlsEx( &icce );
}

static DWORD WINAPI ThreadProc( LPVOID lpThreadParameter )
{
    LPPROGRESSTHREADDATA pThreadData = ( LPPROGRESSTHREADDATA )lpThreadParameter;
	pThreadData->bAlive = true;
	INT_PTR nResult = IDCANCEL;
	if( pThreadData->bTerminate == true )
		goto TerminateThreadProc;

	nResult = ( true == ( *pThreadData->m_lpUserProc )( ( PROGRESSDATA* )lpThreadParameter ) ) ? IDOK : IDCANCEL;

TerminateThreadProc:
	pThreadData->bAlive = false;
	if( pThreadData->bTerminate == false )
		::PostMessage( pThreadData->hThreadWnd, PROGRESSTHREADDATA::WM_PROGRESSTHREADCOMPLETED, MAKEWPARAM( nResult, 0 ), 0 );

	return 0;    
}

Progress::Progress( HWND hParentWnd, LP_PROGRESS_USERPROC lpUserProc, LPVOID lpUserProcParam, LPCTSTR lpszDlgTitle, bool bAllowCancel )
{
	m_hThread = NULL;
	m_hWnd = NULL;
	m_hParentWnd = hParentWnd;	
	m_bAllowCancel = bAllowCancel;	
	m_ThreadData.pUserProcParam	= lpUserProcParam;
	m_ThreadData.m_lpUserProc = lpUserProc;
	::ZeroMemory( m_szDialogCaption, sizeof( m_szDialogCaption ) );
	_tcsncpy( m_szDialogCaption, lpszDlgTitle, ( sizeof( m_szDialogCaption ) / sizeof( m_szDialogCaption[ 0 ] ) ) - 1 );
}

Progress::~Progress()
{
	Cleanup();
}

void Progress::Cleanup()
{
	m_ThreadData.bTerminate = true;

	if( m_ThreadData.bAlive )
	{
		::Sleep( PROGRESS_TERMINATE_DELAY );
		DWORD dwExitCode = 0;
        if( ::GetExitCodeThread( m_hThread, &dwExitCode ) && dwExitCode == STILL_ACTIVE )
			::TerminateThread( m_hThread, IDCANCEL );
	}

	if( m_hThread )
	{
		::CloseHandle( m_hThread );
		m_hThread = NULL;
	}

    m_ThreadData.bAlive = false;
	m_ThreadData.bTerminate = false;
	m_ThreadData.hThreadWnd = NULL;
}

INT_PTR Progress::DoModal( HINSTANCE hInstance )
{
	Cleanup();

	INT_PTR iReturn = ::DialogBoxParam( AfxGetInstanceHandle(), MAKEINTRESOURCE( IDD ), m_hParentWnd, ProgressDlgProc, ( LPARAM )this );
	DWORD dwLastError = GetLastError();
#ifdef _DEBUG
	TRACE( _T( "GetLastError返回值%u\n" ), dwLastError );
#endif

	//有问题
//	TCHAR * pszClassName = _T( "Progress" );
//	WNDCLASS wc = { CS_DBLCLKS,	*( WNDPROC* )&ProgressDlgProc, 0, 0, hInstance, 0, ::LoadCursor( 0, IDC_ARROW ), 0, 0, pszClassName };
//
//	if( !::RegisterClass( &wc ) )
//	{
//		DWORD dwLastError = GetLastError();
//		if( dwLastError != ERROR_CLASS_ALREADY_EXISTS )
//		{
//			TRACE( _T( "注册窗口类错误。GetLastError返回值%u\n" ), dwLastError );
//			_ASSERTE( FALSE );
//			return FALSE;
//		}
//	}
//
//	m_hWnd = ::CreateWindowEx( 0, pszClassName, _T(""), WS_POPUP | WS_BORDER | WS_CAPTION, 0, 0, 390, 130, NULL, NULL, hInstance, NULL );
//	DWORD dwLastError = GetLastError();
//
//	if( m_hWnd == NULL )
//	{
//#ifdef _DEBUG
//		DWORD dwLastError = GetLastError();
//		UNUSED( dwLastError );
//		TRACE( _T( "CreateWindowEx错误， GetLastError返回值%u\n" ), dwLastError );
//		_ASSERTE(m_hWnd);
//#endif
//		return FALSE;
//	}

	return iReturn;
}

INT_PTR CALLBACK ProgressDlgProc( HWND hDlg, UINT Message, WPARAM wParam, LPARAM lParam )
{
	switch( Message )
	{
	case WM_INITDIALOG:
		{
			Progress *pProgressDialog = ( Progress* )lParam;
			if( pProgressDialog->m_bAllowCancel == false )
				::SendMessage( hDlg, PROGRESSTHREADDATA::WM_DISABLECONTROLS, wParam, lParam );

			::SendMessage( ::GetDlgItem( hDlg, PROGRESS_TEXTBOX_ID ), WM_SETTEXT, 0, ( LPARAM )_T( "" ) );
			::SendMessage( ::GetDlgItem( hDlg, PROGRESS_PROGRESSBAR_ID ), PBM_SETPOS, 0, 0 );
			::SendMessage( hDlg, WM_SETTEXT, 0, ( LPARAM )pProgressDialog->m_szDialogCaption );
			SetWindowLongPtr( hDlg, GWL_USERDATA,( LONG_PTR )pProgressDialog );

			( ( LPPROGRESSTHREADDATA )( LPVOID )( &pProgressDialog->m_ThreadData ) )->hThreadWnd = hDlg;
			DWORD dwThreadId = 0;
			pProgressDialog->m_hThread = ::CreateThread( NULL, NULL, ThreadProc, &pProgressDialog->m_ThreadData, 0, &dwThreadId );
			if( pProgressDialog->m_hThread == NULL )
			{
				TRACE( _T( "CreateThread错误， m_hThread为空\n" ) );
				::EndDialog( hDlg, IDCANCEL );
			}

			return TRUE;
		}
	case WM_COMMAND:
		{
			if( PROGRESS_CANCELBUTTON_ID == LOWORD( wParam ) )
			{
				::SendMessage( hDlg, PROGRESSTHREADDATA::WM_CANCELPROGRESSTHREAD, 0, 0 );
				return TRUE;
			}
			break;
		}
	case WM_SYSCOMMAND:
		{
			if( SC_CLOSE == wParam )
			{
				::SendMessage( hDlg, PROGRESSTHREADDATA::WM_CANCELPROGRESSTHREAD, 0, 0 );
				return TRUE;
			}
			break;
		}
	case PROGRESSTHREADDATA::WM_DISABLECONTROLS:
		{
			::EnableMenuItem( ::GetSystemMenu( hDlg, false ), SC_CLOSE, MF_DISABLED | MF_GRAYED | MF_BYCOMMAND );
			::EnableWindow( ::GetDlgItem( hDlg, PROGRESS_CANCELBUTTON_ID ), false );
			return TRUE;
		}
	case PROGRESSTHREADDATA::WM_PROGRESSTHREADCOMPLETED:
		{
			::EndDialog( hDlg, wParam );
			return TRUE;
		}		
	case PROGRESSTHREADDATA::WM_PROGRESSTEXTUPDATE:
		{
			::SendMessage( ::GetDlgItem( hDlg, PROGRESS_TEXTBOX_ID ), WM_SETTEXT, 0, lParam );
			return TRUE;
		}
	case PROGRESSTHREADDATA::WM_PROGRESSBARUPDATE:
		{
			::SendMessage( ::GetDlgItem( hDlg, PROGRESS_PROGRESSBAR_ID ), PBM_SETPOS, wParam, 0 );
			return TRUE;
		}
	case PROGRESSTHREADDATA::WM_CANCELPROGRESSTHREAD:
		{
			LPPROGRESSTHREADDATA pThreadData = ( LPPROGRESSTHREADDATA )( LPVOID )( &( ( Progress* )GetWindowLongPtr( hDlg, GWL_USERDATA ) )->m_ThreadData );
			pThreadData->bTerminate = true;
			::SendMessage( GetDlgItem( hDlg, PROGRESS_TEXTBOX_ID ), WM_SETTEXT, 0, ( LPARAM )( _T( "停止。。。" ) ) );
			::SendMessage( hDlg, PROGRESSTHREADDATA::WM_DISABLECONTROLS, wParam, lParam );
			if( pThreadData->bAlive )
				::Sleep( PROGRESS_TERMINATE_DELAY );
			if( pThreadData->bAlive )
				::SendMessage( ::GetDlgItem( hDlg, PROGRESS_TEXTBOX_ID), WM_SETTEXT, 0, ( LPARAM )( _T("完成！！！" ) ) );
			if( pThreadData->bAlive )
				::Sleep( PROGRESS_TERMINATE_DELAY );
			::EndDialog( hDlg, MAKEWPARAM( PROGRESS_CANCELBUTTON_ID, 1 ) );

			return TRUE;
		}
	}

	return FALSE;
}

CProgressWnd::CProgressWnd()
{
    CommonConstruct();
}

CProgressWnd::CProgressWnd( CWnd* pParent, LPCTSTR pszTitle, BOOL bSmooth )
{
    CommonConstruct();
    m_strTitle = pszTitle;
    Create( pParent, pszTitle, bSmooth );
}

void CProgressWnd::CommonConstruct()
{
    m_wRenenableWnd = NULL;
    m_nNumTextLines = 4;
    m_nPrevPos = 0;
    m_nPrevPercent = 0;
    m_nStep = 1;
    m_nMinValue = 0;
    m_nMaxValue = 100;
    m_strTitle = _T( "进度" );
    m_strCancelLabel = _T( "取消" );
    m_bCancelled = FALSE;
    m_bModal = FALSE;
    m_bPersistantPosition = TRUE;
}

CProgressWnd::~CProgressWnd()
{
    DestroyWindow();
}

BOOL CProgressWnd::Create( CWnd* pParent, LPCTSTR pszTitle, BOOL bSmooth )
{
    BOOL bSuccess;

    CString csClassName = AfxRegisterWndClass( CS_OWNDC | CS_HREDRAW | CS_VREDRAW, ::LoadCursor( NULL, IDC_APPSTARTING ), CBrush( ::GetSysColor( COLOR_BTNFACE ) ) );

    NONCLIENTMETRICS ncm;
    ncm.cbSize = sizeof( NONCLIENTMETRICS );
	VERIFY( ::SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( NONCLIENTMETRICS ), &ncm, 0 ) );
    m_font.CreateFontIndirect( &( ncm.lfMessageFont ) ); 

    if( !pParent )
        pParent = AfxGetMainWnd();

    bSuccess = CreateEx( WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, csClassName, pszTitle, WS_POPUP | WS_BORDER | WS_CAPTION,    
                        0, 0, 200, 80, pParent->GetSafeHwnd(), 0, NULL );    
    if( !bSuccess ) 
	{
		DWORD dwLastError = GetLastError();
		TRACE( _T( "窗口CreateEx错误， GetLastError返回值%u\n" ), dwLastError );
		return FALSE;
	}

    CRect TempRect( 0, 0, 10, 10 );
    bSuccess = m_Text.Create( _T( "" ), WS_CHILD | WS_VISIBLE | SS_NOPREFIX | SS_LEFTNOWORDWRAP, TempRect, this, IDC_TEXT );
    if( !bSuccess )
	{
		DWORD dwLastError = GetLastError();
		TRACE( _T( "文本框Create错误， GetLastError返回值%u\n" ), dwLastError );
		return FALSE;
	}

    DWORD dwProgressStyle = WS_CHILD | WS_VISIBLE;
#ifdef PBS_SMOOTH    
    if( bSmooth )
       dwProgressStyle |= PBS_SMOOTH;
#endif
    bSuccess = m_wndProgress.Create( dwProgressStyle, TempRect, this, IDC_PROGRESS );
    if( !bSuccess ) 
	{
		DWORD dwLastError = GetLastError();
		TRACE( _T( "进度条Create错误， GetLastError返回值%u\n" ), dwLastError );
		return FALSE;
	}

    bSuccess = m_CancelButton.Create( m_strCancelLabel, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON, TempRect, this, IDC_CANCEL );
    if( !bSuccess )
	{
		DWORD dwLastError = GetLastError();
		TRACE( _T( "按扭Create错误， GetLastError返回值%u\n" ), dwLastError );
		return FALSE;
	}

    m_CancelButton.SetFont( &m_font, TRUE );
    m_Text.SetFont( &m_font, TRUE );
    SetWindowSize( m_nNumTextLines, 220 );

    if( m_bPersistantPosition )
        GetPreviousSettings();
    else
        CenterWindow();

    Show();

    return TRUE;
}

BOOL CProgressWnd::Modal( LPCTSTR pszTitle , BOOL bSmooth )
{
    CWnd *pMainWnd = AfxGetMainWnd();

    if( !::IsWindow( m_hWnd ) && !Create( pMainWnd, pszTitle, bSmooth ) )
	{   
		TRACE( _T( "Modal错误！" ) );
		return FALSE;
	}

    CWnd * wnd = this;
    do {
        CWnd * parent = wnd->GetParent();

        if( !parent || !parent->IsWindowEnabled() )
		{
            m_wRenenableWnd = wnd;
            m_wRenenableWnd->EnableWindow( false );
            break;
        }
        wnd = parent;
    }while( 1 );

    EnableWindow( TRUE );
    m_bModal = TRUE;

    return TRUE;
}
    
void CProgressWnd::SetWindowSize( int nNumTextLines, int nWindowWidth )
{
    int nMargin = 10;
    CSize EdgeSize( ::GetSystemMetrics( SM_CXEDGE ), ::GetSystemMetrics( SM_CYEDGE ) );

    CRect TextRect, CancelRect, ProgressRect;
    CSize CancelSize;

    TextRect.SetRect( nMargin, nMargin, nWindowWidth - 2 * nMargin, 100 + 2 * nMargin );

    CDC* pDC = GetDC();
    if( pDC )
	{
        CFont *pOldFont = pDC->SelectObject( &m_font );
        CString str = _T( "M" );
        for( int i = 0; i < nNumTextLines - 1; ++i )
			str += _T( "\nM" );

        pDC->DrawText( str, TextRect, DT_CALCRECT | DT_NOCLIP | DT_NOPREFIX );
        TextRect.right = TextRect.left + nWindowWidth;
        CancelSize = pDC->GetTextExtent( m_strCancelLabel + _T( "  " ) ) + CSize( EdgeSize.cx * 4, EdgeSize.cy * 3 );
        pDC->SelectObject( pOldFont );
        ReleaseDC( pDC );
    }
    
    CancelRect.SetRect( TextRect.right - CancelSize.cx, TextRect.bottom + nMargin, TextRect.right, TextRect.bottom + nMargin + CancelSize.cy );
    ProgressRect.SetRect( TextRect.left, CancelRect.top + EdgeSize.cy, CancelRect.left - nMargin, CancelRect.bottom - EdgeSize.cy );
    CSize ClientSize( nMargin + TextRect.Width() + nMargin, nMargin + TextRect.Height() + nMargin + CancelRect.Height() + nMargin );

    CRect WndRect, ClientRect;
    GetWindowRect( WndRect ); GetClientRect( ClientRect );
    WndRect.right = WndRect.left + WndRect.Width() - ClientRect.Width() + ClientSize.cx;
    WndRect.bottom = WndRect.top + WndRect.Height() - ClientRect.Height() + ClientSize.cy;
    MoveWindow( WndRect );

    m_wndProgress.MoveWindow( ProgressRect );
    m_CancelButton.MoveWindow( CancelRect );
    m_Text.MoveWindow( TextRect );
}

void CProgressWnd::Clear() 
{ 
    SetText( _T( "" ) );
    SetPos( 0 );
    m_bCancelled = FALSE; 
    m_nPrevPos = 0;

    if( ::IsWindow( GetSafeHwnd() ) )
        UpdateWindow();
}

void CProgressWnd::Hide()  
{ 
    if( !::IsWindow( GetSafeHwnd() ) ) 
        return;

    if( IsWindowVisible() )
    {
        ShowWindow( SW_HIDE );
        ModifyStyle( WS_VISIBLE, 0 );
    }
}

void CProgressWnd::Show()  
{ 
    if( !::IsWindow( GetSafeHwnd() ) )
        return;

    if( !IsWindowVisible() )
    {
        ModifyStyle( 0, WS_VISIBLE );
        ShowWindow( SW_SHOWNA );
        RedrawWindow( NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW );
    }
}

void CProgressWnd::SetRange( int nLower, int nUpper, int nStep )    
{
    if( !::IsWindow( GetSafeHwnd() ) ) 
        return;
//防止越界，下同
#ifdef PBM_SETRANGE32
    ASSERT( -0x7FFFFFFF <= nLower && nLower <= 0x7FFFFFFF );
    ASSERT( -0x7FFFFFFF <= nUpper && nUpper <= 0x7FFFFFFF );
    m_wndProgress.SendMessage( PBM_SETRANGE32, ( WPARAM )nLower, ( LPARAM )nUpper );
#else
    ASSERT(0 <= nLower && nLower <= 65535 );
    ASSERT(0 <= nUpper && nUpper <= 65535 );
    m_wndProgress.SetRange( nLower, nUpper );
#endif
    m_nMaxValue = nUpper;
    m_nMinValue = nLower;
    m_nStep     = nStep;

    m_wndProgress.SetStep( nStep );
}

int CProgressWnd::OffsetPos( int nPos )
{ 
    if( !::IsWindow( GetSafeHwnd() ) ) 
        return m_nPrevPos;

    Show();

    return SetPos( m_nPrevPos + nPos );  
}

int CProgressWnd::StepIt()                
{
    if( !::IsWindow( GetSafeHwnd() ) ) 
        return m_nPrevPos;

    Show();

    return SetPos( m_nPrevPos + m_nStep ); 
}

int CProgressWnd::SetStep( int nStep )
{
    int nOldStep = m_nStep;
    m_nStep = nStep;
    if( !::IsWindow( GetSafeHwnd() ) ) 
        return nOldStep;

    return m_wndProgress.SetStep( nStep ); 
}

int CProgressWnd::SetPos( int nPos )                    
{
#ifdef PBM_SETRANGE32
    ASSERT( -0x7FFFFFFF <= nPos && nPos <= 0x7FFFFFFF );
#else
    ASSERT( 0 <= nPos && nPos <= 65535 );
#endif

    if( !::IsWindow( GetSafeHwnd() ) ) 
        return m_nPrevPos;

    Show();

    CString strTitle;
    int nPercentage;
    m_nPrevPos = nPos;
    if( m_nMaxValue > m_nMinValue )
        nPercentage = ( int )( ( ( nPos - m_nMinValue ) * 100.0f ) / ( m_nMaxValue - m_nMinValue ) + 0.5f );
    else
        nPercentage = 0;

    if( nPercentage != m_nPrevPercent ) 
    {
        m_nPrevPercent = nPercentage;
        strTitle.Format( _T( "%s[%d%%]" ), m_strTitle, nPercentage );
        SetWindowText( strTitle );
    }

    return m_wndProgress.SetPos( nPos );        
}

void CProgressWnd::SetText( LPCTSTR fmt, ... )
{
    if( !::IsWindow( GetSafeHwnd() ) ) 
        return;

    va_list args;
    TCHAR buffer[ MAX_PATH * 2 ];
    va_start( args, fmt );
    _vstprintf( buffer, fmt, args );
    va_end( args );

    m_Text.SetWindowText( buffer );
}

BEGIN_MESSAGE_MAP( CProgressWnd, CWnd )
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED( IDC_CANCEL, OnCancel )
END_MESSAGE_MAP()

BOOL CProgressWnd::OnEraseBkgnd( CDC* pDC ) 
{
    CBrush backBrush( GetSysColor( COLOR_BTNFACE ) );
    CBrush *pOldBrush = pDC->SelectObject( &backBrush );
    CRect rect;
    pDC->GetClipBox( &rect );
    pDC->PatBlt( rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY );
    pDC->SelectObject( pOldBrush );

    return TRUE;
}

void CProgressWnd::OnCancel() 
{
    if( m_bPersistantPosition )
        SaveCurrentSettings();

    m_bCancelled = TRUE;
    Hide();

    if( m_bModal )
        PostMessage( WM_CLOSE );

    CWnd *pWnd = AfxGetMainWnd();
    if( pWnd && ::IsWindow( pWnd->m_hWnd ) )
        pWnd->SetForegroundWindow();
}


BOOL CProgressWnd::DestroyWindow() 
{
    if( m_bPersistantPosition )
        SaveCurrentSettings();

    if( m_bModal )
    {
        m_bModal = FALSE;
        CWnd *pMainWnd = AfxGetMainWnd();

        if( m_wRenenableWnd )
            m_wRenenableWnd->EnableWindow( TRUE );
    }
	
    return CWnd::DestroyWindow();
}

void CProgressWnd::PeekAndPump( BOOL bCancelOnESCkey )
{
    if( m_bModal && ::GetFocus() != m_hWnd )
        SetFocus();

    MSG msg;
    while( !m_bCancelled && ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) ) 
    {
        if( bCancelOnESCkey && ( msg.message == WM_CHAR ) && ( msg.wParam == VK_ESCAPE ) )
            OnCancel();

        if( m_bModal && ( msg.message == WM_LBUTTONUP ) )
        {
            CRect rect;
            m_CancelButton.GetWindowRect( rect );
            if( rect.PtInRect( msg.pt ) )
                OnCancel();
        }
  
        if( !AfxGetApp()->PumpMessage() ) 
        {
            ::PostQuitMessage( 0 );
            return;
        } 
    }
}

void CProgressWnd::GetPreviousSettings()
{
    int x = AfxGetApp()->GetProfileInt( szSection, szEntryX, -1 );
    int y = AfxGetApp()->GetProfileInt( szSection, szEntryY, -1 );

    if( x >= 0 && x < GetSystemMetrics( SM_CXSCREEN ) && y >= 0 && y < GetSystemMetrics( SM_CYSCREEN ) )
        SetWindowPos( NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
    else
        CenterWindow();
}

void CProgressWnd::SaveCurrentSettings()
{   
    if( !IsWindow( m_hWnd ) )
        return;

    CRect rect;
    GetWindowRect( rect );

    AfxGetApp()->WriteProfileInt( szSection, szEntryX, rect.left );
    AfxGetApp()->WriteProfileInt( szSection, szEntryY, rect.top );
}