#include "stdafx.h"
#include <GdiPlus.h>
using namespace Gdiplus;
#pragma comment(lib,"gdiplus.lib")

#include "ColorPicker.h"
#include <d3dx9.h>

CColorBar *g_pColorBar = NULL;
CSelectSign *g_pSelectSign = NULL;

IMPLEMENT_DYNAMIC(CColorPicker, CWnd)
CColorPicker::CColorPicker(): m_Color( HexClr( 0xffffffff ) ),
							  m_HotRow( 255 ),
							  m_HotCol( 255 ),
							  m_bCustomColor( true ),
							  m_bMouseDown( false ),
							  m_bMouseMove( false ),
							  m_bHot( false ),
							  m_pNotifyFuncHdl(NULL)
{
}

CColorPicker::~CColorPicker()
{
}

void CColorPicker::InitGDI()
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_diplusToken, &gdiplusStartupInput, NULL);
}

void CColorPicker::ShutdownGDI()
{
	GdiplusShutdown(m_diplusToken);
}

void CColorPicker::SetCurrentColor( DWORD color )
{
	m_Color = color;
}

BOOL CColorPicker::DestroyWindow()
{
	ShutdownGDI();

	return CWnd::DestroyWindow();
}

LRESULT CColorPicker::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CWnd::WindowProc(message, wParam, lParam);
}

void CColorPicker::DrawRectangleColor()
{
    CRect rect;
    GetClientRect(&rect);

    int iHeight = rect.Height();
    int iWidth = rect.Width();
    double dwDiagonal = sqrt((double)(iWidth * iWidth + iHeight * iHeight));

    Rect rectDraw( 0, 0, (int)dwDiagonal, (int)dwDiagonal );
    Rect rectDraw1( 0, 0, (int)dwDiagonal, ((int)dwDiagonal)/3);
    Rect rectDraw2( 0, ((int)dwDiagonal) / 3, (int)dwDiagonal, ((int)dwDiagonal) / 3);
	Rect rectDraw3( 0, ((int)dwDiagonal) / 3 * 2, (int)dwDiagonal, ((int)dwDiagonal) / 2 );

	CDC *pDC = GetWindowDC();
	if( pDC == NULL )
		return;
	Graphics graphics(pDC->GetSafeHdc()); 
	Bitmap bmp(rectDraw.Width, rectDraw.Height);
	Graphics grTmp(&bmp);

    LinearGradientBrush linGrBrush(rectDraw1, Color(255, 0, 0), Color(0, 255, 0), 90);
    grTmp.FillRectangle(&linGrBrush, rectDraw1);
    LinearGradientBrush linGrBrush1(rectDraw2, Color(0, 255, 0),Color(0, 0, 255), 90);
    grTmp.FillRectangle(&linGrBrush1, rectDraw2);
	LinearGradientBrush linGrBrush2(rectDraw3, Color(0, 0, 255),Color(255, 0, 0), 90);
    grTmp.FillRectangle(&linGrBrush2, rectDraw3);

    dwDiagonal *= 0.5;
    double dwAngle = 90 * D3DX_PI / 180.0f;
    double dwCosAngle = cos(dwAngle);
    double dwSinAngle = sin(dwAngle);
    double dwBeta = atan2((double)iHeight, (double)iWidth);
    double dwDistance = dwDiagonal * sin(fabs(dwAngle) + dwBeta);
    double xc = 0.5 * iWidth - dwDistance * dwSinAngle;
    double yc = 0.5 * iHeight - dwDistance * dwCosAngle;
    double xc1 = 0.5 * iWidth + dwDistance * dwSinAngle;
    double yc1 = 0.5 * iHeight + dwDistance * dwCosAngle;
    double dx = dwDiagonal * dwCosAngle;
    double dy = - dwDiagonal * dwSinAngle;

    Point ptDestinationPoints[3];
    ptDestinationPoints[0].X = (INT)(xc - dx);
    ptDestinationPoints[0].Y = (INT)(yc - dy);
    ptDestinationPoints[1].X = (INT)(xc + dx);
    ptDestinationPoints[1].Y = (INT)(yc + dy);
    ptDestinationPoints[2].X = (INT)(xc1 - dx) + 1;
    ptDestinationPoints[2].Y = (INT)(yc1 - dy);
    graphics.DrawImage(&bmp, ptDestinationPoints, 3); 
}

