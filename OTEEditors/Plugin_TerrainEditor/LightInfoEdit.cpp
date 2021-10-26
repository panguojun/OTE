// LightInfoEdit.cpp : ʵ���ļ�
#include "stdafx.h"
#include "Plugin_TerrinEditor.h"
#include "OTEQTSceneManager.h"
#include "LightInfoEdit.h"
#include "ListViewEditors.h"
#include "OTETerrainLightmapMgr.h"
#include ".\lightinfoedit.h"

// ------------------------------------------------
using namespace Ogre;
using namespace OTE;
// ------------------------------------------------

extern OTE::COTEQTSceneManager*		gSceneMgr;			// ����������

// ------------------------------------------------
// CLightInfoEdit �Ի���
CLightInfoEdit CLightInfoEdit::s_Inst;//ʵ������̬����

IMPLEMENT_DYNAMIC(CLightInfoEdit, CDialog)
CLightInfoEdit::CLightInfoEdit(CWnd* pParent /*=NULL*/)
 : CDialog(CLightInfoEdit::IDD, pParent)
{
}

CLightInfoEdit::~CLightInfoEdit()
{
	
}

// ------------------------------------------------
void CLightInfoEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//	DDX_Control(pDX, txtAngleToSun, txtAngeToSun);
	DDX_Control(pDX, IDC_LIST1, m_SunTimeNodeList);
	DDX_Control(pDX, IDC_LIST2, m_LightColorList);
	DDX_Control(pDX, IDC_RADIO256, m_radLightMap256);
	DDX_Control(pDX, IDC_SliderBrightness, m_SilderBrightness);
	DDX_Control(pDX, IDC_StatBrightness, m_lblBrightness);
}

// ------------------------------------------------
BOOL CLightInfoEdit::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();

	m_SunTimeNodeList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_MULTIWORKAREAS);
	m_SunTimeNodeList.InsertColumn(0, "����", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_CENTER, 100); 
	m_SunTimeNodeList.InsertColumn(1, "ʱ���", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_CENTER, 80); 

	m_LightColorList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_MULTIWORKAREAS);//LVS_EX_GRIDLINES|
	m_LightColorList.InsertColumn(0, "", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_CENTER, 50); 
	m_LightColorList.InsertColumn(1, "R", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_CENTER, 70); 
	m_LightColorList.InsertColumn(2, "G", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_CENTER, 70); 
	m_LightColorList.InsertColumn(3, "B", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_CENTER, 70);
	m_LightColorList.InsertColumn(4, "A", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCFMT_CENTER, 70); 

	initSilder();	//��ʼ�����ȵ��ڿؼ�

	return tRet;
}


// ------------------------------------------------
void CLightInfoEdit::OnKeyDown(unsigned int keyCode, bool isShifDwon)
{
	//switch(keyCode)
	//{
	//case 'F': // [ F ] ��
	//	//s_FogState = !s_FogState;
	//	//OTE::CRenderLight::GetInstance()->SetFogState(s_FogState);
	//	break;
	//case 'Y':
	//	if(isShifDwon)
	//	{
	//		//OTE::CRenderLight::GetInstance()->fogStart--;
	//		//OTE::CRenderLight::GetInstance()->SetFogState(s_FogState);
	//		char a[20];
	//		sprintf(a, "%d", OTE::CRenderLight::GetInstance()->fogStart);
	//		m_LightColorList.SetItemText(6, 1, a);	
	//		CString strOldTime;
	//		GetDlgItemText(IDC_lblTime, strOldTime);
	//		updateLightInfoList(strOldTime, WJ_Start, a);//�ܸ��µ�һ��ʱ�������Ϳ���
	//	}
	//	break;
	//case 'U':
	//	if(isShifDwon)
	//	{
	//		//OTE::CRenderLight::GetInstance()->fogStart++;
	//		//OTE::CRenderLight::GetInstance()->SetFogState(s_FogState);
	//		char a[20];
	//		sprintf(a, "%d", OTE::CRenderLight::GetInstance()->fogStart);
	//		m_LightColorList.SetItemText(6, 1, a);	
	//		CString strOldTime;
	//		GetDlgItemText(IDC_lblTime, strOldTime);
	//		updateLightInfoList(strOldTime, WJ_Start, a);//�ܸ��µ�һ��ʱ�������Ϳ���
	//	}
	//	break;
	//case 'I':
	//	if(isShifDwon)
	//	{
	//		//OTE::CRenderLight::GetInstance()->fogEnd--;
	//		//OTE::CRenderLight::GetInstance()->SetFogState(s_FogState);
	//		char a[20];
	//		sprintf(a, "%d", OTE::CRenderLight::GetInstance()->fogEnd);
	//		m_LightColorList.SetItemText(6, 2, a);	
	//		CString strOldTime;
	//		GetDlgItemText(IDC_lblTime, strOldTime);
	//		updateLightInfoList(strOldTime, WJ_End,  a);//�ܸ��µ�һ��ʱ�������Ϳ���
	//	}
	//	break;
	//case 'O':
	//	if(isShifDwon)
	//	{
	//		//OTE::CRenderLight::GetInstance()->fogEnd++;
	//		//OTE::CRenderLight::GetInstance()->SetFogState(s_FogState);
	//		char a[20];
	//		sprintf(a, "%d", OTE::CRenderLight::GetInstance()->fogEnd);
	//		m_LightColorList.SetItemText(6, 2, a);	
	//		CString strOldTime;
	//		GetDlgItemText(IDC_lblTime, strOldTime);
	//		updateLightInfoList(strOldTime, WJ_End,  a);//�ܸ��µ�һ��ʱ�������Ϳ���
	//	}
	//	break;
	//}
}


