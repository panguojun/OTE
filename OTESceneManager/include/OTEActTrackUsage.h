#pragma once
#include "OTEStaticInclude.h"
#include "OTETrackXmlLoader.h"
#include "OTETrack.h"

namespace OTE
{

class COTEAction;
class COTEActorEntity;
// ************************************
// COTEActTrackUsage
// 用于行动的track 绑定实现，相当于adepter
// ************************************

class _OTEExport COTEActTrackUsage
{
public:

	// 更新
	static void UpdateTrackBinding(
						COTEAction* action, 
						COTETrack* track, 
						Ogre::MovableObject* mo, 
						float time);

	static COTETrackXmlLoader* GetTrackXmlLoader();

	static void Destroy();

	static COTETrack* Clone(COTETrack* srcTrack, int trackID);

	static void Clone(COTETrack* srcTrack, COTETrack* desTrack);
	
	static void GetKeyFrameDTrans(const COTEActorEntity* actor, const Ogre::SceneNode* targetSN, const COTETrack* track,
								 Ogre::Vector3& Pos, Ogre::Quaternion& Qt, Ogre::Vector3& Sc);

	static void BindTrans(const COTEActorEntity* actor, const COTETrack* track,  const COTEKeyFrame *kf, 
			Ogre::Vector3& Pos, Ogre::Quaternion& Qt, Ogre::Vector3& Sc);

protected:

	static COTETrackXmlLoader		s_TrackXmlLoader;
	
};


}
