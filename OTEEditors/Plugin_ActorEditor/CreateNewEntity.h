#pragma once


// CreateNewEntity �Ի���

class CreateNewEntity : public CDialog
{
	DECLARE_DYNAMIC(CreateNewEntity)

public:
	CreateNewEntity(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CreateNewEntity();

// �Ի�������
	enum { IDD = IDD_AddEntity };

	static CreateNewEntity s_Inst;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	//����Ի���
	void clear();

	afx_msg void OnBnClickedBrowsermesh();
	afx_msg void OnBnClickedBrowserskeleton();
	afx_msg void OnBnClickedOk();
};
