#pragma once

#include "OTEStaticInclude.h"
#include "OTEReactorBase.h"
#include "OTEEntityRender.h"

namespace OTE
{		
	// ***********************************
	// COTETrackCtrller ����ʵ�� ���ж����
	// ���������
	// ����Զ��ħ����Ҫ�� "��̬�켣" ��Ч��
	// note: ���ǵ���!
	// ����"����"��һ��
	// ***********************************
	class _OTEExport COTETrackCtrller : public CReactorBase
	{
	public:
		// ����
		virtual void Trigger(Ogre::MovableObject* mo);
		// ����
		virtual void Update(Ogre::MovableObject* mo, float dTime);		
		
	public:

		COTETrackCtrller() : 
		m_Creator(NULL),
		m_time0(0),
		m_time(0)
		{
		}

		~COTETrackCtrller()
		{
			Clear();
		}

		//Ч��-Ŀ��

		void AddMagicTarget(Ogre::MovableObject* mag, Ogre::MovableObject* target);	

		void RemoveMagicTarget(Ogre::MovableObject* mag);

		//���ȫ��Ŀ��
		void Clear();

		//���ù��ʱ�䳤��
		void SetLength(float time)
		{
			m_time0 = m_time = time;		
		}

		//�õ�Ŀ������

		int GetTargetNum()
		{
			return int(m_TargetList.size());
		}

	public:

		//���������
		COTEEntity*								m_Creator;

	protected:

		//Ŀ�����б�

		struct Target_t
		{			
			std::string   TargetObjName;
			Ogre::Vector3 Pos;
			Target_t(const std::string& targetObjName, const Ogre::Vector3& pos)
			{
				TargetObjName = targetObjName;
				Pos			  = pos;				
			}
		};
		typedef std::pair<Ogre::MovableObject*, Target_t> ObjTargetPair_t;
		std::vector< ObjTargetPair_t >	m_TargetList;

		//�����ʱ��
		float										m_time;
		float										m_time0;
	};	


	// ***********************************************
	// COTETrackCtrllerFactory 
	// ***********************************************
	class COTETrackCtrllerFactory : public CReactorFactoryBase
	{
	public:

		virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
		{
			return new COTETrackCtrller();
		}
		
		virtual void Delete(CReactorBase* rb)
		{			
			SAFE_DELETE(rb)
		}

		virtual void GetDescription(std::string& rDesc)
		{
			rDesc = "�����̬����";
		}

	};

	// ***********************************
	// COTEParabolaTrackCtrller
	// �����߹��������
	// ***********************************
	class _OTEExport COTEParabolaTrackCtrller : public COTETrackCtrller
	{
	public:
		// ����
		virtual void Trigger(Ogre::MovableObject* mo);
		// ����
		virtual void Update(Ogre::MovableObject* mo, float dTime){}
		
		// �ص�
		enum {eTimeOut, eHgtOut, eHitTarget};
		typedef void(*HitCB_t)(int hitType, void* userData); 		

		void UpdateEx(Ogre::MovableObject* mo, float dTime, HitCB_t hitCB, float floorHgt);

	public:

		// ����

		void SetGravity(float gravity)
		{
			m_Gravity = gravity;
		}

		float GetGravity()
		{
			return m_Gravity;
		}

		void SetStartVec(const Ogre::Vector3& startVec)
		{
			m_StartVec = startVec;
		}

		const Ogre::Vector3& GetStartVec()
		{
			return m_StartVec;
		}

	protected:	

		std::vector< std::pair<Ogre::MovableObject*, Ogre::MovableObject*> >	m_TargetList;

		float			m_Gravity;		// �������ٶ�
		
		Ogre::Vector3	m_StartVec;		// ��ʼ�ٶ�	
		
		float			m_TimeEsp;		// ʱ��
	};

	// ***********************************************
	// COTEParabolaTrackCtrllerFactory 
	// ***********************************************
	class COTEParabolaTrackCtrllerFactory : public CReactorFactoryBase
	{
	public:

		virtual CReactorBase* Create(const Ogre::MovableObject* mo, const std::string& rResName)
		{
			return new COTEParabolaTrackCtrller();
		}
		
		virtual void Delete(CReactorBase* rb)
		{			
			SAFE_DELETE(rb)
		}

		virtual void GetDescription(std::string& rDesc)
		{
			rDesc = "�����߹����̬����";
		}

	};

}