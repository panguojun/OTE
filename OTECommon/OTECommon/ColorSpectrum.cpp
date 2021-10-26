#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <tchar.h>
#include <crtdbg.h>
#include "ColorSpectrum.h"

#pragma warning( disable : 4127 )
#pragma warning( disable : 4996 )

#define UNUSED( x ) x

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ( ( void )0 )
#endif
#endif

#undef TRACE
#define TRACE __noop
#undef TRACERECT
#define TRACERECT __noop

static const int ARROW_WIDTH = 6;
static const int ARROW_HEIGHT = 11;
static const int HORIZONTAL_MARGIN = 2;
static const int VERTICAL_MARGIN = ARROW_HEIGHT / 2 + 1;
static const int SLIDER_WIDTH = 10;
static const int CROSSHAIR_SIZE = 15;

static const int LUMINOSITY_BAR_WIDTH = 14;
static const int SLIDER_OFFSET = SLIDER_WIDTH + HORIZONTAL_MARGIN;
static const int LUMINOSITY_BAR_OFFSET	= SLIDER_OFFSET + LUMINOSITY_BAR_WIDTH + 3;	
static const int SPECTRUM_OFFSET = LUMINOSITY_BAR_OFFSET + 6;

UINT WM_COLORPICKER_SELCHANGE = ::RegisterWindowMessage( _T( "WM_COLORPICKER_SELCHANGE" ) );
UINT WM_COLORPICKER_SELENDOK  = ::RegisterWindowMessage( _T( "WM_COLORPICKER_SELENDOK" ) );

static double HuetoRGB( double m1, double m2, double h )
{
	if( h < 0 ) h += 1.0f;
	if( h > 1 ) h -= 1.0f;
	if( 6.0f * h < 1.0f )
		return ( m1 + ( m2 - m1 ) * h * 6.0f );
	if( 2.0f * h < 1.0f )
		return m2;
	if( 3.0f * h < 2.0f )
		return ( m1 + ( m2 - m1 ) * ( ( 2.0f / 3.0f ) - h ) * 6.0f );
	return m1;
}

void RGBtoHSL( COLORREF cr, double *H, double *S, double *L )
{
	double delta;
	double r = ( double ) GetRValue( cr ) / 255;
	double g = ( double ) GetGValue( cr ) / 255;
	double b = ( double ) GetBValue( cr ) / 255;
	double cmax = max( r, max( g, b ) );
	double cmin = min( r, min( g, b ) );
	
	*L = ( cmax + cmin ) / 2.0f;
	
	if( cmax == cmin )
	{
		*S = 0;
		*H = 0;
	}
	//if( cmax == cmin && cmax == 1.0f && cmin == 1.0f )
	//{
	//	*S = 0;
	//	*H = 0.667f;
	//}
	//else if( cmax == cmin && cmax == 0.0f && cmin == 0.0f )
	//{
	//	*S = 0;
	//	*H = 0.667f;
	//}
	else
	{
		if( *L < 0.5f )
			*S = ( cmax - cmin ) / ( cmax + cmin );
		else
			*S = (cmax - cmin) / ( 2.0f - cmax - cmin );
		
		delta = cmax - cmin;
		if( r == cmax )
			*H = ( g - b ) / delta;
		else if( g == cmax )
			*H = 2.0f + ( b - r ) / delta;
		else
			*H = 4.0f + ( r - g ) / delta;
		
		*H /= 6.0f;
		
		if( *H < 0.0f )
			*H += 1;
	}

	*H *= 240.0f;
	*S *= 240.0f;
	*L *= 240.0f;
}

void RGBtoHSL( COLORREF cr, BYTE *h, BYTE *s, BYTE *l )
{
	double H, S, L;
	RGBtoHSL( cr, &H, &S, &L );
	*h = ( BYTE )( H + 0.5f );
	*s = ( BYTE )( S + 0.5f );
	*l = ( BYTE )( L + 0.5f );
	if( *h > 239 )
		*h = 239;
	if( *s > 240 )
		*s = 240;
	if( *l > 240 )
		*l = 240;
}

COLORREF HSLtoRGB( double H, double S, double L )
{
	double r, g, b;
	double m1, m2;

	H /= 240.0f;
	S /= 240.0f;
	L /= 240.0f;

	if( S == 0 )
		r = g = b = L;
	else
	{
		if( L <= 0.5f )
			m2 = L * ( 1.0f + S );
		else
			m2 = L + S - L * S;
		
		m1 = 2.0f * L - m2;
		r = HuetoRGB( m1, m2, H + 1.0f / 3.0f );
		g = HuetoRGB( m1, m2, H );
		b = HuetoRGB( m1, m2, H - 1.0f / 3.0f );
	}
	
	return RGB( ( BYTE )( r * 255 ), ( BYTE )( g * 255 ), ( BYTE )( b * 255 ) );
}

