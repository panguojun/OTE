#ifndef _COLOR_SPECTRUM_H
#define _COLOR_SPECTRUM_H

#include <crtdbg.h>

#pragma warning( disable : 4127 )

#ifndef NO_COLOR
#define NO_COLOR ( ( COLORREF ) - 1 )
#endif

extern UINT WM_COLORPICKER_SELCHANGE;
extern UINT WM_COLORPICKER_SELENDOK;

void RGBtoHSL( COLORREF cr, double *H, double *S, double *L );
void RGBtoHSL( COLORREF cr, BYTE *h, BYTE *s, BYTE *l );
COLORREF HSLtoRGB( double H, double S, double L );

class ColorDC
{
public:
	ColorDC(): m_hDC( 0 ), m_hWnd( 0 ), m_bRelease(FALSE), m_bDelete( FALSE ) {}
	ColorDC( HDC hdc ): m_hDC( hdc ), m_hWnd( 0 ), m_bRelease( FALSE ), m_bDelete( FALSE ) {}
	ColorDC( HWND hWnd );
	virtual ~ColorDC();

	BOOL Attach( HDC hdc );
	HDC Detach();
	COLORREF GetPixel( int x, int y );
	BOOL SetPixelV( int x, int y, COLORREF cr );

	BOOL BitBlt( int x, int y, int nWidth, int nHeight, ColorDC *pSrcDC, int xSrc, int ySrc, DWORD dwRop );
	void FillSolidRect( int x, int y, int cx, int cy, COLORREF cr );
	BOOL CreateCompatibleDC( HDC hdc );
	BOOL CreateCompatibleDC( ColorDC *pDC );
	HBITMAP CreateCompatibleBitmap( int nWidth, int nHeight );
	HGDIOBJ SelectObject( HGDIOBJ hgdiobj );
	operator HDC() const { return this == 0 ? 0 : m_hDC; }

	HDC m_hDC;

protected:
	HWND m_hWnd;
	BOOL m_bRelease;
	BOOL m_bDelete;
};

class ColorRect: public tagRECT
{
public:
	ColorRect( int l = 0, int t = 0, int r = 0, int b = 0 ) { left = l, top = t, right = r, bottom = b; }

	ColorRect( const RECT& srcRect ) { ::CopyRect( this, &srcRect ); }
	ColorRect( LPCRECT lpSrcRect ) { ::CopyRect( this, lpSrcRect ); }

	BOOL IsRectEmpty() const { return ::IsRectEmpty( this ); }
	BOOL IsRectNull() const { return ( left == 0 && right == 0 && top == 0 && bottom == 0 ); }
	int Width() const { return right - left; }
	int Height() const { return bottom - top; }
	BOOL PtInRect( POINT point ) const { return ::PtInRect( this, point ); }

	void CopyRect( LPCRECT lpSrcRect ) { ::CopyRect( this, lpSrcRect ); }
	void InflateRect( int x, int y ) { ::InflateRect( this, x, y ); }
	void InflateRect( SIZE size ) { ::InflateRect( this, size.cx, size.cy ); }
	void DeflateRect( int x, int y ) { ::InflateRect( this, -x, -y ); }
	void DeflateRect( SIZE size ) { ::InflateRect( this, -size.cx, -size.cy ); }
	void OffsetRect( int x, int y ) { ::OffsetRect( this, x, y ); }
	void OffsetRect( POINT point ) { ::OffsetRect( this, point.x, point.y ); }
	void OffsetRect( SIZE size ) { ::OffsetRect( this, size.cx, size.cy ); }
	void SetRect( int x1, int y1, int x2, int y2 ) { ::SetRect( this, x1, y1, x2, y2 ); }
	void SetRectEmpty() { ::SetRectEmpty( this ); }
	void SwapLeftRight() { SwapLeftRight( LPRECT( this ) ); }
	static void SwapLeftRight( LPRECT lpRect ) { LONG temp = lpRect->left; lpRect->left = lpRect->right, lpRect->right = temp; }

	operator LPRECT() { return this; }
	operator LPCRECT() const { return this; }
	void operator=( const RECT& srcRect ) { ::CopyRect( this, &srcRect ); }
	BOOL operator==( const RECT& rect ) const { return ::EqualRect( this, &rect ); }
};

//¥¶¿Ì
typedef void (*NotifyFuncHdl_t)(DWORD, DWORD);
class ColorSpectrum
{
public:
	ColorSpectrum();
	virtual ~ColorSpectrum();

	virtual BOOL Create( HINSTANCE hInstance, DWORD dwStyle, const RECT& rect, HWND hParent, UINT nID, COLORREF crInitialColor = RGB( 0, 0, 0 ) );
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	
	ColorSpectrum &SetBackground( COLORREF cr ) { m_crBackground = cr; return *this; }
	ColorSpectrum &SetHSL( BYTE h, BYTE s, BYTE l );
	ColorSpectrum &SetRGB( COLORREF cr , bool callBack = true);
	COLORREF GetBackground() { return m_crBackground; }
	void GetHSL( BYTE* h, BYTE* s, BYTE* l );
	COLORREF GetRGB();

	HWND m_hWnd;
	BOOL m_bIsSpectrumFocused;
	BOOL m_bIsSliderFocused;

protected:
	void Destroy();

	void DrawArrow( ColorDC *pDC, int starty, BOOL bHasFocus, BOOL bSendColor );
	void DrawCrosshair( ColorDC *pDC, int startx, int starty, BOOL bHasFocus, BOOL bSendColor );
	void DrawLuminosityBar( ColorDC *pDC);
	void DrawSpectrum( ColorDC *pDC );

	BOOL GetClientCursorPos( POINT& point );
	int GetLuminosity();
	POINT GetPointFromHsl();
	void Internal_SetHSL( BYTE h, BYTE s, BYTE l );

	BOOL IsFocused();
	BOOL IsLeftButtonDown();
	BOOL IsPointInSlider( POINT point );
	BOOL IsPointInSpectrum( POINT point );
	BOOL IsShiftDown() { return GetAsyncKeyState( VK_SHIFT ) & 0x8000; }

	void SendColorToParent( UINT nMessage, COLORREF cr );
	void SetHslFromPoint( POINT point );
	void SetLuminosity( int nLuminosity );

	HWND m_hParent;			
	COLORREF m_crBackground;		
	ColorDC m_dcSpectrum;		
					
	HBITMAP	m_bmpSpectrum;
	HBITMAP	m_OldBitmap;
	ColorRect m_rectCtrl;
	ColorRect m_rectSlider;
	ColorRect m_rectSpectrumClient;
	ColorRect m_rectSliderClient;
	POINT m_ptCurrent;
	int	m_nLuminosity;
	COLORREF m_crLastSent;	
	COLORREF m_crCurrent;
	BYTE m_Hue, m_Sat, m_Lum;
	BOOL m_bSliderDrag;
	BOOL m_bCrosshairDrag;	
	UINT m_nDlgCode;

public:

	NotifyFuncHdl_t		m_pNotifyFuncHdl;

};

#endif