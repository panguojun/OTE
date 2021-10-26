#include "OTEStaticInclude.h"
#include "OTEAction.h"
#include "OTEActorEntity.h"
#include "OTEActTrackUsage.h"

#include "OTEBillBoardSet.h"
#include "OTEParticlesystem.h"
#include "OTEQTSceneManager.h"
#include "OTETrackManager.h"
#include "OTEBillBoardChain.h"

#include "OTED3DManager.h"

using namespace Ogre;
using namespace OTE;

// ----------------------------------
COTETrackXmlLoader		COTEActTrackUsage::s_TrackXmlLoader;
// ----------------------------------
COTETrackXmlLoader* COTEActTrackUsage::GetTrackXmlLoader()
{
	if(!s_TrackXmlLoader.IsInit())
	{
		// 初始化加载器

		const Ogre::String& TrackFile = OTE::OTEHelper::GetOTESetting("ResConfig", "TrackFile");

		Ogre::String filePath;
		Ogre::String fileFullName;
		OTEHelper::GetFullPath(TrackFile, filePath, fileFullName);
		s_TrackXmlLoader.Init(fileFullName);
	}

	return &s_TrackXmlLoader;

}

// -------------------------------------------
// 复制一份
void COTEActTrackUsage::Clone(COTETrack* srcTrack, COTETrack* desTrack)
{
	assert(srcTrack && desTrack && srcTrack != desTrack);

	/// 属性

	desTrack->m_AnimMaxTime = srcTrack->m_AnimMaxTime;
	desTrack->m_AnimMinTime = srcTrack->m_AnimMaxTime;
	desTrack->m_AttBoneName = srcTrack->m_AttBoneName;
	
	desTrack->m_IsBezierCurves = srcTrack->m_IsBezierCurves;
	desTrack->m_Desc           = srcTrack->m_Desc;
	desTrack->m_TexAddrMode    = srcTrack->m_TexAddrMode;

	// 关键帧列表

	std::list<COTEKeyFrame*> kl;
	srcTrack->GetKeyFrameList(&kl);

	std::list<OTE::COTEKeyFrame*>::iterator it = kl.begin();
	while(it != kl.end())
	{			
		OTE::COTEKeyFrame* k1 = COTEKeyFrame::CreateNewKeyFrame();
		*k1 = *(*it);				
		
		desTrack->AddKeyFrame(k1);
		
		++ it;
	}
}

// -------------------------------------------
COTETrack* COTEActTrackUsage::Clone(COTETrack* srcTrack, int trackID)
{
	COTETrack* pTrack = COTETrackManager::GetInstance()->GetTrack(trackID, GetTrackXmlLoader());

	Clone(srcTrack, pTrack);

	return pTrack;
}

// -------------------------------------------
void COTEActTrackUsage::Destroy()
{
	if(s_TrackXmlLoader.IsInit())
		s_TrackXmlLoader.Destroy();
}

// -------------------------------------------
void COTEActTrackUsage::BindTrans(const COTEActorEntity* actor, const COTETrack* track,  const COTEKeyFrame *kf,
								Ogre::Vector3& Pos, Ogre::Quaternion& Qt, Ogre::Vector3& Sc)
{	
	// 更新位置方向缩放等参数	

	if(track->m_AttBoneName.empty())
	{
		// 如果角色有本体动画　邦定物应该邦定到角色原始坐标
		const Ogre::SceneNode *n = const_cast<COTEActorEntity*>(actor)->GetParentSceneNode();

		Pos = n->getWorldPosition() + 
				n->getWorldOrientation() * kf->m_Position;
		Qt = n->getWorldOrientation() * kf->m_Quaternion;
		Sc = n->getScale() * kf->m_Scaling;
	}
	else
	{
		const Ogre::SceneNode* n = const_cast<COTEActorEntity*>(actor)->getParentSceneNode();
		const Ogre::Node *nd = const_cast<COTEActorEntity*>(actor)->getSkeleton()->getBone(track->m_AttBoneName);

		// TODO 需要cach一下最好
		Qt = n->getWorldOrientation() * nd->getWorldOrientation();
		Pos = n->getWorldPosition() + n->getOrientation() * nd->getWorldPosition() + Qt * kf->m_Position;

		Qt = Qt * kf->m_Quaternion;
		Sc = n->getScale() * nd->getScale() * kf->m_Scaling;
	}	
	
}

