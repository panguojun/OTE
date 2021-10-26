// LightmapEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_TerrainEditor.h"
#include "LightmapEditDlg.h"
#include "LightmapBrush.h"
#include "OTETile.h"
#include "OTEQTSceneManager.h"
#include "OTETerrainLightmapMgr.h"
#include <windows.h>
#include "OTERenderLight.h"
#include "Progress.h"

#include "LightEditDlg.h"
#include "OTEEntityLightmapPainter.h"
#include "OTEEntityVertexColorPainter.h"

// -------------------------------------------------
using namespace Ogre;
using namespace OTE;

// -------------------------------------------------
CLightmapEditDlg CLightmapEditDlg::s_Inst;
extern CLightEditDlg *g_pLightEditDlg;


//Fun g_FunArray( SetRange, StepIt, PeekAndPump, Cancelled );
// -------------------------------------------------
IMPLEMENT_DYNAMIC(CLightmapEditDlg, CDialog)
CLightmapEditDlg::CLightmapEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLightmapEditDlg::IDD, pParent)
{	
	m_bRenderShadow = false;
}

CLightmapEditDlg::~CLightmapEditDlg()
{
}

void CLightmapEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_CheckShadow);
	DDX_Control(pDX, IDC_SLIDER1, m_StrengthSlider);
	DDX_Control(pDX, IDC_COMBO1, m_BrushTypeCombo);
}

BOOL CLightmapEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_CheckShadow.SetCheck( FALSE );
	m_StrengthSlider.SetRange(1, 50);
	m_BrushTypeCombo.SetCurSel(0);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CLightmapEditDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_RADIO1, OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON7, OnBnClickedButton7)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClickedCheck1)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER1, OnNMReleasedcaptureSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, OnNMCustomdrawSlider1)
	ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, OnBnClickedButton9)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON10, OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, OnBnClickedButton11)
	ON_BN_CLICKED(IDC_RADIO4, OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_BUTTON12, OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, OnBnClickedButton13)
END_MESSAGE_MAP()


//------------------------------------------------------------------------------------

// -----------------------------------------------
// -----------------------------------------------
// 地表光照图 格式转化与边界连接 
// -----------------------------------------------
// -----------------------------------------------

unsigned int m_pLightmap[1048576];

// -----------------------------------------------
void LoadLMBmp(const std::string& rName)
{
	int m_BitMapWidth = 1024;
	int m_BitMapHeight = 1024;
		 
	FILE *file=fopen(rName.c_str(),"rb");
	int imgRowSize=m_BitMapWidth;
	int imgColSize=m_BitMapHeight;

	BITMAPFILEHEADER FileHeader;
	fread( &FileHeader, sizeof(BITMAPFILEHEADER), 1, file );

	BITMAPINFOHEADER tHeader;
	fread(&tHeader,sizeof(BITMAPINFOHEADER),1,file);

	// 32位
	if( tHeader.biBitCount == 32 )
	{
		//if( tHeader.biSizeImage != imgRowSize * imgColSize*4 )
		if( tHeader.biWidth != imgRowSize || tHeader.biHeight != imgColSize )
		{
			MessageBox(NULL,"图片尺寸不对!","错误",0);
			fclose(file);
			return;
		}
		for(int j=0;j<m_BitMapHeight;j++)
		for(int i=0;i<m_BitMapWidth;i++)
		{
		unsigned int color;
		fread(&color,4,1,file);	   
		unsigned int tcolor=((color&0x000000ff)<<16)|((color&0x00ff0000)>>16)|(color&0x0000ff00);
		tcolor=color|0xff000000;
		m_pLightmap[(m_BitMapHeight-j-1)*m_BitMapWidth+i]=tcolor;
		}		
	}
	// 24位
	else if( tHeader.biBitCount == 24 )
	{
		//if( tHeader.biSizeImage != imgRowSize * imgColSize*3 )
		if( tHeader.biWidth != imgRowSize || tHeader.biHeight != imgColSize )
		{
			MessageBox(NULL,"图片尺寸不对!","错误",0);
			fclose(file);
			return;
		}
		for(int j=0;j<m_BitMapHeight;j++)
		for(int i=0;i<m_BitMapWidth;i++)
		{
		unsigned int color;
		fread(&color,3,1,file);	   
		unsigned int tcolor=((color&0x000000ff)<<16)|((color&0x00ff0000)>>16)|(color&0x0000ff00);
		tcolor=color|0xff000000;
		m_pLightmap[(m_BitMapHeight-j-1)*m_BitMapWidth+i]=tcolor;
		}		
	}

	fclose(file);
}

