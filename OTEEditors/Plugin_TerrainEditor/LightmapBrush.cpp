#include "StdAfx.h"
#include "lightmapbrush.h"

#include "OTETile.h"
#include "OTEQTSceneManager.h"

using namespace	Ogre;
using namespace	OTE;

// ----------------------------------------------
CLightmapBrush::CLightmapBrush(void)
{	
	m_pInnerLineObj = NULL;
	m_pOutterLineObj = NULL;

	m_InnerRad = 1;
	m_OutterRad = 1;
	m_Intensity = 0.01;

	m_Colour = Ogre::ColourValue(0, 0, 0); 

	Init();

}


CLightmapBrush::~CLightmapBrush(void)
{
	for(int i = 0; i < 3; i ++)
	{
		delete [] m_pRawData[i];
		m_pRawData[i] = NULL;
	}

	DeleteBrushEntity();
}

// ----------------------------------------------
void CLightmapBrush::Init()
{
	{
		std::stringstream ss;
		ss << OTEHelper::GetResPath("ResConfig", "HelperPath") << "\\BrushRawData1.raw";

		// 从文件中读取
		FILE* file = fopen(ss.str().c_str(), "rb");
		if(!file)
		{				
			ss << " 没找到！";
			::MessageBox(NULL, ss.str().c_str(), "文件丢失", MB_OK);
			return;
		}

		m_pRawData[0] = new unsigned char[1 * 1];
		fread(m_pRawData[0], 1 * 1, 1, file);
		fclose(file);
	}
	{
		std::stringstream ss;
		ss << OTEHelper::GetResPath("ResConfig", "HelperPath") << "\\BrushRawData4.raw";

		// 从文件中读取
		FILE* file = fopen(ss.str().c_str(), "rb");
		if(!file)
		{		
			ss << " 没找到！";
			::MessageBox(NULL, ss.str().c_str(), "文件丢失", MB_OK);
			return;
		}

		m_pRawData[1] = new unsigned char[4 * 4];
		fread(m_pRawData[1], 4 * 4, 1, file);
		fclose(file);
	}
	{
		std::stringstream ss;
		ss << OTEHelper::GetResPath("ResConfig", "HelperPath") << "\\BrushRawData8.raw";

		// 从文件中读取
		FILE* file = fopen(ss.str().c_str(), "rb");
		if(!file)
		{	
			ss << " 没找到！";
			::MessageBox(NULL, ss.str().c_str(), "文件丢失", MB_OK);
			return;
		}

		m_pRawData[2] = new unsigned char[8 * 8];
		fread(m_pRawData[2], 8 * 8, 1, file);
		fclose(file);
	}

}
// ----------------------------------------------
unsigned char*	CLightmapBrush::GetBrushRawData()
{
	if(m_InnerRad == 1)
	{
		return m_pRawData[0];
	}
	else if(m_InnerRad == 4)
	{
		return m_pRawData[1];
	}
	else
	{
		return m_pRawData[2];
	}
}

// ----------------------------------------------
// 更新绘制	
void CLightmapBrush::UpdateDrawing(const Ogre::Vector3& rBrushPos)
{

 	if(!m_pInnerLineObj)
	{
		m_pInnerLineObj = new CLineObj("BrushDummy.InnerLineObj"); 
		m_pOutterLineObj = new CLineObj("BrushDummy.OutterLineObj");
		
		Ogre::SceneNode* sn = SCENE_MGR->getRootSceneNode()->createChildSceneNode();

		sn->attachObject(m_pInnerLineObj); 
		sn->attachObject(m_pOutterLineObj);
	}
	m_Pos = rBrushPos;
	m_pInnerLineObj->getParentSceneNode()->setPosition(m_Pos);
	m_pInnerLineObj->clearPoints();
	m_pOutterLineObj->clearPoints();

	// draw
	
	for(int count = 0; count<17; count ++)
	{		   
		double angle = (double)count * 22.5 * Math::PI / 180.0f;	
        float px = m_InnerRad * 0.5f * cos(angle);
		float pz = m_InnerRad * 0.5f * sin(angle);
		
		float height=COTETilePage::GetCurrentPage()->GetHeightAt(px + m_Pos.x, pz + m_Pos.z);	
		
		m_pInnerLineObj->addPoint(Vector3(px, height - m_Pos.y + 0.1f, pz));

	}  

	m_OutterRad = m_InnerRad;

	// 绘制外圈（影响范围）

	for(int count = 0; count < 17; count ++)
	{
		double angle=(double)count * 22.5 * Math::PI / 180.0f;	
        float px = m_OutterRad * 0.5f * cos(angle);
		float pz = m_OutterRad * 0.5f * sin(angle);

		float height=COTETilePage::GetCurrentPage()->GetHeightAt(px + m_Pos.x, pz + m_Pos.z);	
		
		m_pOutterLineObj->addPoint( Vector3(px, height - m_Pos.y + 0.1f, pz));			
	}

	m_pInnerLineObj->changeColor(ColourValue(100, 0, 10));
	m_pInnerLineObj->drawLines();

	m_pOutterLineObj->changeColor(ColourValue(0, 0, 200));
	m_pOutterLineObj->drawLines();	

}


// ----------------------------------------------
// 删除
void CLightmapBrush::DeleteBrushEntity()
{
	if(m_pInnerLineObj)
	{	
		(m_pInnerLineObj->getParentSceneNode())->detachObject( m_pInnerLineObj );
		delete m_pInnerLineObj;
        m_pInnerLineObj=0;
	}
	if(m_pOutterLineObj)
	{	
		(m_pOutterLineObj->getParentSceneNode())->detachObject( m_pOutterLineObj );
		delete m_pOutterLineObj;
        m_pOutterLineObj=0;
	}

}

// ----------------------------------------------
// 
OTE::CRectA<float> CLightmapBrush::GetBrushInfluRect(float rad)
{
	OTE::CRectA<float> rect;

	float rr= Ogre::Math::Sqrt(rad);
 	rect.sx=-rr,rect.sz=-rr;
	rect.ex=rr,rect.ez=rr;

	return rect;
}