// -------------------------------------------
void COTEActTrackUsage::GetKeyFrameDTrans(const COTEActorEntity* actor, const Ogre::SceneNode* targetSN, const COTETrack* track,
									  Ogre::Vector3& Pos, Ogre::Quaternion& Qt, Ogre::Vector3& Sc)
{
	// 更新位置方向缩放等参数
	
	if(track->m_AttBoneName.empty())
	{
		// 如果角色有本体动画　邦定物应该邦定到角色原始坐标
		const Ogre::SceneNode *n = const_cast<COTEActorEntity*>(actor)->GetParentSceneNode();

		Qt = targetSN->getWorldOrientation() * n->getWorldOrientation().Inverse();
		Sc = targetSN->getScale() / n->getScale();
		Pos = targetSN->getWorldPosition() - n->getWorldPosition();
		Pos = n->getOrientation().Inverse() * Pos;
	}
	else
	{
		const Ogre::SceneNode* n = const_cast<COTEActorEntity*>(actor)->getParentSceneNode();
		const Ogre::Node *nd = const_cast<COTEActorEntity*>(actor)->getSkeleton()->getBone(track->m_AttBoneName);

		// TODO 需要cach一下最好
		Ogre::Quaternion srcQ = n->getWorldOrientation() * nd->getWorldOrientation();
		Ogre::Vector3 srcScl = n->getScale() * nd->getScale();
		Ogre::Vector3 srcPos = n->getWorldPosition() + n->getOrientation() * nd->getWorldPosition();

		// 差异值
		Qt = targetSN->getWorldOrientation() * srcQ.Inverse();
		Sc = targetSN->getScale() / srcScl;
		Pos = targetSN->getWorldPosition() - srcPos;	
		Pos = srcQ.Inverse() * Pos;
	}	
	
}


// -------------------------------------------
// 更新绑定物
// TODO 需要比较两桢之间的差别来决定是否计算

