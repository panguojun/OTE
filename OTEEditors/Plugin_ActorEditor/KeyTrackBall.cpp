#include "stdafx.h"
#include ".\keytrackball.h"
// ------------------------------------------------
// 后悔操作
// ------------------------------------------------

#define  MAX_STEPS		10

// ------------------------------------------------
CKeyTrackBall CKeyTrackBall::s_Inst;
CKeyTrackBall::CKeyTrackBall(void)
{
}

CKeyTrackBall::~CKeyTrackBall(void)
{
}

// ------------------------------------------------
void CKeyTrackBall::PushStack(  std::vector<CKeyFrameDlg::Key_t>& kl, 
								std::list<OTE::COTEKeyFrame*>& kfl  )
{
	if(m_KeyTrack.size() >= MAX_STEPS)
	{
		m_KeyTrack.pop_back();
	}
	
	m_KeyTrack.push_front(CKeyTrackBall::TrackBall_t());
	(*m_KeyTrack.begin()).Set(kl, kfl);

	::OutputDebugString("准备后悔\n");
}

// ------------------------------------------------
bool CKeyTrackBall::PopStack(   std::vector<CKeyFrameDlg::Key_t>& kl, 
								std::list<OTE::COTEKeyFrame*>& kfl  )
{
	if(m_KeyTrack.size() == 0)
		return false;
	
	(*m_KeyTrack.begin()).Get(kl, kfl);

	m_KeyTrack.pop_front();

	::OutputDebugString("后悔了\n");

	return true;
}