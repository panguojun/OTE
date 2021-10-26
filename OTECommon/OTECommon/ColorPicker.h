#ifndef _COLORPICKER_H
#define _COLORPICKER_H
#define HexClr(rgb) ((DWORD)(rgb>>16)|(rgb&0xFF00)|((rgb&0xFF)<<16))
static DWORD g_dwColorMap[5][8] = 
{
    { HexClr(0x000000), HexClr(0x993300), HexClr(0x333300), HexClr(0x003300), HexClr(0x003366), HexClr(0x000080), HexClr(0x333399), HexClr(0x333333)},
    { HexClr(0x800000), HexClr(0xFF6600), HexClr(0x808000), HexClr(0x008000), HexClr(0x008080), HexClr(0x0000FF), HexClr(0x666699), HexClr(0x808080)},
    { HexClr(0xFF0000), HexClr(0xFF9900), HexClr(0x99CC00), HexClr(0x008080), HexClr(0x33CCCC), HexClr(0x3366FF), HexClr(0x800080), HexClr(0x999999)},
    { HexClr(0xFF00FF), HexClr(0xFFCC00), HexClr(0xFFFF00), HexClr(0x00FF00), HexClr(0x00FFFF), HexClr(0x00CCFF), HexClr(0x993366), HexClr(0xC0C0C0)},
    { HexClr(0xFF99CC), HexClr(0xFFCC99), HexClr(0xFFFF99), HexClr(0xCCFFCC), HexClr(0xCCFFFF), HexClr(0x99CCFF), HexClr(0xCC99FF), HexClr(0xFFFFFF)}
};

typedef void (*NotifyFuncHdl_t)(DWORD, DWORD);

class CColorPicker: public CWnd
{
	DECLARE_DYNAMIC( CColorPicker )
public:
    CColorPicker();
    ~CColorPicker();

	void InitGDI();
	void ShutdownGDI();
    DWORD GetColor() {return m_Color;}
	void SetCurrentColor( DWORD color );

	bool m_bMouseDown;
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	virtual BOOL DestroyWindow();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	void CreateColorPicker( HWND hWnd, POINT *pt);
	void DrawRectangleColor();

	void DrawColor( CDC *hdc );
    void Draw(HDC hdc, byte row, byte col, bool hot, byte correction);
    void ReDraw(byte row, byte col, bool hot);

	void GradientColor( RECT rc, DWORD color1, DWORD color2 );

    DWORD m_Color;
    byte m_HotRow, m_HotCol;
    bool m_bCustomColor;
	bool m_bMouseMove;
  
	bool m_bHot;
 
	ULONG_PTR m_diplusToken;

public:
	NotifyFuncHdl_t		m_pNotifyFuncHdl;
};

class CColorBar: public CWnd
{
	DECLARE_DYNAMIC( CColorBar )
public:
    CColorBar();
    ~CColorBar();

    DWORD GetColor() { return m_Color; }
	void SetBarColor( DWORD color );
	DWORD GetCurrentColor() { return m_OuputColor; }
	afx_msg void OnPaint();

	bool m_bMouseDown;
	bool m_bMouseMove;


protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

private:
	void GradientColor( RECT rc, DWORD color1, DWORD color2 );

    DWORD m_Color;
	DWORD m_OuputColor;
};

class CSelectSign: public CWnd
{
	DECLARE_DYNAMIC( CSelectSign )
public:
    CSelectSign();
    ~CSelectSign();

	afx_msg void OnPaint();
	afx_msg void OnLButtonUp( UINT nFlags, CPoint point );
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );

	bool m_bMouseMove;

protected:
	DECLARE_MESSAGE_MAP()

	void DrawTriangle( CDC *hdc, CPoint point );
};

#endif