// -----------------------------------------------
unsigned int m_pTileLightmaps[4096][256];

// -----------------------------------------------
void ARRANGE_RGB(unsigned int c1, unsigned int c2, unsigned int& c)
{
	unsigned int r1 = GetRValue(c1);
	unsigned int g1 = GetGValue(c1);
	unsigned int b1 = GetBValue(c1);
	
	unsigned int r2 = GetRValue(c2);
	unsigned int g2 = GetGValue(c2);
	unsigned int b2 = GetBValue(c2);

	unsigned int r = (r1 + r2) / 2;	
	unsigned int g = (g1 + g2) / 2;	
	unsigned int b = (b1 + b2) / 2;	

	c = 0xFF000000 | (b << 16) | (g << 8) | r;

}

// -----------------------------------------------
void ExportData(const std::string& lgtmapName)
{
	///////////////////////////光照图/////////////////////////

	LoadLMBmp(lgtmapName);

	// 把光照图 分割成tile

	for(int tileZ = 0; tileZ < 64; tileZ ++)
	for(int tileX = 0; tileX < 64; tileX ++)
	{		
		unsigned int* tileLM = m_pTileLightmaps[tileZ * 64 + tileX];

		for(int z = 0; z < 16; ++ z)
		for(int x = 0; x < 16; ++ x)
		{
			tileLM[z * 16 + x] = m_pLightmap[(z + tileZ * 16) * 1024 + (x + tileX * 16)];		

		}
	}

	for(int tileZ = 1; tileZ < 63; tileZ ++)
	for(int tileX = 1; tileX < 63; tileX ++)
	{	
		// 边界过渡		

		unsigned int* tileLM = m_pTileLightmaps[tileZ * 64 + tileX];

		// left border
		for(int ii=0;ii< 16; ii++)
		{
			ARRANGE_RGB(m_pLightmap[(ii + tileZ * 16) * 1024 + (0 + tileX * 16)		  ], 
						m_pLightmap[(ii + tileZ * 16) * 1024 + (15 + (tileX - 1) * 16)], 
						tileLM[0 + ii * 16]
						);

		}
		// right border
		for(int ii=0;ii< 16; ii++)
		{
			ARRANGE_RGB(m_pLightmap[(ii + tileZ * 16) * 1024 + (15 + tileX * 16)	 ], 
						m_pLightmap[(ii + tileZ * 16) * 1024 + (0 + (tileX + 1) * 16)], 
						tileLM[15 + ii * 16]
						);
		}
		// top border
		for(int ii=0;ii< 16; ii++)
		{
			ARRANGE_RGB(m_pLightmap[(0  + tileZ * 16) * 1024 + (ii + tileX * 16)	  ], 
						m_pLightmap[(15 + (tileZ - 1) * 16) * 1024 + (ii + tileX * 16)], 
						tileLM[0 * 16 + ii]
						);
		}
		// bottom border
		for(int ii=0;ii< 16; ii++)
		{
			ARRANGE_RGB(m_pLightmap[(15 + tileZ * 16) * 1024 + (ii + tileX * 16)	  ], 
						m_pLightmap[(0  + (tileZ + 1) * 16) * 1024 + (ii + tileX * 16)], 
						tileLM[15 * 16 + ii]
						);
		}
		
	}

	// 再保存成光照图

	for(int tileZ = 0; tileZ < 64; tileZ ++)
	for(int tileX = 0; tileX < 64; tileX ++)
	{		
		unsigned int* tileLM = m_pTileLightmaps[tileZ * 64 + tileX];

		for(int z = 0; z < 16; ++ z)
		for(int x = 0; x < 16; ++ x)
		{
			m_pLightmap[(z + tileZ * 16) * 1024 + (x + tileX * 16)] = tileLM[z * 16 + x];		
		}
	}
	
	// 保存成raw

	std::string strFileName = lgtmapName;				
	strFileName.replace(strFileName.find(".bmp"), 4, ".oraw");  

	FILE* file=fopen(strFileName.c_str(),"wb"); 

	fwrite(m_pLightmap,1048576,sizeof(unsigned int),file);

	fclose(file);	


	////////////////////////////////////////////////////////////

}

