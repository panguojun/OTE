/*
-----------------------------------------------------------------------------
Add BY ROMEO
这个部分是由于 不想引入Boost 而自己用win api实现 后台资源加载线程
-----------------------------------------------------------------------------
*/
#pragma once

#include "OTEStaticInclude.h"

#if OGRE_THREAD_SUPPORT
  #include <windows.h>
#endif

namespace OTE
{
/// Identifier of a background process
typedef unsigned long BackgroundProcessTicket;
	
// ***********************************************
// ResourceBackgroundQueueListener
// ***********************************************
class _OTEExport ResourceBackgroundQueueListener
{
public:
	
	virtual void operationCompleted(BackgroundProcessTicket ticket, void* objPtr) = 0;
};

// ***********************************************
// ResourceBackgroundQueue
// ***********************************************
class _OTEExport ResourceBackgroundQueue
{
protected:
	/** Enumerates the type of requests */
	enum RequestType
	{
		RT_INITIALISE_GROUP,
		RT_INITIALISE_ALL_GROUPS,
		RT_LOAD_GROUP,
		RT_LOAD_RESOURCE,
		RT_SHUTDOWN
	};
	/** Encapsulates a queued request for the background queue */
	struct Request
	{
		BackgroundProcessTicket ticketID;
		RequestType type;
		Ogre::String resourceName;
		Ogre::String resourceType;
		Ogre::String groupName;		
		ResourceBackgroundQueueListener* listener;
		void*	objectPtr;
	};
	typedef std::list<Request> RequestQueue;
	typedef std::map<BackgroundProcessTicket, Request*> RequestTicketMap;
	
	/// Queue of requests, used to store and order requests
	RequestQueue mRequestQueue;
	
	/// Request lookup by ticket
	RequestTicketMap mRequestTicketMap;

#if OGRE_THREAD_SUPPORT

	HANDLE		m_Thread;

	HANDLE		m_Event;

	static DWORD WINAPI ThreadProc(LPVOID lpParameter);

	/// Internal method for adding a request; also assigns a ticketID
	BackgroundProcessTicket addRequest(Request& req);
#else
	/// Dummy
	void* m_Thread;
#endif	 

	/// Private mutex, not allowed to lock from outside
	OGRE_AUTO_MUTEX

public:
	/// Next ticket ID
	unsigned long mNextTicketID;

public:
	ResourceBackgroundQueue();
	virtual ~ResourceBackgroundQueue();

	/** Initialise the background queue system. */
	virtual void initialise(void);
	
	/** Shut down the background queue system. */
	virtual void shutdown(void);

	virtual BackgroundProcessTicket initialiseResourceGroup(
		const Ogre::String& name, ResourceBackgroundQueueListener* listener = 0);

	virtual BackgroundProcessTicket initialiseAllResourceGroups( 
		ResourceBackgroundQueueListener* listener = 0);

	virtual BackgroundProcessTicket loadResourceGroup(const Ogre::String& name, 
		ResourceBackgroundQueueListener* listener = 0);

	virtual BackgroundProcessTicket load(
		const Ogre::String& name, 
		const Ogre::String& type, 
        const Ogre::String& group = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		ResourceBackgroundQueueListener* listener = 0,
		void*	objectPtr = 0);


	virtual bool isProcessComplete(BackgroundProcessTicket ticket);

public:

	/// 单体

	static ResourceBackgroundQueue* GetSingleton();

	static void DestroySingleton();	

	// 单体对象
	static ResourceBackgroundQueue*	s_pSingleton;
};

}