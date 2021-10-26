// MeshListDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Plugin_MeshEditor.h"
#include "MeshListDlg.h"
#include <vector>
#include <string>

CString CMeshListDlg::s_MouseMesh;

// CMeshListDlg 对话框

IMPLEMENT_DYNAMIC(CMeshListDlg, CDialog)
CMeshListDlg::CMeshListDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMeshListDlg::IDD, pParent)
{
}

CMeshListDlg::~CMeshListDlg()
{
}

void CMeshListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_MeshTreeList);
}


BEGIN_MESSAGE_MAP(CMeshListDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
END_MESSAGE_MAP()


// CMeshListDlg 消息处理程序

void CMeshListDlg::OnBnClickedOk()
{

	OnOK();
}

void ScanfFiles(const CString& strPathName, const CString& strFileName, std::vector<std::string>& rFileList)
{ 	
	// 搜索文件	
	CFileFind finder;
	BOOL bFound = finder.FindFile(CString(strPathName) + "/" + strFileName);
	while (bFound)
	{
		bFound = finder.FindNextFile();
		if(!finder.IsDirectory() && !finder.IsDots())
		{
			rFileList.push_back((LPCTSTR)(strPathName + "/" + finder.GetFileName()));
		}		
	}				
	finder.Close();
	
}

void AllDirScanfFiles(const CString& strPathName, const CString& strFileName, std::vector<std::string>& rFileList)
{ 	
	// 搜索文件夹
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strPathName + "/*.*");
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if(finder.IsDirectory() && !finder.IsDots())
		{
			ScanfFiles(strPathName + "/" + finder.GetFileName(), strFileName, rFileList);
		}	
	}
	finder.Close();

	// 搜索文件		
	BOOL bFound = finder.FindFile(CString(strPathName) + "/" + strFileName);
	while (bFound)
	{		
		bFound = finder.FindNextFile();
		if(!finder.IsDirectory() && !finder.IsDots())
		{
			std::string ddd = finder.GetFileName();
			rFileList.push_back((LPCTSTR)(strPathName + "/" + finder.GetFileName()));
		}			
	}				
	finder.Close();	
}

BOOL CMeshListDlg::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();
	std::vector<std::string> list;
	AllDirScanfFiles("../res", "*.mesh", list);
	
	HTREEITEM rootItem = m_MeshTreeList.InsertItem("Mesh列表", NULL);
	HTREEITEM newItem;
	for(int i = 0; i < list.size(); i ++)
	{
		m_MeshTreeList.InsertItem(list[i].c_str(), rootItem);
	}

	return tRet;
}

void CMeshListDlg::OnBnClickedButton1()
{

}

void CMeshListDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	s_MouseMesh = m_MeshTreeList.GetItemText(m_MeshTreeList.GetSelectedItem());
}
