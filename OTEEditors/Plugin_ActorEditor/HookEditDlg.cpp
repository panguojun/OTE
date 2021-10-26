// HookEditDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "ActorEditorMainDlg.h"
#include "HookEditDlg.h"
#include "BrowserBone.h"
#include "TransformManager.h"
#include "OTEActorHookMgr.h"
#include "OTEEntityRender.h"
#include "OgreTagPoint.h"

using namespace Ogre;
using namespace OTE;

// ----------------------------------------
CHookEditDlg CHookEditDlg::s_Inst;

// ----------------------------------------
IMPLEMENT_DYNAMIC(CHookEditDlg, CDialog)
CHookEditDlg::CHookEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHookEditDlg::IDD, pParent)
{
	m_pActorHookList = NULL;
	TestObjFile = "";
	mHook = NULL;
	m_WaitForPickBone = false;
}

CHookEditDlg::~CHookEditDlg()
{
}

void CHookEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PotHookList, m_HookList);
	DDX_Control(pDX, IDC_CHECK2, m_IsShowBone);
}


BEGIN_MESSAGE_MAP(CHookEditDlg, CDialog)
	ON_BN_CLICKED(IDC_LoadTest, OnBnClickedLoadtest)
	ON_LBN_SELCHANGE(IDC_PotHookList, OnLbnSelchangePothooklist)
	ON_BN_CLICKED(IDC_Reset, OnBnClickedReset)
	ON_BN_CLICKED(IDC_Save, OnBnClickedSave)
	ON_BN_CLICKED(IDC_Del, OnBnClickedDel)
	ON_BN_CLICKED(IDC_Add, OnBnClickedAdd)
	ON_BN_CLICKED(IDC_DelTest, OnBnClickedDeltest)
	ON_BN_CLICKED(IDC_CHECK2, OnBnClickedCheck2)
END_MESSAGE_MAP()


// -------------------------------------------
// ��Ⱦ�ص�����

void OnSceneRender2(int ud)
{
	if(!CHookEditDlg::s_Inst.m_hWnd || !CHookEditDlg::s_Inst.IsWindowVisible())
		return;

	// ��ʾ�Ǹ�

	if(CActorEditorMainDlg::s_pActorEntity && 
		CHookEditDlg::s_Inst.m_IsShowBone.GetCheck() == BST_CHECKED)
	{
		COTEEntityBoneRender::RenderEntityBones(CActorEditorMainDlg::s_pActorEntity);
	}

}

// -----------------------------------------
//���¹ҵ�
void CHookEditDlg::UpdateHookList()
{
	m_HookList.ResetContent();

	GetDlgItem(IDC_Reset)->EnableWindow(false);
	GetDlgItem(IDC_Save)->EnableWindow(false);
	GetDlgItem(IDC_Add)->EnableWindow(false);
	GetDlgItem(IDC_Del)->EnableWindow(false);
	GetDlgItem(IDC_LoadTest)->EnableWindow(false);
	GetDlgItem(IDC_DelTest)->EnableWindow(false);
	GetDlgItem(IDC_Name)->EnableWindow(false);
	
	SetDlgItemText(IDC_Postion, "X:\tY:\tZ:");
	SetDlgItemText(IDC_Rotation, "X:\tY:\tZ:\tW:");
	SetDlgItemText(IDC_Scale, "X:\tY:\tZ:");
	SetDlgItemText(IDC_Name, "");

	if(CActorEditorMainDlg::s_pActorEntity != NULL && CActorEditorMainDlg::s_pActorEntity->hasSkeleton())
	{
		m_pActorHookList = &(CActorEditorMainDlg::s_pActorEntity->m_PotHookList);
		HashMap<std::string, OTE::Hook_t*>::iterator i = m_pActorHookList->begin();
		while(i != m_pActorHookList->end())
		{
			m_HookList.AddString(i->first.c_str());
			i++;
		}

		GetDlgItem(IDC_Add)->EnableWindow(true);
		GetDlgItem(IDC_LoadTest)->EnableWindow(true);
	}

	if(!TestObjFile.empty())
		GetDlgItem(IDC_DelTest)->EnableWindow(true);
	m_HookList.Invalidate();
}

