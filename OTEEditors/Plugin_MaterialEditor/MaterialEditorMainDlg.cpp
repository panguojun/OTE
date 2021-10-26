// MaterialEditorMainDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_MaterialEditor.h"
#include "OTEQTSceneManager.h"
#include "LightEdit.h"
#include "AlphaEdit.h"
#include "TextureEdit.h"
#include "OtherEdit.h"
#include "MaterialEditorMainDlg.h"
#include "OgreMaterialSerializer.h"
#include "MatLibDlg.h"

using namespace Ogre;
using namespace OTE;

CMaterialEditorMainDlg CMaterialEditorMainDlg::s_Inst;
Ogre::MovableObject* CMaterialEditorMainDlg::s_pObj = NULL;
// CMaterialEditorMainDlg 对话框

IMPLEMENT_DYNAMIC(CMaterialEditorMainDlg, CDialog)
CMaterialEditorMainDlg::CMaterialEditorMainDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMaterialEditorMainDlg::IDD, pParent)
{
	isInited = false;
	m_pMat = NULL;
}

CMaterialEditorMainDlg::~CMaterialEditorMainDlg()
{
}

void CMaterialEditorMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
	DDX_Control(pDX, IDC_MaterialList, mMatList);
	DDX_Control(pDX, IDC_TAB1, m_TabCtrl);
}


BEGIN_MESSAGE_MAP(CMaterialEditorMainDlg, CDialog)
	ON_BN_CLICKED(IDC_Open, OnBnClickedOpen)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnTcnSelchangeTab1)
	ON_NOTIFY(NM_CLICK, IDC_MaterialList, OnNMClickMateriallist)
	ON_BN_CLICKED(IDC_Close, OnBnClickedClose)
	ON_BN_CLICKED(IDC_Save, OnBnClickedSave)
	ON_EN_KILLFOCUS(IDC_MaterialName, OnEnKillfocusMaterialname)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
END_MESSAGE_MAP()

//更新相关内容
void CMaterialEditorMainDlg::UpdataAbout()
{
	mMatList.DeleteAllItems();

	m_pMat = NULL;

	if(!isInited)
	{
		mMatList.InsertColumn(0, "SubMesh", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 60);
		mMatList.InsertColumn(1, "Material", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 120);
		mMatList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_MULTIWORKAREAS);

		if(CLightEdit::s_Inst.m_hWnd)
		{
			CLightEdit::s_Inst.mColorList.InsertColumn(0, "名称", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 40);
			CLightEdit::s_Inst.mColorList.InsertColumn(1, "红", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 30);
			CLightEdit::s_Inst.mColorList.InsertColumn(2, "绿", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 30);
			CLightEdit::s_Inst.mColorList.InsertColumn(3, "蓝", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 30);
			CLightEdit::s_Inst.mColorList.InsertColumn(4, "Alpha", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 40);
			CLightEdit::s_Inst.mColorList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_MULTIWORKAREAS);		

			CLightEdit::s_Inst.m_SilderBrightness.SetRange(0, 255);
			CLightEdit::s_Inst.m_SilderBrightness.SetTicFreq(1);
		}
		
		isInited = true;
	}

	SetDlgItemText(IDC_MaterialName, "");
	
	if(CLightEdit::s_Inst.m_hWnd)
		CLightEdit::s_Inst.ClearAll();

	if(s_pObj != NULL)
	{
		if(s_pObj->getMovableType() == "OTEEntity")
		{
			/*Ogre::Mesh::SubMeshNameMap nm = ((OTE::COTEActorEntity*)s_pObj)->getMesh()->getSubMeshNameMap();
			Ogre::Mesh::SubMeshNameMap::iterator it = nm.begin();
			int i = 0;
			for(; it != nm.end(); ++ it)
			{
				std::string SubMeshName = it->first;
				Ogre::SubEntity* se = ((COTEActorEntity*)s_pObj)->getSubEntity(SubMeshName.c_str());
				if(se != NULL)
				{
					std::string MaterialName = se->getMaterialName();
					mMatList.InsertItem(i, SubMeshName.c_str());
					mMatList.SetItemText(i, 1, MaterialName.c_str());
					i++;
				}
			}*/

			int i = 0;
			HashMap<std::string, std::string>::iterator it = ((COTEActorEntity*)s_pObj)->m_EquipmentMapEx.begin();
			while(it != ((COTEActorEntity*)s_pObj)->m_EquipmentMapEx.end())
			{			
				mMatList.InsertItem(i,       it->first.c_str());
				mMatList.SetItemText(i++, 1, it->first.c_str());
				++ it;
			}
		}
		else if(s_pObj->getMovableType() == "TerrainTile")
		{
			std::string MaterialName = ((COTETile*)s_pObj)->getMaterial()->getName();
			mMatList.InsertItem(0, s_pObj->getName().c_str());
			mMatList.SetItemText(0, 1, MaterialName.c_str());
		}
	}

	mMatList.Invalidate();
}

