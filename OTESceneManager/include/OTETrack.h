#pragma once
#include "OTEStaticInclude.h"
#include "OTEKeyFrame.h"

namespace OTE
{
	class _OTEExport CInterBase
	{
	public:

		virtual const std::string&	GetInterType() = 0;	

	public:
		
		int					m_nID;				// ID

		bool				m_IsBind;			// �Ƿ�

	};

	// *********************************************** //
	// COTETrack 
	// ����"����" ��һ��

	// ������󶨵Ĺ��,���ڲ�������Ķ�̬Ч����
	// ���ֹ������ ʱ���� ���� ֮�ϵ�����
	// ���ڶ�����İ����ڵ� ���ʵ����Եĸı�Ĺؼ�֡��ɡ�
	// �ǹ����

	// *********************************************** //
	class _OTEExport COTETrack : public CInterBase
	{		
	friend class COTEKeyFrame;
	public:	
		COTETrack();
		~COTETrack();

		//��ӹؼ���	
		bool AddKeyFrame(COTEKeyFrame* Key);	

		//��ѯĳһ��
		COTEKeyFrame* GetFrameAt(float Time);

		//��ѯĳһ�ؼ���
		COTEKeyFrame* GetKeyFrameAt(float Time);
		
		//ɾ��ĳһ֡
		void RemoveKeyFrameAt(float Time);

		//������
		int GetFrameNum()
		{
			return (int)m_KeyFrameList.size();
		}

		// clone
		COTETrack* Clone();

		//��ѯ�б�
		void GetKeyFrameList(std::list <COTEKeyFrame*> *pList)
		{			
			std::list <COTEKeyFrame*>::iterator it = m_KeyFrameList.begin();
			while(it != m_KeyFrameList.end())
			{
				pList->push_back(*it);
				it++;
			}
		}
		//�����б�
		void SetKeyFrameList(std::list <COTEKeyFrame*> *pList)
		{
			m_KeyFrameList.clear();
			std::list <COTEKeyFrame*>::iterator it = pList->begin();
			while(it != pList->end())
			{
				m_KeyFrameList.push_back(*it);
				it++;
			}
		}

		//�õ��������
		float GetLength()
		{
			return m_AnimMaxTime - m_AnimMinTime;
		}

		//����켣
		void Clear();

		//ȡ����ʼ�ؼ�֡
		COTEKeyFrame* GetFirstKey()
		{
			if(m_KeyFrameList.size() > 0)
				return *m_KeyFrameList.begin();
			return NULL;
			
		}

		//ȡ�ý����ؼ�֡
		COTEKeyFrame* GetEndKey()
		{
			std::list <COTEKeyFrame*>::iterator end = m_KeyFrameList.end();
			return *(--end);
		}

		//���ð󶨹���
		void SetBindBone(std::string BoneName)
		{
			m_AttBoneName = BoneName;
		}

		const std::string&	GetInterType()
		{
			static std::string sType = "Track";
			return sType;
		}

	public:

		/// �������	
				
		bool			m_IsBezierCurves;	// �Ƿ�ʹ��Bezier Curves 

		float			m_AnimMaxTime;		// �������ʱ��	
		float			m_AnimMinTime;		// ������Сʱ��	

		std::string		m_AttBoneName;		//�ù켣�󶨹���������

		Ogre::TextureUnitState::TextureAddressingMode	m_TexAddrMode;	//����Ѱַģʽ 
		
		std::string		m_Desc;				// ����
	

	protected:

		//�����ӳ�ͣ��Ч��
		float m_LastDelayTimePos;
		//�õ��м���	
		void GetKeyAt(const COTEKeyFrame* K1, const COTEKeyFrame* K2,  float Time, COTEKeyFrame* Ki); 

		std::list <COTEKeyFrame*> m_KeyFrameList; // ���ڴ���ʱ����ʱ���С����˳������
	};

	// ************************************** //
	// COTETrackLoader
	// ************************************** //
	class COTETrackLoader
	{
	public:

		//����ָ��ID��Track
		virtual COTETrack* LoadTrack(unsigned int id) = 0;

		//����ָ��Id��Track
		virtual void TrackWriteXml(int id) = 0; 

		// �Ƴ�track
		virtual void RemoveTrack(int id) = 0;

		//����Track�ļ�
		virtual void SaveFile(const std::string& fileName) = 0;

	};	

}