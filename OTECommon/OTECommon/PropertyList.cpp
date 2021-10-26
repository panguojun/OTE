#include "stdafx.h"
#include "Resource.h"
#include "PropertyList.h"
#include "OgreNoMemoryMacros.h"
#include <d3dx9.h>
#include "OgreMemoryMacros.h"

CString PropertyItem::GetValue()
{
	return this->m_text;
}

PropertyItem::PropertyItem( int m_nItemIndex, ITEMTYPE m_nType, int iExpand, ITEMSTATUS m_nStatus, TCHAR* m_szName,
						   CONTROLTYPE m_nControlType, int iEditor, bool b ): CObject()
{
	this->m_nControlType = m_nControlType;
	this->m_nItemIndex = m_nItemIndex;
	this->m_iExpand = iExpand;
	//this->m_nLeft = m_nLeft;
	this->m_nStatus = m_nStatus;
	this->m_nType = m_nType;
	_tcscpy( this->m_szName, m_szName );
	this->m_iNumChar = iEditor;
	m_bShow = b;
	//this->PropertyItem()
	m_nLevel = 0;
	m_text = "选中值";
}

PropertyItem::PropertyItem():CObject()
{
}

int CPropertyList::GetShowItem()
{
	int nCount = 0;
	FormatList();
	for( int i = 0; i < m_List.GetSize(); ++i )
		if( m_List[ i ]->m_bShow ) ++nCount;

	return nCount;
}

void CPropertyList::FormatList()
{
	if( m_List.GetSize() )
		m_List[ 0 ]->m_nLevel = 0;

	bool bShow = true;
	bool bFolder = false;
	int nHideLevel = -1;
	for( int i = 0; i < m_List.GetSize(); ++i )
	{
		if( i != m_List.GetSize() - 1 )
		{
			if( m_List[ i ]->m_nLevel == 0 && m_List[ i + 1 ]->m_nLevel == 1 )
				bFolder = true;
			else
				bFolder = false;
			
			m_bFolderList.push_back( bFolder );
		}

		if( bShow )
		{
			if( m_List[ i ]->m_bShow == false && m_List[ i ]->m_nType == ITEMTYPE_FOLDER )
			{
				bShow = false;
				nHideLevel = m_List[ i ]->m_nLevel;
				m_List[ i ]->m_nType = ITEMTYPE_CLOSEFOLDER;
				m_List[ i ]->m_bShow = true;
			}
			else
			{
				if( m_List[ i ]->m_nType == ITEMTYPE_FOLDER )
					m_List[ i ]->m_nType = ITEMTYPE_OPENFOLDER;
			}
		}
		else
		{
			if( m_List[ i ]->m_nLevel <= nHideLevel )
			{
				bShow = true;
				if( m_List[ i ]->m_nType == ITEMTYPE_FOLDER )
				{
					if( m_List[ i ]->m_bShow == false ) 
					{
						bShow = false;
						m_List[ i ]->m_nType = ITEMTYPE_CLOSEFOLDER;
					}
					else
						m_List[ i ]->m_nType = ITEMTYPE_OPENFOLDER;
				}
				m_List[ i ]->m_bShow = true;
			}
			else
			{
				if( m_List[ i ]->m_nType == ITEMTYPE_FOLDER )
				{
					if( m_List[ i ]->m_bShow == false ) 
					{
						m_List[ i ]->m_nType = ITEMTYPE_CLOSEFOLDER;
					}
					else
						m_List[ i ]->m_nType = ITEMTYPE_OPENFOLDER;
				}
				m_List[ i ]->m_bShow = false;
			}
		}

		//if( !m_List[ i ]->m_bClose ) ++nCount;
	}

	m_bFolderList.push_back( false );
}

