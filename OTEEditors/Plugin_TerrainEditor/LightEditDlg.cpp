// LightEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_TerrainEditor.h"
#include "TerrainEditorDlg.h"
#include "LightEditDlg.h"
#include "OTEQTSceneManager.h"
#include "OTERenderLight.h"
#include "OTECommon.h"
#include "lighteditdlg.h"

// ------------------------------------------------
using namespace Ogre;
using namespace OTE;

// ------------------------------------------------
CLightEditDlg CLightEditDlg::s_Inst;

CLightEditDlg *g_pLightEditDlg = NULL;

extern CTerrainEditorDlg *g_pTerrainEditorDlg;

static const int g_iId = 9000;

BEGIN_MESSAGE_MAP( ColorEdit, CEdit )
	ON_WM_CHAR()
END_MESSAGE_MAP()

void ColorEdit::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags ) 
{
    if( ( ( nChar >= _T( '0' ) ) && ( nChar <= _T( '9' ) ) ) || ( nChar == VK_BACK ) || ( nChar == _T ( '(' ) )
		|| ( nChar == _T ( ')' ) ) ||( nChar == _T ( '-' ) ) || ( nChar == _T ( ' ' ) ) )
		CEdit::OnChar( nChar, nRepCnt, nFlags );
}

IMPLEMENT_DYNAMIC(CLightEditDlg, CDialog)
CLightEditDlg::CLightEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLightEditDlg::IDD, pParent),
	  m_fTimeValue( 0.0f ),
	  m_iRedText( 0 ),
	  m_iGreenText( 0 ),
	  m_iBlueText( 0 )
{
	g_pLightEditDlg = this;
}

CLightEditDlg::~CLightEditDlg()
{
}

void CLightEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_LightProp);
	DDX_Control(pDX, IDC_SLIDER1, m_LightSlider);
	DDX_Control(pDX, IDC_SLIDER2, m_SliderCtrl);

	DDX_Control(pDX, IDC_RED_EDIT, m_RedText);
	DDX_Control(pDX, IDC_GREEN_EDIT, m_GreenText);
	DDX_Control(pDX, IDC_BLUE_EDIT, m_BlueText);
}

BEGIN_MESSAGE_MAP(CLightEditDlg, CDialog)
	ON_REGISTERED_MESSAGE( WM_COLORPICKER_SELCHANGE, OnSelChange )
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, OnLvnItemchangedList2)
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, OnHdnItemdblclickList2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnNMDblclkList2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnNMReleasedcaptureSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, OnNMCustomdrawSlider2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER2, OnNMReleasedcaptureSlider2)
	ON_NOTIFY(NM_CLICK, IDC_LIST2, OnNMClickList2)
	ON_EN_CHANGE(IDC_RED_EDIT, OnEnChangeRedEdit)
	ON_EN_CHANGE(IDC_GREEN_EDIT, OnEnChangeGreenEdit)
	ON_EN_CHANGE(IDC_BLUE_EDIT, OnEnChangeBlueEdit)
	ON_STN_CLICKED(IDC_COLOR_STATIC, OnStnClickedColorStatic)
END_MESSAGE_MAP()


// CLightEditDlg 消息处理程序

void CLightEditDlg::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CLightEditDlg::OnHdnItemdblclickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CLightEditDlg::SetValues()
{
	m_crCurrent = m_ColorSpectrum.GetRGB();
	m_iRedText = GetRValue( m_crCurrent );
	m_iGreenText = GetGValue( m_crCurrent );
	m_iBlueText = GetBValue( m_crCurrent );

	CString s;
	s.Format( "%d", m_iRedText );
	m_RedText.SetWindowText( s );
	s.Format( "%d", m_iGreenText );
	m_GreenText.SetWindowText( s );
	s.Format( "%d", m_iBlueText );
	m_BlueText.SetWindowText( s );

	if( IsWindow( g_pTerrainEditorDlg->m_TabCtrl.m_hWnd ) )
		g_pTerrainEditorDlg->m_TabCtrl.SendMessage( WM_COLORPICKER_SELCHANGE, ( WPARAM )m_crCurrent, ( LPARAM )g_iId );

	UpdateWindow();
}

