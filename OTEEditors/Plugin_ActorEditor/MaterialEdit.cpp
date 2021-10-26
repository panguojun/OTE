// MaterialEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "MaterialEdit.h"
#include "ActorEditorMainDlg.h"

CMaterialEdit	CMaterialEdit::s_Inst;
// CMaterialEdit 对话框

IMPLEMENT_DYNAMIC(CMaterialEdit, CDialog)
CMaterialEdit::CMaterialEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CMaterialEdit::IDD, pParent)
{
	m_pMat = NULL;
	isInited = false;
}

CMaterialEdit::~CMaterialEdit()
{
	m_pMat = NULL;
}

void CMaterialEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MaterialList, mMatList);
	DDX_Control(pDX, IDC_ColorList, mColorList);
}


BEGIN_MESSAGE_MAP(CMaterialEdit, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_MaterialList, OnNMClickMateriallist)
	ON_NOTIFY(NM_DBLCLK, IDC_ColorList, OnNMDblclkColorlist)
END_MESSAGE_MAP()

//更新模型材质清单
void CMaterialEdit::UpdataMaterialList()
{
	mMatList.DeleteAllItems();
	mColorList.DeleteAllItems();

	if(!isInited)
	{
		mMatList.InsertColumn(0, "SubMesh", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 60);
		mMatList.InsertColumn(1, "Material", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 160);
		mMatList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_MULTIWORKAREAS);

		mColorList.InsertColumn(0, "名称", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 50);
		mColorList.InsertColumn(1, "红", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 40);
		mColorList.InsertColumn(2, "绿", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 40);
		mColorList.InsertColumn(3, "蓝", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 40);
		mColorList.InsertColumn(4, "Alpha", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 50);
		mColorList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_MULTIWORKAREAS);

		isInited = true;
	}

	mColorList.InsertItem(0, "Ambient");
	mColorList.InsertItem(1, "Diffuse");
	mColorList.InsertItem(2, "Specular");
	mColorList.InsertItem(3, "Emissive");
		
	m_pMat = NULL;

	SetDlgItemText(IDC_MatName, "");
	GetDlgItem(IDC_MatName)->EnableWindow(false);

	if(CActorEditorMainDlg::g_pActorEntity != NULL)
	{
		GetDlgItem(IDC_MatName)->EnableWindow(true);
		Ogre::Mesh::SubMeshNameMap nm = CActorEditorMainDlg::g_pActorEntity->getMesh()->getSubMeshNameMap();
		Ogre::Mesh::SubMeshNameMap::iterator it = nm.begin();
		int i = 0;
		for(; it != nm.end(); ++ it)
		{
			std::string SubMeshName = it->first;
			Ogre::SubEntity* se = CActorEditorMainDlg::g_pActorEntity->getSubEntity(SubMeshName.c_str());
			if(se != NULL)
			{
				std::string MaterialName = se->getMaterialName();
				mMatList.InsertItem(i, SubMeshName.c_str());
				mMatList.SetItemText(i, 1, MaterialName.c_str());
				i++;
			}
		}	
	}

	mColorList.Invalidate();
	mMatList.Invalidate();
}

//显示对应的颜色
void CMaterialEdit::Display(Ogre::ColourValue &cv, _ColorType ct)
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
	sprintf(a, "%1.3f", cv.r);
	mColorList.SetItemText(i, 1, a);
	sprintf(a, "%1.3f", cv.g);
	mColorList.SetItemText(i, 2, a);
	sprintf(a, "%1.3f", cv.b);
	mColorList.SetItemText(i, 3, a);
	sprintf(a, "%1.3f", cv.a);
	mColorList.SetItemText(i, 4, a);
}

//单击某个SubMesh
void CMaterialEdit::OnNMClickMateriallist(NMHDR *pNMHDR, LRESULT *pResult)
{
	int tItem, tSubItem;
	CRect tSubItemRect;

	if(mMatList.GetDClickedItem(tItem, tSubItem, tSubItemRect) && CActorEditorMainDlg::g_pActorEntity != NULL)
	{
		if(tItem > mMatList.GetItemCount() || tItem < 0)
			return;

		CString sText = mMatList.GetItemText(tItem, 0);
		if(!sText.IsEmpty())
		{
			Ogre::SubEntity *se = CActorEditorMainDlg::g_pActorEntity->getSubEntity(sText.GetString());
			m_pMat = se->getMaterial().getPointer();
			sText = mMatList.GetItemText(tItem, 1);
			SetDlgItemText(IDC_MatName, sText.GetString());

			Ogre::ColourValue c = m_pMat->getTechnique(0)->getPass(0)->getAmbient();
			Display(c, AMBIENT);

			c = m_pMat->getTechnique(0)->getPass(0)->getDiffuse();
			Display(c, DIFFUSE);

			c = m_pMat->getTechnique(0)->getPass(0)->getSpecular();
			Display(c, SPECULAR);

			c = m_pMat->getTechnique(0)->getPass(0)->getSelfIllumination();
			Display(c, EMISSIVE);
		}
	}

	*pResult = 0;
}

void CMaterialEdit::OnNMDblclkColorlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	int tItem, tSubItem;
	CRect tSubItemRect;
	if(mColorList.GetDClickedItem(tItem, tSubItem, tSubItemRect) && CActorEditorMainDlg::g_pActorEntity != NULL)
	{
		if(tItem > mColorList.GetItemCount() || tItem < 0)
			return;

		DWORD c = 0;
			
		unsigned char a = (unsigned char)(atof(mColorList.GetItemText(tItem, 4)) * 255.0f);
		unsigned char b = (unsigned char)(atof(mColorList.GetItemText(tItem, 3)) * 255.0f);
		unsigned char g = (unsigned char)(atof(mColorList.GetItemText(tItem, 2)) * 255.0f);
		unsigned char r = (unsigned char)(atof(mColorList.GetItemText(tItem, 1)) * 255.0f);
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
		switch(tItem)
		{
		case 0:
			m_pMat->getTechnique(0)->getPass(0)->setAmbient(cv);
			Display(cv, AMBIENT);
			break;
		case 1:
			m_pMat->getTechnique(0)->getPass(0)->setDiffuse(cv);
			Display(cv, DIFFUSE);
			break;
		case 2:
			m_pMat->getTechnique(0)->getPass(0)->setSelfIllumination(cv);
			Display(cv, EMISSIVE);
			break;
		case 3:
			m_pMat->getTechnique(0)->getPass(0)->setDiffuse(cv);
			Display(cv, DIFFUSE);
			break;
		}
	}
	mColorList.Invalidate();
	*pResult = 0;
}