// ------------------------------------------------
void CLightInfoEdit::SunInfoAddAllItem()
{		
	SetLightCorrelationObject(0);
}

// ------------------------------------------------
// ��ʼ���б�
void CLightInfoEdit::InitLightColorList()
{
	if(m_LightColorList.GetItemCount() == 0)
	{
		m_LightColorList.InsertItem(0, "������");
		m_LightColorList.SetItemText(0, 1,"0.5");
		m_LightColorList.SetItemText(0, 2,"0.5");
		m_LightColorList.SetItemText(0, 3,"0.5");
		m_LightColorList.SetItemText(0, 4,"1.0");

		m_LightColorList.InsertItem(1, "����");
		m_LightColorList.SetItemText(1, 1,"0.7");
		m_LightColorList.SetItemText(1, 2,"0.5");
		m_LightColorList.SetItemText(1, 3,"0.2");
		m_LightColorList.SetItemText(1, 4,"1.0");
        
		m_LightColorList.InsertItem(2, "����");
		m_LightColorList.SetItemText(2, 1,"0.3");
		m_LightColorList.SetItemText(2, 2,"0.4");
		m_LightColorList.SetItemText(2, 3,"0.7");
		m_LightColorList.SetItemText(2, 4,"1.0");

		m_LightColorList.InsertItem(3, "���λ�����");
		m_LightColorList.SetItemText(3, 1,"0.5");
		m_LightColorList.SetItemText(3, 2,"0.5");
		m_LightColorList.SetItemText(3, 3,"0.5");
		m_LightColorList.SetItemText(3, 4,"1.0");

		m_LightColorList.InsertItem(4, "��������");
		m_LightColorList.SetItemText(4, 1,"0.7");
		m_LightColorList.SetItemText(4, 2,"0.5");
		m_LightColorList.SetItemText(4, 3,"0.2");
		m_LightColorList.SetItemText(4, 4,"1.0");

		m_LightColorList.InsertItem(5, "����ɫ");
		m_LightColorList.SetItemText(5, 1,"0.7");
		m_LightColorList.SetItemText(5, 2,"0.5");
		m_LightColorList.SetItemText(5, 3,"0.2");
		m_LightColorList.SetItemText(5, 4,"1.0");

		m_LightColorList.InsertItem(6, "���");
		m_LightColorList.SetItemText(6, 1,"20");
		m_LightColorList.SetItemText(6, 2,"40");
	}
}


// ------------------------------------------------
void CLightInfoEdit::UpdateSunInfo(CString strTime)	//int iTime
{
	float rc = atof(m_LightColorList.GetItemText(0, 1));
	float gc = atof(m_LightColorList.GetItemText(0, 2));
	float bc = atof(m_LightColorList.GetItemText(0, 3));
	float ac = atof(m_LightColorList.GetItemText(0, 4));

	gSceneMgr->setAmbientLight(Ogre::ColourValue(rc, gc, bc, ac));
	
	
	Vector3 dir;
	Light* l = gSceneMgr->getLight("MainLight");
	int iTime = atoi(strTime.GetString());
	float delta=(15.0f* iTime -90)*Ogre::Math::PI/180.0f;
	if(delta == 0)
		delta = 0.5;
	else if(delta > 3.0)
		delta = 2.5;

	dir= Vector3(-cos(delta),-sin(delta),0.9);//*800

	l->setDirection(dir);
	rc = atof(m_LightColorList.GetItemText(1, 1));
	gc = atof(m_LightColorList.GetItemText(1, 2));
	bc = atof(m_LightColorList.GetItemText(1, 3));
	ac = atof(m_LightColorList.GetItemText(1, 4));
	l->setDiffuseColour(Ogre::ColourValue(rc, gc, bc, ac));

	Light* l2 = gSceneMgr->getLight("Light2");
	l2->setDirection(-dir);
	rc = atof(m_LightColorList.GetItemText(2, 1));
	gc = atof(m_LightColorList.GetItemText(2, 2));
	bc = atof(m_LightColorList.GetItemText(2, 3));
	ac = atof(m_LightColorList.GetItemText(2, 4));
	l2->setDiffuseColour(Ogre::ColourValue(rc, gc, bc, ac));
}
// ------------------------------------------------
void CLightInfoEdit::UpdateSunInfo()
{
	CString time;
	GetDlgItemText(IDC_lblTime, time);
	
	OTE::CRenderLight::GetInstance()->SetLightColorState(m_strCurrentHeightMapFileName.c_str(), time.GetString());
	if(m_lightType == 5)	//ִ����ɫ����
	{
		//s_FogState = true;
		//OTE::CRenderLight::GetInstance()->SetFogState(true);
	}
}


