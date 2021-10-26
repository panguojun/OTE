#pragma once

#include "EditListCtrl.h"
#include "OTEAction.h"
#include "OTETrackManager.h"
#include "afxwin.h"

// ********************************************
// CActionEdit
// �ж��༭��
// ********************************************

class CActionEdit : public CDialog
{
	DECLARE_DYNAMIC(CActionEdit)

public:
	CActionEdit(CWnd* pParent = NULL);   
	virtual ~CActionEdit();

	enum { IDD = IDD_ActionEdit };

public:

	//�����ж��б�
	void UpdateActionList();

	//������Ч�б�
	void UpdateEffect();

	int Createtrack(int item, int trackId);

	int Createtrack(int item);

	void SelectObj(bool fixedView);

	void OnEndPlugin();	

	// �ж�����

	void SetActionTime();

	// ɾ���ж�

	void DelAction();

	// �༭���

	void EditTrack();

	void EditTrack(int trackID);

	// �Զ�ѡ��Ч���б�����

	void AutoSelEffect(int trackID);

	// �༭Ч��

	void EditEffect();
	
	// ɾ��Ч��Ԫ��

	void DelEffect();

	// ɾ�����

	void DelTrack();

	// ̽���˵�

	void PupMenu(const CPoint& rPoint, DWORD resourceID);

	// ����Ч��Ԫ���б�

	void ImportMagList();

	// ������Ӧ��

	void CreateReactor(const std::string& rResName);

	int CreateReactor(int item, int reactorId, const std::string& rResName);	

	// ���Ч������

	void AddEffectMagic(const std::string& magicFile);

	// ������

	void OnSceneLButtonUp(float x, float y);
	void OnSceneRButtonUp(float x, float y);

	void OnSceneLButtonDown(float x, float y);

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	//DisableAllItem
	void DisableAllItem();

	BOOL OnInitDialog();   
	

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLbnSelchangeActlist();
	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedStop();
	afx_msg void OnLbnDblclkActlist();
	afx_msg void OnBnClickedImport();
	afx_msg void OnBnClickedExport();
	afx_msg void OnBnClickedAdd();
	afx_msg void OnNMClickMagiclist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedAddmagic();
	afx_msg void OnBnClickedUsetrack();
	afx_msg void OnEnKillfocusMagspeed();
	afx_msg void OnBnClickedCreatetrack();

	afx_msg void OnCopyMag();
	
	afx_msg void OnNMDblclkMagiclist(NMHDR *pNMHDR, LRESULT *pResult);	
	afx_msg void OnBnClickedAddmagic2();
	afx_msg void OnBnClickedAddsound();		
	afx_msg void OnBnClickedButton2();		
	afx_msg void OnEnChangeTime();
	afx_msg void OnCbnSelchangeBoneanimcombo();		

	afx_msg void OnEnChangeAnispeed();
	afx_msg void OnBnClickedChangeActionName();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCheck2();

private:

	bool AnalyseInput(int item, unsigned int& interID);

	bool IsActionNameValid(const CString& rText);

public:
	static CActionEdit s_Inst;

	CButton m_IsAutoPlayCheck;	
	CButton m_IsLoop;

	bool				m_Inited;
	bool				m_isEdited;

	bool				m_WaitForPickBone;

	CListBox			m_ActList;
	CEditListCtrl		m_MagList;
	OTE::COTEAction	   *m_pAct;
	CComboBox			m_AniListCombo;
	CButton				m_IsShowBones;	


	afx_msg void OnLvnItemchangedMagiclist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedIsautoplay();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedIsloop();
	afx_msg void OnCbnSelendokBoneanimcombo();
	afx_msg void OnCbnKillfocusBoneanimcombo();
};