// ------------------------------------------------
// 保存光照图
void CLightmapEditDlg::OnBnClickedButton1()
{
	COTETerrainLightmapMgr::GetInstance()->SaveLightMapFile( COTETilePage::GetCurrentPage() );
	//COTETerrainLightmapMgr::GetInstance()->SetLightMapMaterial( COTETilePage::GetCurrentPage() );
	//COTETerrainLightmapMgr::GetInstance()->releaseEmptyLightMap( COTETilePage::GetCurrentPage() );
}


// ------------------------------------------------
// 鼠标编辑
void CLightmapEditDlg::OnMouseMove(float x, float y, float width, float height)
{	
	if(COTETilePage::GetCurrentPage() && COTETilePage::GetCurrentPage()->m_LightmapData == NULL)
	{
		COTETerrainLightmapMgr::GetInstance()->LoadLightMapData(COTETilePage::GetCurrentPage());	
	}

	static float oldMPX = x, oldMPY = y;
		
	if(!(::GetKeyState(VK_SHIFT) & 0x80))
	{
		Ogre::Ray ray = SCENE_CAM->getCameraToViewportRay(x / width, y / height);
		Ogre::Vector3 pos;
		if(COTETilePage::GetCurrentPage() && COTETilePage::GetCurrentPage()->TerrainHitTest(ray, pos))
		{
			m_LightmapBrush.UpdateDrawing(pos);
		}

		if(::GetKeyState(VK_LBUTTON) & 0x80 && !(::GetKeyState(VK_CONTROL) & 0x80))
		{
			// 手工编辑物件光照图
			
			COTEEntity* e = (COTEEntity*)SCENE_MGR->GetCurObjSection();
			if(e)
			{
				COTEEntityLightmapPainter::GenLightmap(e, ray, Ogre::ColourValue::Black, 
					m_LightmapBrush.m_Colour, "", false, 
					NULL, m_LightmapBrush.m_Intensity,
					m_LightmapBrush.m_InnerRad);
			}

			else if(COTETilePage::GetCurrentPage())
			{
				static Ogre::Vector2 s_oldPos;
				if((s_oldPos - Ogre::Vector2(x, y)).length() < 0.5)
					return;
				s_oldPos = Ogre::Vector2(x, y);			
				
				float offset = m_LightmapBrush.m_InnerRad / 2.0f;

				if(m_BrushTypeCombo.GetCurSel() == 0)
				{
					COTETerrainLightmapMgr::GetInstance()->setLightmapColorAt(
						COTETilePage::GetCurrentPage(), 
						pos.x - offset / 2.0f, pos.z - offset / 2.0f, 
						m_LightmapBrush.m_Colour, 
						m_LightmapBrush.m_Intensity == 1.0f ? COTETerrainLightmapMgr::E_COVER : COTETerrainLightmapMgr::E_BLIND, 
						m_LightmapBrush.GetBrushRawData(),
						m_LightmapBrush.m_InnerRad, m_LightmapBrush.m_Intensity);
				}	
			}
			
		}
	}
	
	oldMPX = x; oldMPY = y;
	
}

// ------------------------------------------------
// 鼠标按键
void CLightmapEditDlg::OnLButtonDown(float x, float y)
{
	if(::GetKeyState(VK_CONTROL) & 0x80)
	{
		Ogre::Ray ray = SCENE_CAM->getCameraToViewportRay(x, y);

		// 吸取颜色
		
		COTEEntity* e = (COTEEntity*)SCENE_MGR->GetCurObjSection();
		if(e)
		{
			COTEEntityLightmapPainter::PickLightmapColor(e, ray, m_LightmapBrush.m_Colour);
			return;
		}
		else if(COTETilePage::GetCurrentPage())
		{				
			Ogre::Vector3 pos;
			if(COTETilePage::GetCurrentPage()->TerrainHitTest(ray, pos))
			{
				m_LightmapBrush.UpdateDrawing(pos);
			}

			m_LightmapBrush.m_Colour = COTETerrainLightmapMgr::GetInstance()->PickLightmapColor(COTETilePage::GetCurrentPage(), pos.x, pos.z);
		}

	}
	
	if(COTETilePage::GetCurrentPage())
	{
		if(COTETilePage::GetCurrentPage()->m_LightmapData == NULL)
		{
			COTETerrainLightmapMgr::GetInstance()->LoadLightMapData(COTETilePage::GetCurrentPage());	
		}

		if(!(::GetKeyState(VK_SHIFT) & 0x80) && !(::GetKeyState(VK_CONTROL) & 0x80) )
		{
			float offset = m_LightmapBrush.m_InnerRad / 2.0f;

			Ogre::Ray ray = SCENE_CAM->getCameraToViewportRay(x, y);
			Ogre::Vector3 pos;
			if(COTETilePage::GetCurrentPage() && COTETilePage::GetCurrentPage()->TerrainHitTest(ray, pos))
			{
				m_LightmapBrush.UpdateDrawing(pos);
			}

			if(m_BrushTypeCombo.GetCurSel() == 1)
			{
				Ogre::ColourValue ambientColor	= COTERenderLight::GetInstance()->GetLightKey()->ambient.GetColor();
				
				Ogre::Vector3 mainLightDir		= SCENE_MAINLIGHT->getDirection();//阳光方向
				Ogre::ColourValue mainLightCor	= COTERenderLight::GetInstance()->GetLightKey()->diffuse.GetColor();//阳光色

				COTETerrainLightmapMgr::GetInstance()->GenLightmapOnTiles(
					COTETilePage::GetCurrentPage(), 
					pos.x - offset / 2.0f, pos.z - offset / 2.0f, m_LightmapBrush.m_InnerRad,
					mainLightDir, mainLightCor, ambientColor);
			}
			else if(m_BrushTypeCombo.GetCurSel() == 2)
			{
				COTETerrainLightmapMgr::GetInstance()->Blur(COTETilePage::GetCurrentPage(), 
					pos.x - offset / 2.0f, pos.z - offset / 2.0f, m_LightmapBrush.m_InnerRad);
			}
		}
	}

}


