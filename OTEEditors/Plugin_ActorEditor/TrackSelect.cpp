// TrackSelect.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Plugin_ActorEditor.h"

#include "TrackSelect.h"
#include "tinyxml.h"

#include "ActionEdit.h"
#include "ActorEditorMainDlg.h"
#include "OTETrackManager.h"
#include "OTEActionXmlLoader.h"
#include "OTEActTrackUsage.h"

// ----------------------------------------
using namespace Ogre;
using namespace OTE;

CTrackSelect CTrackSelect::s_Inst;

// ----------------------------------------

//���¿��ù���б�
void CTrackSelect::UpdtatTrackList(bool hasSkeleton)
{
	MTrackList.ResetContent();

	const std::string& TrackFile = OTE::OTEHelper::GetOTESetting("ResConfig", "TrackFile");

	Ogre::String filePath;
	Ogre::String fileFullName;
	OTEHelper::GetFullPath(TrackFile, filePath, fileFullName);

	TiXmlDocument *TrackDoc = new TiXmlDocument(fileFullName);
	TrackDoc->LoadFile();
	if(TrackDoc != NULL)
	{
		//������¼

		TiXmlElement *elem = TrackDoc->RootElement()->FirstChildElement("Track");
		while(elem)
		{
			if(!hasSkeleton && strlen(elem->Attribute("BindBone")) > 0)
			{
				elem = elem->NextSiblingElement();
				continue;
			}

			MTrackList.AddString(CString(elem->Attribute("Id")) + "    \t" + elem->Attribute("Desc"));

			elem = elem->NextSiblingElement();
		}
	}
	delete TrackDoc;

	MTrackList.Invalidate();
}

IMPLEMENT_DYNAMIC(CTrackSelect, CDialog)
CTrackSelect::CTrackSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CTrackSelect::IDD, pParent)
{
}

CTrackSelect::~CTrackSelect()
{
}

void CTrackSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TrackList, MTrackList);
}


BEGIN_MESSAGE_MAP(CTrackSelect, CDialog)
	ON_LBN_DBLCLK(IDC_TrackList, OnLbnDblclkTracklist)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_LBN_SELCHANGE(IDC_TrackList, OnLbnSelchangeTracklist)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()


// -------------------------------
//˫����ӹ��
void CTrackSelect::OnLbnDblclkTracklist()
{
	OnBnClickedOk();
}

// -------------------------------
//ȷ����ť
void CTrackSelect::OnBnClickedOk()
{
	if(CActorEditorMainDlg::s_pActorEntity == NULL || CActionEdit::s_Inst.m_pAct == NULL)
		return;

	int tItem = CActionEdit::s_Inst.m_MagList.GetLastSelect();
	if(tItem > CActionEdit::s_Inst.m_MagList.GetItemCount() || tItem < 0)
		return;
	
	CString sSel;
	MTrackList.GetText(MTrackList.GetCurSel(), sSel);
	if(!sSel.IsEmpty())
	{			
		char fileName[32];	int id;		
		sscanf( LPCTSTR( CActionEdit::s_Inst.m_MagList.GetItemText(tItem, 0) ), "%s %d", fileName, &id);

		if(CActionEdit::s_Inst.m_MagList.GetItemText(tItem, 1).IsEmpty() ||
			CActionEdit::s_Inst.m_MagList.GetItemText(tItem, 1) == "��")
		{
			// ?			
		}
		else
		{
			CActionEdit::s_Inst.m_pAct->RemoveEffectData(id);			
		}
		
		char strID[32];	
		sscanf((char*)LPCTSTR(sSel), "%s", strID);

		CActionEdit::s_Inst.m_MagList.SetItemText(tItem, 1, strID);

		OTE::ActMagicData_t *mag = OTE::ActMagicData_t::CreateActMagic();
		strcpy(mag->MagicFile, fileName);			

		CActionEdit::s_Inst.m_pAct->AddEffectData(mag, atoi(strID));
	
		CActionEdit::s_Inst.OnEnKillfocusMagspeed();
		CActionEdit::s_Inst.UpdateEffect();
	}

	OnOK();
}

