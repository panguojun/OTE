#pragma once

#include "OTEStaticInclude.h"
#include "OTEReactorBase.h"
#include "OTEEntityRender.h"

namespace OTE
{		
	// ***********************************
	// COTETrackCtrller （多实体 与行动邦定）
	// 轨道控制器
	// 比如远程魔法需要的 "动态轨迹" 等效果
	// note: 不是单体!
	// 属于"作用"的一种
	// ***********************************
	class _OTEExport COTETrackCtrller : public CReactorBase
	{
	public:
		// 触发
		virtual void Trigger(Ogre::MovableObject* mo);
		// 更新
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

		//效果-目标

		void AddMagicTarget(Ogre::MovableObject* mag, Ogre::MovableObject* target);	

		void RemoveMagicTarget(Ogre::MovableObject* mag);

		//清除全部目标
		void Clear();

		//设置轨道时间长度
		void SetLength(float time)
		{
			m_time0 = m_time = time;		
		}

		//得到目标数量

		int GetTargetNum()
		{
			return int(m_TargetList.size());
		}

	public:

		//轨道创建者
		COTEEntity*								m_Creator;

	protected:

		//目标轨道列表

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

		//轨道总时长
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
			rDesc = "轨道动态控制";
		}

	};

	// ***********************************
	// COTEParabolaTrackCtrller
	// 抛物线轨道控制器
	// ***********************************
	class _OTEExport COTEParabolaTrackCtrller : public COTETrackCtrller
	{
	public:
		// 触发
		virtual void Trigger(Ogre::MovableObject* mo);
		// 更新
		virtual void Update(Ogre::MovableObject* mo, float dTime){}
		
		// 回调
		enum {eTimeOut, eHgtOut, eHitTarget};
		typedef void(*HitCB_t)(int hitType, void* userData); 		

		void UpdateEx(Ogre::MovableObject* mo, float dTime, HitCB_t hitCB, float floorHgt);

	public:

		// 重力

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

		float			m_Gravity;		// 重力加速度
		
		Ogre::Vector3	m_StartVec;		// 初始速度	
		
		float			m_TimeEsp;		// 时间
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
			rDesc = "抛物线轨道动态控制";
		}

	};

}