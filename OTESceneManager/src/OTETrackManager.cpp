#include <windows.h>
#include "tinyxml.h"
#include "OTETrackManager.h"
#include "OTEMathLib.h"
#include "OTETrackXmlLoader.h"
#include "OTETrackCtrller.h"
#include "OTEActorEntity.h"

using namespace OTE;
using namespace Ogre;

// --------------------------------------
COTETrackManager* COTETrackManager::s_Instance = NULL;	
COTETrackManager* COTETrackManager::GetInstance()
{
	if(!s_Instance)	
		s_Instance = new COTETrackManager();	

	return s_Instance;
}
// --------------------------------------
//����TrackManager
void COTETrackManager::Destroy()
{	
	if(s_Instance)
	{
		s_Instance->Clear();

		SAFE_DELETE(s_Instance)		
	}
	OTE_TRACE("����TrackManager")
}

// --------------------------------------
bool COTETrackManager::IsTrackPresent(int id)
{
	HashMap<int, COTETrack*>::iterator it = m_TrackListCach.find(id);
	if(it != m_TrackListCach.end())
	{
		return true;
	}
	return false;
}

// --------------------------------------
//ȡ��ָ��id��Track
COTETrack* COTETrackManager::GetTrack(int id, COTETrackLoader* loarder)
{	
	HashMap<int, COTETrack*>::iterator it = m_TrackListCach.find(id);
	if(it != m_TrackListCach.end())
	{
		return it->second;
	}

	//���ز�����Track
	COTETrack* track = loarder->LoadTrack(id);
	m_TrackListCach[id] = track;

    return track;		
}
// --------------------------------------
//�Ƴ�ָ����Track
void COTETrackManager::Delete(int id)
{
	HashMap<int, COTETrack*>::iterator it = m_TrackListCach.find(id);
	if(it != m_TrackListCach.end())
	{
		delete it->second;
		m_TrackListCach.erase(it);
		OTE_TRACE("�Ƴ�ָ����Track ID = " << id)
	}	
}


// --------------------------------------
//�����µ�Track����TrackId
COTETrack* COTETrackManager::CreateNewTrack(int id, COTETrackLoader* loarder)
{	
	HashMap<int, COTETrack*>::iterator it = m_TrackListCach.find(id);
	if(it != m_TrackListCach.end())
	{
		::MessageBox(NULL, "��ID�Ѿ����ڣ�", "ʧ��", MB_OK);
		return NULL;
	}

	COTETrack *track = new COTETrack();
	track->m_nID = id;
	m_TrackListCach[id] = track;

	// ����
	if(loarder)
		loarder->TrackWriteXml(id);
	
	OTE_TRACE("�����µ�Track ID = " << id)

	return track;
}

// --------------------------------------
//����
void COTETrackManager::Clear()
{
	// ���� cach
	HashMap<int, COTETrack*>::iterator it = m_TrackListCach.begin();
	while(it != m_TrackListCach.end())
	{
		delete it->second;
		it ++;
	}
	m_TrackListCach.clear();

	OTE_TRACE("���� Tracks")
}