// LightEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_MaterialEditor.h"
#include "MaterialEditorMainDlg.h"
#include "LightEdit.h"

CLightEdit CLightEdit::s_Inst;
// CLightEdit 对话框

IMPLEMENT_DYNAMIC(CLightEdit, CDialog)
CLightEdit::CLightEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CLightEdit::IDD, pParent)
{
	m_Y = 0;
	m_cB = 0;
	m_cR = 0;
}

CLightEdit::~CLightEdit()
{
}

void CLightEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ColorList, mColorList);
	DDX_Control(pDX, IDC_SLIDER1, m_SilderBrightness);
}

//清除窗体显示
void CLightEdit::ClearAll()
{
	mColorList.DeleteAllItems();

	m_SilderBrightness.SetPos(0);
	m_SilderBrightness.EnableWindow(FALSE);

	mColorList.InsertItem(0, "Ambient");
	mColorList.InsertItem(1, "Diffuse");
	mColorList.InsertItem(2, "Specular");
	mColorList.InsertItem(3, "Emissive");
	
	mColorList.Invalidate();
}

//更新窗体数据
void CLightEdit::UpdataAll()
{
	ClearAll();

	if(CMaterialEditorMainDlg::s_Inst.m_pMat != NULL)
	{
		m_SilderBrightness.EnableWindow(TRUE);

		Ogre::ColourValue c = CMaterialEditorMainDlg::s_Inst.m_pMat->getTechnique(0)->getPass(0)->getAmbient();
		Display(c, AMBIENT);

		c = CMaterialEditorMainDlg::s_Inst.m_pMat->getTechnique(0)->getPass(0)->getDiffuse();
		Display(c, DIFFUSE);

		c = CMaterialEditorMainDlg::s_Inst.m_pMat->getTechnique(0)->getPass(0)->getSpecular();
		Display(c, SPECULAR);

		c = CMaterialEditorMainDlg::s_Inst.m_pMat->getTechnique(0)->getPass(0)->getSelfIllumination();
		Display(c, EMISSIVE);
	}

	m_SilderBrightness.Invalidate();
	mColorList.Invalidate();
}

//显示对应的颜色
void CLightEdit::Display(Ogre::ColourValue &cv, _ColorType ct)
{
	int i = 0;
	switch(ct)
	{
	default:
	case NONE:
		Display(cv, AMBIENT);
		Display(cv, DIFFUSE);
		Display(cv, SPECULAR);
		Display(cv, EMISSIVE);
		break;
	case AMBIENT:
		i = 0;
		break;
	case DIFFUSE:
		i = 1;
		break;
	case SPECULAR:
		i = 2;
		break;
	case EMISSIVE:
		i = 3;
		break;
	}

	char a[32];
	int l = 0;
	l = cv.r * 255;
	sprintf(a, "%d", l);
	mColorList.SetItemText(i, 1, a);
	l = cv.g * 255;
	sprintf(a, "%d", l);
	mColorList.SetItemText(i, 2, a);
	l = cv.b * 255;
	sprintf(a, "%d", l);
	mColorList.SetItemText(i, 3, a);
	l = cv.a * 255;
	sprintf(a, "%d", l);
	mColorList.SetItemText(i, 4, a);
}


BEGIN_MESSAGE_MAP(CLightEdit, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_ColorList, OnNMDblclkColorlist)
	ON_NOTIFY(NM_CLICK, IDC_ColorList, OnNMClickColorlist)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CLightEdit 消息处理程序
//设置灯光信息
void CLightEdit::OnNMDblclkColorlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(!CMaterialEditorMainDlg::s_Inst.m_pMat)
		return;

	int tItem, tSubItem;
	CRect tSubItemRect;
	if(mColorList.GetDClickedItem(tItem, tSubItem, tSubItemRect) && CMaterialEditorMainDlg::s_pObj != NULL)
	{
		if(tItem > mColorList.GetItemCount() || tItem < 0)
			return;

		DWORD c = 0;
			
		unsigned char a = (unsigned char)(atoi(mColorList.GetItemText(tItem, 4)));
		unsigned char b = (unsigned char)(atoi(mColorList.GetItemText(tItem, 3)));
		unsigned char g = (unsigned char)(atoi(mColorList.GetItemText(tItem, 2)));
		unsigned char r = (unsigned char)(atoi(mColorList.GetItemText(tItem, 1)));
		c= (b<<16)|(g<<8)|r;
		
		CColorDialog color(c);
		switch(color.DoModal())
		{
		case IDCANCEL:
			break;
		case IDOK:
			c = color.GetColor();
			b = c>>16;
			c = color.GetColor();
			c = c<<16;
			g = c>>24;
			c = color.GetColor();
			c = c<<24;
			r = c>>24;
			break;
		}
		//设置材质颜色
		Ogre::ColourValue cv = Ogre::ColourValue::White;
		cv.r = r / 255.0f;
		cv.g = g / 255.0f;
		cv.b = b / 255.0f;
		cv.a = a / 255.0f;

		char sa[32];
		sprintf(sa, "%d", r);
		mColorList.SetItemText(tItem, 1, sa);
		sprintf(sa, "%d", g);
		mColorList.SetItemText(tItem, 2, sa);
		sprintf(sa, "%d", b);
		mColorList.SetItemText(tItem, 3, sa);
		sprintf(sa, "%d", a);
		mColorList.SetItemText(tItem, 4, sa);

		switch(tItem)
		{
		case 0:
			CMaterialEditorMainDlg::s_Inst.m_pMat->getTechnique(0)->getPass(0)->setAmbient(cv);
			break;
		case 1:
			CMaterialEditorMainDlg::s_Inst.m_pMat->getTechnique(0)->getPass(0)->setDiffuse(cv);
			break;
		case 2:
			CMaterialEditorMainDlg::s_Inst.m_pMat->getTechnique(0)->getPass(0)->setSpecular(cv);
			break;
		case 3:
			CMaterialEditorMainDlg::s_Inst.m_pMat->getTechnique(0)->getPass(0)->setSelfIllumination(cv);
			break;
		}

		RGBtoYCBCR(r, g, b);

		m_SilderBrightness.SetPos(m_Y);
	}
	m_SilderBrightness.Invalidate();
	mColorList.Invalidate();
	*pResult = 0;
}