// -----------------------------------------
void CHookEditDlg::OnSceneLButtonDown(float x, float y)
{
	// ��ѡ�Ǹ�

	if(m_WaitForPickBone &&
		m_IsShowBone.GetCheck() == BST_CHECKED)
	{
		Ogre::Ray tCameraRay = SCENE_CAM->getCameraToViewportRay((float)x, (float)y);	

		Node* n = COTEEntityBonePicker::PickEntityBones(CActorEditorMainDlg::s_pActorEntity, &tCameraRay);
		if(n)
		{			
			COTEEntityBoneRender::SetSelectedBoneName(n->getName());			
			
			COTEActorHookMgr::AddHook(
				CActorEditorMainDlg::s_pActorEntity, 
					n->getName(), n->getName());

			UpdateHookList();
			Save();

			m_WaitForPickBone = false;
			
		}
		return;
	}
}
// -----------------------------------------
void CHookEditDlg::OnSceneLButtonUp(float x, float y)
{
	UpdateHInfomation();
}

// -----------------------------------------
void CHookEditDlg::UpdateDummyPos()
{
	if(mHook->HookEntityList.empty())
		return;

	COTEActorEntity* e = (COTEActorEntity*)(*mHook->HookEntityList.begin());
	if(e)
	{
		CActorEditorMainDlg::s_TransMgr.SetDummyPos(
							e->getParentSceneNode()->getPosition());
	}
}

