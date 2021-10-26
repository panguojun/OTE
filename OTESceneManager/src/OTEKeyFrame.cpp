#include "OTEKeyFrame.h"
#include "OTETrack.h"
#include "OTEMathLib.h"

using namespace OTE;
using namespace Ogre;

// ===================================================
// COTEKeyFrame
// ===================================================
COTEKeyFrame::COTEKeyFrame(void):
m_Time(0),
m_Scaling(Ogre::Vector3::UNIT_SCALE),
m_Position(Ogre::Vector3::ZERO),
m_Angles(Ogre::Vector3::ZERO),
m_Quaternion(Ogre::Quaternion::IDENTITY),
m_DelayTime(0),
m_Colour(Ogre::ColourValue::White),
m_UV(Ogre::Vector2::ZERO), m_UVRot(0), m_UVScl(Ogre::Vector2::UNIT_SCALE),
m_Visible(true)
{
}

COTEKeyFrame::~COTEKeyFrame(void)
{
}


// -------------------------------------------
COTEKeyFrame* COTEKeyFrame::CreateNewKeyFrame()
{
	return new COTEKeyFrame();
}
// -------------------------------------------
void COTEKeyFrame::DeleteKeyFrame(COTEKeyFrame* kf)
{
	if(kf)
		delete kf;
}

// -------------------------------------------
void COTEKeyFrame::BindFrames(float time, const COTEKeyFrame* k1, const COTEKeyFrame* k2, COTETrack* track, COTEKeyFrame* ki)
{
	// 如果两关键桢时间相同

	if(Ogre::Math::Abs(k2->m_Time - k1->m_Time) < 0.0001f)
	{
		*ki	= *k1;		
		return; 
	}

	float factor = (time - k1->m_Time) / (k2->m_Time - k1->m_Time);

	// 位置

	if(k1->m_Position == k2->m_Position)
	{
		ki->m_Position = k1->m_Position;
	}
	else
	{
		if(!track->m_IsBezierCurves)
		{
			// 线形插值

			ki->m_Position	= k1->m_Position * (1.0f - factor) + k2->m_Position * factor;			
		}
		else
		{
			// 贝兹 curve

			float len = (k2->m_Position - k1->m_Position).length();
			
			float t = factor;

			float x0 = k1->m_Position.x;
			float y0 = k1->m_Position.y;
			float z0 = k1->m_Position.z;

			Ogre::Vector3 pt, dir;

			dir = k1->m_Quaternion * Ogre::Vector3::UNIT_Z;		
			pt = k1->m_Position + dir * len * 0.5f;

			float x1 = pt.x;
			float y1 = pt.y;
			float z1 = pt.z;

			dir = k2->m_Quaternion * Ogre::Vector3::UNIT_Z;
			pt = k2->m_Position - dir * len * 0.5f;

			float x2 = pt.x;
			float y2 = pt.y;
			float z2 = pt.z;

			float x3 = k2->m_Position.x;
			float y3 = k2->m_Position.y;
			float z3 = k2->m_Position.z;

			float cx = 3 * (x1 - x0);
			float bx = 3 * (x2 - x1) - cx;
			float ax = x3 - x0 - cx - bx;

			float cy = 3 * (y1 - y0);
			float by = 3 * (y2 - y1) - cy;
			float ay = y3 - y0 - cy - by;

			float cz = 3 * (z1 - z0);
			float bz = 3 * (z2 - z1) - cz;
			float az = z3 - z0 - cz - bz;

			float xt = ax * t * t * t + bx * t * t + cx * t + x0;
			float yt = ay * t * t * t + by * t * t + cy * t + y0;
			float zt = az * t * t * t + bz * t * t + cz * t + z0;

			ki->m_Position.x = xt;
			ki->m_Position.y = yt;
			ki->m_Position.z = zt;
		}
	}

	/// 旋转缩放
	
	if(k1->m_Angles == k2->m_Angles)
	{
		ki->m_Angles = k1->m_Angles;
		ki->m_Quaternion = k1->m_Quaternion;
	}
	else
	{
		ki->m_Angles        =    k1->m_Angles * (1.0f - factor) + k2->m_Angles * factor;
		MathLib::EulerToQuaternion(ki->m_Angles, ki->m_Quaternion);	
	}

	//ki->m_Quaternion	= Ogre::Quaternion::Slerp(factor, k1->m_Quaternion,	k2->m_Quaternion);
	ki->m_Scaling		= k1->m_Scaling == k2->m_Scaling ? k1->m_Scaling : k1->m_Scaling * (1.0f - factor) + k2->m_Scaling * factor;	
	
	/// UV

	ki->m_UV			= k1->m_UV	  == k2->m_UV	 ? k1->m_UV    : k1->m_UV	  * (1.0f - factor) + k2->m_UV	   * factor;	
	ki->m_UVRot			= k1->m_UVRot == k2->m_UVRot ? k1->m_UVRot : k1->m_UVRot  * (1.0f - factor) + k2->m_UVRot  * factor;
	ki->m_UVScl			= k1->m_UVScl == k2->m_UVScl ? k1->m_UVScl : k1->m_UVScl  * (1.0f - factor) + k2->m_UVScl  * factor;	

	/// 扩展属性	

	// 颜色

	ki->m_Colour		= k1->m_Colour == k2->m_Colour ? k1->m_Colour : k1->m_Colour * (1.0f - factor) + k2->m_Colour * factor;

	// 停顿时间 

	if(k1->m_Time > track->m_LastDelayTimePos || k2->m_Time < track->m_LastDelayTimePos)
	{
		track->m_LastDelayTimePos = k1->m_Time;
		ki->m_DelayTime = k1->m_DelayTime;					
	}				
	else
	    ki->m_DelayTime = 0;

	
	// 序列贴图

	int frameSize = (int)k1->m_FrameList.size();
	if(frameSize == 0)
	{				
		if(factor - int(factor) < 0.5f)
			ki->m_TextureName		= k1->m_TextureName;
		else
			ki->m_TextureName		= k2->m_TextureName;
	}
	else
	{
		float ft = (frameSize - 1) * factor;
		float ift = int(ft);
		ift += (ft - ift) > 0.5f ? 1.0f : 0.0f;

		ki->m_TextureName		= k1->m_FrameList[ift];
	}

	// 可见性

	ki->m_Visible				= k1->m_Visible;

}
