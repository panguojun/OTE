#include "OgreResourceBackgroundQueue_Win.h"
#include "OgreSkeletonSerializer.h"

using namespace Ogre;
using namespace OTE;

//-----------------------------------------------------------------------
ResourceBackgroundQueue* ResourceBackgroundQueue::s_pSingleton = NULL;
ResourceBackgroundQueue* ResourceBackgroundQueue::GetSingleton()
{
	if(!s_pSingleton) 
	{
		s_pSingleton = new ResourceBackgroundQueue();
		s_pSingleton->initialise();
	}
	return s_pSingleton; 
}

//-----------------------------------------------------------------------	
void ResourceBackgroundQueue::DestroySingleton()
{
	SAFE_DELETE(s_pSingleton)
}

//------------------------------------------------------------------------
ResourceBackgroundQueue::ResourceBackgroundQueue()
	:mNextTicketID(0), m_Thread(0)
{
}
//------------------------------------------------------------------------
ResourceBackgroundQueue::~ResourceBackgroundQueue()
{
	shutdown();
}
//------------------------------------------------------------------------
void ResourceBackgroundQueue::initialise(void)
{
#if OGRE_THREAD_SUPPORT
	OGRE_LOCK_AUTO_MUTEX
			
	m_Event  =  ::CreateEvent(NULL, TRUE, TRUE, "addRequest");  
	::ResetEvent(m_Event);

	m_Thread = CreateThread( 
			NULL,                        // default security attributes 
			0,                           // use default stack size  
			(LPTHREAD_START_ROUTINE) ThreadProc,                  // thread function 
			0,                        // argument to thread function 
			0,                           // use default creation flags 
			0);                // returns the thread identifier 
		if (m_Thread == NULL) 
			::MessageBox( NULL, "创建后台资源加载线程失败!", "错误", MB_OK );		
	
#else
	LogManager::getSingleton().logMessage(
		"ResourceBackgroundQueue - threading disabled");	
#endif
}
//------------------------------------------------------------------------
void ResourceBackgroundQueue::shutdown(void)
{
#if OGRE_THREAD_SUPPORT
	if (m_Thread)
	{
		// Put a shutdown request on the queue
		Request req;
		req.type = RT_SHUTDOWN;
		addRequest(req);

		// Wait for thread to finish
		::WaitForSingleObject(m_Thread, INFINITE);
		::CloseHandle(m_Thread);
		::CloseHandle(m_Event);		

		m_Thread = 0;
		m_Event = 0;
		mRequestQueue.clear();
		mRequestTicketMap.clear();
	}
#endif
}
//------------------------------------------------------------------------
BackgroundProcessTicket ResourceBackgroundQueue::initialiseResourceGroup(
	const String& name, ResourceBackgroundQueueListener* listener)
{
#if OGRE_THREAD_SUPPORT
	if (!m_Thread)
	{
		OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
			"Thread not initialised",
			"ResourceBackgroundQueue::initialiseResourceGroup");
	}
	// queue a request
	Request req;
	req.type = RT_INITIALISE_GROUP;
	req.groupName = name;
	req.listener = listener;
	return addRequest(req);
#else
	// synchronous
	ResourceGroupManager::getSingleton().initialiseResourceGroup(name);
	return 0; 
#endif
}
//------------------------------------------------------------------------
BackgroundProcessTicket 
ResourceBackgroundQueue::initialiseAllResourceGroups( 
	ResourceBackgroundQueueListener* listener)
{
#if OGRE_THREAD_SUPPORT
	if (!m_Thread)
	{
		OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
			"Thread not initialised",
			"ResourceBackgroundQueue::initialiseAllResourceGroups");
	}
	// queue a request
	Request req;
	req.type = RT_INITIALISE_ALL_GROUPS;
	req.listener = listener;
	return addRequest(req);
#else
	// synchronous
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	return 0; 
#endif
}
//------------------------------------------------------------------------
BackgroundProcessTicket ResourceBackgroundQueue::loadResourceGroup(
	const String& name, ResourceBackgroundQueueListener* listener)
{
#if OGRE_THREAD_SUPPORT
	if (!m_Thread)
	{
		OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
			"Thread not initialised",
			"ResourceBackgroundQueue::loadResourceGroup");
	}
	// queue a request
	Request req;
	req.type = RT_LOAD_GROUP;
	req.groupName = name;
	req.listener = listener;
	return addRequest(req);
#else
	// synchronous
	ResourceGroupManager::getSingleton().loadResourceGroup(name);	
	return 0; 