void CPropertyList::DrawPlus( bool bPlus, CDC*pDC, const CRect &rect )
{
	CBitmap *pBmp = NULL;
	if( bPlus ) pBmp = &m_bmpPlus;
	else
		pBmp = &m_bmpSub;
	CDC memDC;
	memDC.CreateCompatibleDC( pDC );
	CBitmap *pOld = memDC.SelectObject( pBmp );
	pDC->BitBlt( rect.left, rect.top, rect.right, rect.bottom, &memDC, 0, 0, SRCCOPY );
	memDC.SelectObject( pOld );
}

IMPLEMENT_DYNAMIC( CPropertyList, CWnd )
CPropertyList::CPropertyList(): m_bAngleChange( false ),
								m_bScaleChange( false ),
								m_bAlphaChange( false ),
								m_bAlphaRange( false ),
								m_bColorChange( false ),	
								m_bVisChange( false ),									
								m_bEnterKey( false )
{
	//BOOL b = m_bmpSub.LoadBitmap( IDB_SUB );
	//b = m_bmpPlus.LoadBitmap( IDB_PLUS );
	//ASSERT( m_bmpPlus.GetSafeHandle() );

	m_nItemHeight = 20;
	m_nLeft = 0;
}

CPropertyList::~CPropertyList()
{
	Destroy();
}

void CPropertyList::Destroy()
{
	for( int i = ( int )m_List.GetSize() - 1;i >= 0; --i )
		delete m_List[ i ];

	m_List.RemoveAll();
	m_bFolderList.clear();
}

BEGIN_MESSAGE_MAP( CPropertyList, CWnd )
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
    ON_CBN_SELCHANGE( 190, OnSelChange )
	ON_CBN_KILLFOCUS( 190, OnCbnKillfocusCombo )
	ON_EN_KILLFOCUS( 200, OnEnKillfocusEdit )
	ON_BN_CLICKED( 210, OnButtonCliked )
	ON_CBN_SELCHANGE( 220, OnVisSelChange )
END_MESSAGE_MAP()

void CPropertyList::InitPropertyList()
{	
}

COLORREF color_table[]=
{
	0x99a8ac,
	0xc8d0d4,
	0xc8d0d4,
	0x000000,
	0x99a8ac,
	0xc56a31,
	0xc8d0c4,
};
void CPropertyList::DoDraw( CDC *pDC )
{
	ASSERT( pDC );
	CRect rectClient;
	GetWindowRect( &rectClient );
	rectClient.right = rectClient.Width();
	rectClient.bottom = rectClient.Height();
	rectClient.top = rectClient.left = 0;
	
	Draw( pDC, rectClient );
}

