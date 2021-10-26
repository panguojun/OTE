// CreateNewEntity.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "CreateNewEntity.h"
#include "OTEQTSceneManager.h"
#include "ActorEntity.h"
#include "tinyxml.h"
#include "ActorEditorMainDlg.h"


extern OTE::COTEQTSceneManager*			gSceneMgr;			// 场景管理器

extern OTE::CActorEntity*				g_pActorEntity;

CreateNewEntity							CreateNewEntity::s_Inst;

// CreateNewEntity 对话框
IMPLEMENT_DYNAMIC(CreateNewEntity, CDialog)
CreateNewEntity::CreateNewEntity(CWnd* pParent /*=NULL*/)
	: CDialog(CreateNewEntity::IDD, pParent)
{
}

CreateNewEntity::~CreateNewEntity()
{
}

void CreateNewEntity::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CreateNewEntity, CDialog)
	ON_BN_CLICKED(IDC_BrowserMesh, OnBnClickedBrowsermesh)
	ON_BN_CLICKED(IDC_BrowserSkeleton, OnBnClickedBrowserskeleton)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

//清除对话框
void CreateNewEntity::clear()
{		
	SetDlgItemText(IDC_EntityName,"");
	SetDlgItemText(IDC_MeshFile,"");
	SetDlgItemText(IDC_SkeletonFile,"");
}


// CreateNewEntity 消息处理程序

//打开Mesh文件
void CreateNewEntity::OnBnClickedBrowsermesh()
{
	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	CFileDialog* tpFileDialog=new CFileDialog(TRUE, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"mesh文件(*.mesh)|*.mesh|All Files (*.*)|*.*|",
		 NULL 
		); 
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);
	
	if (nResponse == IDOK)
	{
		SetDlgItemText(IDC_MeshFile,tpFileDialog->GetFileName());
	}
}
//打开Skeleton文件
void CreateNewEntity::OnBnClickedBrowserskeleton()
{
	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	CFileDialog* tpFileDialog=new CFileDialog(TRUE, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Skeleton文件(*.skeleton)|*.skeleton|All Files (*.*)|*.*|",
		 NULL 
		); 
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);
	
	if (nResponse == IDOK)
	{
		SetDlgItemText(IDC_SkeletonFile,tpFileDialog->GetFileName());
	}
}

//确定按钮
void CreateNewEntity::OnBnClickedOk()
{
	char name[128];
	char MeshFile[256];
	char SkeletonFile[256];
	GetDlgItemText(IDC_EntityName, name, 128);
	GetDlgItemText(IDC_MeshFile, MeshFile, 256);
	GetDlgItemText(IDC_SkeletonFile, SkeletonFile, 256);

	if(strlen(name) < 1)
	{
		MessageBox("实体名不能为空!");
		return;
	}

	TiXmlDocument *xmlDoc = CActorEditorMainDlg::s_Inst.XmlDoc;
	
	//如果Mesh文件为空则失败
	if(strlen(MeshFile) < 5)
	{
		MessageBox("创建实体时Mesh不能为空!");
		return;
	}
    //检查是否有同名Entity
	TiXmlElement *elem = xmlDoc->RootElement()->FirstChildElement("Entity");
	while(elem)
	{
		if(strcmp(elem->Attribute("Name"), name) == 0)
		{
			MessageBox("已经存在同名实体！");
			return;
		}
		elem = elem->NextSiblingElement();
	}

	//产生新的ID
	int id = 1;
	elem = xmlDoc->RootElement()->FirstChildElement("Entity");
	while(elem)
	{
		if(id == atoi(elem->Attribute("Id")))
		{
			id++;
			elem = xmlDoc->RootElement()->FirstChildElement("Entity");
			continue;
		}

		elem = elem->NextSiblingElement();
	}

	//插入新记录
	elem = new TiXmlElement("Entity");
	elem->SetAttribute("Id", id);
	elem->SetAttribute("Name", name);
	elem->SetAttribute("MeshFile", MeshFile);
	elem->SetAttribute("SkeletonFile", SkeletonFile);

	xmlDoc->RootElement()->InsertEndChild(*elem);

	CActorEditorMainDlg::s_Inst.UpdataResource();

	OnOK();
}