void CColorPicker::GradientColor( RECT rc, DWORD color1, DWORD color2 )
{
	HGDIOBJ hpen, holdpen, hbold;
    HBRUSH hb;

	//InflateRect(&rc, -5, -5);

	CDC *hdc = GetWindowDC();
	if( hdc == NULL )
		return;

	int iHeight = ( rc.right - rc.left ) / 32;
	int r = 0, g = 0, b = 0;
	int r1 = GetRValue( color1 );
	int g1 = GetGValue( color1 );
	int b1 = GetBValue( color1 );

	int r2 = GetRValue( color2 );
	int g2 = GetGValue( color2 );
	int b2 = GetBValue( color2 );

	for( int i = 0; i < 32; ++i ) 
	{ 
		r = r1 + ( r2 - r1 ) * i / 32; 
		g = g1 + ( g2 - g1 ) * i / 32; 
		b = b1 + ( b2 - b1 ) * i / 32;

		hb = CreateSolidBrush( RGB( r, g, b ) );
		hpen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_ACTIVEBORDER));
		hbold = SelectObject(hdc->m_hDC, hb);
		holdpen = SelectObject(hdc->m_hDC, hpen);
		Rectangle(hdc->m_hDC, rc.left + i * iHeight, rc.top, rc.left + ( i + 1 ) * iHeight, rc.bottom );
		SelectObject(hdc->m_hDC, holdpen);
		SelectObject(hdc->m_hDC, hbold);
	}

	DeleteObject(hb);
	DeleteObject(hpen); 
}

void CColorPicker::Draw(HDC hdc, byte row, byte col, bool hot, byte correction)
{
    RECT rc;
    HGDIOBJ hpen, holdpen, hbold;
    HBRUSH hb;
	m_bHot = hot;
    if (row<5 && col<8)
    {
        rc.left = col * 18 + 5 - correction;
        rc.top  = row * 18 + 5 - correction;
		//方格大小
        rc.right = rc.left + 18;
        rc.bottom = rc.top + 18;

        //热区
        if (m_bHot || g_dwColorMap[row][col] == m_Color)
        {
            hb = CreateSolidBrush(GetSysColor(m_bMouseDown ? COLOR_HIGHLIGHT : COLOR_INACTIVECAPTIONTEXT));
            hpen = CreatePen(PS_SOLID, 1, GetSysColor(m_bMouseDown ? COLOR_WINDOWFRAME : COLOR_INACTIVECAPTION));
            hbold = SelectObject(hdc, hb);
            holdpen = SelectObject(hdc, hpen);
            Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
            SelectObject(hdc, holdpen);
            SelectObject(hdc, hbold);
            DeleteObject(hb);
            DeleteObject(hpen);
			m_bHot = false;
        }
        else
            FillRect(hdc, &rc, NULL);
        //颜色
        InflateRect(&rc, -3, -3);
        hb = CreateSolidBrush(g_dwColorMap[row][col]);
        hpen = CreatePen(PS_SOLID, 1, GetSysColor(COLOR_ACTIVEBORDER));
        hbold = SelectObject(hdc, hb);
        holdpen = SelectObject(hdc, hpen);
        Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
        SelectObject(hdc, holdpen);
        SelectObject(hdc, hbold);
        DeleteObject(hb);
        DeleteObject(hpen);
    }
    else if (row==5)
    {
        //文字
        GetClientRect(&rc);
        if (correction)
			OffsetRect(&rc, -correction, -correction); //校正位置

        InflateRect(&rc, -5, -5);
        rc.top = 5*18+10;
        if (hot)
        {
            hb = CreateSolidBrush(GetSysColor(m_bMouseDown ? COLOR_HIGHLIGHT : COLOR_INACTIVECAPTIONTEXT)); 
            hpen = CreatePen(PS_SOLID, 1, GetSysColor(m_bMouseDown ? COLOR_WINDOWFRAME : COLOR_INACTIVECAPTION)); 
            hbold = SelectObject(hdc, hb);
            holdpen = SelectObject(hdc, hpen);
            Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
            SelectObject(hdc, holdpen);
            SelectObject(hdc, hbold);
            DeleteObject(hb);
            DeleteObject(hpen);
        }
        else
            FillRect(hdc, &rc, NULL);

        if (m_bCustomColor) rc.right = 7*18;
        SetBkMode(hdc, TRANSPARENT);
        HFONT m_Font = CreateFont(-12, 0, 0, 0, 400, 0, 0, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");
        HGDIOBJ m_OldFont = SelectObject(hdc, m_Font);
        DrawText(hdc, "其他颜色...", -1, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
        SelectObject(hdc, m_OldFont);
        DeleteObject(m_Font);
        if (m_bCustomColor)
        {
            rc.right = 8*18+3;
            rc.left = 7*18;
            rc.top += 2;
            rc.bottom = rc.top + 15;

            hb = CreateSolidBrush(m_Color);
            hbold = SelectObject(hdc, hb);
            Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);
            //SelectObject(hdc, holdpen);
            DeleteObject(hb);
        }
    }
}

void CColorPicker::ReDraw(byte row, byte col, bool hot)
{
	CDC *hdc = GetWindowDC();

    Draw(hdc->m_hDC, row, col, hot, 0);
    ReleaseDC(hdc);
}

BEGIN_MESSAGE_MAP(CColorPicker, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CColorPicker::OnPaint()
{
    PAINTSTRUCT ps;
    RECT rc;
    HBRUSH hb;

	CDC *hdc = BeginPaint( &ps);
	if( hdc == NULL )
		return;

    GetClientRect(&rc);
    hb = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
    FillRect(hdc->m_hDC, &rc, hb);
    DeleteObject(hb);

	DrawRectangleColor();

    EndPaint(&ps);

}

void CColorPicker::DrawColor( CDC *hdc )
{
	for (byte i=0; i<5; ++i)
	{   for (byte j=0; j<8; ++j)
        {
            Draw(hdc->m_hDC, i, j, false, 1);
            if (m_Color == g_dwColorMap[i][j]) m_bCustomColor = false;
        }
	}

    Draw(hdc->m_hDC, 5, 0, m_HotRow==5, 1);
}

void CColorPicker::OnMouseMove( UINT nFlags, CPoint point )
{
	if( m_bMouseMove )
	{
		m_bMouseDown = true;
		OnPaint();
		CDC *hdc = GetWindowDC();
		if( hdc == NULL )
			return;

		m_Color = GetPixel( hdc->m_hDC, point.x, point.y );
		if(g_pColorBar != NULL )
		{
			g_pColorBar->SetBarColor( m_Color );
			g_pColorBar->OnPaint();
		}

		if(m_pNotifyFuncHdl)
			(*m_pNotifyFuncHdl)(m_Color, 0);
	}
}

void CColorPicker::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bMouseDown = true;
	m_bMouseMove = true;

	OnPaint();
	CDC *hdc = GetWindowDC();
	if( hdc == NULL )
		return;

	m_Color = GetPixel( hdc->m_hDC, point.x, point.y );
	if(g_pColorBar != NULL )
	{
		g_pColorBar->SetBarColor( m_Color );
		g_pColorBar->OnPaint();
	}

	if(m_pNotifyFuncHdl)
		(*m_pNotifyFuncHdl)(m_Color, 0);
}

void CColorPicker::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bMouseMove = false;
	CDC *hdc = GetWindowDC();
	if( hdc == NULL )
		return;

    HGDIOBJ hpen, holdpen, hbold;
    HBRUSH hb;

	hb = CreateSolidBrush( HexClr(0x000000) );
	hpen = CreatePen( PS_SOLID, 1, HexClr( 0x000000 ));
	hbold = SelectObject(hdc->m_hDC, hb);
	holdpen = SelectObject(hdc->m_hDC, hpen);
	Ellipse( hdc->m_hDC, point.x - 3, point.y - 3, point.x + 3, point.y + 3 );
	SelectObject(hdc->m_hDC, holdpen);
	SelectObject(hdc->m_hDC, hbold);
	DeleteObject(hb);
	DeleteObject(hpen);
}