// ------------------------------------------------
// 光照图颜色
void CLightmapEditDlg::OnBnClickedButton2()
{
	COLORREF cor = RGB(m_LightmapBrush.m_Colour.r * 255, m_LightmapBrush.m_Colour.g * 255, m_LightmapBrush.m_Colour.b * 255);
	
	CColorDialog color(cor, CC_FULLOPEN );	//	

	if(color.DoModal() == IDOK)
	{
		cor = color.GetColor();

		m_LightmapBrush.m_Colour.r = GetRValue(cor) / 255.0f;
		m_LightmapBrush.m_Colour.g = GetGValue(cor) / 255.0f;
		m_LightmapBrush.m_Colour.b = GetBValue(cor) / 255.0f;

	}

}

// ------------------------------------------------
// 选择光照图
void CLightmapEditDlg::OnBnClickedButton3()
{
	CWinApp* pApp = AfxGetApp(); 
	static CString path;
	path = pApp->GetProfileString("OTE文件路径配置", "选择光照图");	

	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);

	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		 "光照图 (*.bmp)|*.bmp|All Files (*.*)|*.*||",
		 NULL 
		); 
	tpFileDialog->m_ofn.lpstrInitialDir = path;
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);
	
	if (nResponse == IDOK)
	{ 	 
		CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
		tpFileDialog->GetFileName().GetLength());  

		pApp->WriteProfileString("OTE文件路径配置", "打开角色", strPath);	

		CString pathName = tpFileDialog->GetPathName();

		COTETerrainLightmapMgr::GetInstance()->SetLightMapMaterial(
			COTETilePage::GetCurrentPage(), 
			tpFileDialog->GetFileName().GetString(),
			(pathName.Left( pathName.GetLength() - tpFileDialog->GetFileName().GetLength())).GetString() 
			);

		COTETerrainLightmapMgr::GetInstance()->LoadLightMapData(COTETilePage::GetCurrentPage());
	}
	
	delete tpFileDialog; 
}

// ------------------------------------------------
// 碰撞图生成
void CLightmapEditDlg::OnBnClickedButton4()
{	
	if(::MessageBox(NULL, "你将把整个场景的碰撞信息计算到当前的光照图上，是否继续?", "提示", MB_YESNO) != IDYES)
		return;

	if(!COTETilePage::GetCurrentPage())
		return;
	
	if(COTETilePage::GetCurrentPage()->m_LightmapData == NULL)
	{
		COTETerrainLightmapMgr::GetInstance()->LoadLightMapData(COTETilePage::GetCurrentPage());	
	}

	COTETerrainLightmapMgr::GetInstance()->GenCollisionMap(COTETilePage::GetCurrentPage());
	COTETerrainLightmapMgr::GetInstance()->SaveLightMapFile(
		COTETilePage::GetCurrentPage()		
		);
	COTETerrainLightmapMgr::GetInstance()->RefreshLightmapMaterial(COTETilePage::GetCurrentPage());		
}

