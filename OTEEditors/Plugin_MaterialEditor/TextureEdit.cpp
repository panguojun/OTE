// TextureEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_MaterialEditor.h"
#include "TextureEdit.h"
#include ".\textureedit.h"

CTextureEdit CTextureEdit::s_Inst;
// CTextureEdit 对话框

IMPLEMENT_DYNAMIC(CTextureEdit, CDialog)
CTextureEdit::CTextureEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CTextureEdit::IDD, pParent)
{
	isInit = false;
}

CTextureEdit::~CTextureEdit()
{
}

void CTextureEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO1, m_SingleTexture);
	DDX_Control(pDX, IDC_RADIO2, m_MultiTexture);
	DDX_Control(pDX, IDC_LIST1, m_TextureList);
	DDX_Control(pDX, IDC_COMBO1, m_TextAddrModeComb);
	DDX_Control(pDX, IDC_CHECK1, m_CullModeCheck);
	DDX_Control(pDX, IDC_TexUnitList, m_TexUnitList);
}

// -----------------------------------------------
void CTextureEdit::UpdateCurTextUnit()
{
	if(m_pTextureUnitState != NULL)
	{
		if(m_pTextureUnitState->getNumFrames() > 1)
		{
			//纹理

			m_MultiTexture.SetCheck(1);		
			GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
			
			char sinfo[32];

			sprintf(sinfo, "%3.2f", m_pTextureUnitState->getAnimationDuration());

			SetDlgItemText(IDC_EDIT2, sinfo);

			for(unsigned int i = 0; i < m_pTextureUnitState->getNumFrames(); i++)
			{
				sprintf(sinfo, "%d", i+1);
				m_TextureList.InsertItem(i, sinfo);
				m_TextureList.SetItemText(i, 1, m_pTextureUnitState->getFrameTextureName(i).c_str());
			}
		}
		else
		{
			//单一纹理
			m_SingleTexture.SetCheck(1);
			GetDlgItem(IDC_BUTTON1)->EnableWindow(true);
			SetDlgItemText(IDC_EDIT1, m_pTextureUnitState->getTextureName().c_str());
		}
	}

	m_TextureList.Invalidate();
}

void CTextureEdit::UpdataTexture(Ogre::Material *pMat)
{
	m_SingleTexture.SetCheck(0);
	m_MultiTexture.SetCheck(0);

	SetDlgItemText(IDC_EDIT1, "");
	SetDlgItemText(IDC_EDIT2, "");

	m_TextureList.DeleteAllItems();
	if(!isInit)
	{
		m_TextureList.InsertColumn(0, "ID", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 30);
		m_TextureList.InsertColumn(1, "FileName", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 100);
		m_TextureList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_MULTIWORKAREAS);
		isInit = true;
	}


	GetDlgItem(IDC_BUTTON1)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON4)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON5)->EnableWindow(false);

	m_pMat = pMat;
	if(m_pMat != NULL)
		m_pTextureUnitState = m_pMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
	else
		m_pTextureUnitState = NULL;

	UpdateCurTextUnit();

	// 更新贴图单元列表

	m_TexUnitList.ResetContent();
	unsigned int tusCnt = m_pMat->getTechnique(0)->getPass(0)->getNumTextureUnitStates();
	for(unsigned int i = 0; i < tusCnt; ++ i)
	{
		m_TexUnitList.AddString(m_pMat->getTechnique(0)->getPass(0)->getTextureUnitState(i)->getTextureName().c_str());
	}

}

BEGIN_MESSAGE_MAP(CTextureEdit, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_EN_KILLFOCUS(IDC_EDIT2, OnEnKillfocusEdit2)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnNMClickList1)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_CBN_EDITCHANGE(IDC_COMBO1, OnCbnEditchangeCombo1)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_LBN_SELCHANGE(IDC_TexUnitList, OnLbnSelchangeTexunitlist)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
END_MESSAGE_MAP()