// ------------------------------------------------
BEGIN_MESSAGE_MAP(CLightInfoEdit, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclkList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnNMClickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST2, OnNMDblclkList2)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST1, OnLvnKeydownList1)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_RADIO512, OnBnClickedRadio512)
	ON_BN_CLICKED(IDC_RADIO256, OnBnClickedRadio256)
	ON_BN_CLICKED(IDC_BtnSaveLight, OnBnClickedBtnsavelight)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SliderBrightness, OnNMCustomdrawSliderbrightness)
	ON_NOTIFY(NM_CLICK, IDC_LIST2, OnNMClickList2)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, OnLvnItemchangedList2)
END_MESSAGE_MAP()

// ------------------------------------------------
#define DO_EDITOK(type)  CListViewEditors dlg(type, m_SunTimeNodeList.GetItemText(tItem, tSubItem), tSubItemRect); if(IDOK == dlg.DoModal() && !CListViewEditors::s_Text.IsEmpty())

void CLightInfoEdit::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	int tItem, tSubItem;
	CRect tSubItemRect;

	if(m_SunTimeNodeList.GetDClickedItem(tItem, tSubItem, tSubItemRect))
	{
		if(tSubItem == 0)	//���ܶԵ�һ��Ԫ��༭
			return;

		DO_EDITOK(1)
		{
			int action = 0;
			if(tItem == m_SunTimeNodeList.GetItemCount()-1)
			{
				m_SunTimeNodeList.SetItemText(tItem, tSubItem, CListViewEditors::s_Text);
				if(tSubItem == 0)
					m_SunTimeNodeList.SetItemText(tItem, tSubItem + 1, "6");
				else
					m_SunTimeNodeList.SetItemText(tItem, tSubItem - 1, "�˴��õ��θ߶�ͼ�ļ���");
				m_SunTimeNodeList.InsertItem(m_SunTimeNodeList.GetItemCount(), "");
				m_SunTimeNodeList.SetItemText(m_SunTimeNodeList.GetItemCount()-1, 1,"");

				action = 2;	//����
			}
			else
			{
				m_SunTimeNodeList.SetItemText(tItem, tSubItem, CListViewEditors::s_Text);

				action = 1;	//����
			}


			updateLightInfoList(tItem, action);
			this->SetDlgItemText(IDC_lblTime, vLightinfoList[tItem].time.c_str());	//����lblΪ��ǰ��ʾʱ��
			if(action == 2)	//�ǲ��������ֹ�������
			{
				setLightInfo(tItem);
			}
			CString strTime;
			if(tSubItem == 1)
				strTime = (LPCTSTR)CListViewEditors::s_Text;
			else
				strTime = m_SunTimeNodeList.GetItemText(tItem, 1);
			
			changeSunTimeThenChangeLightColor();	//���µƹ��б���ɫ

			OTE::CRenderLight::GetInstance()->SetLightColorState(m_strCurrentHeightMapFileName.c_str(), strTime.GetString());
		}
	}
}