// ---------------------------------------
//初始化对话框
BOOL CMaterialEditorMainDlg::OnInitDialog()
{
	BOOL tRet = CDialog::OnInitDialog();	

	m_TabCtrl.InsertItem(0, "光照");
	m_TabCtrl.InsertItem(1, "纹理");
	m_TabCtrl.InsertItem(2, "Alpha");
	m_TabCtrl.InsertItem(3, "其它");

	//打开
	((CButton*)GetDlgItem(IDC_Open))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP2)));
	//关闭
	((CButton*)GetDlgItem(IDC_Close))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP3)));
	//保存
	((CButton*)GetDlgItem(IDC_Save))->SetBitmap(
		::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_BITMAP1)));

	if(!CLightEdit::s_Inst.m_hWnd)
		CLightEdit::s_Inst.Create(IDD_Light, this);
	CLightEdit::s_Inst.ShowWindow(SW_SHOW);

	if(!CAlphaEdit::s_Inst.m_hWnd)
		CAlphaEdit::s_Inst.Create(IDD_Alpha, this);
	CAlphaEdit::s_Inst.ShowWindow(SW_HIDE);

	if(!COtherEdit::s_Inst.m_hWnd)
		COtherEdit::s_Inst.Create(IDD_Other, this);
	COtherEdit::s_Inst.ShowWindow(SW_HIDE);

	if(!CTextureEdit::s_Inst.m_hWnd)
		CTextureEdit::s_Inst.Create(IDD_Texture, this);
	CTextureEdit::s_Inst.ShowWindow(SW_HIDE);



	CRect tRect;
	GetClientRect(tRect);	
	CLightEdit::s_Inst.MoveWindow(tRect.left + 12, tRect.top + 195, tRect.right - 25, tRect.bottom - 210);
	CAlphaEdit::s_Inst.MoveWindow(tRect.left + 12, tRect.top + 195, tRect.right - 25, tRect.bottom - 210);
	COtherEdit::s_Inst.MoveWindow(tRect.left + 12, tRect.top + 195, tRect.right - 25, tRect.bottom - 210);
	CTextureEdit::s_Inst.MoveWindow(tRect.left + 12, tRect.top + 195, tRect.right - 25, tRect.bottom - 210);

	return tRet;
}