// ------------------------------------------------
BOOL CLightEditDlg::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();

	m_LightProp.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);//LVS_EX_GRIDLINES|
	m_LightProp.InsertColumn(0, "", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_CENTER, 50); 
	m_LightProp.InsertColumn(1, "R", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_CENTER, 70); 
	m_LightProp.InsertColumn(2, "G", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_CENTER, 70); 
	m_LightProp.InsertColumn(3, "B", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_CENTER, 70);
	//m_LightProp.InsertColumn(4, "A", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_CENTER, 70); 

	// 初始化列表
	UpdateLightColorList();

	m_LightSlider.SetRangeMin(6);
	m_LightSlider.SetRangeMax(18);
	m_LightSlider.SetPos(COTERenderLight::GetInstance()->m_CurrentTime);
	m_fTimeValue = m_LightSlider.GetPos() / 24.0f;

	m_SliderCtrl.SetRangeMin(0);
	m_SliderCtrl.SetRangeMax(400);

	m_RedText.LimitText( 3 );
	m_GreenText.LimitText( 3 );
	m_BlueText.LimitText( 3 );

	CRect rect;
	GetDlgItem( IDC_COLOR_STATIC )->GetWindowRect( &rect );
	ScreenToClient( &rect );
	GetDlgItem( IDC_COLOR_STATIC )->ShowWindow( SW_HIDE );
	m_ColorSpectrum.Create( AfxGetInstanceHandle(), WS_CHILD | WS_VISIBLE | WS_TABSTOP, rect, m_hWnd, g_iId, RGB( 255, 0, 0 ) );
	::SetWindowPos( m_ColorSpectrum.m_hWnd, ::GetDlgItem( m_hWnd, IDC_COLOR_STATIC ), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	SetValues();

	return tRet;
}

// ------------------------------------------------
// 初始化列表
void CLightEditDlg::UpdateLightColorList()
{
	OTE::LightKey_t* lif = COTERenderLight::GetInstance()->GetLightKey();
	
	if(m_LightProp.GetItemCount() == 0)
	{
		m_LightProp.InsertItem(0, "环境光");
		m_LightProp.InsertItem(1, "漫反射");        
		//m_LightProp.InsertItem(2, "补光");
		//m_LightProp.InsertItem(3, "地形环境光");
		//m_LightProp.InsertItem(4, "地形主光");
	}

	m_LightProp.SetItemText(0, 1, OTEHelper::Str(lif->ambient.GetColor().r));
	m_LightProp.SetItemText(0, 2, OTEHelper::Str(lif->ambient.GetColor().g));
	m_LightProp.SetItemText(0, 3, OTEHelper::Str(lif->ambient.GetColor().b));
	//m_LightProp.SetItemText(0, 4, OTEHelper::Str(lif->ambient.GetColor().a));

	m_LightProp.SetItemText(1, 1, OTEHelper::Str(lif->diffuse.GetColor().r));
	m_LightProp.SetItemText(1, 2, OTEHelper::Str(lif->diffuse.GetColor().g));
	m_LightProp.SetItemText(1, 3, OTEHelper::Str(lif->diffuse.GetColor().b));
	//m_LightProp.SetItemText(1, 4, OTEHelper::Str(lif->diffuse.GetColor().a));

	/*m_LightProp.SetItemText(2, 1, OTEHelper::Str(lif->reflect.GetColor().r));
	m_LightProp.SetItemText(2, 2, OTEHelper::Str(lif->reflect.GetColor().g));
	m_LightProp.SetItemText(2, 3, OTEHelper::Str(lif->reflect.GetColor().b));*/
	//m_LightProp.SetItemText(2, 4, OTEHelper::Str(lif->reflect.GetColor().a));

	/*m_LightProp.SetItemText(3, 1, OTEHelper::Str(lif->terrAmb.GetColor().r));
	m_LightProp.SetItemText(3, 2, OTEHelper::Str(lif->terrAmb.GetColor().g));
	m_LightProp.SetItemText(3, 3, OTEHelper::Str(lif->terrAmb.GetColor().b));*/
	//m_LightProp.SetItemText(3, 4, OTEHelper::Str(lif->terrAmb.GetColor().a));

	//m_LightProp.SetItemText(4, 1, OTEHelper::Str(lif->terrDiff.GetColor().r));
	//m_LightProp.SetItemText(4, 2, OTEHelper::Str(lif->terrDiff.GetColor().g));
	//m_LightProp.SetItemText(4, 3, OTEHelper::Str(lif->terrDiff.GetColor().b));
	//m_LightProp.SetItemText(4, 4, OTEHelper::Str(lif->terrDiff.GetColor().a));
}

