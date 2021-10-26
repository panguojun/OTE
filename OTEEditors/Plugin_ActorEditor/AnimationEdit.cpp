// AnimationEdit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "AnimationEdit.h"
#include "ActorEditorMainDlg.h"
#include "ActionEdit.h"
#include "OTEInterfaceInclude.h"
#include "OTEObjDataManager.h"

USING_OTE

CAnimationEdit CAnimationEdit::s_Inst;

// CAnimationEdit �Ի���
IMPLEMENT_DYNAMIC(CAnimationEdit, CDialog)
CAnimationEdit::CAnimationEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CAnimationEdit::IDD, pParent)
{
}

CAnimationEdit::~CAnimationEdit()
{
}

void CAnimationEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AnimationList, m_AnimList);
}


BEGIN_MESSAGE_MAP(CAnimationEdit, CDialog)
	ON_LBN_SELCHANGE(IDC_AnimationList, OnLbnSelchangeAnimationlist)
	ON_LBN_DBLCLK(IDC_AnimationList, OnLbnDblclkAnimationlist)
	ON_BN_CLICKED(IDC_Play, OnBnClickedPlay)
	ON_BN_CLICKED(IDC_Stop, OnBnClickedStop)
	ON_BN_CLICKED(IDC_Add, OnBnClickedAdd)
	ON_BN_CLICKED(IDC_Del, OnBnClickedDel)
	//ON_BN_CLICKED(IDC_AddToAction, OnBnClickedAddtoaction)
END_MESSAGE_MAP()

// ���ڳ�ʼ��

BOOL CAnimationEdit::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();

	return ret;
}

//���´���
void CAnimationEdit::UpdataAnimation()
{
	m_AnimList.ResetContent();

	GetDlgItem(IDC_Play)->EnableWindow(false);
	GetDlgItem(IDC_Stop)->EnableWindow(false);
	GetDlgItem(IDC_Add)->EnableWindow(false);
	GetDlgItem(IDC_Del)->EnableWindow(false);
	//GetDlgItem(IDC_AddToAction)->EnableWindow(false);
	
	if(CActorEditorMainDlg::s_pActorEntity != NULL && CActorEditorMainDlg::s_pActorEntity->hasSkeleton())
	{
		//Ogre::AnimationStateSet* as = CActorEditorMainDlg::s_pActorEntity->getAllAnimationStates();
		//if(as != NULL)
		//{
		//	Ogre::AnimationStateSet::iterator it = as->begin();
		//	Ogre::AnimationStateSet::iterator end = as->end();
		//	if(m_AnimList.m_hWnd)
		//	{
		//		for(; it != end; ++ it)
		//		{
		//			// �����б�
		//			m_AnimList.AddString((*it).second.getAnimationName().c_str());
		//		}
		//	}
		//}

		short numanis = CActorEditorMainDlg::s_pActorEntity->getMesh()->getSkeleton()->getNumAnimations();
				
		for(short i = 0; i < numanis; i ++)
		{
			m_AnimList.AddString(
				CActorEditorMainDlg::s_pActorEntity->getMesh()->getSkeleton()->getAnimation(i)->getName().c_str()
				);			
		}

		GetDlgItem(IDC_Add)->EnableWindow(true);
	}

	m_AnimList.Invalidate();
}

// CAnimationEdit ��Ϣ�������
//ѡ��ĳһ����
void CAnimationEdit::OnLbnSelchangeAnimationlist()
{	
	if(!CActorEditorMainDlg::s_pActorEntity || !(CActorEditorMainDlg::s_pActorEntity->hasSkeleton()))
		return;

	CString itemName;
	m_AnimList.GetText(m_AnimList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{
		if(CActorEditorMainDlg::s_pActorEntity->m_pCurAnimState &&
			strcmp(CActorEditorMainDlg::s_pActorEntity->m_pCurAnimState->getAnimationName().c_str(),itemName.GetString()) == 0 &&
			CActorEditorMainDlg::s_pActorEntity->m_pCurAnimState->getEnabled())
		{
			GetDlgItem(IDC_Del)->EnableWindow(false);	
			GetDlgItem(IDC_Play)->EnableWindow(false);
			GetDlgItem(IDC_Stop)->EnableWindow(true);
			//GetDlgItem(IDC_AddToAction)->EnableWindow(false);
		}
		else
		{
			GetDlgItem(IDC_Del)->EnableWindow(true);
			GetDlgItem(IDC_Play)->EnableWindow(true);
			GetDlgItem(IDC_Stop)->EnableWindow(false);	
			//GetDlgItem(IDC_AddToAction)->EnableWindow(true);
		}
	}
}

//˫��ĳһ����
void CAnimationEdit::OnLbnDblclkAnimationlist()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !(CActorEditorMainDlg::s_pActorEntity->hasSkeleton()))
		return;

	CString itemName;
	if(m_AnimList.GetCurSel() == -1)
		return;

	m_AnimList.GetText(m_AnimList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{
		CActorEditorMainDlg::s_pActorEntity->SetCurrentAnim((LPCTSTR)itemName);
		if(CActorEditorMainDlg::s_pActorEntity->m_pCurAnimState)
			CActorEditorMainDlg::s_pActorEntity->m_pCurAnimState->setTimePosition(0);

		GetDlgItem(IDC_Del)->EnableWindow(false);	
		GetDlgItem(IDC_Play)->EnableWindow(false);
		GetDlgItem(IDC_Stop)->EnableWindow(true);
		//GetDlgItem(IDC_AddToAction)->EnableWindow(false);
	}
}

