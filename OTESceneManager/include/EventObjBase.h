#pragma once
#include "OTEStaticInclude.h"
#include "OTECommon.h"

namespace OTE
{
// **********************************
// CEventObjBase
// **********************************

// ��C���������Ϊ arg, Я����Ϸ����,�¼�����; �ڽű��ﲻ������
#define OTE_TRIGGER_EVENT(event){HashMap<std::string, EventHandlerList_t>::iterator it = m_EventHandlerMap.find(event);if(it != m_EventHandlerMap.end()){CEventObjBase::EventArg_t arg;	arg.strObjName = GetName();	arg.strObjEventName = event;	Ogre::LogManager::getSingleton().logMessage(Ogre::String("OTE�����¼�: ") + GetName() + "::" + event); for(unsigned int i = 0; i < it->second.size(); ++ i){(*it->second[i])(arg);}} }
// ��C���������Ϊ arg, Я����Ϸ����,�¼�����,�û��ַ�������; �ڽű������Ϊud
#define OTE_TRIGGER_EVENT_WITHUSERSTRING(event, ud){HashMap<std::string, EventHandlerList_t>::iterator it = m_EventHandlerMap.find(event);if(it != m_EventHandlerMap.end()){CEventObjBase::EventArg_t arg;	arg.strObjName = GetName();	arg.strObjEventName = event; arg.strUserData = ud;	Ogre::LogManager::getSingleton().logMessage(Ogre::String("OTE�����¼�: ") + arg.strObjName + "::" + event); for(unsigned int i = 0; i < it->second.size(); ++ i){(*it->second[i])(arg);}} }

#define OTE_TRIGGER_EVENT_NOLOG(event){HashMap<std::string, EventHandlerList_t>::iterator it = m_EventHandlerMap.find(event);if(it != m_EventHandlerMap.end()){CEventObjBase::EventArg_t arg;	arg.strObjName = GetName();	arg.strObjEventName = event;	for(unsigned int i = 0; i < it->second.size(); ++ i){(*it->second[i])(arg);}} }
#define OTE_TRIGGER_EVENT_WITHUSERSTRING_NOLOG(event, ud){HashMap<std::string, EventHandlerList_t>::iterator it = m_EventHandlerMap.find(event);if(it != m_EventHandlerMap.end()){CEventObjBase::EventArg_t arg;	arg.strObjName = GetName();	arg.strObjEventName = event; arg.strUserData = ud;	for(unsigned int i = 0; i < it->second.size(); ++ i){(*it->second[i])(arg);}} }

// ��C���������Ϊ arg, Я����Ϸ����,�¼�����,�û�����;  
#define OTE_TRIGGER_EVENT_WITHUSERDATA(event, ud){HashMap<std::string, EventHandlerList_t>::iterator it = m_EventHandlerMap.find(event);if(it != m_EventHandlerMap.end()){CEventObjBase::EventArg_t arg;	arg.strObjName = GetName();	arg.strObjEventName = event; arg.pUserData = ud;	Ogre::LogManager::getSingleton().logMessage(Ogre::String("OTE�����¼�: ") + arg.strObjName + "::" + event); for(unsigned int i = 0; i < it->second.size(); ++ i){(*it->second[i])(arg);}} }

// ��������б�  
#define OTE_TRIGGER_EVENT_WITHARG(event, arg){HashMap<std::string, EventHandlerList_t>::iterator it = m_EventHandlerMap.find(event);if(it != m_EventHandlerMap.end()){Ogre::LogManager::getSingleton().logMessage(Ogre::String("OTE�����¼�: ") + arg.strObjName + "::" + event); for(unsigned int i = 0; i < it->second.size(); ++ i){(*it->second[i])(arg);}} }


class _OTEExport CEventObjBase
{
public:
	struct EventArg_t
	{
		std::string strObjName;			// ��Ϸ�������� 
		std::string strObjEventName;	// ��Ϸ�¼�����
		std::string	strUserData;		// �û��ַ�������
		mutable void* pUserData;		// �û�����

		std::vector<void*> vParamList;	// �����б�
	};

	typedef void(*EventHandler)(const EventArg_t& rArg);

public: /// ����

	// ����¼��ص�����

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
	
	// �Ƴ��¼��ص�����

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

	// �Ƴ������¼��ص�����

	void RemoveEventHandlers()
	{	
		m_EventHandlerMap.clear();
	}

	// �ֹ�����

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
	
public: /// ʵ�ֽӿ�

	virtual std::string GetName() = 0;

protected: /// ����

	// �¼���

	typedef std::vector<EventHandler> EventHandlerList_t;
	HashMap<std::string, EventHandlerList_t>	m_EventHandlerMap;

};

}