void CPropertyList::Draw( CDC*pDC, const CRect &rect )
{
	ASSERT( pDC );
	int nCount = GetShowItem();
	CPen border_pen( PS_SOLID, 1, color_table[ 0 ] );
	CPen*pOldPen = pDC->SelectObject( &border_pen );
	pDC->Rectangle( rect );
    if( nCount <= 0 )
	{
		pDC->SelectObject( pOldPen );
		return;
	}
	
	CBrush margin_brush( color_table[ 1 ] );
	int nMarginHeight = nCount * m_nItemHeight;
	CBrush *pOldBrush = pDC->SelectObject( &margin_brush );
	CPen line_pen( PS_SOLID, 1, color_table[ 2 ] );
	pDC->SelectObject( &line_pen );
	if( rect.Height() < nMarginHeight + 1 )
		pDC->Rectangle( 1, 1, MARGIN_WIDTH + 1, rect.Height() - 1 );
	else
	pDC->Rectangle( 1, 1, MARGIN_WIDTH + 1, nMarginHeight + 1 );
	
	int nLeft = ( rect.Width() - MARGIN_WIDTH - 1 - 1 ) / 2;
	//nLeft = m_List[ 0 ].m_nLeft
	if( m_nLeft <= 0 ) m_nLeft = nLeft;
	else
		nLeft = m_nLeft;
	for( int i = 1;i <= nCount; ++i )
	{
		if( i * m_nItemHeight + 1 > rect.Height() )
			break;
		pDC->MoveTo( MARGIN_WIDTH + 1, i * m_nItemHeight + 1 );
		pDC->LineTo( rect.right - 1, i * m_nItemHeight + 1 );
	}
	pDC->MoveTo( nLeft + MARGIN_WIDTH + 1, 1 );
	if( rect.Height() < nMarginHeight + 1 )
		pDC->LineTo( nLeft + MARGIN_WIDTH + 1, rect.Height() );
	else
	   pDC->LineTo( nLeft + MARGIN_WIDTH + 1, nMarginHeight + 1 );

	CFont *pFont = this->GetFont();
	ASSERT( pFont );
	CFont *pOldFont = pDC->SelectObject( pFont );
	pOldFont->DeleteObject();
	//delete pOldFont;
	pDC->SetTextColor( color_table[ 3 ] );
	CRect rcItem( MARGIN_WIDTH + 1 + 1, 2, MARGIN_WIDTH + 10, 3 );
	CRect rcRight;

	//跳过关闭的项目
	int j = 0;
	CRect rectPlus;
	CString strValue( _T( "" ) );
	int n = pDC->GetTextExtent( _T( "  " ) ).cx;
	int nTimes = 0;
	std::list< bool >::iterator iter = m_bFolderList.begin();
	for( int i = 0; i < nCount;++i )
	{
		PropertyItem *pItem = m_List[ j ];
		while( pItem && ( pItem->m_bShow == false ) )
		{
			++j;
			pItem = m_List[ j ];
		}

		rcItem.top = 1 + i * m_nItemHeight;
		rcItem.bottom = rcItem.top + m_nItemHeight;
		if( rcItem.bottom > rect.Height() ) 
			break;//不再画了

		rcItem.right = rcItem.left + m_nLeft;
		rcRight.left = rcItem.right + 1;
		rcRight.top = rcItem.top;
		rcRight.bottom = rcItem.bottom;
		rcRight.right = rect.right - 1;
		rectPlus.top = rcItem.top + ( rcItem.Height() - 8 ) / 2;
		rectPlus.bottom = rcItem.top + ( rcItem.Height() + 8 ) / 2;
		rectPlus.right = ( MARGIN_WIDTH + 8 ) / 2 + 1;
		rectPlus.left = ( MARGIN_WIDTH - 8 ) / 2 + 1;
		
		if( pItem->m_nStatus == ITEMSTATUS_DISABLE )
			pDC->SetTextColor( color_table[ 4 ] );

		strValue = pItem->m_szName;
		nTimes = 0;
		for( int k = 0; k < pItem->m_nLevel; ++k )
		{
			strValue = _T( "  " ) + strValue;
			++nTimes;
		}
		rectPlus.right += nTimes * n;
		rectPlus.left += nTimes * n;
		
		pDC->DrawText( strValue, &rcItem, DT_VCENTER | DT_SINGLELINE );
		pDC->DrawText( pItem->m_text, rcRight, DT_VCENTER | DT_SINGLELINE );
		if( pItem->m_nStatus == ITEMSTATUS_DISABLE )
			pDC->SetTextColor( color_table[ 3 ] );

		if( pItem->m_nType == ITEMTYPE_OPENFOLDER && pItem->m_iExpand == 1 ) 
		{
			//DrawPlus( false, pDC, rectPlus );
			//pItem->m_icon = rectPlus;
		}
		else 
			if( pItem->m_nType == ITEMTYPE_CLOSEFOLDER && pItem->m_iExpand == 1 ) 
			{
				//DrawPlus( true, pDC, rectPlus );
				//pItem->m_icon = rectPlus;
			}
		++j;
	}
	
	pDC->SelectObject( pOldPen );
	pDC->SelectObject( pOldBrush );
	GdiFlush();
}

void CPropertyList::OnPaint()
{
	CPaintDC dc( this );
	DoDraw( &dc );
}