void COTEActTrackUsage::UpdateTrackBinding(
			COTEAction* action, COTETrack* track, 
			Ogre::MovableObject* mo, float time)
{
	//取出当前时间的KeyFrame

	const COTEKeyFrame *kf = track->GetFrameAt(time);	
//	assert(kf);
	if(!kf)
		return;

	Ogre::Vector3 Pos;
	Ogre::Quaternion Qt = Ogre::Quaternion::IDENTITY;
	Ogre::Vector3 Sc = Ogre::Vector3::UNIT_SCALE;

	if(action)
		BindTrans(action->m_Creator, track, kf, Pos, Qt, Sc);
	Ogre::SceneNode* parentSceneNode = mo->getParentSceneNode();
	

	if(parentSceneNode && mo->getMovableType() != "Camera")
	{
		if(!(mo->m_UserFlags & MOVABLE_UD_BINDTOTARGET))
			parentSceneNode->setPosition(Pos);
		//parentSceneNode->setOrientation(Qt);
		//parentSceneNode->setScale(Sc);
	}

	/// 模型 uv动画(暂时只用于模型实体)
	
	if(mo->getMovableType() == "OTEEntity")
	{			
		mo->setVisible(kf->m_Visible);			

		std::vector<COTESubEntity*>* selist = ((COTEActorEntity*)mo)->GetSubEntityList();
		
		// 材质

		std::vector<COTESubEntity*>::iterator ita = selist->begin();
		while(ita != selist->end())
		{				
			Ogre::Material* m = (*ita)->getMaterial().getPointer();			

			Ogre::Pass* pass = m->getTechnique(0)->getPass(0);

			if(pass->getNumTextureUnitStates() == 0)			
				pass->createTextureUnitState();	

			Ogre::TextureUnitState* tus = pass->getTextureUnitState(0);

			// 属性改变

			m->setAmbient(kf->m_Colour);
			m->setDiffuse(kf->m_Colour);	

			if(!kf->m_TextureName.empty())
				tus->setTextureName		(kf->m_TextureName);

			if(kf->m_UV.x != 0.0f || kf->m_UV.y != 0.0f)
				tus->setTextureScroll(kf->m_UV.x,	kf->m_UV.y);
			if(kf->m_UVRot != 0.0f)
				tus->setTextureRotate(Ogre::Radian(Ogre::Degree(kf->m_UVRot)));
			if(kf->m_UVScl.x != 0.0f || kf->m_UVScl.y != 0.0f)
				tus->setTextureScale (kf->m_UVScl.x, kf->m_UVScl.y);							
		
			++ ita;
		}

		// trans

		if(!(mo->m_UserFlags & MOVABLE_UD_BINDTOTARGET))
			parentSceneNode->setOrientation(Qt);
		parentSceneNode->setScale(Sc);
	}	

	/// Billboard

	else if(mo->getMovableType() == "BBS")
	{
		COTEBillBoardSet* bb = (COTEBillBoardSet*)mo;
		bb->setVisible(kf->m_Visible);					

		Ogre::Pass* pass = bb->getMaterial()->getBestTechnique(0)->getPass(0);

		Ogre::TextureUnitState* tus = pass->getTextureUnitState(0);
		bb->SetColour(kf->m_Colour);

		if(!kf->m_TextureName.empty())
			tus->setTextureName		(kf->m_TextureName);	
		
		tus->setTextureScroll(kf->m_UV.x,	kf->m_UV.y);
		tus->setTextureRotate(Ogre::Radian(Ogre::Degree(kf->m_UVRot)));
		tus->setTextureScale (kf->m_UVScl.x, kf->m_UVScl.y);	
		tus->setTextureAddressingMode(track->m_TexAddrMode);

		// transforms

		if(!(mo->m_UserFlags & MOVABLE_UD_BINDTOTARGET))
			bb->SetRotation(kf->m_Angles, Qt);
		bb->SetScale(Sc);
	}
	else if(mo->getMovableType() == "BBC")
	{
		COTEBillBoardChain* bb = (COTEBillBoardChain*)mo;
		bb->setVisible(kf->m_Visible);					

		Ogre::Pass* pass = bb->getMaterial()->getBestTechnique(0)->getPass(0);

		Ogre::TextureUnitState* tus = pass->getTextureUnitState(0);
		bb->getMaterial()->setAmbient(kf->m_Colour);
		bb->getMaterial()->setDiffuse(kf->m_Colour);

		if(!kf->m_TextureName.empty())
			tus->setTextureName		(kf->m_TextureName);	
		
		tus->setTextureScroll(kf->m_UV.x,	kf->m_UV.y);
		tus->setTextureRotate(Ogre::Radian(Ogre::Degree(kf->m_UVRot)));
		tus->setTextureScale (kf->m_UVScl.x, kf->m_UVScl.y);	
		tus->setTextureAddressingMode(track->m_TexAddrMode);

		// transforms

		//if(!(mo->m_UserFlags & MOVABLE_UD_BINDTOTARGET))
			
		bb->getParentSceneNode()->setScale(Sc);
	}

	/// 粒子系统

	else if(mo->getMovableType() == "PS")
	{
		COTEParticleSystem* ps = (COTEParticleSystem*)mo;
		ps->SetVisible(kf->m_Visible);	

		if( ps->getMaterial()->getNumTechniques() > 0 &&
			ps->getMaterial()->getTechnique(0)->getNumPasses() > 0)
		{
			Ogre::Pass* pass = ps->getMaterial()->getTechnique(0)->getPass(0);

			Ogre::TextureUnitState* tus = pass->getTextureUnitState(0);
			ps->getMaterial()->setAmbient(kf->m_Colour);
			ps->getMaterial()->setDiffuse(kf->m_Colour);				
		
			if(!kf->m_TextureName.empty())
				tus->setTextureName		(kf->m_TextureName);	
			
			tus->setTextureScroll(kf->m_UV.x,	 kf->m_UV.y);
			tus->setTextureRotate(Ogre::Radian(Ogre::Degree(kf->m_UVRot)));
			tus->setTextureScale (kf->m_UVScl.x, kf->m_UVScl.y);	
			tus->setTextureAddressingMode(track->m_TexAddrMode);			
	
			// 影响器动态属性

			COTEParticleSystemAffector* psa = ps->GetResPtr()->m_ParticleSystemAffector;

			Ogre::ColourValue cor = kf->m_Colour;

			cor.a = kf->m_Scaling.x;					// x缩放对应缩放

			psa->SetVal("colorData", cor);

			psa->SetVal("strength", kf->m_Scaling.z);	// z缩放对应力度			

			// trans
			
			if(!(mo->m_UserFlags & MOVABLE_UD_BINDTOTARGET))
				parentSceneNode->setOrientation(Qt);
		}

		// 为了解决 效果同步问题 temp!

		if(ps->GetBindReactor())
		{
			((CReactorBase*)mo->GetBindReactor())->Update(ps, COTED3DManager::s_FrameTime);	
			((CReactorBase*)mo->GetBindReactor())->m_StateFlag |= 0xFF000000;
		}

	}

	// 摄像机

	else if(mo->getMovableType() == "Camera")
	{
		// trans

		// temp ! 可以把摄像机发射出去???
		{
			((Ogre::Camera*)mo)->setPosition(Pos);
			((Ogre::Camera*)mo)->setOrientation(Qt);		
		}
	}

	if(mo->getMovableType() == "OTEMagicEntity")
	{			
		mo->setVisible(kf->m_Visible);			

		// trans

		if(!(mo->m_UserFlags & MOVABLE_UD_BINDTOTARGET))
			parentSceneNode->setOrientation(Qt);
		parentSceneNode->setScale(Sc);
	}	

	// 当前延迟时间

	if(kf->m_DelayTime > 0 && action && action->m_CurrentDelay <= 0)
		action->SetCurDelay(kf->m_DelayTime);

}
