// CreateNewEntity.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "CreateNewEntity.h"
#include "OTEQTSceneManager.h"
#include "ActorEntity.h"
#include "tinyxml.h"
#include "ActorEditorMainDlg.h"


extern OTE::COTEQTSceneManager*			gSceneMgr;			// ����������

extern OTE::CActorEntity*				g_pActorEntity;

CreateNewEntity							CreateNewEntity::s_Inst;

// CreateNewEntity �Ի���
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

//����Ի���
void CreateNewEntity::clear()
{		
	SetDlgItemText(IDC_EntityName,"");
	SetDlgItemText(IDC_MeshFile,"");
	SetDlgItemText(IDC_SkeletonFile,"");
}


// CreateNewEntity ��Ϣ�������

//��Mesh�ļ�
void CreateNewEntity::OnBnClickedBrowsermesh()
{
	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	CFileDialog* tpFileDialog=new CFileDialog(TRUE, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"mesh�ļ�(*.mesh)|*.mesh|All Files (*.*)|*.*|",
		 NULL 
		); 
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);
	
	if (nResponse == IDOK)
	{
		SetDlgItemText(IDC_MeshFile,tpFileDialog->GetFileName());
	}
}
//��Skeleton�ļ�
void CreateNewEntity::OnBnClickedBrowserskeleton()
{
	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	CFileDialog* tpFileDialog=new CFileDialog(TRUE, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Skeleton�ļ�(*.skeleton)|*.skeleton|All Files (*.*)|*.*|",
		 NULL 
		); 
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);
	
	if (nResponse == IDOK)
	{
		SetDlgItemText(IDC_SkeletonFile,tpFileDialog->GetFileName());
	}
}

//ȷ����ť
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
		MessageBox("ʵ��������Ϊ��!");
		return;
	}

	TiXmlDocument *xmlDoc = CActorEditorMainDlg::s_Inst.XmlDoc;
	
	//���Mesh�ļ�Ϊ����ʧ��
	if(strlen(MeshFile) < 5)
	{
		MessageBox("����ʵ��ʱMesh����Ϊ��!");
		return;
	}
    //����Ƿ���ͬ��Entity
	TiXmlElement *elem = xmlDoc->RootElement()->FirstChildElement("Entity");
	while(elem)
	{
		if(strcmp(elem->Attribute("Name"), name) == 0)
		{
			MessageBox("�Ѿ�����ͬ��ʵ�壡");
			return;
		}
		elem = elem->NextSiblingElement();
	}

	//�����µ�ID
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

	//�����¼�¼
	elem = new TiXmlElement("Entity");
	elem->SetAttribute("Id", id);
	elem->SetAttribute("Name", name);
	elem->SetAttribute("MeshFile", MeshFile);
	elem->SetAttribute("SkeletonFile", SkeletonFile);

	xmlDoc->RootElement()->InsertEndChild(*elem);

	CActorEditorMainDlg::s_Inst.UpdataResource();

	OnOK();
}