void CLightEditDlg::SetSceneLight()
{
	POSITION pos = m_LightProp.GetFirstSelectedItemPosition();
	int index = m_LightProp.GetNextSelectedItem( pos );

	COLORREF color = m_ColorSpectrum.GetRGB();
	if( index != -1 )
	{
		if( index == 0 )
		{
			COTERenderLight::GetInstance()->GetLightKey()->ambient.baseCor.r = GetRValue( color ) / 255.0f; 
			COTERenderLight::GetInstance()->GetLightKey()->ambient.baseCor.g = GetGValue( color ) / 255.0f; 
			COTERenderLight::GetInstance()->GetLightKey()->ambient.baseCor.b = GetBValue( color ) / 255.0f;
		}
		else if( index == 1 )
		{
			COTERenderLight::GetInstance()->GetLightKey()->diffuse.baseCor.r = GetRValue( color ) / 255.0f; 
			COTERenderLight::GetInstance()->GetLightKey()->diffuse.baseCor.g = GetGValue( color ) / 255.0f; 
			COTERenderLight::GetInstance()->GetLightKey()->diffuse.baseCor.b = GetBValue( color ) / 255.0f;
		}
		/*else if( index == 2 )
		{
			COTERenderLight::GetInstance()->GetLightKey()->reflect.baseCor.r = GetRValue( color ) / 255.0f; 
			COTERenderLight::GetInstance()->GetLightKey()->reflect.baseCor.g = GetGValue( color ) / 255.0f; 
			COTERenderLight::GetInstance()->GetLightKey()->reflect.baseCor.b = GetBValue( color ) / 255.0f;
		}
		else if( index == 3 )
		{
			COTERenderLight::GetInstance()->GetLightKey()->terrAmb.baseCor.r = GetRValue( color ) / 255.0f; 
			COTERenderLight::GetInstance()->GetLightKey()->terrAmb.baseCor.g = GetGValue( color ) / 255.0f; 
			COTERenderLight::GetInstance()->GetLightKey()->terrAmb.baseCor.b = GetBValue( color ) / 255.0f;
		}
		else if( index == 4 )
		{
			COTERenderLight::GetInstance()->GetLightKey()->terrDiff.baseCor.r = GetRValue( color ) / 255.0f; 
			COTERenderLight::GetInstance()->GetLightKey()->terrDiff.baseCor.g = GetGValue( color ) / 255.0f; 
			COTERenderLight::GetInstance()->GetLightKey()->terrDiff.baseCor.b = GetBValue( color ) / 255.0f;
		}*/
	}

	int value = m_LightSlider.GetPos();
	COTERenderLight::GetInstance()->SetCurrentTime(value);

	UpdateLightColorList();
}

// ---------------------------------------------
// 双击一项
OTE::LightColor_t* g_CurrentLightCor = NULL;	
COLORREF g_BaseCor;

#define DO_EDITOK2(type)  CListViewEditors dlg(type, m_LightProp.GetItemText(tItem, tSubItem), tSubItemRect); if(IDOK == dlg.DoModal() && !CListViewEditors::s_Text.IsEmpty())

