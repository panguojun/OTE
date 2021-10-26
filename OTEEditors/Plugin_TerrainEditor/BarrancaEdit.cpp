// BarrancaEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_TerrinEditor.h"
#include "BarrancaEdit.h"
#include "OTEQTSceneManager.h"
#include ".\barrancaedit.h"
#include "TerrainMatBrush.h"
#include "OTETerrainHeightMgr.h"

// CBarrancaEdit 对话框
using namespace OTE;
CBarrancaEdit CBarrancaEdit::s_Inst;//实例化静态变量

//策划用
static vector<CTerrainMatBrush *> vecAreaLine;
static int opIndex = -1;
//策划用end

extern OTE::COTEQTSceneManager*		gSceneMgr;			// 场景管理器

IMPLEMENT_DYNAMIC(CBarrancaEdit, CDialog)
CBarrancaEdit::CBarrancaEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CBarrancaEdit::IDD, pParent)
{
	
	m_pBrushEntity = NULL;
	
}

CBarrancaEdit::~CBarrancaEdit()
{
}

BOOL CBarrancaEdit::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();
	
	opType = ter;
	m_radioTerrain.SetCheck(true);
	
	return tRet; 
}


void CBarrancaEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_Height, txtHeightValue);
	DDX_Control(pDX, IDC_RADIO1, m_radioTerrain);
}

Ogre::Vector3 beginPos(-1, -1, -1);
Ogre::Vector3 pos(-1, -1, -1);
void CBarrancaEdit::OnKeyDown(unsigned int keyCode, bool isShifDwon)
{
	int width = 0, height = 0;
	if(beginPos > Ogre::Vector3(-1, -1, -1))
	{
		//暂时配合策划
		isShifDwon = false;
		//暂时配合策划end
		if(isShifDwon)
		{
			switch(keyCode)
			{
			case 318://VK_RIGHT
				pos.x += 1;

				width = abs(beginPos.x - pos.x);
				height = abs(beginPos.z - pos.z);

				//m_pBrushEntity->UpdateDrawingActiveArea(beginPos, width, height);
				//vecAreaLine[opIndex]->UpdateDrawingActiveArea(beginPos, width, height);
				break;
			case 316://VK_LEFT
				pos.x -= 1;

				width = abs(beginPos.x - pos.x);
				height = abs(beginPos.z - pos.z);

				//m_pBrushEntity->UpdateDrawingActiveArea(beginPos, width, height);
				//vecAreaLine[opIndex]->UpdateDrawingActiveArea(beginPos, width, height);
				break;
			case 317://VK_UP
				pos.z -= 1;

				width = abs(beginPos.x - pos.x);
				height = abs(beginPos.z - pos.z);

				//m_pBrushEntity->UpdateDrawingActiveArea(beginPos, width, height);
				//vecAreaLine[opIndex]->UpdateDrawingActiveArea(beginPos, width, height);
				break;
			case 319://VK_DOWN
				pos.z += 1;

				width = abs(beginPos.x - pos.x);
				height = abs(beginPos.z - pos.z);

				//m_pBrushEntity->UpdateDrawingActiveArea(beginPos, width, height);
				//vecAreaLine[opIndex]->UpdateDrawingActiveArea(beginPos, width, height);
				break;
			}

			updateShowText(beginPos, width, height);
		}
	}

	switch(keyCode)
	{
		case 127://VK_DELETE
			//策划用
			if(opType == hatch)
			{
				if(opIndex != -1)
				{
					delete vecAreaLine[opIndex];
					vecAreaLine.erase(vecAreaLine.begin() + opIndex);
				}
			}
			//策划用end
			break;
	}
}

void CBarrancaEdit::updateShowText(Ogre::Vector3 beginPos, int width, int height)
{
	//临时用于策划划区域
	char a[40];
	sprintf(a, "左上角(x,z)： [ %d , %d ]", int(beginPos.x), int(beginPos.z));
	SetDlgItemText(IDC_STATICZB1, a);

	char b[40];
	sprintf(b, "右下角(x,z)： [ %d , %d ]", int(beginPos.x) + width, int(beginPos.z) + height);
	SetDlgItemText(IDC_STATICZB, b);
	//临时用于策划划区域end
}