// ------------------------------------------------
void CLightInfoEdit::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	int tItem, tSubItem;
	CRect tSubItemRect;

	m_SunTimeNodeList.GetDClickedItem(tItem, tSubItem, tSubItemRect);	//�˴�����һ�����ΰ������ֹ���ʱ
	if(tItem < m_SunTimeNodeList.GetItemCount() -1 && 0 <= tItem)
	{
		SetLightCorrelationObject(tItem);
	}
}
// ------------------------------------------------
void CLightInfoEdit::SetLightCorrelationObject(int index)
{
	CString strOldTime;
	GetDlgItemText(IDC_lblTime, strOldTime);

	CString str = m_SunTimeNodeList.GetItemText(index, 1);
	setLightInfo(index);

	changeSunTimeThenChangeLightColor();

	OTE::CRenderLight::GetInstance()->SetLightColorState(m_strCurrentHeightMapFileName.c_str(), str.GetString());
	this->SetDlgItemText(IDC_lblTime, str.GetString());

	//��ʾ����ͼ
	CString time;
	CLightInfoEdit::s_Inst.GetDlgItemText(IDC_lblTime, time);
	
	//��ʾ����ͼend
}
// ------------------------------------------------
void CLightInfoEdit::setLightInfo(int i)
{
	
	//if(0 == m_LightColorList.GetItemCount())
	//{
	//	m_LightColorList.InsertItem(0, vLightinfoList[i].h_lightName.c_str());
	//	m_LightColorList.InsertItem(1, vLightinfoList[i].z_lightName.c_str());
	//	m_LightColorList.InsertItem(2, vLightinfoList[i].b_lightName.c_str());

	//	m_LightColorList.InsertItem(3, vLightinfoList[i].Th_lightName.c_str());
	//	m_LightColorList.InsertItem(4, vLightinfoList[i].Tz_lightName.c_str());

	//	m_LightColorList.InsertItem(5, vLightinfoList[i].W_lightName.c_str());

	//	m_LightColorList.InsertItem(6, vLightinfoList[i].WJ_Name.c_str());

	//	m_LightColorList.SetColumnWidth(0, 80);
	//}
	////������
	//m_LightColorList.SetItemText(0, 1,vLightinfoList[i].h_red.c_str());
	//m_LightColorList.SetItemText(0, 2,vLightinfoList[i].h_green.c_str());		
	//m_LightColorList.SetItemText(0, 3,vLightinfoList[i].h_blue.c_str());
	//m_LightColorList.SetItemText(0, 4,vLightinfoList[i].h_alpha.c_str());	
	//

	////����		
	//m_LightColorList.SetItemText(1, 1,vLightinfoList[i].z_red.c_str());		
	//m_LightColorList.SetItemText(1, 2,vLightinfoList[i].z_green.c_str());	
	//m_LightColorList.SetItemText(1, 3,vLightinfoList[i].z_blue.c_str());
	//m_LightColorList.SetItemText(1, 4,vLightinfoList[i].z_alpha.c_str());
	//
	////����
	//m_LightColorList.SetItemText(2, 1,vLightinfoList[i].b_red.c_str());
	//m_LightColorList.SetItemText(2, 2,vLightinfoList[i].b_green.c_str());
	//m_LightColorList.SetItemText(2, 3,vLightinfoList[i].b_blue.c_str());
	//m_LightColorList.SetItemText(2, 4,vLightinfoList[i].b_alpha.c_str());

	////���λ�����
	//m_LightColorList.SetItemText(3, 1,vLightinfoList[i].Th_red.c_str());
	//m_LightColorList.SetItemText(3, 2,vLightinfoList[i].Th_green.c_str());		
	//m_LightColorList.SetItemText(3, 3,vLightinfoList[i].Th_blue.c_str());
	//m_LightColorList.SetItemText(3, 4,vLightinfoList[i].Th_alpha.c_str());	
	//

	////��������		
	//m_LightColorList.SetItemText(4, 1,vLightinfoList[i].Tz_red.c_str());		
	//m_LightColorList.SetItemText(4, 2,vLightinfoList[i].Tz_green.c_str());	
	//m_LightColorList.SetItemText(4, 3,vLightinfoList[i].Tz_blue.c_str());
	//m_LightColorList.SetItemText(4, 4,vLightinfoList[i].Tz_alpha.c_str());

	////����ɫ		
	//m_LightColorList.SetItemText(5, 1,vLightinfoList[i].W_red.c_str());		
	//m_LightColorList.SetItemText(5, 2,vLightinfoList[i].W_green.c_str());	
	//m_LightColorList.SetItemText(5, 3,vLightinfoList[i].W_blue.c_str());
	//m_LightColorList.SetItemText(5, 4,vLightinfoList[i].W_alpha.c_str());

	////���
	//m_LightColorList.SetItemText(6, 1, vLightinfoList[i].WJ_Start.c_str());	
	//m_LightColorList.SetItemText(6, 2, vLightinfoList[i].WJ_End.c_str());
}


// ------------------------------------------------
#define DO_EDITOK2(type)  CListViewEditors dlg(type, m_LightColorList.GetItemText(tItem, tSubItem), tSubItemRect); if(IDOK == dlg.DoModal() && !CListViewEditors::s_Text.IsEmpty())