ColorDC::ColorDC( HWND hWnd ): m_hDC( 0 ), 
							   m_hWnd( 0 ), 
							   m_bRelease( FALSE ), 
							   m_bDelete( FALSE )
{
	m_hWnd = hWnd;
	_ASSERTE( m_hWnd && IsWindow( m_hWnd ) );
	if( m_hWnd && IsWindow( m_hWnd ) )
	{
		m_hDC = ::GetDC( m_hWnd );
		_ASSERTE( m_hDC );
		m_bRelease = m_hDC != 0;
	}
}

ColorDC::~ColorDC()
{
	if( m_hDC && m_bRelease )
	{
		::ReleaseDC( m_hWnd, m_hDC );
	}

	if( m_hDC && m_bDelete )
	{
		::DeleteDC( m_hDC );
	}

	m_hDC = 0;
	m_hWnd = 0;
}

BOOL ColorDC::Attach( HDC hdc )
{
	BOOL rc = hdc != 0;
	_ASSERTE( hdc );
	_ASSERTE( m_hDC == 0 );
	m_hDC = hdc;
	m_bRelease = FALSE;
	m_bDelete = FALSE;
	return rc;
}

HDC ColorDC::Detach()
{
	HDC rc = m_hDC;
	m_hDC = 0;
	return rc;
}

COLORREF ColorDC::GetPixel( int x, int y )
{
	COLORREF cr = NO_COLOR;
	_ASSERTE( m_hDC );
	if( m_hDC )
		cr = ::GetPixel( m_hDC, x, y );
	return cr;
}

BOOL ColorDC::SetPixelV( int x, int y, COLORREF cr )
{
	BOOL rc = FALSE;
	_ASSERTE( m_hDC );
	if( m_hDC )
		rc = ::SetPixelV( m_hDC, x, y, cr );
	return rc;
}

BOOL ColorDC::BitBlt( int x, int y, int nWidth, int nHeight, ColorDC *pSrcDC, int xSrc, int ySrc, DWORD dwRop )
{
	BOOL rc = FALSE;
	_ASSERTE( m_hDC );
	_ASSERTE( pSrcDC );
	if( m_hDC && pSrcDC && pSrcDC->m_hDC )
		rc = ::BitBlt( m_hDC, x, y, nWidth, nHeight, pSrcDC->m_hDC, xSrc, ySrc, dwRop );
	return rc;
}

void ColorDC::FillSolidRect( int x, int y, int cx, int cy, COLORREF cr )
{
	_ASSERTE( m_hDC );
	if( m_hDC )
	{
		::SetBkColor( m_hDC, cr );
		RECT rect = { x, y, x + cx, y + cy };
		::ExtTextOut( m_hDC, 0, 0, ETO_OPAQUE, &rect, 0, 0, 0 );
	}
}

BOOL ColorDC::CreateCompatibleDC( HDC hdc )
{
	BOOL rc = FALSE;
	_ASSERTE( hdc );
	_ASSERTE( m_hDC == 0 );
	if( hdc )
	{
		m_hDC = ::CreateCompatibleDC( hdc );
		_ASSERTE( m_hDC );
		rc = m_hDC != 0;
		m_bDelete = TRUE;
		m_bRelease = FALSE;
	}

	return rc;
}

BOOL ColorDC::CreateCompatibleDC( ColorDC *pDC )
{
	BOOL rc = FALSE;
	_ASSERTE( pDC && pDC->m_hDC );
	_ASSERTE( m_hDC == 0 );
	if( pDC && pDC->m_hDC )
	{
		m_hDC = ::CreateCompatibleDC( pDC->m_hDC );
		_ASSERTE( m_hDC );
		rc = m_hDC != 0;
		m_bDelete = TRUE;
		m_bRelease = FALSE;
	}

	return rc;
}

HBITMAP ColorDC::CreateCompatibleBitmap( int nWidth, int nHeight )
{
	HBITMAP hbitmap = 0;
	_ASSERTE( m_hDC );
	if( m_hDC )
		hbitmap = ::CreateCompatibleBitmap( m_hDC, nWidth, nHeight );

	return hbitmap;
}

HGDIOBJ ColorDC::SelectObject( HGDIOBJ hgdiobj )
{
	HGDIOBJ old = 0;
	_ASSERTE( m_hDC );
	if( m_hDC )
		old = ::SelectObject( m_hDC, hgdiobj );
	return old;
}