void CPropertyList::OnButtonCliked()
{
	D3DXCOLOR c( m_Color );
	COLORREF color = RGB( c.r * 255.0f, c.g * 255.0f, c.b * 255.0f );
	CColorDialog ColorDialog( color, CC_FULLOPEN );
	if( ColorDialog.DoModal() == IDOK )
	{
		m_Color = D3DCOLOR_ARGB( 0, GetRValue( ColorDialog.GetColor() ), GetGValue( ColorDialog.GetColor() ), GetBValue( ColorDialog.GetColor() ) );
		m_bColorChange = true;
	}

	Invalidate();
}

void CPropertyList::OnLButtonDown( UINT nFlags, CPoint point )
{
	int nCount = GetShowItem();
	TRACE( "%d\n", nCount );
	int j = 0;
	for( int i = 0; i < nCount; ++i )
	{
		PropertyItem *pItem = m_List[ j ];
		while( pItem && ( pItem->m_bShow == false ) )
		{
			++j;
			pItem = m_List[ j ];
		}
		m_ListBox.ShowWindow( SW_HIDE );
		m_Edit.ShowWindow( SW_HIDE );
		m_Button.ShowWindow( SW_HIDE );
		if( pItem->m_nType == ITEMTYPE_CLOSEFOLDER && pItem->m_icon.PtInRect( point ) /*&& pItem->m_nStatus != DISABLE*/ )
		{
			//AfxMessageBox( "打开" );
			m_nItemIndex = j;
			OpenNode( j );
			break;
		}
		else
		{
			if( ( pItem->m_nType == ITEMTYPE_OPENFOLDER && pItem->m_icon.PtInRect( point ) ) )
			{
				//AfxMessageBox( "关闭" );
				m_nItemIndex = j;
				CloseNode( j );
				break;
			}
			else
			{
				//点在值的区域
				//AfxMessageBox( "ddd" );
				CRect rcWin;
				GetClientRect( &rcWin );
				CRect rcValue, rcName;
				rcValue.left = rcWin.left + MARGIN_WIDTH + m_nLeft + 1;
				rcValue.right = rcWin.right - 1;
				rcValue.top = 1 + m_nItemHeight * i;
				rcValue.bottom = rcValue.top + m_nItemHeight;
				TRACE( "j = %d, %s\n", j, pItem->m_text );
				if( rcValue.PtInRect( point ) && pItem->m_nStatus != ITEMSTATUS_DISABLE )
				{
					m_nItemIndex = j;
					ClickValueArea( j, rcValue );
					break;
				}
			}
		}
		++j;
	}

	CWnd::OnLButtonDown( nFlags, point );
}

void CPropertyList::OpenNode( int nIndex )
{
	for( int i = nIndex + 1; i < m_List.GetSize(); ++i )
	{
		if( m_List[ i ]->m_nLevel <= m_List[ nIndex ]->m_nLevel )
			break;
		else
		{
			if( m_List[ i ]->m_nLevel == m_List[ nIndex ]->m_nLevel + 1 )
			{
				m_List[ i ]->m_bShow = true;
				if( m_List[ i ]->m_nType != ITEMTYPE_ITEM )
					m_List[ i ]->m_nType = ITEMTYPE_CLOSEFOLDER;
			}
		}
	}

	m_List[ nIndex ]->m_nType = ITEMTYPE_OPENFOLDER;
	//this->CalcuRgn();
	Invalidate();
}

void CPropertyList::CloseNode( int nIndex )
{
	for( int i = nIndex + 1; i < m_List.GetSize(); ++i )
	{
		if( m_List[ i ]->m_nLevel <= m_List[ nIndex ]->m_nLevel )
			break;
		else
			m_List[ i ]->m_bShow = false;
	}

	m_List[ nIndex ]->m_nType = ITEMTYPE_CLOSEFOLDER;
	//CalcuRgn();
	Invalidate();
}