void CBarrancaEdit::OnLButtonDown(int x, int z, int width, int height)
{
	float x1 = (float)x / width;
	float z1 = (float)z / height;

	Ogre::Ray ray = gSceneMgr->getCamera("CamMain")->getCameraToViewportRay( x1, z1);
	
	gSceneMgr->m_TerrainPage && gSceneMgr->m_TerrainPage->TerrainHitTest(ray, beginPos);

	//策划用
	if(opType == hatch)
	{
		m_pBrushEntity = NULL;
		opIndex = -1;
		
		for(int i=0; i<vecAreaLine.size(); i++)
		{
			if(beginPos.x >= vecAreaLine[i]->m_LUPointX && beginPos.z >= vecAreaLine[i]->m_LUPointZ 
				&& beginPos.x <= vecAreaLine[i]->m_RDPointX && beginPos.z <= vecAreaLine[i]->m_RDPointZ)
			{
				opIndex = i;

				updateShowText(Ogre::Vector3(vecAreaLine[i]->m_LUPointX, 0, vecAreaLine[i]->m_LUPointZ), 
								int(vecAreaLine[i]->m_RDPointX - vecAreaLine[i]->m_LUPointX), 
								int(vecAreaLine[i]->m_RDPointZ - vecAreaLine[i]->m_LUPointZ));
			}
		}
	}
	//策划用end
		
}

void CBarrancaEdit::OnLButtonUp(int x, int z, int width, int height)
{
	
}

void CBarrancaEdit::OnMouseMove(int x, int z, int width, int height)
{
	
	if(::GetKeyState(VK_LBUTTON) & 0x80)
	{
		float x1 = (float)x / width;
		float z1 = (float)z / height;

		/*Ogre::Vector3 currentPos;*/
		Ogre::Ray ray = gSceneMgr->getCamera("CamMain")->getCameraToViewportRay( x1, z1);

		
		bool hitText = false;

		if(gSceneMgr->m_TerrainPage && gSceneMgr->m_TerrainPage->TerrainHitTest(ray, pos))
				hitText = true;

		if(hitText)
		{
			
/*
			//pos = currentPos;
			
			{
				int width = abs(beginPos.x - pos.x);
				int height = abs(beginPos.z - pos.z);

				
				m_pBrushEntity->UpdateDrawingActiveArea(beginPos, width, height);

				updateShowText(beginPos, width, height);
			}
			else
			{
				m_pBrushEntity = new CTerrainMatBrush();
			}
*/

			if(opIndex != -1 || (m_pBrushEntity != NULL && opType == ter))
			{
				int width = abs(beginPos.x - pos.x);
				int height = abs(beginPos.z - pos.z);
				
				m_pBrushEntity = vecAreaLine[opIndex];
				
			//	m_pBrushEntity->UpdateDrawingActiveArea(beginPos, width, height);

				updateShowText(beginPos, width, height);
			}
			else
			{
				m_pBrushEntity = new CTerrainMatBrush();

				opIndex = vecAreaLine.size();
				vecAreaLine.push_back(m_pBrushEntity);
			}
		}
	}
	
}


BEGIN_MESSAGE_MAP(CBarrancaEdit, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
END_MESSAGE_MAP()


// CBarrancaEdit 消息处理程序
void CBarrancaEdit::OnBnClickedButton1()
{
	CString str = "";
	txtHeightValue.GetWindowText(str);
	if(str.Trim().GetLength() == 0)
	{
		::MessageBox(NULL, "输入高度值不正确！", "友情提示", MB_OK);
		return;
	}

	if(pos.x - beginPos.x < 1 && pos.z - beginPos.z < 1)
		return;

	float value = atof(str.Trim());
	bool isRaise;
	if(value > 0)
	{
		value = abs(value);
		//升高
		isRaise = true;
	}
	else if(value < 0)
	{
		value = abs(value);
		//降低
		isRaise = false;
	}

	COTETerrainHeightMgr::GetInstance()->SetHeightAt(gSceneMgr->m_TerrainPage, beginPos, pos, value, isRaise);

	float outRad = 0;
	//if((pos.x - beginPos.x) > (pos.z - beginPos.z))
	//	outRad = (pos.x - beginPos.x) /** 2*/;
	//else
	//	outRad = (pos.z - beginPos.z)/* * 2*/;

	outRad = 300.0f;

	beginPos.x += ((pos.x - beginPos.x)/2);
	beginPos.z += ((pos.z - beginPos.z)/2);

	gSceneMgr->m_TerrainPage->buildAreaVertexNormal(
				beginPos.x, //x坐标
				beginPos.z, //z坐标
				outRad	//外圈
				);
}

void CBarrancaEdit::OnBnClickedRadio1()
{
	opType = ter;	//地形
	opIndex = -1;
	m_pBrushEntity = NULL;

	for(int i=0; i<vecAreaLine.size(); i++)
	{
		delete vecAreaLine[i];
		vecAreaLine.erase(vecAreaLine.begin() + i);
	}
}

void CBarrancaEdit::OnBnClickedRadio2()
{
	opType = hatch;//策划
}