//RGB转YCBCR
void CLightEdit::RGBtoYCBCR(int r, int g, int b)
{
	m_Y	 =   0.299*r	+   0.587*g		+   0.114*b; //    (亮度)   
	m_cB   =   -0.1687*r	-   0.3313*g	+   0.5*b		+   128;
	m_cR   =   0.5*r		-   0.4187*g	-   0.0813*b	+   128;
}

//YCBCR转RGB
void CLightEdit::YCBCRtoRGB(int &r, int &g, int &b)
{
	r   =   m_Y + (int)(1.402 * (m_cR - 128)) ;  
	if(r> 255)
		r = 255;
	else if(r<0)
		r = 0;

	g   =   m_Y - (int)(0.34414 * (m_cB - 128)) - (int)(0.71414 * (m_cR - 128))  ; 
	if(g> 255)
		g = 255;
	else if(g<0)
		g = 0;

	b   =   m_Y + (int)(1.772 * (m_cB - 128))  ;
	if(b> 255)
		b = 255;
	else if(b<0)
		b = 0;
}

//点选一个灯光
void CLightEdit::OnNMClickColorlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	int tItem, tSubItem;
	CRect tSubItemRect;
	if(mColorList.GetDClickedItem(tItem, tSubItem, tSubItemRect) && CMaterialEditorMainDlg::s_pObj != NULL)
	{
		if(tItem > mColorList.GetItemCount() || tItem < 0)
			return;

		int a = atoi(mColorList.GetItemText(tItem, 4));
		int b = atoi(mColorList.GetItemText(tItem, 3));
		int g = atoi(mColorList.GetItemText(tItem, 2));
		int r = atoi(mColorList.GetItemText(tItem, 1));
		
		RGBtoYCBCR(r, g, b);

		m_SilderBrightness.SetPos(m_Y);
	}
	*pResult = 0;
}
//改变颜色亮度
void CLightEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	m_Y = m_SilderBrightness.GetPos();

	int a = 255, r, g, b;

	YCBCRtoRGB(r, g, b);

	int tItem = mColorList.GetLastSelect();
	if(tItem > mColorList.GetItemCount() || tItem < 0)
			return;


	char sa[32];
	sprintf(sa, "%d", r);
	mColorList.SetItemText(tItem, 1, sa);
	sprintf(sa, "%d", g);
	mColorList.SetItemText(tItem, 2, sa);
	sprintf(sa, "%d", b);
	mColorList.SetItemText(tItem, 3, sa);
	sprintf(sa, "%d", a);
	mColorList.SetItemText(tItem, 4, sa);

	//设置材质颜色
	Ogre::ColourValue cv = Ogre::ColourValue::White;
	cv.r = r / 255.0f;
	cv.g = g / 255.0f;
	cv.b = b / 255.0f;
	cv.a = a / 255.0f;

	switch(tItem)
	{
	case 0:
		CMaterialEditorMainDlg::s_Inst.m_pMat->getTechnique(0)->getPass(0)->setAmbient(cv);
		break;
	case 1:
		CMaterialEditorMainDlg::s_Inst.m_pMat->getTechnique(0)->getPass(0)->setDiffuse(cv);
		break;
	case 2:
		CMaterialEditorMainDlg::s_Inst.m_pMat->getTechnique(0)->getPass(0)->setSpecular(cv);
		break;
	case 3:
		CMaterialEditorMainDlg::s_Inst.m_pMat->getTechnique(0)->getPass(0)->setSelfIllumination(cv);
		break;
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