ColorSpectrum::ColorSpectrum():	m_hWnd( NULL ),
								m_OldBitmap( NULL ),
								m_nLuminosity( 0 ),
								m_Hue( 0 ), 
								m_Sat( 0 ), 
								m_Lum( 0 ),
								m_crLastSent( NO_COLOR ),
								m_crCurrent( NO_COLOR ),
								m_crBackground( NO_COLOR ),
								m_bSliderDrag( FALSE ),
								m_bCrosshairDrag( FALSE ),
								m_nDlgCode( DLGC_WANTARROWS | DLGC_WANTTAB ),
								m_bIsSpectrumFocused( FALSE ),
								m_bIsSliderFocused( FALSE ),
								m_pNotifyFuncHdl(NULL)
{
	m_ptCurrent.x = 0;
	m_ptCurrent.y = VERTICAL_MARGIN;
}

ColorSpectrum::~ColorSpectrum()
{
	Destroy();
}

static LRESULT __stdcall ColorSpectrumProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
		case WM_CREATE:
			{
				CREATESTRUCT *pcs = ( CREATESTRUCT* )lParam;
				if( !pcs )
				{
					TRACE( _T("错误！！！CREATESTRUCT结构参数lParam为零！\n" ) );
					_ASSERTE( pcs );
					return -1;
				}
				::SetWindowLongPtr( hWnd, GWLP_USERDATA, ( LONG_PTR )pcs->lpCreateParams );
				return 0;
			}
			break;
		default:
			{
				LONG_PTR lData = ::GetWindowLongPtr( hWnd, GWLP_USERDATA );
				if( lData )
				{
					ColorSpectrum *pCtrl = ( ColorSpectrum* )lData;
					return pCtrl->WindowProc( message, wParam, lParam );
				}
				else
					TRACE( _T( "message值：0x%04X\n" ), message );
			}
			break;
	}

	return ::DefWindowProc( hWnd, message, wParam, lParam );
}

BOOL ColorSpectrum::Create( HINSTANCE hInstance, DWORD dwStyle, const RECT& rect, HWND hParent, UINT nID, COLORREF crInitialColor )
{
	TRACE( _T("调用Create！\n" ) );

	m_hWnd = NULL;
	m_hParent = hParent;
	_ASSERTE( IsWindow( m_hParent ) );
	if( !IsWindow( m_hParent ) )
		return FALSE;

	m_rectCtrl = rect;
	TRACERECT( m_rectCtrl );
	TRACE( _T( "宽 = %d  高 = %d\n" ), m_rectCtrl.Width(), m_rectCtrl.Height() );

	m_rectSlider = m_rectCtrl;
	TRACERECT( m_rectSlider );

	TCHAR * pszClassName = _T( "ColorSpectrum" );
	WNDCLASS wc = { CS_DBLCLKS,	ColorSpectrumProc, 0, 0, hInstance, 0, ::LoadCursor( 0, IDC_ARROW ), 0, 0, pszClassName };

	if( !::RegisterClass( &wc ) )
	{
		DWORD dwLastError = GetLastError();
		if( dwLastError != ERROR_CLASS_ALREADY_EXISTS )
		{
			TRACE( _T( "注册窗口类错误。GetLastError返回值%u\n" ), dwLastError );
			_ASSERTE( FALSE );
			return FALSE;
		}
	}

	m_hWnd = ::CreateWindowEx( 0, pszClassName, _T(""), dwStyle, m_rectCtrl.left, m_rectCtrl.top, m_rectCtrl.Width(), m_rectCtrl.Height(), hParent, ( HMENU )nID, hInstance, this );

	if( m_hWnd == NULL )
	{
#ifdef _DEBUG
		DWORD dwLastError = GetLastError();
		UNUSED( dwLastError );
		TRACE( _T( "CreateWindowEx错误， GetLastError返回值%u\n" ), dwLastError );
		_ASSERTE(m_hWnd);
#endif
		return FALSE;
	}

	::GetClientRect( m_hWnd, &m_rectSpectrumClient );
	m_rectSliderClient = m_rectSpectrumClient;
	m_rectSpectrumClient.right -= SPECTRUM_OFFSET;
	m_rectSpectrumClient.top += VERTICAL_MARGIN;
	m_rectSpectrumClient.bottom -= VERTICAL_MARGIN;

	m_rectSliderClient.left = m_rectSliderClient.right - LUMINOSITY_BAR_OFFSET;
	m_rectSliderClient.right -= HORIZONTAL_MARGIN;
	m_rectSliderClient.top += VERTICAL_MARGIN - ARROW_HEIGHT / 2;
	m_rectSliderClient.bottom -= VERTICAL_MARGIN - ARROW_HEIGHT / 2;

	m_crLastSent = m_crCurrent = crInitialColor;
	RGBtoHSL( m_crLastSent, &m_Hue, &m_Sat, &m_Lum );
	TRACE( _T( "m_crCurrent = %06X  h = %d  s = %d  l = %d\n" ), m_crCurrent, m_Hue, m_Sat, m_Lum );

	m_ptCurrent = GetPointFromHsl();
	m_nLuminosity = GetLuminosity();

	DrawCrosshair( NULL, m_ptCurrent.x, m_ptCurrent.y, FALSE, FALSE );
	DrawArrow( NULL, m_nLuminosity, FALSE, FALSE );

	return m_hWnd != 0;
}