void CLightEditDlg::OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	int tItem, tSubItem;
	CRect tSubItemRect;

	if(m_LightProp.GetDClickedItem(tItem, tSubItem, tSubItemRect))
	{
		CString tmTxt = m_LightProp.GetItemText(tItem, 0);			
		
		if(tmTxt == "投影光")
		{
			g_CurrentLightCor = &COTERenderLight::GetInstance()->GetLightKey()->ambient;
			m_SliderCtrl.SetPos(200 * g_CurrentLightCor->gama);
		}
		else if(tmTxt == "主光")
		{
			g_CurrentLightCor = &COTERenderLight::GetInstance()->GetLightKey()->diffuse;
			m_SliderCtrl.SetPos(200 * g_CurrentLightCor->gama);
		}
	/*	else if(tmTxt == "补光")
		{
			g_CurrentLightCor = &COTERenderLight::GetInstance()->GetLightKey()->reflect;
			m_SliderCtrl.SetPos(200 * g_CurrentLightCor->gama);
		}
		else if(tmTxt == "地形环境光")
		{
			g_CurrentLightCor = &COTERenderLight::GetInstance()->GetLightKey()->terrAmb;
			m_SliderCtrl.SetPos(200 * g_CurrentLightCor->gama);
		}
		else if(tmTxt == "地形主光")
		{
			g_CurrentLightCor = &COTERenderLight::GetInstance()->GetLightKey()->terrDiff;
			m_SliderCtrl.SetPos(200 * g_CurrentLightCor->gama);
		}*/
		
		if(g_CurrentLightCor)
		{			
			g_BaseCor = RGB(	g_CurrentLightCor->baseCor.r * 255.0f, 
								g_CurrentLightCor->baseCor.g * 255.0f, 
								g_CurrentLightCor->baseCor.b * 255.0f
								);

			CColorDialog color(g_BaseCor, CC_FULLOPEN );

			if(color.DoModal() == IDOK)
			{
				unsigned char r = GetRValue(color.GetColor()) > 127 ? 127 : GetRValue(color.GetColor());
				unsigned char g = GetGValue(color.GetColor()) > 127 ? 127 : GetGValue(color.GetColor());
				unsigned char b = GetBValue(color.GetColor()) > 127 ? 127 : GetBValue(color.GetColor());

				g_BaseCor = RGB(r, g, b);
				g_CurrentLightCor->baseCor = Ogre::ColourValue(
					r / 255.0f,
					g / 255.0f, 
					b / 255.0f
					);	

				m_iRedText = ( int )r, m_iGreenText = ( int )g, m_iBlueText = ( int )b;
				m_crCurrent = RGB( m_iRedText, m_iGreenText, m_iBlueText );
				m_ColorSpectrum.SetRGB( m_crCurrent );
				SetValues();
			}
		}

		int value = m_LightSlider.GetPos();
		COTERenderLight::GetInstance()->SetCurrentTime(value);		

		Invalidate();
		m_LightProp.Invalidate();
	}

	UpdateLightColorList();
	
}

void CLightEditDlg::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{	
	int  value = m_LightSlider.GetPos();
	m_fTimeValue = value / 24.0f;
	COTERenderLight::GetInstance()->SetCurrentTime(value);	

	UpdateLightColorList();

}

void CLightEditDlg::OnNMThemeChangedSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// 该功能要求使用 Windows XP 或更高版本。
	// 符号 _WIN32_WINNT 必须 >= 0x0501。
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CLightEditDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CLightEditDlg::OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CLightEditDlg::OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(g_CurrentLightCor)
	{	
		g_CurrentLightCor->gama = m_SliderCtrl.GetPos() / 200.0f;			

		int value = m_LightSlider.GetPos();
		COTERenderLight::GetInstance()->SetCurrentTime(value);	
		UpdateLightColorList();
	}
}

void CLightEditDlg::OnNMThemeChangedSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	
}


