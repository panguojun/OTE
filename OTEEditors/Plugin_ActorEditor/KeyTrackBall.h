#pragma once
#include "KeyFrameDlg.h"
#include "OTETrack.h"

// ************************************************
// CKeyTrackBall
// ************************************************
class CKeyTrackBall
{
public:
	static CKeyTrackBall s_Inst;
	
public:
	
	// the track

	struct TrackBall_t
	{
		std::vector<CKeyFrameDlg::Key_t> keylist;
		std::list<OTE::COTEKeyFrame*>  keyframeList;	

		TrackBall_t()
		{
		}
		TrackBall_t(std::vector<CKeyFrameDlg::Key_t>& kl, 
					std::list<OTE::COTEKeyFrame*>& kfl)
		{
			Set(kl, kfl);
		}
		~TrackBall_t()
		{
			 Clear();
		}

		void Clear()
		{
			keylist.clear();

			std::list<OTE::COTEKeyFrame*>::iterator ita = keyframeList.begin();
			while(ita != keyframeList.end())
			{				
				OTE::COTEKeyFrame::DeleteKeyFrame (*ita);
				++ ita;
			}
			keyframeList.clear();
		}

		void operator = ( TrackBall_t& tb )
		{
			Set(tb.keylist, tb.keyframeList);				
		}

		void Set(std::vector<CKeyFrameDlg::Key_t>& kl, 
				 std::list<OTE::COTEKeyFrame*>& kfl)
		{
			Clear();
					
			std::vector<CKeyFrameDlg::Key_t>::iterator it = kl.begin();
			while(it != kl.end())
			{
				keylist.push_back(*it);
				++ it;
			}			
			
			std::list<OTE::COTEKeyFrame*>::iterator ita = kfl.begin();
			while(ita != kfl.end())
			{
				OTE::COTEKeyFrame* kf = OTE::COTEKeyFrame::CreateNewKeyFrame();
				*kf = *(*ita);
				keyframeList.push_back(kf);
				++ ita;
			}
		}

		void Get(std::vector<CKeyFrameDlg::Key_t>& kl, 
				 std::list<OTE::COTEKeyFrame*>& kfl)
		{			
			kl.clear();
			std::list<OTE::COTEKeyFrame*>::iterator it1 = kfl.begin();
			while(it1 != kfl.end())
			{				
				OTE::COTEKeyFrame::DeleteKeyFrame (*it1);
				++ it1;
			}
			kfl.clear();
					
			std::vector<CKeyFrameDlg::Key_t>::iterator it = keylist.begin();
			while(it != keylist.end())
			{
				kl.push_back(*it);
				++ it;
			}			
			
			std::list<OTE::COTEKeyFrame*>::iterator ita = keyframeList.begin();
			while(ita != keyframeList.end())
			{
				OTE::COTEKeyFrame* kf = OTE::COTEKeyFrame::CreateNewKeyFrame();
				*kf = *(*ita);				
				kfl.push_back(kf);
				++ ita;
			}
		}

		
	};

	std::list<TrackBall_t>		m_KeyTrack;

public:

	void PushStack(std::vector<CKeyFrameDlg::Key_t>& kl, 
								std::list<OTE::COTEKeyFrame*>& kfl);

	bool PopStack(std::vector<CKeyFrameDlg::Key_t>& kl, 
								std::list<OTE::COTEKeyFrame*>& kfl);

public:
	CKeyTrackBall(void);
	~CKeyTrackBall(void);

};