void CLightInfoEdit::OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	int tItem, tSubItem;
	CRect tSubItemRect;

	if(m_LightColorList.GetDClickedItem(tItem, tSubItem, tSubItemRect))
	{
		if(!(tItem < m_LightColorList.GetItemCount()-1))
			return;
			
		//��ɫ��
		GetLightColorList(m_aryColor, 4, tItem);	//���ȳ�ʼ���ĸ���ɫֵ
		DWORD c = 0;
		
		unsigned char a = 255;
		unsigned char b = (unsigned char)(m_aryColor[2] * 255.0f);
		unsigned char g = (unsigned char)(m_aryColor[1] * 255.0f);
		unsigned char r = (unsigned char)(m_aryColor[0] * 255.0f);
		c= (b<<16)|(g<<8)|r;	//��ʽ:abgr
		CColorDialog color(c);	//	
		
		DWORD valCol = 0;	
	
		switch(color.DoModal())
		{
		case IDCANCEL:
			m_LightColorList.Invalidate();
			return;
			break;
		case IDOK:
			c = color.GetColor();
			
			valCol = c;
			b = c>>16;

			c = valCol;
			c = c<<16;
			g = c>>24;

			c = valCol;
			c = c<<24;
			r = c>>24;

			//
			setSilderCorrelation(r, g, b);
			
			//

			m_lightType = tItem;
			updateBrightnessLabel();
			
			updateLightColorGroupControler(tItem, r, g, b, a);
			m_LightColorList.Invalidate();
			break;	
			
		}

	}

}
// ------------------------------------------------
int CLightInfoEdit::getAlterObject(int item, int subitem)
{
	int type = -1;
	switch(item)
	{
	case 0:
		switch(subitem)
		{
		case 1:
			type = h_red;
			break;
		case 2:
			type = h_green;
			break;
		case 3:
			type = h_blue;
			break;
		case 4:
			type = h_alpha;
			break;
		}
		break;
	case 1:
		switch(subitem)
		{
		case 1:
			type = z_red;
			break;
		case 2:
			type = z_green;
			break;
		case 3:
			type = z_blue;
			break;
		case 4:
			type = z_alpha;
			break;
		}
		break;
	case 2:
		switch(subitem)
		{
		case 1:
			type = b_red;
			break;
		case 2:
			type = b_green;
			break;
		case 3:
			type = b_blue;
			break;
		case 4:
			type = b_alpha;
			break;
		}
		break;
	case 3:
		switch(subitem)
		{
		case 1:
			type = Th_red;
			break;
		case 2:
			type = Th_green;
			break;
		case 3:
			type = Th_blue;
			break;
		case 4:
			type = Th_alpha;
			break;
		}
		break;
	case 4:
		switch(subitem)
		{
		case 1:
			type = Tz_red;
			break;
		case 2:
			type = Tz_green;
			break;
		case 3:
			type = Tz_blue;
			break;
		case 4:
			type = Tz_alpha;
			break;
		}
		break;
	case 5:
		switch(subitem)
		{
		case 1:
			type = W_red;
			break;
		case 2:
			type = W_green;
			break;
		case 3:
			type = W_blue;
			break;
		case 4:
			type = W_alpha;
			break;
		}
		break;
	}

	return type;
}
// ------------------------------------------------
void CLightInfoEdit::GetLightColorList(float* ary, int arySize, int item)
{
	for(int i=1; i<=arySize; i++)
	{
		ary[i-1] = atof(m_LightColorList.GetItemText(item, i));
	}
	
}


// ------------------------------------------------
void CLightInfoEdit::OnLvnKeydownList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	
	//switch(pLVKeyDow->wVKey)
	//{
	//case VK_DELETE:
	//	{
	//		int a = m_SunTimeNodeList.GetSelectionMark();
	//		if(a != 0 && a < (m_SunTimeNodeList.GetItemCount()-1))
	//		{
	//			for(int i=0; i<OTE::CRenderLight::GetInstance()->vLightinfoList.size(); i++)
	//			{
	//				if(strcmp(vLightinfoList[a].time.c_str(), OTE::CRenderLight::GetInstance()->vLightinfoList[i].time.c_str())==0 
	//					&& strcmp(vLightinfoList[a].timeName.c_str(), OTE::CRenderLight::GetInstance()->vLightinfoList[i].timeName.c_str())==0)
	//				{
	//					OTE::CRenderLight::GetInstance()->vLightinfoList.erase(OTE::CRenderLight::GetInstance()->vLightinfoList.begin() + i);	//�Ƴ��б��ж���
	//				}
	//			}
	//			vLightinfoList.erase(vLightinfoList.begin() + a);

	//			m_SunTimeNodeList.DeleteItem(a);
	//		}
	//	}
	//	break;
	//}
}

