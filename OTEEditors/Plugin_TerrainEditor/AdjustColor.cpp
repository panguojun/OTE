// AdjustColor.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "Plugin_TerrainEditor.h"
#include "AdjustColor.h"

// CAdjustColor �Ի���
CAdjustColor CAdjustColor::m_Inst;//ʵ������̬����

IMPLEMENT_DYNAMIC(CAdjustColor, CDialog)
CAdjustColor::CAdjustColor(CWnd* pParent /*=NULL*/)
	: CDialog(CAdjustColor::IDD, pParent)
{
	
}

CAdjustColor::~CAdjustColor()
{
}

void CAdjustColor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_silderRed, m_silderRed);
	DDX_Control(pDX, IDC_silderGreen, m_silderGreen);
	DDX_Control(pDX, IDC_silderBlue, m_silderBlue);
	DDX_Control(pDX, IDC_silderAlpha, m_silderAlpha);
}

BOOL CAdjustColor::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();
	
	
	
	return tRet;
}

void CAdjustColor::silderBindColor()
{
	m_silderRed.SetRange(0, 100);
	m_silderRed.SetTicFreq(1);
	m_silderRed.SetPos(int(m_aryColor[0]*100));

	m_silderGreen.SetRange(0, 100);
	m_silderGreen.SetTicFreq(1);
	m_silderGreen.SetPos(int(m_aryColor[1]*100));

	m_silderBlue.SetRange(0, 100);
	m_silderBlue.SetTicFreq(1);
	m_silderBlue.SetPos(int(m_aryColor[2]*100));

	m_silderAlpha.SetRange(0, 100);
	m_silderAlpha.SetTicFreq(1);
	m_silderAlpha.SetPos(int(m_aryColor[3]*100));
}


BEGIN_MESSAGE_MAP(CAdjustColor, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(ID_CANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDCANCEL, OnClickedCancel)	//��ر�ʱִ��
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_silderRed, OnNMReleasedcapturesilderred)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_silderGreen, OnNMReleasedcapturesildergreen)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_silderBlue, OnNMReleasedcapturesilderblue)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_silderAlpha, OnNMReleasedcapturesilderalpha)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_silderRed, OnNMReleasedcapturesilderred)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_silderGreen, OnNMReleasedcapturesildergreen)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_silderBlue, OnNMReleasedcapturesilderblue)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_silderAlpha, OnNMReleasedcapturesilderalpha)
END_MESSAGE_MAP()


// CAdjustColor ��Ϣ�������

void CAdjustColor::OnBnClickedOk()
{
	this->DestroyWindow();
}

//ȡ���������������õ�ֵ���������ļ�
void CAdjustColor::OnBnClickedCancel()
{
	//char a[100];
	//for(int i=0; i<4; i++)
	//{
	//	sprintf(a, "%f", m_aryColor[i]);
	//	CLightInfoEdit::s_Inst.m_LightColorList.SetItemText(m_operateObject, i+1, LPCTSTR(a));
	//	
	//	//�����б�ֵ
	//	int type = CLightInfoEdit::s_Inst.getAlterObject(m_operateObject, i+1);
	//	CString time;
	//	CLightInfoEdit::s_Inst.GetDlgItemText(IDC_lblTime, time);
	//	CLightInfoEdit::s_Inst.updateLightInfoList(time, type, a);
	//}
	////CLightInfoEdit::s_Inst.writeConfigFile();

	//CLightInfoEdit::s_Inst.UpdateSunInfo();
	
	this->DestroyWindow();
}

void CAdjustColor::updateLightColorList(enumColor c, float value)
{
	//char a[100];
	//sprintf(a, "%f", value);
	//CLightInfoEdit::s_Inst.m_LightColorList.SetItemText(m_operateObject, c, LPCTSTR(a));
	////CLightInfoEdit::s_Inst.writeConfigFile();
	//
	////�����б�ֵ
	//int type = CLightInfoEdit::s_Inst.getAlterObject(m_operateObject, c);
	//CString time;
	//CLightInfoEdit::s_Inst.GetDlgItemText(IDC_lblTime, time);
	//CLightInfoEdit::s_Inst.updateLightInfoList(time, type, a);

	//CLightInfoEdit::s_Inst.UpdateSunInfo();
}

void CAdjustColor::OnClickedCancel()
{
	this->DestroyWindow();	//���ٴ���
}
void CAdjustColor::OnNMReleasedcapturesilderred(NMHDR *pNMHDR, LRESULT *pResult)
{
	float value;
	value = (float)m_silderRed.GetPos()/100.0;
	updateLightColorList(red, value);
}

void CAdjustColor::OnNMReleasedcapturesildergreen(NMHDR *pNMHDR, LRESULT *pResult)
{
	float value;
	value = (float)m_silderGreen.GetPos()/100.0;
	updateLightColorList(green, value);
}

void CAdjustColor::OnNMReleasedcapturesilderblue(NMHDR *pNMHDR, LRESULT *pResult)
{
	float value;
	value = (float)m_silderBlue.GetPos()/100.0;
	updateLightColorList(blue, value);
}

void CAdjustColor::OnNMReleasedcapturesilderalpha(NMHDR *pNMHDR, LRESULT *pResult)
{
	float value;
	value = (float)m_silderAlpha.GetPos()/100.0;
	updateLightColorList(alpha, value);
}