// -----------------------------------------
// CHookEditDlg ��Ϣ�������
void CHookEditDlg::OnMouseMove(int x, int y)
{
	if(!CActorEditorMainDlg::s_pActorEntity || !CActorEditorMainDlg::s_pActorEntity->hasSkeleton())
		return;

	static CPoint oldPos = CPoint(x, y);	
	if((::GetKeyState(VK_LBUTTON) & 0x80 ) && mHook && (!mHook->BoneName.empty()))
	{
		Ogre::Bone* bone = CActorEditorMainDlg::s_pActorEntity->getSkeleton()->getBone(mHook->BoneName);

		Ogre::Vector2 screenMove(x - oldPos.x, y - oldPos.y);

		// movement
		if(CActorEditorMainDlg::s_TransMgr.GetEditType() == CTransformManager::ePOSITION)
		{
			Ogre::Vector3 ov, nv;	
			//ov = CActorEditorMainDlg::s_pActorEntity->getSkeleton()->getTagPoint(mHook->TagPointName)->getWorldPosition();
			ov = bone->getWorldPosition() + bone->getWorldOrientation() * mHook->OffestPos;
			Ogre::Vector3 dMove = Ogre::Vector3::ZERO;

			if(SCENE_CAM->getProjectionType() == Ogre::PT_ORTHOGRAPHIC)
			{
				// ǰ��ͼ				
				
				if((DWORD)SCENE_CAM->getUserObject() & 0xF0000000)
				{
					dMove =	Ogre::Vector3(screenMove.x, - screenMove.y, 0);								
				}

				// ����ͼ

				else if((DWORD)SCENE_CAM->getUserObject() & 0x0F000000)
				{
					dMove =	Ogre::Vector3(screenMove.x, 0, screenMove.y);							
				}

				// ����ͼ

				else if((DWORD)SCENE_CAM->getUserObject() & 0x00F00000)
				{
					dMove =	Ogre::Vector3(0, - screenMove.y, - screenMove.x);					
				}	

				dMove /= 300.0f;

			    nv = ov + dMove;
			}
			else
			{									
				CActorEditorMainDlg::s_TransMgr.GetViewportMove(
					ov, ov, Ogre::Vector2(x - oldPos.x, y - oldPos.y), nv);
				dMove = nv - ov;
			}			
			
			mHook->OffestPos += (bone->getWorldOrientation()).Inverse() * dMove;

			COTEActorHookMgr::UpdateHook(CActorEditorMainDlg::s_pActorEntity, mHook->HookName);
			//nv = CActorEditorMainDlg::s_pActorEntity->getSkeleton()->getTagPoint(mHook->TagPointName)->getWorldPosition();
			//nv = bone->getWorldPosition() + bone->getWorldOrientation() * mHook->OffestPos;
			//CActorEditorMainDlg::s_TransMgr.SetDummyPos(nv);		
			UpdateDummyPos();
		}

		// rotate
		if(CActorEditorMainDlg::s_TransMgr.GetEditType() == CTransformManager::eROTATION)
		{
			Ogre::Quaternion nq;
			Ogre::Quaternion oq = mHook->OffestQuaternion;

			// ǰ��ͼ						
					
			if((DWORD)SCENE_CAM->getUserObject() & 0xF0000000)
			{
				nq = oq * Ogre::Quaternion(Ogre::Radian(Ogre::Degree(screenMove.length())), Ogre::Vector3::UNIT_Z);														
			}

			// ����ͼ

			else if((DWORD)SCENE_CAM->getUserObject() & 0x0F000000)
			{
				nq = oq * Ogre::Quaternion(Ogre::Radian(Ogre::Degree(screenMove.length())), Ogre::Vector3::UNIT_Y);
																		
			}

			// ����ͼ

			else if((DWORD)SCENE_CAM->getUserObject() & 0x00F00000)
			{
				nq = oq * Ogre::Quaternion(Ogre::Radian(Ogre::Degree(screenMove.length())), Ogre::Vector3::UNIT_X);
																		
			}
			else
			{

				Ogre::Vector3 ov;
				ov = mHook->OffestPos;				
				
				CActorEditorMainDlg::s_TransMgr.GetViewportRotate(
					ov, Ogre::Vector2(x - oldPos.x, y - oldPos.y), oq, nq);
			}

			mHook->OffestQuaternion = nq;

			COTEActorHookMgr::UpdateHook(CActorEditorMainDlg::s_pActorEntity, mHook->HookName);
			//Ogre::Vector3 nv = CActorEditorMainDlg::s_pActorEntity->getSkeleton()->getTagPoint(mHook->TagPointName)->getWorldPosition();
			//Ogre::Vector3 nv = bone->getWorldPosition() + bone->getWorldOrientation() * mHook->OffestPos;
			//CActorEditorMainDlg::s_TransMgr.SetDummyPos(nv);	

			UpdateDummyPos();

		}

		//scale
		if(CActorEditorMainDlg::s_TransMgr.GetEditType() == CTransformManager::eSCALING)
		{
			Ogre::Vector3 ov;
			ov = mHook->OffestPos;	
			Ogre::Vector3 os, ns;
			os = mHook->Scale;

			CActorEditorMainDlg::s_TransMgr.GetViewportScaling(
				ov, Ogre::Vector2(x - oldPos.x, y - oldPos.y), os, ns);
			mHook->Scale = ns;
						
			COTEActorHookMgr::UpdateHook(CActorEditorMainDlg::s_pActorEntity, mHook->HookName);
			//Ogre::Vector3 nv = CActorEditorMainDlg::s_pActorEntity->getSkeleton()->getTagPoint(mHook->TagPointName)->getWorldPosition();
			//Ogre::Vector3 nv = bone->getWorldPosition() + bone->getWorldOrientation() * mHook->OffestPos;
			//CActorEditorMainDlg::s_TransMgr.SetDummyPos(nv);	
						
			UpdateDummyPos();
		}		
	}

	oldPos = CPoint(x, y);
}

// -----------------------------------------
//���ز�����Ʒ
void CHookEditDlg::OnBnClickedLoadtest()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !CActorEditorMainDlg::s_pActorEntity->hasSkeleton())
		return;

	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		 "ģ���ļ� (*.mesh)|*.mesh|ģ���ļ� (*.obj)|*.obj|All Files (*.*)|*.*|",
		 NULL 
		); 
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{ 	 
		OnBnClickedDeltest();
		TestObjFile = tpFileDialog->GetFileName().GetString();
		GetDlgItem(IDC_DelTest)->EnableWindow(true);

		CActorEditorMainDlg::s_TransMgr.ShowDummyA(true);
		UpdateHInfomation();
	}

	delete tpFileDialog; 
}

