// MeshEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "MeshEditDlg.h"
#include "ActorEditorMainDlg.h"
#include "OTEActorEquipmentMgr.h"
#include "OTEEntityRender.h"
#include "OTEObjDataManager.h"

// ---------------------------------------
using namespace Ogre;
using namespace OTE;

// ---------------------------------------
CMeshEditDlg CMeshEditDlg::s_Inst;

IMPLEMENT_DYNAMIC(CMeshEditDlg, CDialog)
CMeshEditDlg::CMeshEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMeshEditDlg::IDD, pParent)
{
}

CMeshEditDlg::~CMeshEditDlg()
{
}

void CMeshEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MeshList, m_EquipmentList);
}

// ---------------------------------------
BEGIN_MESSAGE_MAP(CMeshEditDlg, CDialog)
	ON_BN_CLICKED(IDC_Hide, OnBnClickedHide)
	ON_LBN_SELCHANGE(IDC_MeshList, OnLbnSelchangeMeshlist)
	ON_BN_CLICKED(IDC_Show, OnBnClickedShow)
	ON_BN_CLICKED(IDC_Export, OnBnClickedExport)
	ON_BN_CLICKED(IDC_Import, OnBnClickedImport)
	ON_BN_CLICKED(IDC_Delete, OnBnClickedDelete)
	ON_BN_CLICKED(IDC_Replace, OnBnClickedReplace)
	ON_BN_CLICKED(IDC_Save, OnBnClickedSave)
	ON_BN_CLICKED(IDC_ReName, OnBnClickedRename)
	ON_BN_CLICKED(IDC_SetMaterial, OnBnClickedSetmaterial)
END_MESSAGE_MAP()

// ---------------------------------------
// 闪烁
//
//void OnRenderSubMesh()
//{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	if(!CActorEditorMainDlg::s_pActorEntity ||
//		!CMeshEditDlg::s_Inst.m_hWnd || 
//		!CMeshEditDlg::s_Inst.IsWindowVisible())
//		return;
//
//	CString itemName;
//	if(CMeshEditDlg::s_Inst.m_EquipmentList.GetCount() > 0)
//		CMeshEditDlg::s_Inst.m_EquipmentList.GetText(CMeshEditDlg::s_Inst.m_EquipmentList.GetCurSel(), itemName);
//
//	if(!itemName.IsEmpty())
//	{	
//		static DWORD sSubTime = 0;	
//		static int sCount = 0;
//		static bool sIsVisible = false;
//		if(::GetTickCount() - sSubTime > 100)
//		{					
//			sSubTime = ::GetTickCount();		
//			
//			if((sCount ++) >= 1 )
//			{							
//				CActorEditorMainDlg::s_pActorEntity->SetSubEntityVisible(itemName.GetString(), sIsVisible);
//				sIsVisible = !sIsVisible;
//
//				sCount = 0;
//			}
//		}
//	}
//	
//
//}

// ---------------------------------------
void CMeshEditDlg::UpdateList()//窗体更新
{
	m_EquipmentList.ResetContent();
	
	GetDlgItem(IDC_Hide)->EnableWindow(false);
	GetDlgItem(IDC_Show)->EnableWindow(false);
	GetDlgItem(IDC_Export)->EnableWindow(false);
	GetDlgItem(IDC_Import)->EnableWindow(false);
	GetDlgItem(IDC_Replace)->EnableWindow(false);
	GetDlgItem(IDC_Delete)->EnableWindow(false);
	GetDlgItem(IDC_Save)->EnableWindow(false);
	GetDlgItem(IDC_ReName)->EnableWindow(false);
	GetDlgItem(IDC_SetMaterial)->EnableWindow(false);
	SetDlgItemText(IDC_Material, "");
	SetDlgItemText(IDC_Name, "");
	
	if(CActorEditorMainDlg::s_pActorEntity != NULL)
	{
		/*Ogre::Mesh::SubMeshNameMap nm = CActorEditorMainDlg::s_pActorEntity->getMesh()->getSubMeshNameMap();
		Ogre::Mesh::SubMeshNameMap::iterator it = nm.begin();
		for(; it != nm.end(); ++ it)
		{
			m_EquipmentList.AddString(it->first.c_str());
		}	*/
		HashMap<std::string, std::string>::iterator it = CActorEditorMainDlg::s_pActorEntity->m_EquipmentMapEx.begin();
		while(it != CActorEditorMainDlg::s_pActorEntity->m_EquipmentMapEx.end())
		{	
			m_EquipmentList.AddString(it->first.c_str());
			++ it;
		}

		GetDlgItem(IDC_Hide)->EnableWindow(false);
		GetDlgItem(IDC_Show)->EnableWindow(false);
		GetDlgItem(IDC_Export)->EnableWindow(false);
		GetDlgItem(IDC_Import)->EnableWindow(true);
		GetDlgItem(IDC_Replace)->EnableWindow(false);
		GetDlgItem(IDC_Delete)->EnableWindow(false);
		GetDlgItem(IDC_Save)->EnableWindow(true);	
	}	

	m_EquipmentList.Invalidate();
}

