#pragma once
#include "resource.h"
#include "afxcmn.h"
#include "EditListCtrl.h"
#include "TransformManager.h"
#include "OTEQTSceneManager.h"
#include "Ogre.h"
#include "OTEActorEntity.h"
#include "OTEMagicManager.h"
#include "TrackBall.h"
#include "OTELineObj.h"
#include "EntCrtDlg.h"

// ********************************************************
// CLogicEditorDlg �Ի���
// ********************************************************

class CLogicEditorDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogicEditorDlg)

public:
	CLogicEditorDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CLogicEditorDlg();

// �Ի�������
	enum { IDD = IDD_EntEditMain };
public:
	static	CLogicEditorDlg		s_Inst;
	static CTransformManager	s_TransMgr;
	static CString   			s_EditState;
	static std::list<Ogre::MovableObject*>		s_pSelGroup;
	static std::list<EntityEditData_t>			s_pCopyGroup;

	std::vector<LogicSector_t>			m_vLogicSectors;

public:	
	Ogre::Vector3     m_SnapStartPos;
	Ogre::Vector3     m_SnapEndPos;
	CLineObj*		  m_Snap3DMark;
	bool			  m_IsShiftCloned;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	// �������
	void ClearScene();

	// ���ѡ��
	void CancelSelection();

	// �����а���
	void OnSceneKeyDown();

	// ѡ�񳡾����
	void SelectSceneEntity(Ogre::MovableObject* e, bool autoSel = false);

	void RectSelEntities();

	// �������ƶ����
	void OnSceneMouseMove(const Ogre::Vector2& rScreenMove,float x, float y);

	// �������ƶ����
	void RemoveSelEntities();

	// ���������
	void OnLButtonDown(float x, float y);

	// �������
	void OnLButtonUp(float x, float y);

	// ���˫��
	void OnLButtonDClick(float x, float y);

	// ͨ������ѡ
	bool SelectEntityByMouse(float x, float y);

	// Ⱥѡ
	void RectSelEntities(const OTE::CRectA<float>& rect);

	bool ReadLogicSectorNpcInfor(unsigned int secID);

	int GetLogicSector(unsigned int id);

	bool ShowSectorMap(const std::string& logicSectorMap);

	bool LoadSectorMap(const std::string& logicSectorMap);

	unsigned int GetLogicSectorIDAt(const Ogre::Vector3& pos);

	int GetLogicSectorAt(const Ogre::Vector3& pos);

	void ReadSectorList();

	void SaveLogicSectorNpcInfor();

	unsigned int* m_SectorMapData; 

	CTabCtrl m_TabCtrl;
	CTreeCtrl m_SceneTreeCtrl;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnTcnSelchangeTab2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedCheck1();
};
