#include "OTETrackXmlLoader.h"
#include "OTETrack.h"
#include "OTEMathLib.h"

// -------------------------------------------
using namespace OTE;
using namespace Ogre;

// -------------------------------------------
COTETrack::COTETrack() : 
CInterBase(),
m_AnimMinTime(0.0f),
m_AnimMaxTime(2.0f),
m_LastDelayTimePos(-1.0f),
m_IsBezierCurves(false),
m_TexAddrMode(Ogre::TextureUnitState::TextureAddressingMode::TAM_CLAMP)
{	
	m_IsBind = true; // ����ǳ��󶨵�
}	

COTETrack::~COTETrack()
{
	Clear();	
}
// -------------------------------------------
//����켣
void COTETrack::Clear()
{
	std::list <COTEKeyFrame*>::iterator it = m_KeyFrameList.begin();
	while(it != m_KeyFrameList.end())
	{
		if(*it != NULL)			
			delete *it;			

		++ it;
	}

	m_KeyFrameList.clear();
}

// -------------------------------------------
//��ӹؼ���	
bool COTETrack::AddKeyFrame(COTEKeyFrame* K)
{
	assert(K);	

	OTE_TRACE("�ؼ���: " << K->m_Time)

	// ����б�Ϊ��
	if(m_KeyFrameList.empty())
	{
		m_KeyFrameList.push_back(K);
		return true;
	}

	// �б��е�һ��ʱ�������ӵ���	
	if((*m_KeyFrameList.begin())->m_Time > K->m_Time)
	{
		m_KeyFrameList.push_front(K);
		return true;
	}

	// ����б������һ��ʱ��С����ӵ���
	std::list <COTEKeyFrame*>::iterator end = m_KeyFrameList.end();		
	if((*(--end))->m_Time < K->m_Time)
	{
		m_KeyFrameList.push_back(K);
		return true;
	}


	// ����� λ�ڵ�һ������һ��֮��
	std::list <COTEKeyFrame*>::iterator it = m_KeyFrameList.begin();
	for(; it != m_KeyFrameList.end(); it ++)
	{		
		// ������ڸ����������ӵ����
		if(((*it)->m_Time - K->m_Time) > 0.001f)
		{
			m_KeyFrameList.insert(it, K);
			return true; 
		}
	}

	return false;
}
// -------------------------------------------
//ɾ���ؼ���
void COTETrack::RemoveKeyFrameAt(float Time)
{
	std::list <COTEKeyFrame*>::iterator it = m_KeyFrameList.begin();
	while(it != m_KeyFrameList.end())
	{
		if(abs((*it)->m_Time - Time) < 0.001f)
		{
			OTE_TRACE("ɾ���ؼ���: " << Time)
			m_KeyFrameList.erase(it);
			break;
		}
		++ it;
	}
}
// -------------------------------------------
//��ѯĳһ�ؼ���
COTEKeyFrame* COTETrack::GetKeyFrameAt(float Time)
{
	std::list <COTEKeyFrame*>::iterator it = m_KeyFrameList.begin();
	while(it != m_KeyFrameList.end())
	{
		if(abs((*it)->m_Time - Time) < 0.001f)
		{
			return *it;
		}
		++ it;
	}

	return NULL;
}

// -------------------------------------------
//��ѯĳһ��
COTEKeyFrame* COTETrack::GetFrameAt(float Time)
{
	static COTEKeyFrame keyFrame;

	std::list <COTEKeyFrame*>::iterator it = m_KeyFrameList.begin();
	std::list <COTEKeyFrame*>::iterator endIt = m_KeyFrameList.end();

	if(it != endIt)
	{
		// ������ڵ�һ��			
		if((*it)->m_Time >= Time)
		{
			return (*it);
		}
		// ����������һ��
		if((*(-- endIt))->m_Time <= Time)
		{
			return (*endIt);
		}	
		
		// ֮������

		while(it != m_KeyFrameList.end())
		{
			if(abs((*it)->m_Time - Time) < 0.001f)
			{
				return (*it);
			}

			if((*it)->m_Time > Time && (*(--it))->m_Time < Time)
			{
				std::list <COTEKeyFrame*>::iterator i = it++;

				GetKeyAt(*i, *it, Time, &keyFrame);
				return &keyFrame;
			}

			++ it;
		}
	}
	
	//throw;
	return NULL;
}

// -------------------------------------------
//�õ��м���

void COTETrack::GetKeyAt(const COTEKeyFrame* K1, const COTEKeyFrame* K2,  float Time, COTEKeyFrame* Ki)
{
	// �߽�	

	if(Time < K1->m_Time)
		Time = K1->m_Time;
	if(Time > K2->m_Time)
		Time = K2->m_Time;
	
	Ki->m_Time = Time;	

	// ���

	COTEKeyFrame::BindFrames(Time, K1, K2, this, Ki);

}