// -----------------------------------------
BOOL CHookEditDlg::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();

	SCENE_MGR->AddListener(COTEQTSceneManager::eRenderCB, OnSceneRender2);

	return tRet;
}

// -----------------------------------------
//ѡ��ҵ�
void CHookEditDlg::OnLbnSelchangePothooklist()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !CActorEditorMainDlg::s_pActorEntity->hasSkeleton())
		return;

	SetDlgItemText(IDC_Postion, "X:\tY:\tZ:");
	SetDlgItemText(IDC_Rotation, "X:\tY:\tZ:\tW:");
	SetDlgItemText(IDC_Scale, "X:\tY:\tZ:");
	SetDlgItemText(IDC_Name, "");

	CString sel;
	m_HookList.GetText(m_HookList.GetCurSel(), sel);
	if(!sel.IsEmpty())
	{
		GetDlgItem(IDC_Name)->EnableWindow(true);
		GetDlgItem(IDC_Del)->EnableWindow(true);
		GetDlgItem(IDC_Reset)->EnableWindow(true);
		GetDlgItem(IDC_Save)->EnableWindow(true);
		mHook = COTEActorHookMgr::GetHook(CActorEditorMainDlg::s_pActorEntity, sel.GetString());
		if(mHook)
			COTEEntityBoneRender::SetSelectedBoneName(mHook->BoneName);
		else
			COTEEntityBoneRender::SetSelectedBoneName("");
	}	
	else
	{
		GetDlgItem(IDC_Name)->EnableWindow(false);
		GetDlgItem(IDC_Del)->EnableWindow(false);
		GetDlgItem(IDC_Reset)->EnableWindow(true);
		GetDlgItem(IDC_Save)->EnableWindow(true);
		mHook = NULL;
		COTEEntityBoneRender::SetSelectedBoneName("");
	}

	UpdateHInfomation();
	
}

// -----------------------------------------
//���¹ҵ���Ϣ
void CHookEditDlg::UpdateHInfomation()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !CActorEditorMainDlg::s_pActorEntity->hasSkeleton() || mHook == NULL)
		return;
	
	char s[256];
	sprintf(s, "X:%2.3f  Y:%2.3f  Z:%2.3f", mHook->OffestPos.x, mHook->OffestPos.y, mHook->OffestPos.z);
	SetDlgItemText(IDC_Postion, s);
	sprintf(s, "X:%2.3f  Y:%2.3f  Z:%2.3f  W:%2.3f", mHook->OffestQuaternion.x,
		mHook->OffestQuaternion.y, mHook->OffestQuaternion.z, mHook->OffestQuaternion.w);
	SetDlgItemText(IDC_Rotation, s);
	sprintf(s, "X:%2.3f  Y:%2.3f  Z:%2.3f", mHook->Scale.x, mHook->Scale.y, mHook->Scale.z);
	SetDlgItemText(IDC_Scale, s);
	SetDlgItemText(IDC_Name, mHook->HookName.c_str());

			
//	if(!mHook->BoneName.empty())
//	{
		//Ogre::Vector3 nv = CActorEditorMainDlg::s_pActorEntity->getSkeleton()->getTagPoint(mHook->TagPointName)->getWorldPosition();
		//Ogre::Bone* bone = CActorEditorMainDlg::s_pActorEntity->getSkeleton()->getBone(mHook->BoneName);
		//Ogre::Vector3 nv = bone->getWorldPosition() + bone->getWorldOrientation() * mHook->OffestPos;

		//CActorEditorMainDlg::s_TransMgr.SetDummyPos(SCENE_MGR->GetEntity(TestObjFile)->getParentSceneNode()->getPosition());
//	}

	if(!TestObjFile.empty())
	{
		COTEActorEntity* e = SCENE_MGR->GetEntity(TestObjFile);
		if(e)
		{
			COTEActorHookMgr::DettachObjectFromHook(
				CActorEditorMainDlg::s_pActorEntity,
				mHook->HookName.c_str(),
				e
				);
		}

		//SCENE_MGR->RemoveEntity(TestObjFile);

		COTEActorHookMgr::AttachObjectToHook(
			CActorEditorMainDlg::s_pActorEntity,
			mHook->HookName.c_str(), TestObjFile.c_str());

		UpdateDummyPos();
	}
}

