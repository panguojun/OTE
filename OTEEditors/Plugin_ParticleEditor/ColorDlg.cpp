// ColorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ColorDlg.h"
#include "Resource.h"
#include "ParticleEditDlg.h"

// --------------------------------------------
CColorDlg	CColorDlg::s_Inst;
// --------------------------------------------
IMPLEMENT_DYNAMIC(CColorDlg, CDialog)
CColorDlg::CColorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CColorDlg::IDD, pParent)
{
	
}

CColorDlg::~CColorDlg()
{
}

void CColorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RED_EDIT, m_RedText);
	DDX_Control(pDX, IDC_GREEN_EDIT, m_GreenText);
	DDX_Control(pDX, IDC_BLUE_EDIT, m_BlueText);
}


BEGIN_MESSAGE_MAP(CColorDlg, CDialog)
	ON_STN_CLICKED(IDC_COLOR_STATIC, OnStnClickedColorStatic)
	ON_REGISTERED_MESSAGE( WM_COLORPICKER_SELCHANGE, OnSelChange )
	ON_WM_PAINT()	
	ON_WM_LBUTTONDOWN()
	ON_EN_CHANGE(IDC_RED_EDIT, OnEnChangeRedEdit)
	ON_EN_CHANGE(IDC_GREEN_EDIT, OnEnChangeGreenEdit)
	ON_EN_CHANGE(IDC_BLUE_EDIT, OnEnChangeBlueEdit)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
END_MESSAGE_MAP()

// 初始化

BOOL CColorDlg::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();

	CRect rect;
	GetDlgItem( IDC_COLOR_STATIC )->GetWindowRect( &rect );
	ScreenToClient( &rect );
	GetDlgItem( IDC_COLOR_STATIC )->ShowWindow( SW_HIDE );

	m_ColorPicker.Create( AfxGetInstanceHandle(), WS_CHILD | WS_VISIBLE | WS_TABSTOP, rect, m_hWnd, 9000, RGB( 255, 0, 0 ) );
	::SetWindowPos( m_ColorPicker.m_hWnd, ::GetDlgItem( m_hWnd, IDC_COLOR_STATIC ), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

	
	return ret;

}
void CColorDlg::OnStnClickedColorStatic()
{
}

void CColorDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting	

}

void CColorDlg::Show(bool vis, unsigned int color)
{
	if(!m_hWnd)
	{
		Create(IDD_COLORDLG);
	}

	if(vis)
	{		
		ShowWindow(SW_SHOW);

		m_crCurrent = color;
		m_crOldColor = m_crCurrent;			

		m_iRedText = GetRValue( m_crCurrent );
		m_iGreenText = GetGValue( m_crCurrent );
		m_iBlueText = GetBValue( m_crCurrent );

		m_ColorPicker.SetRGB( m_crCurrent, false );

		CString s;
		s.Format( "%d", m_iRedText );
		m_RedText.SetWindowText( s );
		s.Format( "%d", m_iGreenText );
		m_GreenText.SetWindowText( s );
		s.Format( "%d", m_iBlueText );
		m_BlueText.SetWindowText( s );			
	}
	else
	{
		ShowWindow(SW_HIDE);
	}
}

void CColorDlg::OnStnClickedColorBarStatic()
{
}

void CColorDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnLButtonDown(nFlags, point);
}

// ----------------------------------------------------
void CColorDlg::OnEnChangeRedEdit()
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
		m_ColorPicker.SetRGB( m_crCurrent );
		// SetSceneLight();
	}
}

// ----------------------------------------------------
void CColorDlg::OnEnChangeGreenEdit()
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
		m_ColorPicker.SetRGB( m_crCurrent );
		//SetSceneLight();
	}
}

// ----------------------------------------------------
void CColorDlg::OnEnChangeBlueEdit()
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
		m_ColorPicker.SetRGB( m_crCurrent );
	//	SetSceneLight();
	}
}
// ----------------------------------------------------
LRESULT CColorDlg::OnSelChange( WPARAM wParam, LPARAM lParam )
{
	m_crCurrent = m_ColorPicker.GetRGB();
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

	return 0;
}

// ----------------------------------------------------
// 确定
void CColorDlg::OnBnClickedButton1()
{
	this->Show(false);
}

// ----------------------------------------------------
// 取消
void CColorDlg::OnBnClickedButton2()
{
	m_ColorPicker.SetRGB(m_crOldColor);
	if(m_ColorPicker.m_pNotifyFuncHdl)
	{
		(*m_ColorPicker.m_pNotifyFuncHdl)(m_crOldColor, 0);
	}

	this->Show(false);
}

// ----------------------------------------------------
// 指定到所有桢
void CColorDlg::OnBnClickedButton3()
{	
	if(m_ColorPicker.m_pNotifyFuncHdl)
	{
		(*m_ColorPicker.m_pNotifyFuncHdl)(m_ColorPicker.GetRGB(), 1);
	}

	this->Show(false);
}
