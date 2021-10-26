#include "OTEStaticInclude.h"
#include "OgreVertexIndexData.h"
#include "OgreMovableObject.h"
#include "OgreRenderable.h"
#include "OgreTextureUnitState.h"
#include "OgrePass.h"

#include "OgreOverlay.h"
#include "OgrePanelOverlayElement.h"
#include "OgreTextAreaOverlayElement.h"
#include "OgreBorderPanelOverlayElement.h"

namespace OTE
{

class _OTEExport CTerrainTexturePic
{
public :
	float m_picWidth;	//图片宽
	float m_picHeight;	//图片高

	float m_SelectUnitWidth;	//选择框单位宽度
	float m_SelectUnitHeight;	//选择框单位宽度

	int m_DefaultSelectIndexX1;	//默认选择区所在的索引值
	int m_DefaultSelectIndexY1;
	int m_DefaultSelectIndexX2;
	int m_DefaultSelectIndexY2;
	

	int m_posX;		//图片显示坐标
	int m_posY;		//图片显示坐标


private :
	
	bool m_b_isShow;			//显示状态
	int m_catch_LUX;			//图片截获鼠标的矩形区域坐标
	int m_catch_LUY;
	int m_catch_RDX;
	int m_catch_RDY;

	std::string m_borderPicFileName;
	std::string m_selectBackPic;

	int m_zOrder;

	const int CATCHDIFFERENCE;//48是获取鼠标的坐置Y值的偏差值

private :
	std::string m_TextureFileName;	//图片名称

	Ogre::Pass* m_pass;
	Ogre::TextureUnitState* m_t;
	Ogre::TextureUnitState* m_terrainT;


	Ogre::Overlay *m_pGuiOverlay;
	
	Ogre::OverlayContainer *m_pContainer;			//地形纹理图容器
	Ogre::MaterialPtr m_pPanMat;					//纹理图
	Ogre::MaterialPtr m_pBorderMat;				//纹理图边框
	Ogre::PanelOverlayElement *m_pPanel;			//地形纹理图面板
	Ogre::BorderPanelOverlayElement *m_pBorder;	//地形纹理图面板边框线

	Ogre::OverlayContainer *m_pSubContainerBorder;	//选择框容器
	Ogre::MaterialPtr m_pSubbackMat;					//选择框透明背景图
	Ogre::PanelOverlayElement *m_pSubPanel;			//选择框背景面板
	Ogre::BorderPanelOverlayElement *m_pSubBorder;	//选择框背景边框线

	Ogre::OverlayContainer *m_pSubContainerBorder2;	//选择框2容器
	Ogre::PanelOverlayElement *m_pSubPanel2;			//选择框2背景面板
	Ogre::BorderPanelOverlayElement *m_pSubBorder2;	//选择框2背景边框线

public :

	CTerrainTexturePic();
	virtual ~CTerrainTexturePic();

	void Create(std::string strFileName="", int posX=/*1*/-128, int posY=590, float width=256/*96*/, float height=128);

	void LoadTerrainMaterialPic();

	void IsShow(bool state)
	{
		m_b_isShow = state;

		if(m_b_isShow)
			m_pGuiOverlay->show();
		else
			m_pGuiOverlay->hide();
	}

	bool GetShowState()
	{
		return m_b_isShow;
	}

	void SetSelectUnitSize(float width, float height)
	{
		m_SelectUnitWidth = width;
		m_SelectUnitHeight = height;
	}

	void SetTextureFileName(std::string strFileName)
	{
		m_TextureFileName = strFileName;
	}

	void SetCatchMouseAreaOffset(int UnitOffsetWidth, int UnitOffsetHeight, int LU_X_OffsetNum, int LU_Y_OffsetNum, int RD_X_OffsetNum, int RD_Y_OffsetNum)
	{
		m_SelectUnitWidth = UnitOffsetWidth;
		m_SelectUnitHeight = UnitOffsetHeight;

		m_catch_LUX = m_pPanel->getLeft() + (UnitOffsetWidth * LU_X_OffsetNum);			
		m_catch_LUY = m_pPanel->getTop() + (UnitOffsetHeight * LU_Y_OffsetNum - CATCHDIFFERENCE);//48是获取鼠标的坐置Y值的偏差值
		m_catch_RDX = m_pPanel->getLeft() + (UnitOffsetWidth * RD_X_OffsetNum);
		m_catch_RDY = m_pPanel->getTop() + (UnitOffsetHeight * RD_Y_OffsetNum - CATCHDIFFERENCE);
	}

	bool CheckCatchMouseInArea(int X, int Y)	//图片需捕获鼠标的区域
	{
		if(X > m_catch_LUX && X < m_catch_RDX && Y > m_catch_LUY && Y < m_catch_RDY)
			return true;
		else
			return false;
	}
	void GetSelectAreaIndex(int x, int y, int& resultX, int& resultY);	

	void SetSelect_1_AreaPos(int indexX, int indexY)
	{
		m_pSubPanel->setPosition(m_pPanel->getLeft() + m_SelectUnitWidth * indexX, 
								m_pPanel->getTop() + m_SelectUnitHeight * indexY);

	}
	void SetSelect_2_AreaPos(int indexX, int indexY)
	{
		m_pSubPanel2->setPosition(m_pPanel->getLeft() + m_SelectUnitWidth * indexX, 
								m_pPanel->getTop() + m_SelectUnitHeight * indexY);

	}


};

}