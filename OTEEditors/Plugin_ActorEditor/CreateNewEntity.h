#pragma once


// CreateNewEntity 对话框

class CreateNewEntity : public CDialog
{
	DECLARE_DYNAMIC(CreateNewEntity)

public:
	CreateNewEntity(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CreateNewEntity();

// 对话框数据
	enum { IDD = IDD_AddEntity };

	static CreateNewEntity s_Inst;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	//清除对话框
	void clear();

	afx_msg void OnBnClickedBrowsermesh();
	afx_msg void OnBnClickedBrowserskeleton();
	afx_msg void OnBnClickedOk();
};
