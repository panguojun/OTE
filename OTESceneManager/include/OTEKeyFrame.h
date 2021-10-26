#pragma once
#include "OTEStaticInclude.h"
#include "OgreVector2.h"
#include "OgreVector3.h"
#include "OgreColourValue.h"
#include "OgreQuaternion.h"

namespace OTE
{

// ************************************************** //
// �ؼ���
// ************************************************** //

class _OTEExport COTEKeyFrame
{
friend class COTETrack;
protected:
	COTEKeyFrame(void);
	virtual ~COTEKeyFrame(void);
public:

	/// ���� / ɾ��

	static COTEKeyFrame* CreateNewKeyFrame();

	static void DeleteKeyFrame(COTEKeyFrame* kf);

	/// ���

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

	float				m_Time;			// ʱ�� 
		
	float				m_DelayTime;	// ͣ��ʱ��

	// ������Ա仯

	Ogre::Vector3		m_Position;		// λ��
	Ogre::Vector3		m_Angles;		// ��ת�Ƕ�
	Ogre::Quaternion	m_Quaternion;	// ��ת
	Ogre::Vector3		m_Scaling;		// ����	
	bool				m_Visible;		// �ɼ���

	// �������Ա仯

	Ogre::Vector2		m_UV;			// UV����
	float				m_UVRot;		// UV��ת
	Ogre::Vector2		m_UVScl;		// UV����
	Ogre::ColourValue	m_Colour;		// ��ɫ	
	Ogre::String		m_TextureName;	// ͼƬ
	std::vector<Ogre::String>	m_FrameList;	// ��ͼƬ�б�

};

}
