#include "OTETrackXmlLoader.h"
#include "OTEKeyFrame.h"
#include "OTETrack.h"
#include "OTETrackManager.h"
#include "OTEMathLib.h"

using namespace OTE;
using namespace Ogre;

// ----------------------------------------------
COTETrackXmlLoader::COTETrackXmlLoader(const std::string& TrackFile)
{	
	Init(TrackFile);
}

COTETrackXmlLoader::COTETrackXmlLoader() :
m_pTrackDoc(NULL)
{

}

COTETrackXmlLoader::~COTETrackXmlLoader()
{
	Destroy();
}

// ----------------------------------------------
void COTETrackXmlLoader::Destroy()
{
	if(m_pTrackDoc != NULL)
	{
		delete m_pTrackDoc;
		m_pTrackDoc = NULL;
	}
}

// ----------------------------------------------
void COTETrackXmlLoader::Init(const std::string& TrackFile)
{
	if(!TrackFile.empty())
	{
		m_pTrackDoc = new TiXmlDocument(TrackFile);
		if(!m_pTrackDoc->LoadFile())
		{
			OTE_MSG_ERR("读取轨道文件失败: " << TrackFile)
			return;
		}

		OTE_TRACE("Track 文件读入完成。")
	}
}

// ----------------------------------------------
// 加载
COTETrack* COTETrackXmlLoader::LoadTrack(unsigned int id) 
{	
	if(m_pTrackDoc != NULL)
	{
		TiXmlElement *elem = m_pTrackDoc->RootElement()->FirstChildElement("Track");
		while(elem)
		{
			if(id == atoi(elem->Attribute("Id")))
			{
				//创建Track
				COTETrack *track = new COTETrack();				
				track->m_nID = id;

				const char* cha;

				// 骨骼
				cha = elem->Attribute("BindBone");
				if(cha)
					track->SetBindBone(cha);

				//轨道起始时间
				track->m_AnimMinTime = atof(elem->Attribute("StartTime"));
				//轨道结束时间
				track->m_AnimMaxTime = atof(elem->Attribute("EndTime"));					

				// BezierCurves
				cha = elem->Attribute("BezierCurves");
				if(cha)
					track->m_IsBezierCurves = (strcmp(cha, "true") == 0);

				cha = elem->Attribute("Desc");
				if(cha)
					track->m_Desc = cha;
			
				cha = elem->Attribute("TexAddrMode");
				if(cha)
					track->m_TexAddrMode = (Ogre::TextureUnitState::TextureAddressingMode)atoi(cha);

				//加载关键帧列表

				const char* attr;
				TiXmlElement *Felem = elem->FirstChildElement("KeyFrame");
				while(Felem)
				{
					COTEKeyFrame *kf = COTEKeyFrame::CreateNewKeyFrame();
					kf->m_Time = atof(Felem->Attribute("Time"));
					sscanf(Felem->Attribute("Position"), 
						"%f %f %f", &(kf->m_Position.x), &(kf->m_Position.y), &(kf->m_Position.z));
					sscanf(Felem->Attribute("Rotation"), "%f %f %f %f",
						&(kf->m_Quaternion.x), &(kf->m_Quaternion.y), &(kf->m_Quaternion.z), &(kf->m_Quaternion.w));
					attr = Felem->Attribute("Rotation2");
					if(attr)
					{
						sscanf(Felem->Attribute("Rotation2"), "%f %f %f", &(kf->m_Angles.x), &(kf->m_Angles.y), &(kf->m_Angles.z));
						MathLib::EulerToQuaternion(kf->m_Angles, kf->m_Quaternion);
					}
					sscanf(Felem->Attribute("Scale"), 
						"%f %f %f", &(kf->m_Scaling.x), &(kf->m_Scaling.y), &(kf->m_Scaling.z));
					attr = Felem->Attribute("Delay");
					if(attr)
						sscanf(attr, "%f", &kf->m_DelayTime);
					attr = Felem->Attribute("UV");
					if(attr)
						sscanf(attr, "%f %f", &kf->m_UV.x, &kf->m_UV.y);
					attr = Felem->Attribute("UVRot");
					if(attr)
						sscanf(attr, "%f", &kf->m_UVRot);
					attr = Felem->Attribute("UVScl");
					if(attr)
						sscanf(attr, "%f %f", &kf->m_UVScl.x, &kf->m_UVScl.y);
					attr = Felem->Attribute("Colour");
					if(attr)
						sscanf(attr, "%f %f %f %f", &kf->m_Colour.r, &kf->m_Colour.g, &kf->m_Colour.b, &kf->m_Colour.a);
					attr = Felem->Attribute("Texture");
					if(attr)	
						kf->m_TextureName = attr;

					// 关键帧图片
					attr = Felem->Attribute("FrameList");
					if(attr)
					{						
						int length = strlen(attr);
						const char* pCha = attr;
						while(pCha < attr + length)
						{	
							char framlist[32];
							sscanf(pCha, "%s ", framlist);
							pCha += strlen(framlist) + 1;								

							kf->m_FrameList.push_back(framlist);								
						}
					}

					attr = Felem->Attribute("Visible");
					if(attr)	
						kf->m_Visible = (strcmp(attr, "true") == 0);

					track->AddKeyFrame(kf);

					Felem = Felem->NextSiblingElement();
				}

				OTE_TRACE("Track 信息读入完成， TrackID:" << track->m_nID)

                return track;						
			}

			elem = elem->NextSiblingElement();
		}
	}

	return NULL;
}