COLORREF ColorSpectrum::GetRGB()
{
	TRACE( _T( "RGB值：%06X\n" ), m_crCurrent );
	return m_crCurrent;
}

ColorSpectrum &ColorSpectrum::SetRGB( COLORREF cr, bool callBack )
{
	TRACE( _T( "调用SetRGB\n" ) );

	m_crLastSent = m_crCurrent = cr;
	RGBtoHSL( m_crCurrent, &m_Hue, &m_Sat, &m_Lum );
	if( m_bIsSliderFocused == false )
		m_ptCurrent = GetPointFromHsl();
	m_nLuminosity = GetLuminosity();

	DrawCrosshair( NULL, m_ptCurrent.x, m_ptCurrent.y, IsFocused() && m_bIsSpectrumFocused, FALSE );
	DrawArrow( NULL, m_nLuminosity, IsFocused() && m_bIsSliderFocused, FALSE );
	
	if(m_pNotifyFuncHdl && callBack)
		(*m_pNotifyFuncHdl)(cr, 0);

	return *this;
}

void ColorSpectrum::GetHSL( BYTE *h, BYTE *s, BYTE *l )
{
	*h = m_Hue;
	*s = m_Sat;
	*l = m_Lum;
}

void ColorSpectrum::SetHslFromPoint( POINT point )
{
	int y = point.y - VERTICAL_MARGIN;
	m_Hue = ( BYTE )( ( point.x * 239 ) / ( m_rectSpectrumClient.Width() - 1 ) );
	m_Sat = ( BYTE )( ( ( m_rectSpectrumClient.Height() - y - 1 ) * 240 ) / ( m_rectSpectrumClient.Height() - 1 ) );
	if( m_Hue > 239 )
		m_Hue = 239;
	if( m_Sat > 240 )
		m_Sat = 240;
}

void ColorSpectrum::Internal_SetHSL( BYTE h, BYTE s, BYTE l )
{
	if( h > 239 )
		h = 239;
	if( s > 240 )
		s = 240;
	if( l > 240 )
		l = 240;

	m_Hue = h;
	m_Sat = s;
	m_Lum = l;
	m_crCurrent = HSLtoRGB( h, s, l );
	m_ptCurrent = GetPointFromHsl();
	m_nLuminosity = GetLuminosity();
}

ColorSpectrum &ColorSpectrum::SetHSL( BYTE h, BYTE s, BYTE l )
{
	TRACE( _T( "HSL值：y = %d   %d，%d，%d\n" ), m_ptCurrent.y, h, s, l );

	Internal_SetHSL( h, s, l );
	m_crLastSent = m_crCurrent;
	DrawCrosshair( NULL, m_ptCurrent.x, m_ptCurrent.y, m_bIsSpectrumFocused, FALSE );
	DrawArrow( NULL, m_nLuminosity, m_bIsSliderFocused, FALSE );

	return *this;
}

BOOL ColorSpectrum::IsLeftButtonDown()
{
	BOOL rc = FALSE;

	SHORT state = 0;
	if( GetSystemMetrics( SM_SWAPBUTTON ) )
		state = ::GetAsyncKeyState( VK_RBUTTON );
	else
		state = ::GetAsyncKeyState( VK_LBUTTON );

	if( state < 0 )
		rc = TRUE;

	return rc;
}

BOOL ColorSpectrum::IsFocused()
{
	BOOL rc = FALSE;

	HWND hFocus = ::GetFocus();
	if( hFocus == m_hWnd )
		rc = TRUE;

	return rc;
}

BOOL ColorSpectrum::GetClientCursorPos( POINT &point )
{
	BOOL rc = FALSE;
	if( ::GetCursorPos( &point ) )
	{
		::ScreenToClient( m_hWnd, &point );
		rc = TRUE;
	}

	return rc;
}

BOOL ColorSpectrum::IsPointInSpectrum( POINT point )
{
	BOOL rc = m_rectSpectrumClient.PtInRect( point );
	return rc;
}

