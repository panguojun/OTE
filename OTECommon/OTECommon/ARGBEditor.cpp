#include "stdafx.h"
#include "Ogre.h"
#include "ARGBEditor.h"


// Globles

CRect CARGBEditor::sColorSelRect = CRect(250, 25, 330, 105);
Ogre::ColourValue*		CARGBEditor::spColour;
D3DXVECTOR4*			CARGBEditor::spColourD3DVec;

// CARGBEditor 对话框

IMPLEMENT_DYNAMIC(CARGBEditor, CDialog)
CARGBEditor::CARGBEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CARGBEditor::IDD, pParent)
{	
}

CARGBEditor::~CARGBEditor()
{
}

void CARGBEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CARGBEditor, CDialog)	
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()	
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnNMCustomdrawSlider1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnNMReleasedcaptureSlider1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER2, OnNMReleasedcaptureSlider2)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER3, OnNMReleasedcaptureSlider3)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER4, OnNMReleasedcaptureSlider4)
	ON_WM_ACTIVATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CARGBEditor 消息处理程序

// -----------------------------------------------
void CARGBEditor::OnPaint()
{
	CPaintDC dc(this); // device context for painting	
	CDialog::OnPaint();

	// after draw dialog	
	COLORREF drawColour = 0;
	if(spColour)
		drawColour = RGB((*spColour).r*255, (*spColour).g*255, (*spColour).b*255);
	if(spColourD3DVec)
		drawColour = RGB((*spColourD3DVec).x*255, (*spColourD3DVec).y*255, (*spColourD3DVec).z*255);

	dc.FillRect(sColorSelRect, &CBrush(drawColour));	

}

// -----------------------------------------------
void CARGBEditor::SetColourSliders()
{
	COLORREF drawColour = 0;
	int	alpha = 0;
	if(spColour)
	{
		drawColour = RGB((*spColour).r*255, (*spColour).g*255, (*spColour).b*255);
		alpha = (*spColour).a * 255;
	}
	if(spColourD3DVec)
	{
		drawColour = RGB((*spColourD3DVec).x*255, (*spColourD3DVec).y*255, (*spColourD3DVec).z*255);
		alpha = (*spColourD3DVec).w * 255;
	}
	
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER1))->SetPos(GetRValue(drawColour));
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER2))->SetPos(GetGValue(drawColour));
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER3))->SetPos(GetBValue(drawColour));
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER4))->SetPos(alpha);
}

// -----------------------------------------------
void CARGBEditor::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDown(nFlags, point);

	if(sColorSelRect.PtInRect(point))
	{
		CColorDialog dlg;		
		if(dlg.DoModal() == IDOK)
		{	
			if(spColour)
			{
				(*spColour).r = GetRValue(dlg.GetColor()) / 255.0f;
				(*spColour).g = GetGValue(dlg.GetColor()) / 255.0f;
				(*spColour).b = GetBValue(dlg.GetColor()) / 255.0f;
			}
			if(spColourD3DVec)
			{
				(*spColourD3DVec).x = GetRValue(dlg.GetColor()) / 255.0f;
				(*spColourD3DVec).y = GetGValue(dlg.GetColor()) / 255.0f;
				(*spColourD3DVec).z = GetBValue(dlg.GetColor()) / 255.0f;
			}

			SetColourSliders();
			this->InvalidateRect(NULL);
		}
	}
	
}

void CARGBEditor::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
}
// -----------------------------------------------
BOOL CARGBEditor::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();

	// colour range

	((CSliderCtrl*)GetDlgItem(IDC_SLIDER1))->SetRange(0, 255);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER2))->SetRange(0, 255);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER3))->SetRange(0, 255);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER4))->SetRange(0, 255);



	return tRet;
}

// -----------------------------------------------
// red
void CARGBEditor::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	int pos = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER1))->GetPos();
	if(spColour)
		(*spColour).r = pos / 255.0;
	if(spColourD3DVec)
		(*spColourD3DVec).x = pos / 255.0;

	this->InvalidateRect(NULL);
}
// -----------------------------------------------
// green
void CARGBEditor::OnNMReleasedcaptureSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	int pos = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER2))->GetPos();
	if(spColour)
		(*spColour).g = pos / 255.0;
	if(spColourD3DVec)
		(*spColourD3DVec).y = pos / 255.0;

	this->InvalidateRect(NULL);
}
// -----------------------------------------------
// blue
void CARGBEditor::OnNMReleasedcaptureSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{
	int pos = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER3))->GetPos();
	if(spColour)
		(*spColour).b = pos / 255.0;
	if(spColourD3DVec)
		(*spColourD3DVec).z = pos / 255.0;
	this->InvalidateRect(NULL);
}

// -----------------------------------------------
// aplha
void CARGBEditor::OnNMReleasedcaptureSlider4(NMHDR *pNMHDR, LRESULT *pResult)
{
	int pos = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER4))->GetPos();
	if(spColour)
		(*spColour).a = pos / 255.0;
	if(spColourD3DVec)
		(*spColourD3DVec).w = pos / 255.0;
	this->InvalidateRect(NULL);
}

void CARGBEditor::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	if(nState == WA_ACTIVE)
		SetColourSliders();
}

void CARGBEditor::OnDestroy()
{
	CDialog::OnDestroy();
}
