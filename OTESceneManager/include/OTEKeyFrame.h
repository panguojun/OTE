#pragma once
#include "OTEStaticInclude.h"
#include "OgreVector2.h"
#include "OgreVector3.h"
#include "OgreColourValue.h"
#include "OgreQuaternion.h"

namespace OTE
{

// ************************************************** //
// 关键桢
// ************************************************** //

class _OTEExport COTEKeyFrame
{
friend class COTETrack;
protected:
	COTEKeyFrame(void);
	virtual ~COTEKeyFrame(void);
public:

	/// 创建 / 删除

	static COTEKeyFrame* CreateNewKeyFrame();

	static void DeleteKeyFrame(COTEKeyFrame* kf);

	/// 混合

	static void BindFrames(float time, const COTEKeyFrame* k1, const COTEKeyFrame* k2, COTETrack* track, COTEKeyFrame* ki);

public:

	inline COTEKeyFrame& operator= (const COTEKeyFrame& other)
	{
		m_Time				= 	other.m_Time;
		m_Position			= 	other.m_Position;
		m_Angles			= 	other.m_Angles;
		m_Quaternion		= 	other.m_Quaternion;
		m_Scaling			= 	other.m_Scaling;

		m_DelayTime			= 	other.m_DelayTime;
		m_UV				= 	other.m_UV;		
		m_UVRot				= 	other.m_UVRot;		
		m_UVScl				= 	other.m_UVScl;		
		m_TextureName		= 	other.m_TextureName;	
		m_Colour			= 	other.m_Colour;		
		
		m_FrameList.clear();

		for(unsigned int i = 0; i < other.m_FrameList.size(); ++ i)
		{
			m_FrameList.push_back(other.m_FrameList[i]);
		}

		return *this;
	}

public:

	float				m_Time;			// 时间 
		
	float				m_DelayTime;	// 停顿时间

	// 结点属性变化

	Ogre::Vector3		m_Position;		// 位置
	Ogre::Vector3		m_Angles;		// 旋转角度
	Ogre::Quaternion	m_Quaternion;	// 旋转
	Ogre::Vector3		m_Scaling;		// 缩放	
	bool				m_Visible;		// 可见性

	// 材质属性变化

	Ogre::Vector2		m_UV;			// UV坐标
	float				m_UVRot;		// UV旋转
	Ogre::Vector2		m_UVScl;		// UV缩放
	Ogre::ColourValue	m_Colour;		// 颜色	
	Ogre::String		m_TextureName;	// 图片
	std::vector<Ogre::String>	m_FrameList;	// 桢图片列表

};

}