// -----------------------------------------
//���ùҵ�
void CHookEditDlg::OnBnClickedReset()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !CActorEditorMainDlg::s_pActorEntity->hasSkeleton() || mHook == NULL)
		return;
	mHook->OffestPos = Ogre::Vector3::ZERO;
	mHook->OffestQuaternion = Ogre::Quaternion::IDENTITY;
	mHook->Scale = Ogre::Vector3::UNIT_SCALE;
	UpdateHInfomation();
}

// -----------------------------------------
//����ҵ�
void CHookEditDlg::OnBnClickedSave()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !CActorEditorMainDlg::s_pActorEntity->hasSkeleton() || mHook == NULL)
		return;

	char newName[256];
	GetDlgItemText(IDC_Name, newName, 256);
	if(strcmp(newName, mHook->HookName.c_str()) != 0)
	{
		if(COTEActorHookMgr::GetHook(CActorEditorMainDlg::s_pActorEntity, newName) != NULL)
		{
			MessageBox("�Ѿ�����ͬ���ҵ�������������");
			return;
		}

		COTEActorHookMgr::AddHook(CActorEditorMainDlg::s_pActorEntity, 
												newName, mHook->BoneName.c_str(), mHook->OffestPos,
												mHook->OffestQuaternion, mHook->Scale);
		COTEActorHookMgr::RemoveHook(
			CActorEditorMainDlg::s_pActorEntity, 
			mHook->HookName.c_str());
		UpdateHookList();
	}

	Save();
}
// -----------------------------------------
//��ȡ�ҵ���Ϣ
void CHookEditDlg::Load()
{
	HashMap<std::string, Hook_t*>& rActorHookList = (CActorEditorMainDlg::s_pActorEntity->m_PotHookList);

	//ȡ�ҵ���Ϣ

	const std::string& file = OTEHelper::GetOTESetting("Misc", "PotHookFile");

	TiXmlDocument xmlDoc;
	if(xmlDoc.LoadFile(file))
	{
		TiXmlElement *elem = xmlDoc.RootElement()->FirstChildElement("PotHook");
		while(elem)
		{
			const char* attr = elem->Attribute("EntityId");
			if(attr && CActorEditorMainDlg::s_pActorEntity->getName() == attr)
			{
				const char *name = elem->Attribute("Name");
				const char *bone = elem->Attribute("Bone");
				Ogre::Vector3 vp;
				sscanf(elem->Attribute("OffestPos"), "%f %f %f", &(vp.x), &(vp.y), &(vp.z));
				Ogre::Quaternion qr;
				sscanf(elem->Attribute("Rotation"), "%f %f %f %f", &(qr.x), &(qr.y), &(qr.z), &(qr.w));
				Ogre::Vector3 vS;
				sscanf(elem->Attribute("Scale"), "%f %f %f", &(vS.x), &(vS.y), &(vS.z));
				
				COTEActorHookMgr::AddHook(CActorEditorMainDlg::s_pActorEntity, 
											name, bone, vp, qr, vS);
			}
			elem = elem->NextSiblingElement();
		}
	}

	UpdateHookList();
}