//���Ŷ���
void CAnimationEdit::OnBnClickedPlay()
{
	OnLbnDblclkAnimationlist();
}

//ֹͣ����
void CAnimationEdit::OnBnClickedStop()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !(CActorEditorMainDlg::s_pActorEntity->hasSkeleton()))
		return;

	CActorEditorMainDlg::s_pActorEntity->EnalbeAnim(false);

	GetDlgItem(IDC_Del)->EnableWindow(true);	
	GetDlgItem(IDC_Play)->EnableWindow(true);
	GetDlgItem(IDC_Stop)->EnableWindow(false);
//	GetDlgItem(IDC_AddToAction)->EnableWindow(true);
}

// ��Ӷ���
void CAnimationEdit::OnBnClickedAdd()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !(CActorEditorMainDlg::s_pActorEntity->hasSkeleton()))
		return;

	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	CWinApp* pApp = AfxGetApp();   

	static CString path;
	path = pApp->GetProfileString("OTE�ļ�·������", "ѡ�񶯻�");

	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,
		"�����ļ�(*.ani)|*.ani|All Files (*.*)|*.*|",						
		NULL 
		); 
	tpFileDialog->m_ofn.lpstrInitialDir = path;
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{ 	
		CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
																				tpFileDialog->GetFileName().GetLength());   
		pApp->WriteProfileString("OTE�ļ�·������", "ѡ�񶯻�", strPath);


		char filename[256];
		GetPrivateProfileString("ResConfig","AnimationFile","\0",
										filename,sizeof(filename),".\\OTESettings.cfg");

		POSITION   pos  =  tpFileDialog->GetStartPosition(); 
		Ogre::Animation* ani = NULL;
		CString   strFilename;
		while(pos) 
		{ 				 
			CString szFilePathName = tpFileDialog->GetNextPathName(pos);
			strFilename   =   szFilePathName.Mid(szFilePathName.ReverseFind('\\') + 1);
			Ogre::DataStreamPtr stream = 
						Ogre::ResourceGroupManager::getSingleton().openResource(strFilename.GetString());

			Ogre::SkeletonSerializer BINssr;
			ani = BINssr.readAnimation(stream, CActorEditorMainDlg::s_pActorEntity->getSkeleton());	

			std::string resFile = CActorEditorMainDlg::s_pActorEntity->m_ResFile;
			resFile.replace(resFile.find(".mesh"), 5, ".obj"); 
			OTE::ObjData_t* odt = COTEObjDataManager::GetInstance()->GetObjData(resFile);
			if(odt)
			{
				odt->AniFileList[ani->getName()] = strFilename.GetString();			
			}

		} 	

		CActorEditorMainDlg::s_pActorEntity->getMesh()->_initAnimationState(CActorEditorMainDlg::s_pActorEntity->getAllAnimationStates());

		UpdataAnimation();			
	}

}

// ɾ������
void CAnimationEdit::OnBnClickedDel()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !(CActorEditorMainDlg::s_pActorEntity->hasSkeleton()))
		return;

	CString itemName;
	m_AnimList.GetText(m_AnimList.GetCurSel(), itemName);
	if(!itemName.IsEmpty())
	{
		CActorEditorMainDlg::s_pActorEntity->getSkeleton()->removeAnimation(itemName.GetString());
		/*CActorEditorMainDlg::s_pActorEntity->getMesh()->_notifySkeleton(
			Ogre::SkeletonPtr(CActorEditorMainDlg::s_pActorEntity->getSkeleton())
			);*/
		
		UpdataAnimation();

		std::string resFile = CActorEditorMainDlg::s_pActorEntity->m_ResFile;
		resFile.replace(resFile.find(".mesh"), 5, ".obj"); 
		OTE::ObjData_t* odt = COTEObjDataManager::GetInstance()->GetObjData(resFile);
		if(odt)
		{
			std::map<std::string, std::string>::iterator it = odt->AniFileList.find(itemName.GetString());
			if(it != odt->AniFileList.end())
			{
				odt->AniFileList.erase(it);				
			}
		}
	}
}
//��������ӵ���ǰ�༭���ж���
void CAnimationEdit::OnBnClickedAddtoaction()
{
	
}