#endif
}
//------------------------------------------------------------------------
BackgroundProcessTicket ResourceBackgroundQueue::load(
	const String& name,
	const String& type,
	const String& group, 
	ResourceBackgroundQueueListener* listener,
	void*	objectPtr)
{
#if OGRE_THREAD_SUPPORT
	if (!m_Thread)
	{
		OGRE_EXCEPT(Exception::ERR_INTERNAL_ERROR, 
			"Thread not initialised",
			"ResourceBackgroundQueue::load");
	}
	// queue a request
	Request req;
	req.type = RT_LOAD_RESOURCE;
	req.resourceType = type;
	req.resourceName = name;
	req.groupName = group;	
	req.listener = listener;
	req.objectPtr = objectPtr;

	return addRequest(req);
#else
	// synchronous
	ResourceManager* rm = 
		ResourceGroupManager::getSingleton()._getResourceManager(type);
	rm->load(name, group);

	if(listener)
		listener->operationCompleted(0, objectPtr);

	return 0; 
#endif
}
//------------------------------------------------------------------------
bool ResourceBackgroundQueue::isProcessComplete(
		BackgroundProcessTicket ticket)
{
	return mRequestTicketMap.find(ticket) == mRequestTicketMap.end();
}
//------------------------------------------------------------------------
#if OGRE_THREAD_SUPPORT
BackgroundProcessTicket ResourceBackgroundQueue::addRequest(Request& req)
{
	// Lock
	OGRE_LOCK_AUTO_MUTEX

	req.ticketID = ++mNextTicketID;
	mRequestQueue.push_back(req);
	Request* requestInList = &(mRequestQueue.back());
	mRequestTicketMap[req.ticketID] = requestInList;

	// Notify to wake up loading thread
	::SetEvent(m_Event);
	return req.ticketID;
}	

//------------------------------------------------------------------------
DWORD WINAPI ResourceBackgroundQueue::ThreadProc(LPVOID lpParameter)
{
	// Background thread implementation 
	// Static (since no params allowed), so get instance
	ResourceBackgroundQueue& queueInstance = 
		*ResourceBackgroundQueue::GetSingleton();

	bool shuttingDown = false;
	// Spin forever until we're told to shut down
	while (!shuttingDown)
	{
		Request* req;
		// Manual scope block just to define scope of lock
		{
			OGRE_LOCK_MUTEX(queueInstance.OGRE_AUTO_MUTEX_NAME)
			if (queueInstance.mRequestQueue.empty())
			{
				// frees lock and suspends the thread					
				if(ogrenameLock.OGRE_AUTO_MUTEX_NAME)
				{
					LeaveCriticalSection( &ogrenameLock.OGRE_AUTO_MUTEX_NAME->cs ); 
					ogrenameLock.OGRE_AUTO_MUTEX_NAME = NULL;
				}

				::WaitForSingleObject(queueInstance.m_Event, INFINITE);
				::ResetEvent(queueInstance.m_Event);
				
			}		

			// Process one request
			assert(!queueInstance.mRequestQueue.empty());
			req = &(queueInstance.mRequestQueue.front());
		} 

		ResourceManager* rm = 0;
		switch (req->type)
		{
		case RT_INITIALISE_GROUP:
			ResourceGroupManager::getSingleton().initialiseResourceGroup(
				req->groupName);
			break;
		case RT_INITIALISE_ALL_GROUPS:
			ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
			break;
		case RT_LOAD_GROUP:
			ResourceGroupManager::getSingleton().loadResourceGroup(
				req->groupName);
			break;
		case RT_LOAD_RESOURCE:
			
			//if(req->resourceType == "animation")
			//{
			//	Ogre::DataStreamPtr stream = 
			//		Ogre::ResourceGroupManager::getSingleton().openResource(req->resourceName, req->groupName);

			//	Ogre::SkeletonSerializer BINssr;
			//	BINssr.readAnimation(stream, skeleton.getPointer());
			//}
			//else
			{
				// 有管理器的

				ResourceManager* rm = 
					ResourceGroupManager::getSingleton()._getResourceManager(req->resourceType);
				rm->load(req->resourceName, req->groupName);
			}

			if(req->listener)
				req->listener->operationCompleted(req->ticketID, req->objectPtr);

			break;
		case RT_SHUTDOWN:
			// That's all folks
			shuttingDown = true;
			break;
		};

		{
			// re-lock to consume completed request

			OGRE_LOCK_MUTEX(queueInstance.OGRE_AUTO_MUTEX_NAME)

			// Consume the ticket
			queueInstance.mRequestTicketMap.erase(req->ticketID);
			queueInstance.mRequestQueue.pop_front();
			::ResetEvent(queueInstance.m_Event);
		}
	}

	return 1;
}
#endif