// 小
void CLightmapEditDlg::OnBnClickedRadio1()
{
	m_LightmapBrush.m_InnerRad = 1;
	m_LightmapBrush.m_OutterRad = 1;
}
// 中
void CLightmapEditDlg::OnBnClickedRadio2()
{
	m_LightmapBrush.m_InnerRad = 4;
	m_LightmapBrush.m_OutterRad = 4;
}
// 大
void CLightmapEditDlg::OnBnClickedRadio3()
{
	m_LightmapBrush.m_InnerRad = 8;
	m_LightmapBrush.m_OutterRad = 8;
}

// ------------------------------------------------
// 计算光照图
void CLightmapEditDlg::OnBnClickedButton5()
{
	if(!COTETilePage::GetCurrentPage())
		return;	
	
	if(::MessageBox(NULL, "你将对整个场景重新计算光照，是否继续?", "提示", MB_YESNO) != IDYES)
		return;

	
	Ogre::ColourValue ambientColor	= COTERenderLight::GetInstance()->GetLightKey()->ambient.GetColor();

	Ogre::Vector3 mainLightDir		= SCENE_MAINLIGHT->getDirection();
	Ogre::ColourValue mainLightCor	= COTERenderLight::GetInstance()->GetLightKey()->diffuse.GetColor();//阳光色

	if(!COTETerrainLightmapMgr::GetInstance()->LoadLightMapData(COTETilePage::GetCurrentPage()))
	{
		//加载空光照图

		std::string strFileName;	

		switch(COTETilePage::GetCurrentPage()->m_PageSizeX)
		{
			case 256:
				strFileName = std::string(OTEHelper::GetResPath("ResConfig", "HelperPath")) + "lightMapTemplate512.bmp";
				break;
			case 64:
				strFileName = std::string(OTEHelper::GetResPath("ResConfig", "HelperPath")) + "lightMapTemplate128.bmp";
				break;
			default:
				strFileName = std::string(OTEHelper::GetResPath("ResConfig", "HelperPath")) + "lightMapTemplate512.bmp";
		}
		COTETerrainLightmapMgr::GetInstance()->LoadLightMapData(COTETilePage::GetCurrentPage(), strFileName);	
	}

	//计算光照图中颜色

	CString s;
	GetDlgItemText(IDC_lblTime, s);

	//Ogre::ColourValue terAmbientColor = COTERenderLight::GetInstance()->GetLightKey()->terrAmb.GetColor();
	//Ogre::ColourValue shadowColor =  ambientColor;
	//Ogre::ColourValue terMainColor = COTERenderLight::GetInstance()->GetLightKey()->terrDiff.GetColor();

	COTETerrainLightmapMgr::GetInstance()->GenLightmap(	COTETilePage::GetCurrentPage(),
																mainLightDir,	
																mainLightCor, 		
																ambientColor,	
																COTETerrainLightmapMgr::E_COVER,
																m_bRenderShadow
															);

	//设置光照图

	//COTETerrainLightmapMgr::GetInstance()->SetLightMapMaterial(	COTETilePage::GetCurrentPage() );

	::MessageBox(NULL, "渲染完成！", "提示", MB_OK);
}
// ------------------------------------------------
void CLightmapEditDlg::OnBnClickedButton7()
{
	if(!COTETilePage::GetCurrentPage())
		return;

	// TODO: 在此添加控件通知处理程序代码
	
	std::string str = COTETilePage::GetCurrentPage()->m_LightmapPath + COTETilePage::GetCurrentPage()->m_LightmapName;
	if( str.c_str() )
		::remove( str.c_str() );
}
// ------------------------------------------------
void CLightmapEditDlg::OnBnClickedCheck1()
{
	m_bRenderShadow = !m_bRenderShadow;	
}

void CLightmapEditDlg::OnNMReleasedcaptureSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CLightmapEditDlg::OnNMCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);

	m_LightmapBrush.m_Intensity = m_StrengthSlider.GetPos() / 50.0f;

	*pResult = 0;
}
// ------------------------------------------------
void CLightmapEditDlg::OnBnClickedButton8()
{
	Ogre::ColourValue& ambient = COTERenderLight::GetInstance()->GetLightKey()->ambient.baseCor;
	COLORREF cor = RGB(ambient.r * 255, ambient.g * 255, ambient.b * 255);
	
	CColorDialog color(cor, CC_FULLOPEN );

	if(color.DoModal() == IDOK)
	{
		cor = color.GetColor();

		ambient.r = GetRValue(cor) / 255.0f;
		ambient.g = GetGValue(cor) / 255.0f;
		ambient.b = GetBValue(cor) / 255.0f;

	}

}
// ------------------------------------------------
void CLightmapEditDlg::OnBnClickedButton9()
{
	if(::MessageBox(NULL, "你将对整个场景羽化，是否继续?", "提示", MB_YESNO) != IDYES)
		return;
	COTETerrainLightmapMgr::GetInstance()->Blur(COTETilePage::GetCurrentPage());
}