// --------------------------------------
//保存Track文件
void COTETrackXmlLoader::SaveFile(const std::string& fileName)
{	
	if(fileName.empty())
		m_pTrackDoc->SaveFile();	
	else
		m_pTrackDoc->SaveFile(fileName);

	OTE_TRACE("Track 信息写入完成， 文件:" << fileName)
}

// --------------------------------------
// 保存指定Id的Track
void COTETrackXmlLoader::TrackWriteXml(int id)
{
	TrackWriteXml( id, COTETrackManager::GetInstance()->GetTrack(id, this) );
}

// --------------------------------------
// 保存指定Id的Track
void COTETrackXmlLoader::TrackWriteXml(int id, COTETrack* track)
{	
	assert(track);
	if(m_pTrackDoc != NULL)
	{
		//先删除已经存在的记录
		TiXmlElement *elem = m_pTrackDoc->RootElement()->FirstChildElement("Track");
		while(elem)
		{
			if(id == atoi(elem->Attribute("Id")))
			{
				m_pTrackDoc->RootElement()->RemoveChild(elem);
				break;
			}

			elem = elem->NextSiblingElement();
		}		

		//插入新的记录
		elem = new TiXmlElement("Track");
		elem->SetAttribute("Id", id);
		elem->SetAttribute("BindBone", track->m_AttBoneName);
		char s[32];
		sprintf(s, "%1.3f", track->m_AnimMinTime);
		elem->SetAttribute("StartTime", s);
		sprintf(s, "%1.3f", track->m_AnimMaxTime);
		elem->SetAttribute("EndTime", s);
		elem->SetAttribute("BezierCurves", track->m_IsBezierCurves ? "true" : "false");
		elem->SetAttribute("Desc", track->m_Desc.c_str());

		sprintf(s, "%d", track->m_TexAddrMode);
		elem->SetAttribute("TexAddrMode", s);

		std::list <COTEKeyFrame*> kf;
		track->GetKeyFrameList(&kf);

		std::list <COTEKeyFrame*>::iterator it = kf.begin();
		for(; it != kf.end(); it ++)
		{
			TiXmlElement *Felem = new TiXmlElement("KeyFrame");
			sprintf(s, "%1.3f", (*it)->m_Time);
			Felem->SetAttribute("Time", s);
			sprintf(s, "%1.3f %1.3f %1.3f", (*it)->m_Position.x, (*it)->m_Position.y, (*it)->m_Position.z);
			Felem->SetAttribute("Position", s);
			sprintf(s, "%1.3f %1.3f %1.3f %1.3f", (*it)->m_Quaternion.x, (*it)->m_Quaternion.y, (*it)->m_Quaternion.z, (*it)->m_Quaternion.w);
			Felem->SetAttribute("Rotation", s);					
			sprintf(s, "%1.3f %1.3f %1.3f", (*it)->m_Angles.x, (*it)->m_Angles.y, (*it)->m_Angles.z);
			Felem->SetAttribute("Rotation2", s);
			sprintf(s, "%1.3f %1.3f %1.3f", (*it)->m_Scaling.x, (*it)->m_Scaling.y, (*it)->m_Scaling.z);
			Felem->SetAttribute("Scale", s);
			sprintf(s, "%1.3f", (*it)->m_DelayTime);
			Felem->SetAttribute("Delay", s);
			sprintf(s, "%1.3f %1.3f", (*it)->m_UV.x, (*it)->m_UV.y);
			Felem->SetAttribute("UV", s);
			sprintf(s, "%1.3f", (*it)->m_UVRot);
			Felem->SetAttribute("UVRot", s);
			sprintf(s, "%1.3f %1.3f", (*it)->m_UVScl.x, (*it)->m_UVScl.y);
			Felem->SetAttribute("UVScl", s);
			sprintf(s, "%1.3f %1.3f %1.3f %1.3f", (*it)->m_Colour.r, (*it)->m_Colour.g, (*it)->m_Colour.b, (*it)->m_Colour.a);
			Felem->SetAttribute("Colour", s);

			Felem->SetAttribute("Texture", (*it)->m_TextureName);					

			if((*it)->m_FrameList.size() > 0)
			{
				std::stringstream ss;
				for(unsigned int i = 0; i < (*it)->m_FrameList.size(); i ++)
				{
					ss << (*it)->m_FrameList[i] << " ";
				}
				
				Felem->SetAttribute("FrameList", ss.str());		
			}

			if((*it)->m_Visible)
				Felem->SetAttribute("Visible", "true");
			else
				Felem->SetAttribute("Visible", "false");				
			
			elem->InsertEndChild(*Felem);				
		}

		Ogre::LogManager::getSingleton().logMessage(Ogre::String("成功保存轨道: ") + Ogre::StringConverter::toString(id));
		
		m_pTrackDoc->RootElement()->InsertEndChild(*elem);				
	}	
			
}

// --------------------------------------
// 从文件中移除指定的Track

void COTETrackXmlLoader::RemoveTrack(int id)
{
	if(m_pTrackDoc != NULL)
	{
		TiXmlElement *elem = m_pTrackDoc->RootElement()->FirstChildElement("Track");
		while(elem)
		{
			if(id == atoi(elem->Attribute("Id")))
			{
				m_pTrackDoc->RootElement()->RemoveChild(elem);
				OTE_TRACE("移除Track完成， TrackID: " << id)
				break;
			}
		
			elem = elem->NextSiblingElement();
		}
	}
}