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
	float m_picWidth;	//ͼƬ��
	float m_picHeight;	//ͼƬ��

	float m_SelectUnitWidth;	//ѡ���λ���
	float m_SelectUnitHeight;	//ѡ���λ���

	int m_DefaultSelectIndexX1;	//Ĭ��ѡ�������ڵ�����ֵ
	int m_DefaultSelectIndexY1;
	int m_DefaultSelectIndexX2;
	int m_DefaultSelectIndexY2;
	

	int m_posX;		//ͼƬ��ʾ����
	int m_posY;		//ͼƬ��ʾ����


private :
	
	bool m_b_isShow;			//��ʾ״̬
	int m_catch_LUX;			//ͼƬ�ػ����ľ�����������
	int m_catch_LUY;
	int m_catch_RDX;
	int m_catch_RDY;

	std::string m_borderPicFileName;
	std::string m_selectBackPic;

	int m_zOrder;

	const int CATCHDIFFERENCE;//48�ǻ�ȡ��������Yֵ��ƫ��ֵ

private :
	std::string m_TextureFileName;	//ͼƬ����

	Ogre::Pass* m_pass;
	Ogre::TextureUnitState* m_t;
	Ogre::TextureUnitState* m_terrainT;


	Ogre::Overlay *m_pGuiOverlay;
	
	Ogre::OverlayContainer *m_pContainer;			//��������ͼ����
	Ogre::MaterialPtr m_pPanMat;					//����ͼ
	Ogre::MaterialPtr m_pBorderMat;				//����ͼ�߿�
	Ogre::PanelOverlayElement *m_pPanel;			//��������ͼ���
	Ogre::BorderPanelOverlayElement *m_pBorder;	//��������ͼ���߿���

	Ogre::OverlayContainer *m_pSubContainerBorder;	//ѡ�������
	Ogre::MaterialPtr m_pSubbackMat;					//ѡ���͸������ͼ
	Ogre::PanelOverlayElement *m_pSubPanel;			//ѡ��򱳾����
	Ogre::BorderPanelOverlayElement *m_pSubBorder;	//ѡ��򱳾��߿���

	Ogre::OverlayContainer *m_pSubContainerBorder2;	//ѡ���2����
	Ogre::PanelOverlayElement *m_pSubPanel2;			//ѡ���2�������
	Ogre::BorderPanelOverlayElement *m_pSubBorder2;	//ѡ���2�����߿���

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
		m_catch_LUY = m_pPanel->getTop() + (UnitOffsetHeight * LU_Y_OffsetNum - CATCHDIFFERENCE);//48�ǻ�ȡ��������Yֵ��ƫ��ֵ
		m_catch_RDX = m_pPanel->getLeft() + (UnitOffsetWidth * RD_X_OffsetNum);
		m_catch_RDY = m_pPanel->getTop() + (UnitOffsetHeight * RD_Y_OffsetNum - CATCHDIFFERENCE);
	}

	bool CheckCatchMouseInArea(int X, int Y)	//ͼƬ�貶����������
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