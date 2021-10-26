#pragma once
#include "afxcmn.h"
#include "Ogre.h"
#include "OTEMagicManager.h"
#include "trackball.h"
#include "afxwin.h"

// CEntCrtDlg �Ի���
class CEntCrtDlg : public CDialog
{
	DECLARE_DYNAMIC(CEntCrtDlg)
public:
	static CEntCrtDlg		sInst;
	
public:
	CEntCrtDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CEntCrtDlg();

// �Ի�������
	enum { IDD = IDD_CRT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog();

public:	
	//�Ƿ�ʹ���������
	bool m_bUseRDirection;
	//�Ƿ�ʹ�������С
	bool m_bUserRSize;
	//�Ƿ�ʹ����ײ���
	bool m_bCollide;

	CTreeCtrl m_MeshResTree;
	HTREEITEM RootItem;

	//������ԴĿ¼
	void AllDirScanfFiles(std::string strPathName, HTREEITEM& item);
	//���ĳһ��֦
	void Clear(HTREEITEM &t);
	//������Դ�б�
	void UpdataRes();

	void OnSceneMouseMove(float x, float y);

	bool OnSceneLBDown(float x, float y);

	//�ж��Ƿ�ᷢ����ײ,����Ϊ��С����
	bool TestCollide(Ogre::Real r = 0.0f);
	//ʹ�������С�ͷ���
	void RandomSizeDir();

	//���ű���
	Ogre::Real			scale;

	//��ת����
	Ogre::Quaternion	direction;
	
public:
	Ogre::MovableObject*	 m_MouseObj;
	
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
};