// -------------------------------
//ȡ����ť
void CTrackSelect::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

// -------------------------------
// ɾ�����
void CTrackSelect::OnBnClickedButton1()
{
	CString sSel;
	MTrackList.GetText(MTrackList.GetCurSel(), sSel);
	if(!sSel.IsEmpty())
	{
		if(MessageBox("ȷ��Ҫɾ���ù����?", "ȷ��", MB_YESNO) == IDYES)
		{
			int id;
			sscanf((char*)LPCTSTR(sSel), "%d", &id);			

			// ���ж�������Ƿ����õ���

			std::string refActionName;
			HashMap<std::string, ActionDataMap_t>::iterator actDmIt = COTEActionXmlLoader::GetSingleton()->m_ActionDataMap.begin();
			while(actDmIt != COTEActionXmlLoader::GetSingleton()->m_ActionDataMap.end())	
			{
				HashMap<int, COTEActionData*>::iterator it = actDmIt->second.DataMap.begin();
				while(it != actDmIt->second.DataMap.end())
				{
					MagBindTrackData_t* magInterMapData = &it->second->m_MagicInterMapData;

					MagBindTrackData_t::iterator ita = magInterMapData->begin();
					while(ita != magInterMapData->end())
					{		
						if(ita->second->InterID == id)
						{							
							refActionName = it->second->m_ResName;
							break;
						}
						++ ita;
					}	

					++ it;
				}

				++ actDmIt;
			}

			if(refActionName.empty())
			{
				COTETrackManager::GetInstance()->Delete(id);
				COTEActTrackUsage::GetTrackXmlLoader()->RemoveTrack(id);

				COTEActTrackUsage::GetTrackXmlLoader()->SaveFile();

				UpdtatTrackList(CActorEditorMainDlg::s_pActorEntity->hasSkeleton());				
			}
			else
			{
				OTE_MSG("���ж����õ�������ID �� �� " << refActionName, "ʧ��")
			}
		}
	}

}

void CTrackSelect::OnLbnSelchangeTracklist()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

// ----------------------------------------
// ����
void CTrackSelect::OnBnClickedButton2()
{
	for(int i = 0; i < MTrackList.GetCount(); i ++)
	{
		CString sSel;
		MTrackList.GetText(i, sSel);
		int id;
		sscanf((char*)LPCTSTR(sSel), "%d", &id);	

		if(!sSel.IsEmpty() && id > 0)
		{
			std::string refActionName;
			HashMap<std::string, ActionDataMap_t>::iterator actDmIt = COTEActionXmlLoader::GetSingleton()->m_ActionDataMap.begin();
			while(actDmIt != COTEActionXmlLoader::GetSingleton()->m_ActionDataMap.end())	
			{
				HashMap<int, COTEActionData*>::iterator it = actDmIt->second.DataMap.begin();
				while(it != actDmIt->second.DataMap.end())
				{
					MagBindTrackData_t* magInterMapData = &it->second->m_MagicInterMapData;

					MagBindTrackData_t::iterator ita = magInterMapData->begin();
					while(ita != magInterMapData->end())
					{		
						if(ita->second->InterID == id)
						{							
							refActionName = it->second->m_ResName;
							break;
						}
						++ ita;
					}	

					++ it;
				}

				++ actDmIt;
			}

			if(refActionName.empty())
			{
				COTETrackManager::GetInstance()->Delete(id);					
				COTEActTrackUsage::GetTrackXmlLoader()->RemoveTrack(id);
			}
		}
	}

	COTEActTrackUsage::GetTrackXmlLoader()->SaveFile();

	UpdtatTrackList(CActorEditorMainDlg::s_pActorEntity->hasSkeleton());

}