// ------------------------------------------------
void CLightInfoEdit::updateLightInfoList(int i, int action)
{
	//char name[20];
	//sprintf(name, "%s", m_SunTimeNodeList.GetItemText(i, 0));
	//char time[20];
	//sprintf(time, "%s", m_SunTimeNodeList.GetItemText(i, 1));
	//if(action == 1)	//���²���
	//{
	//	vLightinfoList[i].timeName = name;
	//	vLightinfoList[i].time = time;
	//	
	//	CString strOldTime;
	//	GetDlgItemText(IDC_lblTime, strOldTime);

	//	UpdateLightInfoSource(i, strOldTime.GetString());
	//}
	//if(action == 2)	//�������
	//{
	//	LightInfo_t b;
	//	b.timeName = name;
	//	b.time = time;

	//	b.h_lightName =  "������";
	//	b.h_red = "0.5";
	//	b.h_green = "0.5";
	//	b.h_blue =  "0.5";
	//	b.h_alpha = "1.0";
	//	
	//	b.z_lightName = "����";
	//	b.z_red = "0.7";
	//	b.z_green = "0.5";
	//	b.z_blue = "0.2";
	//	b.z_alpha = "1.0";
	//	

	//	b.b_lightName = "����";
	//	b.b_red = "0.3";
	//	b.b_green = "0.4";
	//	b.b_blue = "0.7";
	//	b.b_alpha = "1.0";

	//	b.Th_lightName =  "���λ�����";
	//	b.Th_red = "0.5";
	//	b.Th_green = "0.5";
	//	b.Th_blue =  "0.5";
	//	b.Th_alpha = "1.0";
	//	
	//	b.Tz_lightName = "��������";
	//	b.Tz_red = "0.7";
	//	b.Tz_green = "0.5";
	//	b.Tz_blue = "0.2";
	//	b.Tz_alpha = "1.0";

	//	b.W_lightName = "����ɫ";
	//	b.W_red = "0.7";
	//	b.W_green = "0.5";
	//	b.W_blue = "0.2";
	//	b.W_alpha = "1.0";

	//	b.WJ_Name = "���";
	//	b.WJ_Start = "20";
	//	b.WJ_End = "40";

	//	vLightinfoList.push_back(b);
	//	
	//	OTE::CRenderLight::GetInstance()->vLightinfoList.push_back(b);
	//}
}
// ------------------------------------------------
void CLightInfoEdit::updateLightInfoList(CString strTime, int cType, CString strValue)
{
	//int j = -1;

	//for(int i=0; i<vLightinfoList.size(); i++)
	//{
	//	if(strcmp(vLightinfoList[i].time.c_str(), strTime.GetString())==0 )
	//	{		
	//		j = i;
	//		switch(cType)
	//		{
	//			case h_red:
	//				vLightinfoList[i].h_red = strValue;
	//			break;
	//			case h_green:
	//				vLightinfoList[i].h_green = strValue;
	//			break;
	//			case h_blue:
	//				vLightinfoList[i].h_blue = strValue;
	//			break;
	//			case h_alpha:
	//				vLightinfoList[i].h_alpha = strValue;
	//			break;
	//			case z_red:
	//				vLightinfoList[i].z_red = strValue;
	//			break;
	//			case z_green:
	//				vLightinfoList[i].z_green = strValue;
	//			break;
	//			case z_blue:
	//				vLightinfoList[i].z_blue = strValue;
	//			break;
	//			case z_alpha:
	//				vLightinfoList[i].z_alpha = strValue;
	//			break;
	//			case b_red:
	//				vLightinfoList[i].b_red = strValue;
	//			break;
	//			case b_green:
	//				vLightinfoList[i].b_green = strValue;
	//			break;
	//			case b_blue:
	//				vLightinfoList[i].b_blue = strValue;
	//			break;
	//			case b_alpha:
	//				vLightinfoList[i].b_alpha = strValue;
	//			break;
	//			case Th_red:
	//				vLightinfoList[i].Th_red = strValue;
	//			break;
	//			case Th_green:
	//				vLightinfoList[i].Th_green = strValue;
	//			break;
	//			case Th_blue:
	//				vLightinfoList[i].Th_blue = strValue;
	//			break;
	//			case Th_alpha:
	//				vLightinfoList[i].Th_alpha = strValue;
	//			break;
	//			case Tz_red:
	//				vLightinfoList[i].Tz_red = strValue;
	//			break;
	//			case Tz_green:
	//				vLightinfoList[i].Tz_green = strValue;
	//			break;
	//			case Tz_blue:
	//				vLightinfoList[i].Tz_blue = strValue;
	//			break;
	//			case Tz_alpha:
	//				vLightinfoList[i].Tz_alpha = strValue;
	//			break;
	//			case W_red:
	//				vLightinfoList[i].W_red = strValue;
	//			break;
	//			case W_green:
	//				vLightinfoList[i].W_green = strValue;
	//			break;
	//			case W_blue:
	//				vLightinfoList[i].W_blue = strValue;
	//			break;
	//			case W_alpha:
	//				vLightinfoList[i].W_alpha = strValue;
	//			break;
	//			case WJ_Start:
	//				vLightinfoList[i].WJ_Start = strValue;
	//			break;
	//			case WJ_End:
	//				vLightinfoList[i].WJ_End = strValue;
	//			break;
	//			
	//		}

	//		break;
	//	}
	//}

	////����Դ������
	//if(-1 != j)
	//{
	//	UpdateLightInfoSource(j, strTime.GetString());
	//}
}