IMPLEMENT_DYNAMIC(CColorBar, CWnd)
CColorBar::CColorBar(): m_Color( HexClr( 0xffffffff ) ),
						m_OuputColor( HexClr( 0xffffffff ) ),
						m_bMouseDown( false ),
						m_bMouseMove( false )
{
	g_pColorBar = this;
}

CColorBar::~CColorBar()
{
}

void CColorBar::SetBarColor( DWORD color )
{
	m_Color = color;
}

LRESULT CColorBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CWnd::WindowProc(message, wParam, lParam);
}

void CColorBar::GradientColor( RECT rc, DWORD color1, DWORD color2 )
{
	CDC *pDC = GetWindowDC();
	if( pDC == NULL )
		return;

    int r1 = GetRValue(color1);
    int g1 = GetGValue(color1);
    int b1 = GetBValue(color1);

    int r2 = GetRValue(color2);
    int g2 = GetGValue(color2);
    int b2 = GetBValue(color2);

	int iWidth = rc.right - rc.left;
	int iHeight = rc.bottom - rc.top;

    Graphics graphics(pDC->GetSafeHdc()); 
	LinearGradientBrush linGrBrush( Rect( rc.left, rc.top, iWidth, iHeight ), Color( 255, r1, g1, b1 ), Color( 255, r2, g2, b2 ), 0 );
    graphics.FillRectangle(&linGrBrush, Rect( rc.left, rc.top, iWidth, iHeight ) );
}