void CPropertyList::ClickValueArea( int nIndex, const CRect &rect )
{
	if( m_List[ nIndex ]->m_nStatus != ITEMSTATUS_DISABLE )
	{
		if( m_List[ nIndex ]->m_nControlType == CONTROLTYPE_LISTBOX )
		{
			CRect r( rect );
			r.bottom += rect.Height() * 4;
			m_ListBox.MoveWindow( r );
			m_ListBox.ResetContent();
			int n = 0;
			for( int i = 0; i < m_List[ nIndex ]->m_strings.GetSize(); ++i )
			{
				m_ListBox.AddString( m_List[ nIndex ]->m_strings[ i ] );
				if( m_List[ nIndex ]->m_strings[ i ] == m_List[ nIndex ]->m_text && n == 0 )
					n = i;
			}

			//for( int i = 0; i < m_List[ nIndex ]->m_strings.GetSize() )
			m_ListBox.SetCurSel( n );
			m_ListBox.ShowWindow( SW_SHOW );
			m_nItemIndex = nIndex;
		}
		else if( m_List[ nIndex ]->m_nControlType == CONTROLTYPE_EDIT )
		{
			//CRect r( rect );
			//r.bottom += rect.Height() * 4;
			m_Edit.MoveWindow( rect );
			m_Edit.ShowWindow( SW_SHOW );
			m_nItemIndex = nIndex;
			m_Edit.SetWindowText( m_List[ nIndex ]->m_text );
			//AfxMessageBox( "d" );
		}
		else 
		{
			CRect r( rect );
			//r.left += 40;
			m_Button.MoveWindow( r );
			m_Button.ShowWindow( SW_SHOW );
			m_nItemIndex = nIndex;
		}
	}
}