// ------------------------------------------------
//��Ⱦ����
void CLightInfoEdit::OnBnClickedButton1()
{
	int answer = ::MessageBox(NULL, "�����Ⱦ��Ӱ��?", "������ʾ",
								MB_YESNO);
	if (!(answer == IDYES))
	{
		return ;
	}
	if(NULL == gSceneMgr->m_TerrainPage)
		return;
	
	Ogre::ColourValue ambientColor = gSceneMgr->getAmbientLight();
	Light* l = gSceneMgr->getLight("MainLight");
	Ogre::Vector3 mainLightDir = l->getDirection();	//���ⷽ��
	Ogre::ColourValue mainLightCor = l->getDiffuseColour();//����ɫ

	//������ɫ
	l = gSceneMgr->getLight("Light2");
	Ogre::ColourValue patchLightCor = l->getDiffuseColour();
	Ogre::Vector3 patchLightDir = l->getDirection();
	//������ɫend

	float brightness = 0.1;//0.5	//Ӱ������ϵ��
	Ogre::ColourValue shadowness = Ogre::ColourValue(0.5, 0.5, 0.5);
	float contrast = 0.5;//0.5
	float roughness = 0.8;//0.8

	float lightmapgentarget = 0;	



	//���ؿչ���ͼ
	std::string strFileName;
	switch(m_lightmapSize)
	{
		case map512:
			strFileName = "../Res/01_Scene/SystemFilesPic/lightMapTemplate512.bmp";
			break;
		case map128:
			strFileName = "../Res/01_Scene/SystemFilesPic/lightMapTemplate128.bmp";
			break;
		default:
			strFileName = "../Res/01_Scene/SystemFilesPic/lightMapTemplate.bmp";
	}
	COTETerrainLightmapMgr::GetInstance()->LoadLightMapData(gSceneMgr->m_TerrainPage, strFileName);	

	//�������ͼ����ɫ

	CString s;
	GetDlgItemText(IDC_lblTime, s);
	COTETerrainLightmapMgr::GetInstance()->GenDirectLightmap(gSceneMgr->m_TerrainPage, mainLightDir,mainLightCor, patchLightDir, patchLightCor, ambientColor,
				brightness,shadowness,contrast,roughness,0,lightmapgentarget);

	COTETerrainLightmapMgr::GetInstance()->releaseEmptyLightMap(gSceneMgr->m_TerrainPage);

	//���ù���ͼ

	CString strOldTime;
	GetDlgItemText(IDC_lblTime, strOldTime);
	COTETerrainLightmapMgr::GetInstance()->ResetLightmapMaterial(
		gSceneMgr->m_TerrainPage		
		);

	::MessageBox(NULL, "��Ⱦ��ɣ���������", "������ʾ", MB_OK);
}
// ------------------------------------------------
void CLightInfoEdit::OnBnClickedRadio512()
{
	m_lightmapSize = map128;
}

void CLightInfoEdit::OnBnClickedRadio256()
{
	m_lightmapSize = map512;
}
// ------------------------------------------------
void CLightInfoEdit::OnBnClickedBtnsavelight()
{
	if(NULL == gSceneMgr->m_TerrainPage)
		return;
	// writeConfigFile();//����ʱ��㼰����Ϣ
	::MessageBox(NULL, "ȫ��ʱ�����ü�����ɫ������ɣ���������", "������ʾ", MB_OK);
}
// ------------------------------------------------
void CLightInfoEdit::initSilder()
{
	m_SilderBrightness.SetRange(0, 255);
	m_SilderBrightness.SetTicFreq(1);
}
// ------------------------------------------------
void CLightInfoEdit::silderBindColor(float fbrightness)
{	
	m_SilderBrightness.SetPos(int(fbrightness * 255));
}
// ------------------------------------------------
void CLightInfoEdit::silderBindColor()
{	
	m_SilderBrightness.SetPos(m_Y);
}
// ------------------------------------------------
void CLightInfoEdit::OnNMCustomdrawSliderbrightness(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_Y = (float)m_SilderBrightness.GetPos();
	int R;
	int G;
	int B;
	int A;
	convertColorToRGB(R, G, B, A);

	updateLightColorGroupControler(m_lightType , R, G, B, A);
	
}
// ------------------------------------------------
void CLightInfoEdit::updateLightColorGroupControler(int lightType, unsigned int r, unsigned int g, unsigned int b, unsigned int a)
{
	float fa = float(a) / 255.0f;
	float fb = float(b) / 255.0f;
	float fg = float(g) / 255.0f;
	float fr = float(r) / 255.0f;

	CString time;
	GetDlgItemText(IDC_lblTime, time);
	char val[60];
	for(int i=1; i<4; i++)
	{
		switch(i)
		{
		case 1:
			sprintf(val, "%f", fr);
			break;
		case 2:
			sprintf(val, "%f", fg);
			break;
		case 3:
			sprintf(val, "%f", fb);
			break;
		case 4:
			sprintf(val, "%f", fa);
			break;
		}
		m_LightColorList.SetItemText(lightType, i, LPCTSTR(val));	//���ֹ⣬�ĸ�ɫ,ֵ
		
		int type = CLightInfoEdit::s_Inst.getAlterObject(lightType, i);
		updateLightInfoList(time, type, val);
	}
	
	UpdateSunInfo();
}


