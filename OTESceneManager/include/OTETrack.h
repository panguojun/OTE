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

		bool				m_IsBind;			// 是否邦定

	};

	// *********************************************** //
	// COTETrack 
	// 属于"作用" 的一种

	// 跟物件绑定的轨道,用于产生物件的动态效果。
	// 这种轨道是由 时间轴 加上 之上的种种
	// 用于对物件的包括节点 材质等属性的改变的关键帧组成。
	// 是共享的

	// *********************************************** //
	class _OTEExport COTETrack : public CInterBase
	{		
	friend class COTEKeyFrame;
	public:	
		COTETrack();
		~COTETrack();

		//添加关键桢	
		bool AddKeyFrame(COTEKeyFrame* Key);	

		//查询某一桢
		COTEKeyFrame* GetFrameAt(float Time);

		//查询某一关键桢
		COTEKeyFrame* GetKeyFrameAt(float Time);
		
		//删除某一帧
		void RemoveKeyFrameAt(float Time);

		//桢数量
		int GetFrameNum()
		{
			return (int)m_KeyFrameList.size();
		}

		// clone
		COTETrack* Clone();

		//查询列表
		void GetKeyFrameList(std::list <COTEKeyFrame*> *pList)
		{			
			std::list <COTEKeyFrame*>::iterator it = m_KeyFrameList.begin();
			while(it != m_KeyFrameList.end())
			{
				pList->push_back(*it);
				it++;
			}
		}
		//设置列表
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

		//得到轨道长度
		float GetLength()
		{
			return m_AnimMaxTime - m_AnimMinTime;
		}

		//清理轨迹
		void Clear();

		//取得起始关键帧
		COTEKeyFrame* GetFirstKey()
		{
			if(m_KeyFrameList.size() > 0)
				return *m_KeyFrameList.begin();
			return NULL;
			
		}

		//取得结束关键帧
		COTEKeyFrame* GetEndKey()
		{
			std::list <COTEKeyFrame*>::iterator end = m_KeyFrameList.end();
			return *(--end);
		}

		//设置绑定骨骼
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

		/// 轨道属性	
				
		bool			m_IsBezierCurves;	// 是否使用Bezier Curves 

		float			m_AnimMaxTime;		// 动画最大时间	
		float			m_AnimMinTime;		// 动画最小时间	

		std::string		m_AttBoneName;		//该轨迹绑定骨骼的名称

		Ogre::TextureUnitState::TextureAddressingMode	m_TexAddrMode;	//材质寻址模式 
		
		std::string		m_Desc;				// 描述
	

	protected:

		//用于延迟停顿效果
		float m_LastDelayTimePos;
		//得到中间桢	
		void GetKeyAt(const COTEKeyFrame* K1, const COTEKeyFrame* K2,  float Time, COTEKeyFrame* Ki); 

		std::list <COTEKeyFrame*> m_KeyFrameList; // ！在创建时按照时间从小到大顺序排列
	};

	// ************************************** //
	// COTETrackLoader
	// ************************************** //
	class COTETrackLoader
	{
	public:

		//加载指定ID的Track
		virtual COTETrack* LoadTrack(unsigned int id) = 0;

		//保存指定Id的Track
		virtual void TrackWriteXml(int id) = 0; 

		// 移除track
		virtual void RemoveTrack(int id) = 0;

		//保存Track文件
		virtual void SaveFile(const std::string& fileName) = 0;

	};	

}