void CLightEditDlg::OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	POSITION pos = m_LightProp.GetFirstSelectedItemPosition();
	int index = m_LightProp.GetNextSelectedItem( pos );
	if( index != -1 )
	{
		if( index == 0 )
		{
			g_CurrentLightCor = &COTERenderLight::GetInstance()->GetLightKey()->ambient;
			m_SliderCtrl.SetPos(200 * g_CurrentLightCor->gama);
			m_iRedText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->ambient.baseCor.r * 255.0f );
			m_iGreenText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->ambient.baseCor.g * 255.0f );
			m_iBlueText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->ambient.baseCor.b * 255.0f );
		}
		else if( index == 1 )
		{
			g_CurrentLightCor = &COTERenderLight::GetInstance()->GetLightKey()->diffuse;
			m_SliderCtrl.SetPos(200 * g_CurrentLightCor->gama);
			m_iRedText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->diffuse.baseCor.r * 255.0f );
			m_iGreenText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->diffuse.baseCor.g * 255.0f );
			m_iBlueText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->diffuse.baseCor.b * 255.0f );
		}
		/*else if( index == 2 )
		{
			g_CurrentLightCor = &COTERenderLight::GetInstance()->GetLightKey()->reflect;
			m_SliderCtrl.SetPos(200 * g_CurrentLightCor->gama);
			m_iRedText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->reflect.baseCor.r * 255.0f );
			m_iGreenText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->reflect.baseCor.g * 255.0f );
			m_iBlueText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->reflect.baseCor.b * 255.0f );
		}
		else if( index == 3 )
		{
			g_CurrentLightCor = &COTERenderLight::GetInstance()->GetLightKey()->terrAmb;
			m_SliderCtrl.SetPos(200 * g_CurrentLightCor->gama);
			m_iRedText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->terrAmb.baseCor.r * 255.0f );
			m_iGreenText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->terrAmb.baseCor.g * 255.0f );
			m_iBlueText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->terrAmb.baseCor.b * 255.0f );
		}
		else if( index == 4 )
		{
			g_CurrentLightCor = &COTERenderLight::GetInstance()->GetLightKey()->terrDiff;
			m_SliderCtrl.SetPos(200 * g_CurrentLightCor->gama);
			m_iRedText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->terrDiff.baseCor.r * 255.0f );
			m_iGreenText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->terrDiff.baseCor.g * 255.0f );
			m_iBlueText = ( int )( COTERenderLight::GetInstance()->GetLightKey()->terrDiff.baseCor.b * 255.0f );
		}*/

		m_crCurrent = RGB( m_iRedText, m_iGreenText, m_iBlueText );
		m_ColorSpectrum.SetRGB( m_crCurrent );
		SetValues();
	}
}

void CLightEditDlg::OnEnChangeRedEdit()
{
	char szText[ 10 ] = "";
	m_RedText.GetWindowText( szText, 10 );
	int iValue = ::atoi( szText );

	if( iValue < 0 || iValue > 255 )
	{
		AfxMessageBox( "超出0到255的泛围！", 0, 0 );
		return;
	}

	CWnd *pWnd = GetFocus();
	if( pWnd && *pWnd == m_RedText )
	{
		m_iRedText = iValue;
		m_crCurrent = RGB( m_iRedText, m_iGreenText, m_iBlueText );
		m_ColorSpectrum.SetRGB( m_crCurrent );
		SetSceneLight();
	}
}

void CLightEditDlg::OnEnChangeGreenEdit()
{
	char szText[ 10 ] = "";
	m_GreenText.GetWindowText( szText, 10 );
	int iValue = ::atoi( szText );

	if( iValue < 0 || iValue > 255 )
	{
		AfxMessageBox( "超出0到255的泛围！", 0, 0 );
		return;
	}

	CWnd *pWnd = GetFocus();
	if( pWnd && *pWnd == m_GreenText )
	{
		m_iGreenText = iValue;
		m_crCurrent = RGB( m_iRedText, m_iGreenText, m_iBlueText );
		m_ColorSpectrum.SetRGB( m_crCurrent );
		SetSceneLight();
	}
}

void CLightEditDlg::OnEnChangeBlueEdit()
{
	char szText[ 10 ] = "";
	m_BlueText.GetWindowText( szText, 10 );
	int iValue = ::atoi( szText );

	if( iValue < 0 || iValue > 255 )
	{
		AfxMessageBox( "超出0到255的泛围！", 0, 0 );
		return;
	}

	CWnd *pWnd = GetFocus();
	if( pWnd && *pWnd == m_BlueText )
	{
		m_iBlueText = iValue;
		m_crCurrent = RGB( m_iRedText, m_iGreenText, m_iBlueText );
		m_ColorSpectrum.SetRGB( m_crCurrent );
		SetSceneLight();
	}
}

LRESULT CLightEditDlg::OnSelChange( WPARAM wParam, LPARAM lParam )
{
	SetValues();
	SetSceneLight();

	if( IsWindow( g_pTerrainEditorDlg->m_TabCtrl.m_hWnd ) )
		g_pTerrainEditorDlg->m_TabCtrl.SendMessage( WM_COLORPICKER_SELCHANGE, wParam, lParam );

	return 0;
}
void CLightEditDlg::OnStnClickedColorStatic()
{
	// TODO: 在此添加控件通知处理程序代码
}