// ------------------------------------------------
void CLightInfoEdit::OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	int tItem, tSubItem;
	CRect tSubItemRect;

	if(m_LightColorList.GetDClickedItem(tItem, tSubItem, tSubItemRect))
	{
		if(!(tItem < m_LightColorList.GetItemCount()-1))	//���һ���ǵ���಻�õ�ɫ�����Է���
			return;

		m_lightType = tItem;
		updateBrightnessLabel();

		changeSunTimeThenChangeLightColor();

		//if(m_lightType == 5)	//ִ����ɫ
		//	OTE::CRenderLight::GetInstance()->SetFogState(true);
	}
}
// ------------------------------------------------
void CLightInfoEdit::changeSunTimeThenChangeLightColor()
{
	unsigned int r, g, b, a;
	convertColorFloatToInt(atof(m_LightColorList.GetItemText(m_lightType, 1)), atof(m_LightColorList.GetItemText(m_lightType, 2)),
								atof(m_LightColorList.GetItemText(m_lightType, 3)), atof(m_LightColorList.GetItemText(m_lightType, 4)),
								r, g, b, a);
	setSilderCorrelation(r, g, b);	
}

void CLightInfoEdit::updateBrightnessLabel()
{
	switch(m_lightType)
		{
		case 0:
			m_lblBrightness.SetWindowText("����(������):");
			break;
		case 1:
			m_lblBrightness.SetWindowText("����(����):");
			break;
		case 2:
			m_lblBrightness.SetWindowText("����(����):");
			break;
		case 3:
			m_lblBrightness.SetWindowText("����(�ػ���):");
			break;
		case 4:
			m_lblBrightness.SetWindowText("����(������):");
			break;
		case 5:
			m_lblBrightness.SetWindowText("����(����ɫ):");
			break;
		}
}
// ------------------------------------------------
void CLightInfoEdit::setSilderCorrelation(unsigned int r, unsigned int g, unsigned int b)
{
	convertColorToYCC(r, g, b);	//��ʼ��YCC��ɫֵ
	silderBindColor();	//�������Ȼ���
}
// ------------------------------------------------
void CLightInfoEdit::convertColorToYCC(unsigned int r, unsigned int g, unsigned int b)
{
	m_Y	 =   0.299*r	+   0.587*g		+   0.114*b; //    (����)   
	m_Cb   =   -0.1687*r	-   0.3313*g	+   0.5*b		+   128;
	m_Cr   =   0.5*r		-   0.4187*g	-   0.0813*b	+   128;
}
// ------------------------------------------------
void CLightInfoEdit::convertColorToRGB(/*unsigned*/ int& r, /*unsigned*/ int& g, /*unsigned*/ int& b, /*unsigned*/ int& a)
{	//�����ɫ����������ڴ˹�ʽ����������
	//ԭ��
	r   =   m_Y + (int)(1.402 * (m_Cr - 128)) ;  
	if(r> 255)
		r = 255;
	else if(r<0)
		r = 0;

	g   =   m_Y - (int)(0.34414 * (m_Cb - 128)) - (int)(0.71414 * (m_Cr - 128))  ; 
	if(g> 255)
		g = 255;
	else if(g<0)
		g = 0;

	b   =   m_Y + (int)(1.772 * (m_Cb - 128))  ;
	if(b> 255)
		b = 255;
	else if(b<0)
		b = 0;
	a   =   255;
	//ԭ��  end

	
}
// ------------------------------------------------
void CLightInfoEdit::convertColorFloatToInt(float fr, float fg, float fb, float fa, unsigned int& r, unsigned int& g, unsigned int& b, unsigned int& a)
{
	r = (unsigned int)(fr * 255.0f);
	g = (unsigned int)(fg * 255.0f);
	b = (unsigned int)(fb * 255.0f);
	a = (unsigned int)(fa * 255.0f);
}
// ------------------------------------------------
void CLightInfoEdit::clearLightinfoList()
{
	m_SunTimeNodeList.DeleteAllItems();

	vLightinfoList.clear();
}
void CLightInfoEdit::OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}