// --------------------------------------------------------
BOOL CMeshEditDlg::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();
	
	//SCENE_MGR->AddListener("渲染回调", OnRenderSubMesh);	

	return ret;
}

// -------------------------------------------
void CMeshEditDlg::OnEndPlugin()
{
	//SCENE_MGR->RemoveListener(OnRenderSubMesh);

}

// ---------------------------------------
//隐藏SubMesh
void CMeshEditDlg::OnBnClickedHide()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;
	
	// hide selected
	CString itemName;
	m_EquipmentList.GetText(m_EquipmentList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{
		COTEActorEquipmentMgr::ShowEquipment(
			CActorEditorMainDlg::s_pActorEntity, 
			itemName.GetString(), false);
		GetDlgItem(IDC_Hide)->EnableWindow(false);
		GetDlgItem(IDC_Show)->EnableWindow(true);
	}
}
// ---------------------------------------
//Mesh列表选择变更
void CMeshEditDlg::OnLbnSelchangeMeshlist()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;
	
	// hide selected
	CString itemName;
	m_EquipmentList.GetText(m_EquipmentList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{
		COTESubEntity* se = COTEActorEquipmentMgr::GetSubEntity(
									CActorEditorMainDlg::s_pActorEntity, 
									itemName.GetString());
		if(se && se->isVisible())
		{
			GetDlgItem(IDC_Hide)->EnableWindow(true);
			GetDlgItem(IDC_Show)->EnableWindow(false);
			GetDlgItem(IDC_Export)->EnableWindow(true);
			GetDlgItem(IDC_Import)->EnableWindow(true);
			GetDlgItem(IDC_Replace)->EnableWindow(true);
			GetDlgItem(IDC_Delete)->EnableWindow(true);
		}
		else
		{
			GetDlgItem(IDC_Hide)->EnableWindow(false);
			GetDlgItem(IDC_Show)->EnableWindow(true);
			GetDlgItem(IDC_Export)->EnableWindow(true);
			GetDlgItem(IDC_Import)->EnableWindow(true);
			GetDlgItem(IDC_Replace)->EnableWindow(true);
			GetDlgItem(IDC_Delete)->EnableWindow(true);
		}
		GetDlgItem(IDC_ReName)->EnableWindow(true);
		GetDlgItem(IDC_SetMaterial)->EnableWindow(true);
		SetDlgItemText(IDC_Material, se->getMaterialName().c_str());
		SetDlgItemText(IDC_Name, 
			COTEActorEquipmentMgr::GetSubEntityName(
			CActorEditorMainDlg::s_pActorEntity,
			itemName.GetString()).c_str()
			);
	}
}
// ---------------------------------------
//显示SubMesh
void CMeshEditDlg::OnBnClickedShow()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;
	
	// show selected
	CString itemName;
	m_EquipmentList.GetText(m_EquipmentList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{
		COTEActorEquipmentMgr::ShowEquipment(
			CActorEditorMainDlg::s_pActorEntity,
			itemName.GetString(), true);
		GetDlgItem(IDC_Hide)->EnableWindow(true);
		GetDlgItem(IDC_Show)->EnableWindow(false);
	}
}
// ---------------------------------------
//导出SubMesh
void CMeshEditDlg::OnBnClickedExport()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;
	
	// export selected
	CString itemName;
	m_EquipmentList.GetText(m_EquipmentList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{
		CHAR curDir[256];
		::GetCurrentDirectory(256, curDir);
		CFileDialog* tpFileDialog=new CFileDialog(false, // TRUE for FileOpen, FALSE for FileSaveAs
			NULL,
			NULL,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"子Mesh文件 (*.subMesh)|*.subMesh|All Files (*.*)|*.*|",
			NULL 
			); 
		int nResponse =tpFileDialog->DoModal();
		::SetCurrentDirectory(curDir);

		if (nResponse == IDOK)
		{ 
			char filename[256];
			strcpy(filename, tpFileDialog->GetPathName());

			if(strlen(tpFileDialog->GetFileExt()) == 0)
				strcat(filename, ".subMesh");

			COTEActorEquipmentMgr::ExportSubMesh(CActorEditorMainDlg::s_pActorEntity, itemName.GetString(), filename);
		}		

		delete tpFileDialog; 	
	}
}
// ---------------------------------------
//导入SubMesh
void CMeshEditDlg::OnBnClickedImport()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;

	char filename[256];
	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"子Mesh文件 (*.subMesh)|*.subMesh|All Files (*.*)|*.*|",
		NULL 
		); 
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);
	
	if (nResponse == IDOK)
	{ 		
		strcpy(filename, tpFileDialog->GetFileName());

		COTEActorEquipmentMgr::ShowEquipment(
			CActorEditorMainDlg::s_pActorEntity, 
			filename);		
	}		

	delete tpFileDialog; 	

	UpdateList();

	std::string resFile = CActorEditorMainDlg::s_pActorEntity->m_ResFile;
	resFile.replace(resFile.find(".mesh"), 5, ".obj"); 
	OTE::ObjData_t* odt = COTEObjDataManager::GetInstance()->GetObjData(resFile);
	if(odt)
	{
		bool isFound = false;
		std::list<std::string>::iterator it = odt->SubMeshList.begin();
		while(it != odt->SubMeshList.end())
		{
			if((*it) == filename)
			{
				isFound = true;			
				break;
			}
			++ it;
		}

		if(!isFound)
		{
			odt->SubMeshList.push_back(filename);
		}
	}
}
// ---------------------------------------
//删除SubMesh
void CMeshEditDlg::OnBnClickedDelete()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;
	// delete selected
	CString itemName;
	m_EquipmentList.GetText(m_EquipmentList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{	
		COTEActorEquipmentMgr::RemoveSubMesh(
			CActorEditorMainDlg::s_pActorEntity, itemName.GetString());
		COTEActorEquipmentMgr::ShowEquipment(
			CActorEditorMainDlg::s_pActorEntity, itemName.GetString());
	}

	UpdateList();
				
	std::string resFile = CActorEditorMainDlg::s_pActorEntity->m_ResFile;
	resFile.replace(resFile.find(".mesh"), 5, ".obj"); 
	OTE::ObjData_t* odt = COTEObjDataManager::GetInstance()->GetObjData(resFile);
	if(odt)
	{
		std::list<std::string>::iterator it = odt->SubMeshList.begin();
		while(it != odt->SubMeshList.end())
		{
			if((*it) == itemName.GetString())
			{
				odt->SubMeshList.erase(it);				
				break;
			}
			++ it;
		}
	}
}
// ---------------------------------------
//替换SubMesh
void CMeshEditDlg::OnBnClickedReplace()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;
	// replace selected
	CString itemName;
	m_EquipmentList.GetText(m_EquipmentList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{
		CHAR curDir[256];
		::GetCurrentDirectory(256, curDir);
		CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
			NULL,
			NULL,
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"模型文件 (*.mesh)|*.mesh|模型文件 (*.obj)|*.obj|All Files (*.*)|*.*|",
			NULL 
			); 
		int nResponse =tpFileDialog->DoModal();
		::SetCurrentDirectory(curDir);

		if (nResponse == IDOK)
		{ 
			char filename[256];
			strcpy(filename, tpFileDialog->GetFileName());

			COTEActorEquipmentMgr::ShowEquipment(
				CActorEditorMainDlg::s_pActorEntity, 
				itemName.GetString()
				);
		}		

		delete tpFileDialog; 	
	}
	
	UpdateList();


}
// ---------------------------------------
//保存对Mesh的修改
void CMeshEditDlg::OnBnClickedSave()
{
	//if(!CActorEditorMainDlg::s_pActorEntity || strlen(CActorEditorMainDlg::s_Inst.OpenFile) < 1)
	//	return;

	//CHAR curDir[256];
	//::GetCurrentDirectory(256, curDir);
	//CFileDialog* tpFileDialog=new CFileDialog(false, // TRUE for FileOpen, FALSE for FileSaveAs
	//	NULL,
	//	NULL,
	//	OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
	//	"Mesh文件 (*.Mesh)|*.Mesh|All Files (*.*)|*.*|",
	//	NULL 
	//	); 
	//int nResponse =tpFileDialog->DoModal();
	//::SetCurrentDirectory(curDir);

	//if (nResponse == IDOK)
	//{ 
	//	CString sel = tpFileDialog->GetPathName();
	//	if(sel.Find(".Mesh") == -1 && sel.Find(".mesh") == -1)
	//		sel += ".mesh";
	//	
	//	COTEActorEquipmentMgr::ExportMesh(CActorEditorMainDlg::s_pActorEntity, sel.GetString());
	//}

	//delete tpFileDialog;
}
// ---------------------------------------
//更改SubMesh名称
void CMeshEditDlg::OnBnClickedRename()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;

	// replace selected
	CString itemName;
	m_EquipmentList.GetText(m_EquipmentList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{
		char NewName[256];
		GetDlgItemText(IDC_Name, NewName, 256);
		if(strlen(NewName) > 0 && itemName != NewName)
		{
			COTEActorEquipmentMgr::SetSubMeshName(
				CActorEditorMainDlg::s_pActorEntity,
				itemName.GetString(), NewName);
			UpdateList();
		}
	}
}
// ---------------------------------------
//更改材质
void CMeshEditDlg::OnBnClickedSetmaterial()
{
	if(!CActorEditorMainDlg::s_pActorEntity)
		return;

	CString itemName;
	m_EquipmentList.GetText(m_EquipmentList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{
		COTESubEntity* se = COTEActorEquipmentMgr::GetSubEntity(
									CActorEditorMainDlg::s_pActorEntity,
									itemName.GetString());
		if(se)
		{
			GetDlgItemText(IDC_Material, itemName);
			if(!(itemName.IsEmpty() || Ogre::MaterialManager::getSingleton().getByName(itemName.GetString()).isNull()))
			{
				se->setMaterialName(itemName.GetString());
				UpdateList();
			}
		}
	}
}