BOOL ColorSpectrum::IsPointInSlider( POINT point )
{
	BOOL rc = m_rectSliderClient.PtInRect( point );
	return rc;
}

void ColorSpectrum::Destroy()
{
	if( m_OldBitmap )
		m_dcSpectrum.SelectObject( m_OldBitmap );
	m_OldBitmap = NULL;

	if( m_bmpSpectrum )
		::DeleteObject( m_bmpSpectrum );
}

void ColorSpectrum::SetLuminosity( int nLuminosity )
{
	if( nLuminosity < 0 )
		nLuminosity = 0;
	if( nLuminosity > ( m_rectSpectrumClient.Height() - 1 ) )
		nLuminosity = m_rectSpectrumClient.Height() - 1;
	m_nLuminosity = nLuminosity;
	m_Lum = ( BYTE )( ( ( m_rectSpectrumClient.Height() - m_nLuminosity - 1 ) * 240 ) / ( m_rectSpectrumClient.Height() - 1 ) );
	if( m_Lum > 240 )
		m_Lum = 240;
	m_crCurrent = HSLtoRGB( m_Hue, m_Sat, m_Lum );
}

int ColorSpectrum::GetLuminosity()
{
	UINT l = m_Lum;
	int lum = m_rectSpectrumClient.Height() - ( ( m_rectSpectrumClient.Height() * l ) / 240 );
	if( lum < 0 )
		lum = 0;
	if( lum > m_rectSpectrumClient.Height() - 1 )
		lum = m_rectSpectrumClient.Height() - 1;

	return lum;
}

POINT ColorSpectrum::GetPointFromHsl()
{
	POINT point = { 0 };

	UINT hue = m_Hue;
	UINT sat = m_Sat;
	point.x = ( hue * m_rectSpectrumClient.Width() ) / 239;
	if( point.x > ( m_rectSpectrumClient.right - 1 ) )
		point.x = m_rectSpectrumClient.right - 1;

	point.y = m_rectSpectrumClient.Height() - ( ( sat * m_rectSpectrumClient.Height() ) / 240 );
	point.y += VERTICAL_MARGIN;
	if( point.y > ( m_rectSpectrumClient.bottom - 1 ) )
		point.y = m_rectSpectrumClient.bottom - 1;

	return point;
}

void ColorSpectrum::DrawSpectrum( ColorDC *pDC )
{
	TRACE( _T( "调用DrawSpectrum\n" ) );

	_ASSERTE( pDC );
	if( !pDC )
		return;

	TRACERECT( m_rectCtrl );
	if( m_OldBitmap == NULL )
	{
		if( !m_dcSpectrum.CreateCompatibleDC( pDC ) )
		{
			TRACE( _T( "CreateCompatibleDC失败！\n" ) );
			_ASSERTE( FALSE );
			return;
		}
		m_bmpSpectrum = pDC->CreateCompatibleBitmap( m_rectCtrl.Width(), m_rectCtrl.Height() );
		m_OldBitmap = ( HBITMAP ) m_dcSpectrum.SelectObject( m_bmpSpectrum );
		if( m_crBackground == NO_COLOR )
			m_dcSpectrum.BitBlt( 0, 0, m_rectCtrl.Width(), m_rectCtrl.Height(), pDC, 0, 0, SRCCOPY );
		else
			m_dcSpectrum.FillSolidRect( 0, 0, m_rectCtrl.Width(), m_rectCtrl.Height(), m_crBackground );

		UINT cx = m_rectCtrl.Width() - SPECTRUM_OFFSET;
		UINT cy = m_rectCtrl.Height() - 2 * VERTICAL_MARGIN;
		double cxd = cx;
		double cyd = cy;
		for( UINT y = 0; y < cy; ++y )
		{
			for( UINT x = 0; x < cx; ++x )
			{
				double hue = x;
				hue *= 240.0f;
				hue /= cxd - 1.0;
				double sat = y;
				sat *= 240.0f;
				sat /= cyd - 1.0;
				COLORREF cr = HSLtoRGB( hue, sat, 120.0f );
				m_dcSpectrum.SetPixelV( x, cy - y - 1 + VERTICAL_MARGIN, cr );
			}
		}
	}

	pDC->BitBlt( 0, 0, m_rectCtrl.Width(), m_rectCtrl.Height(), &m_dcSpectrum, 0, 0, SRCCOPY );			
}