void CLightmapEditDlg::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
}

// ------------------------------------------------
Ogre::String CLightmapEditDlg::GetTemplateLightmap()
{
		 if(((CButton*)GetDlgItem(IDC_RADIO4))->GetCheck() == BST_CHECKED)
	{
		return "lightMapTemplate128.bmp";
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO5))->GetCheck() == BST_CHECKED)
	{
		return "lightMapTemplate256.bmp";
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO6))->GetCheck() == BST_CHECKED)
	{
		return "lightMapTemplate512.bmp";
	}
	else if(((CButton*)GetDlgItem(IDC_RADIO7))->GetCheck() == BST_CHECKED)
	{
		return "lightMapTemplate1024.bmp";
	}

	return "lightMapTemplate256.bmp";

}

// ------------------------------------------------
// 计算光照图
void CLightmapEditDlg::OnBnClickedButton10()
{

	// 计算光照图

	
	Ogre::ColourValue& ambient = COTERenderLight::GetInstance()->GetLightKey()->ambient.GetColor();
	Ogre::ColourValue& diffuse = COTERenderLight::GetInstance()->GetLightKey()->diffuse.GetColor();
	COTEEntity* e = (COTEEntity*)SCENE_MGR->GetCurObjSection();
	if(e && e->getName().find("Lgt_") == std::string::npos)
	{
		COTEEntityLightmapPainter::GenLightmap(
			e,
			Ogre::Ray(Ogre::Vector3::ZERO, SCENE_MAINLIGHT->getDirection()),
			diffuse, 
			ambient,//SCENE_MGR->getAmbientLight(),			
			GetTemplateLightmap(),
			m_bRenderShadow
			);	
	}
	else
	{		
		COTEEntityLightmapPainter::GenSceneLightmaps(	
			SCENE_MAINLIGHT->getDirection(),
			diffuse, 
			ambient,//SCENE_MGR->getAmbientLight(),	
			NULL,
			"Lgt_",
			m_bRenderShadow,
			NULL,
			1.0f
			);
		
	}
}

// ------------------------------------------------
// 顶点色

void CLightmapEditDlg::OnBnClickedButton11()
{
	
	Ogre::ColourValue& ambient = COTERenderLight::GetInstance()->GetLightKey()->ambient.GetColor();
	Ogre::ColourValue& diffuse = COTERenderLight::GetInstance()->GetLightKey()->diffuse.GetColor();
	COTEEntity* e = (COTEEntity*)SCENE_MGR->GetCurObjSection();
	if(e && e->getName().find("Lgt_") == std::string::npos)
	{
		COTEEntityVertexColorPainter::GenLightDiffmap(
			(COTEActorEntity*)SCENE_MGR->GetCurObjSection(),
			SCENE_MAINLIGHT->getDirection(), diffuse, ambient,  1.0, m_bRenderShadow);

	}
	else
	{
		COTEEntityVertexColorPainter::GenLightDiffmap(			
			SCENE_MAINLIGHT->getDirection(), diffuse, ambient,  1.0, "Lgt_", m_bRenderShadow);

	}
	
}

void CLightmapEditDlg::OnBnClickedRadio4()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CLightmapEditDlg::OnBnClickedButton12()
{
	COTEEntity* e = (COTEEntity*)SCENE_MGR->GetCurObjSection();
	if(e && e->getName().find("Lgt_") == std::string::npos)
	{
		COTEEntityLightmapPainter::SaveLightmap(e);

	}
	else
	{
		COTEEntityLightmapPainter::SaveLightmap();

	}
	
}

void CLightmapEditDlg::OnBnClickedButton13()
{
	COTEEntity* e = (COTEEntity*)SCENE_MGR->GetCurObjSection();
	if(e && e->getName().find("Lgt_") == std::string::npos)
	{
		COTEEntityVertexColorPainter::SaveVertexColorData(e);

	}
	else
	{
		COTEEntityVertexColorPainter::SaveVertexColorData();

	}
	
}
