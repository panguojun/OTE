#pragma once
#include "OTEStaticInclude.h"
#include "OTECommon.h"

namespace OTE
{
// **********************************
// CEventObjBase
// **********************************

// 在C代码里参数为 arg, 携带游戏对象,事件名称; 在脚本里不带参数
#define OTE_TRIGGER_EVENT(event){HashMap<std::string, EventHandlerList_t>::iterator it = m_EventHandlerMap.find(event);if(it != m_EventHandlerMap.end()){CEventObjBase::EventArg_t arg;	arg.strObjName = GetName();	arg.strObjEventName = event;	Ogre::LogManager::getSingleton().logMessage(Ogre::String("OTE对象事件: ") + GetName() + "::" + event); for(unsigned int i = 0; i < it->second.size(); ++ i){(*it->second[i])(arg);}} }
// 在C代码里参数为 arg, 携带游戏对象,事件名称,用户字符串数据; 在脚本里参数为ud
#define OTE_TRIGGER_EVENT_WITHUSERSTRING(event, ud){HashMap<std::string, EventHandlerList_t>::iterator it = m_EventHandlerMap.find(event);if(it != m_EventHandlerMap.end()){CEventObjBase::EventArg_t arg;	arg.strObjName = GetName();	arg.strObjEventName = event; arg.strUserData = ud;	Ogre::LogManager::getSingleton().logMessage(Ogre::String("OTE对象事件: ") + arg.strObjName + "::" + event); for(unsigned int i = 0; i < it->second.size(); ++ i){(*it->second[i])(arg);}} }

#define OTE_TRIGGER_EVENT_NOLOG(event){HashMap<std::string, EventHandlerList_t>::iterator it = m_EventHandlerMap.find(event);if(it != m_EventHandlerMap.end()){CEventObjBase::EventArg_t arg;	arg.strObjName = GetName();	arg.strObjEventName = event;	for(unsigned int i = 0; i < it->second.size(); ++ i){(*it->second[i])(arg);}} }
#define OTE_TRIGGER_EVENT_WITHUSERSTRING_NOLOG(event, ud){HashMap<std::string, EventHandlerList_t>::iterator it = m_EventHandlerMap.find(event);if(it != m_EventHandlerMap.end()){CEventObjBase::EventArg_t arg;	arg.strObjName = GetName();	arg.strObjEventName = event; arg.strUserData = ud;	for(unsigned int i = 0; i < it->second.size(); ++ i){(*it->second[i])(arg);}} }

// 在C代码里参数为 arg, 携带游戏对象,事件名称,用户数据;  
#define OTE_TRIGGER_EVENT_WITHUSERDATA(event, ud){HashMap<std::string, EventHandlerList_t>::iterator it = m_EventHandlerMap.find(event);if(it != m_EventHandlerMap.end()){CEventObjBase::EventArg_t arg;	arg.strObjName = GetName();	arg.strObjEventName = event; arg.pUserData = ud;	Ogre::LogManager::getSingleton().logMessage(Ogre::String("OTE对象事件: ") + arg.strObjName + "::" + event); for(unsigned int i = 0; i < it->second.size(); ++ i){(*it->second[i])(arg);}} }

// 传入参数列表  
#define OTE_TRIGGER_EVENT_WITHARG(event, arg){HashMap<std::string, EventHandlerList_t>::iterator it = m_EventHandlerMap.find(event);if(it != m_EventHandlerMap.end()){Ogre::LogManager::getSingleton().logMessage(Ogre::String("OTE对象事件: ") + arg.strObjName + "::" + event); for(unsigned int i = 0; i < it->second.size(); ++ i){(*it->second[i])(arg);}} }


class _OTEExport CEventObjBase
{
public:
	struct EventArg_t
	{
		std::string strObjName;			// 游戏对象名称 
		std::string strObjEventName;	// 游戏事件名称
		std::string	strUserData;		// 用户字符串数据
		mutable void* pUserData;		// 用户数据

		std::vector<void*> vParamList;	// 参数列表
	};

	typedef void(*EventHandler)(const EventArg_t& rArg);

public: /// 方法

	// 添加事件回调函数

	void AddEventHandler(const std::string& szEventName, EventHandler eventHandler)
	{
		HashMap<std::string, EventHandlerList_t>::iterator it = m_EventHandlerMap.find(szEventName);
		if(it == m_EventHandlerMap.end())
		{					
			m_EventHandlerMap[szEventName] = EventHandlerList_t();
			m_EventHandlerMap[szEventName].push_back(eventHandler);
		}
		else
		{
			bool isFind = false;	
			for(unsigned int i = 0; i < it->second.size(); ++ i)
			{
				if(it->second[i] == eventHandler)
				{
					isFind = true;
					break;
				}
			}

			if(!isFind)
				it->second.push_back(eventHandler);
		}	
	}
	
	// 移除事件回调函数

	void RemoveEventHandler(const std::string& szEventName, EventHandler eventHandler)
	{
		HashMap<std::string, EventHandlerList_t>::iterator it = m_EventHandlerMap.find(szEventName);
		if(it != m_EventHandlerMap.end())
		{		
			EventHandlerList_t::iterator ita = it->second.begin();
			while(ita != it->second.end())
			{
				if(*ita == eventHandler)
				{
					it->second.erase(ita);

					break;
				}
				
				++ ita;
			}
		}		
	}

	// 移除所有事件回调函数

	void RemoveEventHandlers()
	{	
		m_EventHandlerMap.clear();
	}

	// 手工触发

	void TriggerEvent(const std::string& eventName)
	{
		OTE_TRIGGER_EVENT(eventName)
	}

	void TriggerEvent(const std::string& eventName, const std::string& userData)
	{
		OTE_TRIGGER_EVENT_WITHUSERSTRING_NOLOG(eventName, userData)
	}

	void TriggerEvent(const std::string& eventName, void* userData)
	{
		OTE_TRIGGER_EVENT_WITHUSERDATA(eventName, userData)
	}

	void TriggerEvent(const std::string& eventName, const EventArg_t& arg)
	{
		OTE_TRIGGER_EVENT_WITHARG(eventName, arg)
	}
	
public: /// 实现接口

	virtual std::string GetName() = 0;

protected: /// 属性

	// 事件表

	typedef std::vector<EventHandler> EventHandlerList_t;
	HashMap<std::string, EventHandlerList_t>	m_EventHandlerMap;

};

}