// ----------------------------------------------
//更换单一纹理
void CTextureEdit::OnBnClickedButton1()
{
	if(m_pMat == NULL || m_pTextureUnitState == NULL)
		return;

	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"纹理图片(*.bmp, *.tga, *.jpg, *.gif, *.dds)|*.bmp;*.tga;*.jpg;*.gif;*.dds|All Files (*.*)|*.*|",
		 NULL 
		); 
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{
		m_pTextureUnitState->setTextureName(tpFileDialog->GetFileName().GetString());
		UpdataTexture(m_pMat);
	}

	delete tpFileDialog;
}
// ----------------------------------------------
//更新动画纹理状态
void CTextureEdit::UpdataAniTextState(Ogre::TextureUnitState *pTextureUnitState)
{
	CString buf;
	GetDlgItemText(IDC_EDIT2, buf);
	float f = atof(buf.GetString());

	std::vector<Ogre::String> TextureVec;
	
	unsigned int i = m_TextureList.GetItemCount();
	for(unsigned int j = 0; j < i; j++)
	{
		TextureVec.push_back(m_TextureList.GetItemText(j, 1).GetString());
	}

	if(f > 0.0f && i > 0)
	{
		pTextureUnitState->setAnimatedTextureName((Ogre::String*)&TextureVec[0], i, f);
	}
}
// ----------------------------------------------
//更改动画纹理时间
void CTextureEdit::OnEnKillfocusEdit2()
{
	if(m_pMat == NULL || m_pTextureUnitState == NULL)
		return;

	if(m_TextureList.GetItemCount() > 1)
	{
		UpdataAniTextState(m_pTextureUnitState);

		UpdataTexture(m_pMat);
	}
}
// ----------------------------------------------
//选择静态纹理
void CTextureEdit::OnBnClickedRadio1()
{
	if(m_pMat == NULL || m_pTextureUnitState == NULL)
		return;

	if(m_pTextureUnitState->getNumFrames() > 1)
	{
		m_pTextureUnitState->setTextureName(m_pTextureUnitState->getFrameTextureName(0));	
	}

	UpdataTexture(m_pMat);
}
// ----------------------------------------------
//选择动画纹理
void CTextureEdit::OnBnClickedRadio2()
{
	if(m_pMat == NULL || m_pTextureUnitState == NULL)
		return;

	if(m_pTextureUnitState->getNumFrames() > 1)
		return;

	if(m_TextureList.GetItemCount() < 1)
	{
		GetDlgItem(IDC_BUTTON2)->EnableWindow(true);
		SetDlgItemText(IDC_EDIT1, "");
		SetDlgItemText(IDC_EDIT2, "1.00");
		std::vector<Ogre::String> TextureVec;
		TextureVec.push_back(m_pTextureUnitState->getTextureName());
		m_TextureList.InsertItem(0, "1");
		m_TextureList.SetItemText(0, 1, m_pTextureUnitState->getTextureName().c_str());
		m_pTextureUnitState->setAnimatedTextureName((Ogre::String*)&TextureVec[0], 1, 1.0);	
	}
}
// ----------------------------------------------
//添加纹理动画桢
void CTextureEdit::OnBnClickedButton2()
{
	if(m_pMat == NULL || m_pTextureUnitState == NULL)
		return;

	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"纹理图片(*.bmp, *.tga, *.jpg, *.gif, *.dds)|*.bmp;*.tga;*.jpg;*.gif;*.dds|All Files (*.*)|*.*|",
		 NULL 
		); 
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{
		int i = m_TextureList.GetItemCount();
		char buf[32];
		itoa(i, buf, 10);
        m_TextureList.InsertItem(i, buf);
		m_TextureList.SetItemText(i, 1, tpFileDialog->GetFileName().GetString());
		UpdataAniTextState(m_pTextureUnitState);
	}

	delete tpFileDialog;

	UpdataTexture(m_pMat);
}
// ----------------------------------------------
//删除纹理
void CTextureEdit::OnBnClickedButton3()
{
	if(m_pMat == NULL || m_pTextureUnitState == NULL || m_TextureList.GetItemCount() < 2)
		return;

	int i = m_TextureList.GetLastSelect();
	if(i < m_TextureList.GetItemCount())
	{
		m_TextureList.DeleteItem(i);
		UpdataAniTextState(m_pTextureUnitState);
	}

	UpdataTexture(m_pMat);
}