// -----------------------------------------
//����ҵ���Ϣ
void CHookEditDlg::Save()
{
	const std::string& File = OTEHelper::GetOTESetting("Misc", "PotHookFile");
	if(File.empty())
	{
		OTE_MSG_ERR("OTESetting �� Misc,PotHookFileһ��û���ҵ�!")
		return;
	}

	Ogre::String filePath;
	Ogre::String fileFullName;
	OTEHelper::GetFullPath(File, filePath, fileFullName);

	TiXmlDocument *xmlDoc = new TiXmlDocument(fileFullName.c_str());
	xmlDoc->LoadFile();
	TiXmlElement *elem = xmlDoc->RootElement()->FirstChildElement("PotHook");
	while(elem)
	{
		if(strcmp(elem->Attribute("EntityId"), CActorEditorMainDlg::s_pActorEntity->m_ResFile.c_str()) == 0)
		{
			xmlDoc->RootElement()->RemoveChild(elem);
			elem = xmlDoc->RootElement()->FirstChildElement("PotHook");
			continue;
		}
		elem = elem->NextSiblingElement();
	}

	m_pActorHookList = &(CActorEditorMainDlg::s_pActorEntity->m_PotHookList);
	HashMap<std::string, OTE::Hook_t*>::iterator i = m_pActorHookList->begin();
	while(i != m_pActorHookList->end())
	{
		elem = new TiXmlElement("PotHook");
		elem->SetAttribute("name", ((OTE::Hook_t*)(i->second))->HookName.c_str());
		elem->SetAttribute("EntityId", CActorEditorMainDlg::s_pActorEntity->m_ResFile.c_str());
		elem->SetAttribute("Bone", ((OTE::Hook_t*)(i->second))->BoneName.c_str());
		char s[256];
		sprintf(s, "%f %f %f", ((OTE::Hook_t*)(i->second))->OffestPos.x, ((OTE::Hook_t*)(i->second))->OffestPos.y, ((OTE::Hook_t*)(i->second))->OffestPos.z);
		elem->SetAttribute("OffestPos", s);
		sprintf(s, "%f %f %f %f", ((OTE::Hook_t*)(i->second))->OffestQuaternion.x,
			((OTE::Hook_t*)(i->second))->OffestQuaternion.y, ((OTE::Hook_t*)(i->second))->OffestQuaternion.z, ((OTE::Hook_t*)(i->second))->OffestQuaternion.w);
		elem->SetAttribute("Rotation", s);
		sprintf(s, "%f %f %f", ((OTE::Hook_t*)(i->second))->Scale.x, ((OTE::Hook_t*)(i->second))->Scale.y, ((OTE::Hook_t*)(i->second))->Scale.z);
		elem->SetAttribute("Scale", s);
		xmlDoc->RootElement()->InsertEndChild(*elem);
		i++;
	}

	xmlDoc->SaveFile();

	delete xmlDoc;
}

// -----------------------------------------
//ɾ���ҵ�
void CHookEditDlg::OnBnClickedDel()
{	
	CString sel;
	m_HookList.GetText(m_HookList.GetCurSel(), sel);
	if(sel.IsEmpty()) return;
	COTEActorHookMgr::RemoveHook(CActorEditorMainDlg::s_pActorEntity, sel.GetString());
	UpdateHookList();
	Save();
}

// -----------------------------------------
//��ӹҵ�
void CHookEditDlg::OnBnClickedAdd()
{
	if(!CActorEditorMainDlg::s_pActorEntity || !CActorEditorMainDlg::s_pActorEntity->hasSkeleton())
		return;

	CBrowserBone::s_Inst.isAddHook = true;
	CBrowserBone::s_Inst.isAddTrack = false;
	CBrowserBone::s_Inst.ShowWindow(SW_SHOW);
	CBrowserBone::s_Inst.UpdataBoneList();
}

// -----------------------------------------
//ɾ��������Ʒ
void CHookEditDlg::OnBnClickedDeltest()
{
	if(CActorEditorMainDlg::s_pActorEntity != NULL && CActorEditorMainDlg::s_pActorEntity->hasSkeleton())
	{
		m_pActorHookList = &(CActorEditorMainDlg::s_pActorEntity->m_PotHookList);
		HashMap<std::string, OTE::Hook_t*>::iterator i = m_pActorHookList->begin();
		while(i != m_pActorHookList->end())
		{
			COTEActorHookMgr::DettachAllObjectsFromHook(
						CActorEditorMainDlg::s_pActorEntity, 
						i->first.c_str()
						);
			i++;
		}
	}

	GetDlgItem(IDC_DelTest)->EnableWindow(false);

	CActorEditorMainDlg::s_TransMgr.ShowDummyA(false);
}

// -----------------------------------------
void CHookEditDlg::OnBnClickedCheck2()
{
	
}