void CPropertyList::PreSubclassWindow()
{
	m_ListBox.Create( WS_CHILD & ( ~WS_VISIBLE ) | WS_VSCROLL | CBS_DROPDOWNLIST | WS_TABSTOP, CRect( 0, 0, 0, 0 ), this, 190 );
	m_ListBox.SetFont( GetFont() );
	m_Edit.Create( WS_CHILD & ( ~WS_VISIBLE ) | WS_TABSTOP | WS_BORDER, CRect( 0, 0, 100, 30 ), this, 200 );
	ASSERT( m_Edit.GetSafeHwnd() );
	m_Edit.SetFont( GetFont() );
	m_Button.Create( "...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON , CRect( 0, 0, 100, 30 ), this, 210 );
	m_Button.SetFont( GetFont() );
	m_Button.ShowWindow( SW_HIDE );
	CWnd::PreSubclassWindow();
}

void CPropertyList::OnSelChange()
{
	m_List[ m_nItemIndex ]->m_text = m_List[ m_nItemIndex ]->m_strings[ m_ListBox.GetCurSel() ];
	
	if(m_nItemIndex == 5)
		m_bVisChange = true;

}
void CPropertyList::OnVisSelChange()
{
	
}

void CPropertyList::OnCbnKillfocusCombo()
{
	m_ListBox.ShowWindow( SW_HIDE );
	m_Button.ShowWindow( SW_HIDE );
}

void CPropertyList::OnEnKillfocusEdit()
{
	CString str, text;
	text = m_List[ m_nItemIndex ]->m_text;
	m_Edit.GetWindowText( str );

	float fValue = ( float )( ::atof( str.GetBuffer() ) );
	if( m_nItemIndex == 2 )
	{
		if( fValue < 0.0f || fValue > 1.0f )
		{
			AfxMessageBox( "超出0.0f到1.0f的范围！" );
			m_List[ m_nItemIndex ]->m_text = text;
			return;
		}
	}
	if( m_nItemIndex == 3 )
	{
		if( fValue < 1.0f )
		{
			AfxMessageBox("该值不能小于1.0f！");
			m_List[ m_nItemIndex ]->m_text = text;
			return;
		}
	}

	if( m_List[ m_nItemIndex ]->m_iNumChar )
	{
		char *pTemp = str.GetBuffer();
		//::strcpy( pTemp, str );
		while( *pTemp != '\0' )
		{
			if( ( *pTemp > '9' || *pTemp < '0' ) && *pTemp != '.' )
			{
				AfxMessageBox("只能输入数字！");
				m_List[ m_nItemIndex ]->m_text = text;
				return;
			}

			++pTemp;
		}

		//delete [] pTemp;
		//pTemp = NULL;
	}

	m_List[ m_nItemIndex ]->m_text = str;
	if( m_nItemIndex == 2 )
		m_bAlphaChange = true;
	if( m_nItemIndex == 3 )
		m_bAlphaRange = true;
	if( m_nItemIndex == 6 )
		m_bAngleChange = true;
	if( m_nItemIndex == 7 )
		m_bScaleChange = true;
}

void CPropertyList::SetListValue( int item, float fValue )
{
	CString s;
	s.Format( "%0.1f", fValue );
	m_List[ item ]->m_text = s;
	m_nItemIndex = item;

	CRect rcWin;
	GetClientRect( &rcWin );
	CRect rcValue,rcName;
	rcValue.left = 106/*rcWin.left + MARGIN_WIDTH + m_nLeft + 1*/;
	rcValue.right = rcWin.right - 1;
	rcValue.top = 1 + m_nItemHeight * item;
	rcValue.bottom = rcValue.top+m_nItemHeight;
	if( m_List[ item ]->m_nControlType == CONTROLTYPE_EDIT )
	{
		m_Edit.MoveWindow( rcValue );
		m_Edit.ShowWindow( SW_SHOW );
		m_Edit.SetWindowText( m_List[ item ]->m_text );
	}
}

void CPropertyList::GetListValue( int item, float &fValue )
{
	fValue = ( float )::atof( m_List[ item ]->m_text );
}

void CPropertyList::SetListBoxSelect( int index )
{
	m_ListBox.SetCurSel( index );
}

void CPropertyList::SetColorData( DWORD color )
{
	m_Color = color;
}

void CPropertyList::GetColorData( DWORD &color )
{
	color = m_Color;
}

void CPropertyList::GetListBoxSelect( int &iSelect )
{
	iSelect = m_ListBox.GetCurSel();
}

void CPropertyList::ParseStringList( CString szList, PropertyItem &_item )
{
	int nPos = 0;
	CString str = szList.Tokenize( "#", nPos );
	_item.m_strings.RemoveAll();
	while( str != "" )
	{
		_item.m_strings.Add( str );
		str = szList.Tokenize( "#", nPos );
	}

	if( _item.m_strings.GetSize() )
		_item.m_text = _item.m_strings[ 0 ];
}

void CPropertyList::CalcuRgn()
{
	if( m_rgn.m_hObject )
		m_rgn.DeleteObject();

	CRect rcClient;
	GetClientRect( &rcClient );
	if( m_nItemIndex == -1 ) 
	{
		m_rgn.CreateRectRgn( rcClient.left, rcClient.top, rcClient.right, rcClient.bottom );
		return;
	}

	int nCount = GetShowItem();
	//int nAll = nCount;
	int j = m_nItemIndex;
	for( ; j < this->m_List.GetSize(); ++j )
	{
		if( this->m_List[ j ]->m_bShow )
			--nCount;
	}

	rcClient.top = nCount * m_nItemHeight + 1;
	//AfxMessageBox( "d" );
	TRACE( "%d,%d,%d,%d\n", rcClient.left, rcClient.top, rcClient.right, rcClient.bottom );
	m_rgn.CreateRectRgn( rcClient.left, rcClient.top, rcClient.right, rcClient.bottom );
}

LRESULT CPropertyList::WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) 
{
	switch( message )
    {
		case WM_KEYDOWN:
			{
				switch( wParam )
				{
				case VK_RETURN:
					{
						m_bEnterKey = true;
						//AfxMessageBox("d");
						break;
					}
				}
			}
			break;
    }

    return CWnd::WindowProc( message, wParam, lParam );
}