void ColorSpectrum::SendColorToParent( UINT nMessage, COLORREF cr )
{
	TRACE( _T( "SendColorToParent值：cr = %06X  m_crLastSent = %06X\n" ), cr, m_crLastSent );

	//if( ( nMessage == WM_COLORPICKER_SELENDOK ) || ( cr != m_crLastSent ) )
	{
		m_crLastSent = cr;
		if( IsWindow( m_hParent ) && ( cr != NO_COLOR ) )
		{
			TRACE( _T( ">>>>> 发送 %06X\n" ), m_crLastSent );
			::SendMessage( m_hParent, nMessage, m_crLastSent, ::GetDlgCtrlID( m_hWnd ) );
		}
	}
}

//十字丝
void ColorSpectrum::DrawCrosshair( ColorDC *pDC, int startx, int starty, BOOL bHasFocus, BOOL bSendColor )
{
	TRACE( _T( "调用DrawCrosshair\n" ) );
	BOOL bRelease = FALSE;

	if( pDC == NULL )
	{
		pDC = new ColorDC( m_hWnd );
		bRelease = TRUE;
	}

	if( m_OldBitmap )
		pDC->BitBlt( 0, 0, m_rectCtrl.Width() - SPECTRUM_OFFSET, m_rectCtrl.Height(), &m_dcSpectrum, 0, 0, SRCCOPY );

	static BYTE pixels[][ CROSSHAIR_SIZE ] = { 0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
											   0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,
											   1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,
											   0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0 };
	COLORREF crFill = ::GetSysColor( COLOR_WINDOWTEXT );
	if( !bHasFocus )
		crFill = ::GetSysColor( COLOR_WINDOW );

	POINT pt;
	for( int row = 0; row < CROSSHAIR_SIZE; ++row )
	{
		for( int col = 0; col < CROSSHAIR_SIZE; ++col )
		{
			if( pixels[ row ][ col ] != 1 )
				continue;
			pt.x = startx - CROSSHAIR_SIZE / 2 + col;
			pt.y = starty - CROSSHAIR_SIZE / 2 + row;
			if( m_rectSpectrumClient.PtInRect( pt ) )
				pDC->SetPixelV( pt.x, pt.y, crFill );
		}
	}

	DrawLuminosityBar( pDC );
	if( bRelease )
	{
		if( pDC )
		{
			delete pDC;
			pDC = NULL;
		}
	}

	if ( bSendColor )
		SendColorToParent( WM_COLORPICKER_SELCHANGE, m_crCurrent );
}

void ColorSpectrum::DrawLuminosityBar( ColorDC *pDC )
{
	TRACE( _T( "调用DrawLuminosityBar\n" ) );

	BOOL bRelease = FALSE;
	if( pDC == NULL )
	{
		pDC = new ColorDC( m_hWnd );
		_ASSERTE( pDC );
		bRelease = TRUE;
	}

	ColorDC memDC;
	if( !memDC.CreateCompatibleDC( pDC ) )
	{
		TRACE( _T( "CreateCompatibleDC失败！\n" ) );
		_ASSERTE( FALSE );
		return;
	}
	HBITMAP bmp = pDC->CreateCompatibleBitmap( m_rectSlider.Width(), m_rectSlider.Height() );
	HBITMAP hOldBitmap = ( HBITMAP )memDC.SelectObject( bmp );
	memDC.BitBlt( 0, 0, m_rectSlider.Width(), m_rectSlider.Height(), pDC, 0, 0, SRCCOPY );

	COLORREF cr = m_crCurrent;
	if( GetRValue( m_crCurrent ) >= 254 && GetGValue( m_crCurrent ) >= 254 && GetBValue( m_crCurrent ) >= 254 || 
		GetRValue( m_crCurrent ) <= 1 && GetGValue( m_crCurrent ) <= 1 && GetBValue( m_crCurrent ) <= 1 )
			cr = pDC->GetPixel( m_ptCurrent.x, m_ptCurrent.y );

	if( cr == NO_COLOR )
		return;

	DOUBLE h, s, l;
	RGBtoHSL( cr, &h, &s, &l );
	//if( h > 239.0f )
	//	h = 239.0f;
	if( h > 240.0f )
		h = 240.0f;
	if( s > 240.0f )
		s = 240.0f;

	UINT cx = m_rectSlider.Width();
	UINT cy = m_rectSlider.Height() - 2 * VERTICAL_MARGIN;
	double cyd = cy;
	for( UINT y = 0; y < cy; ++y )
	{
		l = y;
		l /= cyd - 1.0;
		l *= 240.0f;
		COLORREF rgb = HSLtoRGB( h, s, l );
		for( UINT x = cx - LUMINOSITY_BAR_OFFSET + 3; x < cx - LUMINOSITY_BAR_OFFSET + LUMINOSITY_BAR_WIDTH + 3; ++x )
			memDC.SetPixelV( x, cy - y - 1 + VERTICAL_MARGIN, rgb );
	}

	pDC->BitBlt( 0, 0, m_rectSlider.Width(), m_rectSlider.Height(), &memDC, 0, 0, SRCCOPY );			

	if( hOldBitmap )
		memDC.SelectObject( hOldBitmap );
	if( bmp )
		::DeleteObject( bmp );

	if( bRelease )
	{
		if( pDC )
		{
			delete pDC;
			pDC = NULL;
		}
	}
}

