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
//销毁TrackManager
void COTETrackManager::Destroy()
{	
	if(s_Instance)
	{
		s_Instance->Clear();

		SAFE_DELETE(s_Instance)		
	}
	OTE_TRACE("销毁TrackManager")
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
//取出指定id的Track
COTETrack* COTETrackManager::GetTrack(int id, COTETrackLoader* loarder)
{	
	HashMap<int, COTETrack*>::iterator it = m_TrackListCach.find(id);
	if(it != m_TrackListCach.end())
	{
		return it->second;
	}

	//加载并创建Track
	COTETrack* track = loarder->LoadTrack(id);
	m_TrackListCach[id] = track;

    return track;		
}
// --------------------------------------
//移除指定的Track
void COTETrackManager::Delete(int id)
{
	HashMap<int, COTETrack*>::iterator it = m_TrackListCach.find(id);
	if(it != m_TrackListCach.end())
	{
		delete it->second;
		m_TrackListCach.erase(it);
		OTE_TRACE("移除指定的Track ID = " << id)
	}	
}


// --------------------------------------
//创建新的Track返回TrackId
COTETrack* COTETrackManager::CreateNewTrack(int id, COTETrackLoader* loarder)
{	
	HashMap<int, COTETrack*>::iterator it = m_TrackListCach.find(id);
	if(it != m_TrackListCach.end())
	{
		::MessageBox(NULL, "该ID已经存在！", "失败", MB_OK);
		return NULL;
	}

	COTETrack *track = new COTETrack();
	track->m_nID = id;
	m_TrackListCach[id] = track;

	// 保存
	if(loarder)
		loarder->TrackWriteXml(id);
	
	OTE_TRACE("创建新的Track ID = " << id)

	return track;
}

// --------------------------------------
//清理
void COTETrackManager::Clear()
{
	// 清理 cach
	HashMap<int, COTETrack*>::iterator it = m_TrackListCach.begin();
	while(it != m_TrackListCach.end())
	{
		delete it->second;
		it ++;
	}
	m_TrackListCach.clear();

	OTE_TRACE("清理 Tracks")
}