// ---------------------------------------
//打开实体
void CMaterialEditorMainDlg::OnBnClickedOpen()
{
	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"实体对象文件(*.obj)|*.obj|All Files (*.*)|*.*|",
		 NULL 
		); 
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{
		if(s_pObj != NULL)
			SCENE_MGR->removeEntity(s_pObj->getName());
		s_pObj = SCENE_MGR->CreateEntity(tpFileDialog->GetFileName().GetString(), "MaterialEdit");

		UpdataAbout();
	}

	delete tpFileDialog;
}
// ---------------------------------------
//改变选项卡
void CMaterialEditorMainDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	CLightEdit::s_Inst.ShowWindow(SW_HIDE);
	CAlphaEdit::s_Inst.ShowWindow(SW_HIDE);
	COtherEdit::s_Inst.ShowWindow(SW_HIDE);
	CTextureEdit::s_Inst.ShowWindow(SW_HIDE);

	switch(m_TabCtrl.GetCurSel())
	{
	case 0://灯光
		CLightEdit::s_Inst.ShowWindow(SW_SHOW);
		break;
	case 1://纹理
		CTextureEdit::s_Inst.ShowWindow(SW_SHOW);
		break;
	case 2://Alpha
		CAlphaEdit::s_Inst.ShowWindow(SW_SHOW);
		break;
	case 3://其它
		COtherEdit::s_Inst.ShowWindow(SW_SHOW);
		break;
	}

	*pResult = 0;
}
// ---------------------------------------
//点选材质
void CMaterialEditorMainDlg::OnNMClickMateriallist(NMHDR *pNMHDR, LRESULT *pResult)
{
	int tItem, tSubItem;
	CRect tSubItemRect;

	if(mMatList.GetDClickedItem(tItem, tSubItem, tSubItemRect) && s_pObj != NULL)
	{
		if(tItem > mMatList.GetItemCount() || tItem < 0)
			return;

		m_SubName = mMatList.GetItemText(tItem, 0);
		if(!m_SubName.IsEmpty())
		{
			if(s_pObj->getMovableType() == "OTEEntity")
			{
				//Ogre::SubEntity *se = ((COTEActorEntity*)s_pObj)->getSubEntity(m_SubName.GetString());
				m_pMat = (Ogre::Material*)Ogre::MaterialManager::getSingleton().getByName(m_SubName.GetString()).getPointer();
				assert(m_pMat);

				//m_pMat = se->getMaterial().getPointer();
			}
			else
			{
				m_pMat = ((COTETile*)s_pObj)->getMaterial().getPointer();
			}
			SetDlgItemText(IDC_MaterialName, m_pMat->getName().c_str());
			
			if(CLightEdit::s_Inst.m_hWnd)
				CLightEdit::s_Inst.UpdataAll();
			if(CTextureEdit::s_Inst.m_hWnd)
				CTextureEdit::s_Inst.UpdataTexture(m_pMat);
			if(CAlphaEdit::s_Inst.m_hWnd)
				CAlphaEdit::s_Inst.UpdataAlpha(m_pMat);
			if(COtherEdit::s_Inst.m_hWnd)
				COtherEdit::s_Inst.UpdataOther(m_pMat);
		}
	}

	*pResult = 0;
}
// ---------------------------------------
//关闭按键
void CMaterialEditorMainDlg::OnBnClickedClose()
{
	if(s_pObj != NULL)
	{
		if(s_pObj->getMovableType() == "OTEEntity")
			SCENE_MGR->removeEntity(s_pObj->getName());
		s_pObj = NULL;
	}
	
	m_pMat = NULL;
	m_SubName = "";

	UpdataAbout();

	if(CLightEdit::s_Inst.m_hWnd)
		CLightEdit::s_Inst.UpdataAll();
	if(CTextureEdit::s_Inst.m_hWnd)
		CTextureEdit::s_Inst.UpdataTexture(m_pMat);
	if(CAlphaEdit::s_Inst.m_hWnd)
		CAlphaEdit::s_Inst.UpdataAlpha(m_pMat);
	if(COtherEdit::s_Inst.m_hWnd)
		COtherEdit::s_Inst.UpdataOther(m_pMat);
}
// ---------------------------------------
//保存材质
void CMaterialEditorMainDlg::OnBnClickedSave()
{
	if(s_pObj == NULL || s_pObj->getMovableType() != "OTEEntity" || m_pMat == NULL || m_SubName.IsEmpty())
		return;

	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	
	CString fileName = ((OTE::COTEActorEntity*)s_pObj)->getMesh()->getName().c_str();
	fileName.TrimRight(".mesh");
	CFileDialog* tpFileDialog=new CFileDialog(false, // TRUE for FileOpen, FALSE for FileSaveAs
		 "material",
		 fileName,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"材质脚本(*.material)|*.material|All Files (*.*)|*.*|",
		 NULL 
		); 

	tpFileDialog->m_ofn.lpstrInitialDir = "..\\Res\\02_Character\\dds";

	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);
	

	if (nResponse == IDOK)
	{
		Ogre::MaterialSerializer ms;

		CString file = tpFileDialog->GetPathName();

		if(file.Find(".material") == -1 && file.Find(".Material") == -1)
		{		
			file += ".material";		
		}
		
		FILE* f = fopen(file.GetString(), "w");
		assert(f);
		Ogre::Mesh::SubMeshNameMap::iterator i = ((COTEActorEntity*)s_pObj)->getMesh()->mSubMeshNameMap.begin() ;
		while(i != ((COTEActorEntity*)s_pObj)->getMesh()->mSubMeshNameMap.end())
		{
			COTESubEntity* se = ((COTEActorEntity*)s_pObj)->getSubEntity(i->first.c_str());
			if(se != NULL)
			{
				ms.queueForExport(se->getMaterial());				
				fprintf(f, ms.getQueuedAsString().c_str());
				fprintf(f, "\n");
			}
			++ i;
		}		
		
		
		fclose(f);
	}

	delete tpFileDialog;


}
// ---------------------------------------
//更新材质名称
void CMaterialEditorMainDlg::OnEnKillfocusMaterialname()
{
	if(s_pObj == NULL || s_pObj->getMovableType() != "OTEEntity" || m_pMat == NULL || m_SubName.IsEmpty())
		return;

	CString buf;
	GetDlgItemText(IDC_MaterialName, buf);

	if(!buf.IsEmpty())
	{
		Ogre::Material *newMat = m_pMat->clone(buf.GetString()).getPointer();

		COTESubEntity *se = ((COTEActorEntity*)s_pObj)->getSubEntity(m_SubName.GetString());
		se->setMaterialName(buf.GetString());

		m_pMat = newMat;
	}

	UpdataAbout();
}

void CMaterialEditorMainDlg::OnBnClickedButton1()
{
	CMatLibDlg dlg;
	dlg.DoModal();
}