//箭头
void ColorSpectrum::DrawArrow( ColorDC *pDC, int starty, BOOL bHasFocus, BOOL bSendColor )
{
	BOOL bRelease = FALSE;

	if( pDC == NULL )
	{
		pDC = new ColorDC( m_hWnd );
		bRelease = TRUE;
	}

	if( m_OldBitmap )
		pDC->BitBlt( m_rectCtrl.Width() - SLIDER_OFFSET, 0, SLIDER_OFFSET, m_rectCtrl.Height(), &m_dcSpectrum, m_rectCtrl.Width() - SLIDER_OFFSET, 0, SRCCOPY );

	static BYTE pixels[][ ARROW_WIDTH ] = { 0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,1,2,1,0,0,1,2,2,1,0,1,2,2,2,1,1,2,2,2,2,1,0,1,2,2,2,1,0,0,1,2,2,1,0,0,0,1,2,1,0,0,0,0,1,1,0,0,0,0,0,1 };
	COLORREF crWindowText = ::GetSysColor( COLOR_WINDOWTEXT );
	COLORREF crBtnShadow  = ::GetSysColor( COLOR_BTNSHADOW );
	COLORREF crFill = crWindowText;
	if( !bHasFocus )
		crFill = crBtnShadow;

	int startx = m_rectCtrl.Width() - SLIDER_OFFSET;
	if( starty < 0 )
		starty = 0;
	if( starty > m_rectSliderClient.Height() - 2 * ( ARROW_HEIGHT / 2 ) - 1 )
		starty = m_rectSliderClient.Height() - 2 * ( ARROW_HEIGHT / 2 ) - 1;

	for( int row = 0; row < ARROW_HEIGHT; ++row )
	{
		for( int col = 0; col < ARROW_WIDTH; ++col )
		{
			COLORREF crFill = crWindowText;
			if( pixels[ row ][ col ] == 1 )
				crFill = crWindowText;
			else if( pixels[ row ][ col ] == 2 )
				if( bHasFocus )
					crFill = crWindowText;
				else
					crFill = crBtnShadow;
			else
				continue;
			pDC->SetPixelV( startx + col, starty + row - ARROW_HEIGHT / 2 + VERTICAL_MARGIN, crFill );
		}
	}

	if( bRelease )
	{
		if( pDC )
		{
			delete pDC;
			pDC = NULL;
		}
	}

	if( bSendColor )
		SendColorToParent( WM_COLORPICKER_SELCHANGE, m_crCurrent );
}