// ----------------------------------------------
//点选贴图
void CTextureEdit::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(m_pMat == NULL || m_pTextureUnitState == NULL)
		return;

	int tItem, tSubItem;
	CRect tSubItemRect;
	if(m_TextureList.GetDClickedItem(tItem, tSubItem, tSubItemRect))
	{
		if(tItem > m_TextureList.GetItemCount() || tItem < 0)
		{
			GetDlgItem(IDC_BUTTON3)->EnableWindow(false);
			return;
		}
		else
		{
			GetDlgItem(IDC_BUTTON3)->EnableWindow(true);
		}

		if(tItem <= 0)
			GetDlgItem(IDC_BUTTON4)->EnableWindow(false);
		else
			GetDlgItem(IDC_BUTTON4)->EnableWindow(true);

		if(tItem >= m_TextureList.GetItemCount() - 1)
			GetDlgItem(IDC_BUTTON5)->EnableWindow(false);
		else
            GetDlgItem(IDC_BUTTON5)->EnableWindow(true);

	}

	*pResult = 0;
}
// ----------------------------------------------
//向上移动贴图
void CTextureEdit::OnBnClickedButton4()
{
	if(m_pMat == NULL || m_pTextureUnitState == NULL)
		return;

	int tItem = m_TextureList.GetLastSelect();
	if(tItem <= 0)
		return;

	CString sText = m_TextureList.GetItemText(tItem, 1);

	m_TextureList.SetItemText(tItem, 1, m_TextureList.GetItemText(tItem - 1, 1).GetString());
	m_TextureList.SetItemText(tItem - 1, 1, sText.GetString());

	UpdataAniTextState(m_pTextureUnitState);
	UpdataTexture(m_pMat);
}
// ----------------------------------------------
//向下移动贴图
void CTextureEdit::OnBnClickedButton5()
{
	if(m_pMat == NULL || m_pTextureUnitState == NULL)
		return;

	int tItem = m_TextureList.GetLastSelect();
	if(tItem >= m_TextureList.GetItemCount() - 1)
		return;

	CString sText = m_TextureList.GetItemText(tItem, 1);

	m_TextureList.SetItemText(tItem, 1, m_TextureList.GetItemText(tItem + 1, 1).GetString());
	m_TextureList.SetItemText(tItem + 1, 1, sText.GetString());

	UpdataAniTextState(m_pTextureUnitState);
	UpdataTexture(m_pMat);
}

// ----------------------------------------------
// 选择贴图方式

void CTextureEdit::OnCbnSelchangeCombo1()
{
	if(m_pMat == NULL || m_pTextureUnitState == NULL)
		return;	

	CString str; this->GetDlgItemText(IDC_COMBO1, str);

	if(str == "TAM_WRAP")
		m_pTextureUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_WRAP);
	else if(str == "TAM_MIRROR")
		m_pTextureUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_MIRROR);
	else if(str == "TAM_CLAMP")
		m_pTextureUnitState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);

}

// ----------------------------------------------
void CTextureEdit::OnCbnEditchangeCombo1()
{
	
}

// ----------------------------------------------
void CTextureEdit::OnBnClickedCheck1()
{
	if(m_pMat == NULL)
		return;

	if(m_CullModeCheck.GetCheck() == BST_CHECKED)
		m_pMat->setCullingMode(Ogre::CULL_NONE);
	else
		m_pMat->setCullingMode(Ogre::CULL_CLOCKWISE);

}

void CTextureEdit::OnLbnSelchangeTexunitlist()
{
	if(!m_pMat)
		return;

	int curSel = m_TexUnitList.GetCurSel();
	if( curSel >= 0)
	{
		m_pTextureUnitState = m_pMat->getTechnique(0)->getPass(0)->getTextureUnitState(curSel);
		UpdateCurTextUnit();
	}	
}

void CTextureEdit::OnEnChangeEdit1()
{

}

void CTextureEdit::OnBnClickedButton6()
{
	if(m_pMat == NULL)
		return;

	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"纹理图片(*.bmp, *.tga, *.jpg, *.gif, *.dds)|*.bmp;*.tga;*.jpg;*.gif;*.dds|All Files (*.*)|*.*|",
		 NULL 
		); 
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{
		Ogre::Pass* pass = m_pMat->getTechnique(0)->getPass(0);
		if(pass->hasVertexProgram()) 
			pass->createTextureUnitState(tpFileDialog->GetFileName().GetString(), pass->getNumTextureUnitStates());
		else
			pass->createTextureUnitState(tpFileDialog->GetFileName().GetString(), pass->getNumTextureUnitStates());

		UpdataTexture(m_pMat);
	}

	delete tpFileDialog;
}