BEGIN_MESSAGE_MAP(CColorBar, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CColorBar::OnPaint()
{
	PAINTSTRUCT ps;
    RECT rc;

	CDC *hdc = BeginPaint( &ps);
	if( hdc == NULL )
		return;

	GetClientRect(&rc);
	int right = rc.right;
	rc.right = right / 2;
	GradientColor( rc, HexClr( 0xffffff ), m_Color );
	rc.left = right / 2, rc.right = right;
	GradientColor( rc, m_Color, HexClr( 0x000000 ) );

	EndPaint(&ps);
}

void CColorBar::OnMouseMove( UINT nFlags, CPoint point )
{
	if( m_bMouseMove )
	{
		OnPaint();
		CDC *hdc = GetWindowDC();
		if( hdc == NULL )
			return;

		m_bMouseDown = true;
		m_OuputColor = GetPixel( hdc->m_hDC, point.x, point.y );
		//g_pSelectSign->OnMouseMove( nFlags, point );
	}
}

CPoint g_Point = CPoint( 88, 0 );
void CColorBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bMouseMove = true;
	OnPaint();
	CDC *hdc = GetWindowDC();
	if( hdc == NULL )
		return;

	m_bMouseDown = true;
	m_OuputColor = GetPixel( hdc->m_hDC, point.x, point.y );
	//g_pSelectSign->OnLButtonDown( nFlags, point );
}


void CColorBar::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bMouseMove = false;
	g_Point = point;
	//g_pSelectSign->OnLButtonUp( nFlags, point );

	CDC *hdc = GetWindowDC();
	if( hdc == NULL )
		return;

	HGDIOBJ hpen, holdpen, hbold;
	HBRUSH hb;

	hb = CreateSolidBrush( HexClr(0xffffff) );
	hpen = CreatePen( PS_SOLID, 1, HexClr( 0xffffff ));
	hbold = SelectObject(hdc->m_hDC, hb);
	holdpen = SelectObject(hdc->m_hDC, hpen);
	hdc->MoveTo( point.x - 2,  point.y - 2 );
	LineTo( hdc->m_hDC, point.x - 2, point.y + 2 );
	hdc->MoveTo( point.x - 2, point.y + 2 );
	LineTo( hdc->m_hDC, point.x + 4, point.y + 2 );
	hdc->MoveTo( point.x + 4, point.y + 2 );
	LineTo( hdc->m_hDC, point.x + 4, point.y - 2 );
	hdc->MoveTo( point.x + 4, point.y - 2 );
	LineTo( hdc->m_hDC, point.x - 2, point.y - 2 );
	SelectObject(hdc->m_hDC, holdpen);
	SelectObject(hdc->m_hDC, hbold);
	DeleteObject(hb);
	DeleteObject(hpen); 
}

IMPLEMENT_DYNAMIC(CSelectSign, CWnd)
CSelectSign::CSelectSign(): m_bMouseMove( false )
{
	g_pSelectSign = this;
}

CSelectSign::~CSelectSign()
{
}

BEGIN_MESSAGE_MAP(CSelectSign, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

void CSelectSign::OnPaint()
{
	PAINTSTRUCT ps;

	CDC *hdc = BeginPaint( &ps);
	if( hdc == NULL )
		return;

	DrawTriangle( hdc, g_Point );

	EndPaint(&ps);
}

void CSelectSign::DrawTriangle( CDC *hdc, CPoint point )
{
	HGDIOBJ hpen, holdpen, hbold;
	HBRUSH hb;

	hb = CreateSolidBrush( HexClr(0x000000) );
	hpen = CreatePen( PS_SOLID, 1, HexClr( 0x000000 ));
	hbold = SelectObject(hdc->m_hDC, hb);
	holdpen = SelectObject(hdc->m_hDC, hpen);
	hdc->MoveTo( point.x,  0 );
	LineTo( hdc->m_hDC, point.x - 3,  4 );
	hdc->MoveTo( point.x - 3, 4 );
	LineTo( hdc->m_hDC, point.x + 4, 4 );
	hdc->MoveTo( point.x + 4, 4 );
	LineTo( hdc->m_hDC, point.x, 0 );
	SelectObject(hdc->m_hDC, holdpen);
	SelectObject(hdc->m_hDC, hbold);
	DeleteObject(hb);
	DeleteObject(hpen); 
}

void CSelectSign::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bMouseMove = true;
	OnPaint();
}


void CSelectSign::OnLButtonUp( UINT nFlags, CPoint point )
{
	m_bMouseMove = false;

	CDC *hdc = GetWindowDC();
	if( hdc == NULL )
		return;

	DrawTriangle( hdc, point );
}

void CSelectSign::OnMouseMove( UINT nFlags, CPoint point )
{
	if( m_bMouseMove )
		OnPaint();
}