LRESULT ColorSpectrum::WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) 
{
	switch( message )
	{
		case WM_CREATE:
			break;
		case WM_ERASEBKGND:
			return TRUE;
		case WM_PAINT:
		{
			TRACE( _T( "触发WM_PAINT\n" ) );
			PAINTSTRUCT ps; 
			HDC hdc = ( wParam != NULL ) ? ( HDC )wParam : ::BeginPaint( m_hWnd, &ps );
			if( hdc == NULL )
				return 0;
			ColorDC dc( hdc );
			DrawSpectrum( &dc );
			DrawCrosshair( &dc, m_ptCurrent.x, m_ptCurrent.y, IsFocused() && m_bIsSpectrumFocused, FALSE );
			DrawArrow( &dc, m_nLuminosity, IsFocused() && m_bIsSliderFocused, FALSE );
			if( wParam == NULL )
				::EndPaint( m_hWnd, &ps );

			return 0;
		}
		case WM_GETDLGCODE:
			return m_nDlgCode;
		case WM_LBUTTONDOWN:
		{
			POINT point;
			point.x = GET_X_LPARAM( lParam );
			point.y = GET_Y_LPARAM( lParam );
			TRACE( _T( "WM_LBUTTONDOWN中值：point.x = %d  point.y = %d\n" ), point.x, point.y );

			if( IsPointInSpectrum( point ) )
			{
				SetHslFromPoint( point );
				//TRACE( _T( "m_Hue = %d  m_Sat = %d  m_Lum = %d\n" ), m_Hue, m_Sat, m_Lum );
				m_crCurrent = HSLtoRGB( m_Hue, m_Sat, m_Lum );
				//TRACE( _T( "m_crCurrent = %06X\n" ), m_crCurrent );
				m_ptCurrent = point;
			}
			else if( IsPointInSlider( point ) )
			{
				SetLuminosity( point.y - VERTICAL_MARGIN );
				m_crCurrent = HSLtoRGB( m_Hue, m_Sat, m_Lum );
			}

			HWND hFocus = ::GetFocus();
			if( hFocus != m_hWnd )
				::SetFocus( m_hWnd );

			if( IsPointInSpectrum( point ) )
			{
				m_bIsSpectrumFocused = TRUE;
				m_bIsSliderFocused = FALSE;
				DrawCrosshair( NULL, m_ptCurrent.x, m_ptCurrent.y, TRUE, TRUE );
				DrawArrow( NULL, m_nLuminosity, FALSE, FALSE );
				m_bCrosshairDrag = TRUE;
			}
			else if( IsPointInSlider( point ) )
			{
				m_bIsSpectrumFocused = FALSE;
				m_bIsSliderFocused = TRUE;
				DrawCrosshair( NULL, m_ptCurrent.x, m_ptCurrent.y, FALSE, FALSE );
				DrawArrow( NULL, m_nLuminosity, TRUE, TRUE );
				m_bSliderDrag = TRUE;
			}
			
			if(m_pNotifyFuncHdl)
				(*m_pNotifyFuncHdl)(this->GetRGB(), 0);

			TRACE( _T( "WM_LBUTTONDOWN中值：m_crCurrent = %06X\n" ), m_crCurrent );
			break;
		}
		case WM_LBUTTONUP:
			TRACE( _T( "调用WM_LBUTTONUP\n" ) );
			m_bSliderDrag = FALSE;
			m_bCrosshairDrag = FALSE;
			break;
		case WM_LBUTTONDBLCLK:
		{
			TRACE( _T( "调用OnLButtonDblClk\n" ) );

			POINT point;
			point.x = GET_X_LPARAM( lParam );
			point.y = GET_Y_LPARAM( lParam );
			if( IsPointInSlider( point ) || IsPointInSpectrum( point ) )
				SendColorToParent( WM_COLORPICKER_SELENDOK, m_crCurrent );
			break;
		}
		case WM_SETFOCUS:
		{
			TRACE( _T( "触发WM_SETFOCUS\n" ) );
			LRESULT lResult = ::DefWindowProc( m_hWnd, message, wParam, lParam );

			if( IsShiftDown() )
				m_bIsSliderFocused = TRUE;
			else
				m_bIsSpectrumFocused = TRUE;

			DrawCrosshair( NULL, m_ptCurrent.x, m_ptCurrent.y, IsFocused() && m_bIsSpectrumFocused, FALSE );
			DrawArrow( NULL, m_nLuminosity, IsFocused() && m_bIsSliderFocused, FALSE );
			return lResult;
		}
		case WM_KILLFOCUS:
		{
			TRACE( _T( "触发WM_KILLFOCUS\n" ) );
			LRESULT lResult = ::DefWindowProc( m_hWnd, message, wParam, lParam );
			DrawCrosshair( NULL, m_ptCurrent.x, m_ptCurrent.y, FALSE, FALSE );
			DrawArrow( NULL, m_nLuminosity, FALSE, FALSE );
			m_bIsSpectrumFocused = FALSE;
			m_bIsSliderFocused = FALSE;
			return lResult;
		}
		case WM_MOUSEMOVE:
		{
			POINT point;
			point.x = GET_X_LPARAM( lParam );
			point.y = GET_Y_LPARAM( lParam );

			if( !IsLeftButtonDown() )
			{
				m_bSliderDrag = FALSE;
				m_bCrosshairDrag = FALSE;
			}

			if( m_bSliderDrag )
			{
				if( IsPointInSlider( point ) )
				{
					SetLuminosity( point.y - VERTICAL_MARGIN );
					DrawArrow( NULL, m_nLuminosity, TRUE, TRUE );
				}
			}
			else if( m_bCrosshairDrag )
			{
				if( IsPointInSpectrum( point ) )
				{
					m_ptCurrent = point;
					SetHslFromPoint( m_ptCurrent );
					m_crCurrent = HSLtoRGB( m_Hue, m_Sat, m_Lum );
					DrawCrosshair( NULL, point.x, point.y, TRUE, TRUE );
				}
			}
			
			if(m_pNotifyFuncHdl && ::GetKeyState(VK_LBUTTON) & 0x80)
				(*m_pNotifyFuncHdl)(this->GetRGB(), 0);

			break;
		}
	}

	return ::DefWindowProc( m_hWnd, message, wParam, lParam );
}
