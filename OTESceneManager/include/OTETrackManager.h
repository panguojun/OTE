#pragma once

#include "OTEStaticInclude.h"
#include "OTEKeyFrame.h"
#include "OTETrack.h"
#include "tinyxml.h"
#include "OTETrackXmlLoader.h"

namespace OTE
{
	// ****************************************
	// 由于各种不同用途的track的相似性， 这个
	// 管理类设计被成世界唯一, 在不同的用途上 
	// 有不同的 usage 来匹配
	// ****************************************
	class _OTEExport COTETrackManager
	{
	public:
		// 获取TrackManager
		static COTETrackManager* GetInstance();

		// 销毁TrackManager
		static void Destroy();

	public:		
		// 是否已经存在
		bool IsTrackPresent(int id);

		// 取出指定id的Track
		COTETrack* GetTrack(int id, COTETrackLoader* loarder);

		// 移除指定的Track
		void Delete(int id);

		// 创建新的Track

		COTETrack* CreateNewTrack(int id, COTETrackLoader* loarder = NULL); // loarder = NULL时 不保存
	
	protected:

		//清理内存
		void Clear();	

	protected:	

		static COTETrackManager* s_Instance;

		//保存全部已经使用的静态Track
		HashMap<int, COTETrack*> m_TrackListCach;

	};	

}