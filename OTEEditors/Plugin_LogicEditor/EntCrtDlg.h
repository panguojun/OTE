#pragma once
#include "afxcmn.h"
#include "Ogre.h"
#include "OTEMagicManager.h"
#include "trackball.h"
#include "afxwin.h"

// ******************************************
#define NPCMAXWP 64
/* Npc Start Position */
struct NpcStartPos_t
{
	char         name[32];
	char         npcClassName[32];
	unsigned int ID;
	bool         alive;
	Vector3      Pos;
	float        Rot;
    unsigned int npcID;
	int          territoryRadious;
	
	int			 secIndex;
	
	Vector3	     WayPoints[NPCMAXWP];
	int			 numWP;
	//PathTailHelper_t*	WPTailHelper[NPCMAXWP];
	//PathWayHelper_t*	WPPathHelper[NPCMAXWP];

	char		 npcspeech[5][256];	
};

/* Npc for output */
struct npc_t
{
    unsigned int npcID;
	char         name[16];
	float        Pos[3];
	float        Rot;
	int          territoryRadious;
	int		     numWP;
	float		 WayPoints[NPCMAXWP][3];

	char		 npcspeech[5][256];
};

/* Logic Sector */
struct LogicSector_t
{
   char Name[32];
   bool isLoaded;
   unsigned int LogicSectorID;  
   char NpcFileName[32];
   std::vector<NpcStartPos_t> vNpcStartPos;
};

// ******************************************
// CEntCrtDlg 对话框
class CEntCrtDlg : public CDialog
{
	DECLARE_DYNAMIC(CEntCrtDlg)
public:
	static CEntCrtDlg		sInst;
	
public:
	CEntCrtDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEntCrtDlg();

// 对话框数据
	enum { IDD = IDD_CRT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog();

public:	
	//是否使用随机朝向
	bool m_bUseRDirection;
	//是否使用随机大小
	bool m_bUserRSize;
	//是否使用碰撞检测
	bool m_bCollide;

	CTreeCtrl m_MeshResTree;
	HTREEITEM RootItem;

	//清除某一分枝
	void Clear(HTREEITEM &t);
	//更新资源列表
	void UpdataRes();

	void OnSceneMouseMove(float x, float y);

	bool OnSceneLBDown(float x, float y);

	//判断是否会发生碰撞,参数为最小距离
	bool TestCollide(Ogre::Real r = 0.0f);
	//使用随机大小和方向
	void RandomSizeDir();

	//缩放倍数
	Ogre::Real			scale;

	//旋转朝向
	Ogre::Quaternion	direction;
	
public:
	Ogre::MovableObject*			m_MouseObj;
	
	std::vector<std::string>		m_vNpcList;
	void SearchNpcList();

public:
	void ClearMouseEntity();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	afx_msg void OnBnClickedCheck3();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnEnChangeEdit1();
	CComboBox m_CrtType;
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
};
