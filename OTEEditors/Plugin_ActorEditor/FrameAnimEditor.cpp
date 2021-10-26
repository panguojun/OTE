// FrameAnimEditor.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FrameAnimEditor.h"
#include "Ogre.h"
#include "KeyFrameDlg.h"
#include "ActionEdit.h"
#include "Texture.h"
#include "OTECommon.h"

// CFrameAnimEditor �Ի���

CFrameAnimEditor CFrameAnimEditor::s_Inst;

IMPLEMENT_DYNAMIC(CFrameAnimEditor, CDialog)
CFrameAnimEditor::CFrameAnimEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CFrameAnimEditor::IDD, pParent)
{

}

CFrameAnimEditor::~CFrameAnimEditor()
{
}

void CFrameAnimEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FRAMEANI_PIC, m_AnimPic);
	DDX_Control(pDX, IDC_LIST2, m_FileNameList);
}


BEGIN_MESSAGE_MAP(CFrameAnimEditor, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_EN_CHANGE(IDC_EDIT3, OnEnChangeEdit3)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
//	ON_LBN_SELCHANGE(IDC_LIST2, OnLbnSelchangeList2)
	ON_LBN_DBLCLK(IDC_LIST2, OnLbnDblclkList2)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
	ON_LBN_SELCHANGE(IDC_LIST2, OnLbnSelchangeList2)
END_MESSAGE_MAP()


// ----------------------------------------
// ��ʼ��
BOOL CFrameAnimEditor::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();	

	Update();

	return ret;
}

// ----------------------------------------
// ����
void CFrameAnimEditor::Update()
{	
	m_FileNameList.ResetContent();
	if(CKeyFrameDlg::s_Inst.m_pTrack && CKeyFrameDlg::s_Inst.m_CurKey)
	{
		OTE::COTEKeyFrame *kf = 
			CKeyFrameDlg::s_Inst.m_pTrack->GetKeyFrameAt(CKeyFrameDlg::s_Inst.m_CurKey->Time);

		if(kf)
		{
			for(int i = 0; i < kf->m_FrameList.size(); i ++)
			{
				m_FileNameList.AddString(kf->m_FrameList[i].c_str());
			}
		}
	}

}

// ----------------------------------------
// ȡ��

void CFrameAnimEditor::OnBnClickedButton2()
{
	
}

// ----------------------------------------
// ��ָ��

void CFrameAnimEditor::OnBnClickedButton3()
{
	if(CKeyFrameDlg::s_Inst.m_pTrack && CKeyFrameDlg::s_Inst.m_CurKey)
	{
		OTE::COTEKeyFrame* kf = (OTE::COTEKeyFrame*)CKeyFrameDlg::s_Inst.m_CurKey->pUserData;
		if(kf)
		{	
			kf->m_FrameList.clear();
			if(m_FileNameList.GetSelCount() == 1)
			{
				CString str;
				m_FileNameList.GetText(m_FileNameList.GetCurSel(), str);
				kf->m_TextureName = str.GetString();
			}
			else
			{
				int  *sels  =  new  int[m_FileNameList.GetSelCount()];   
				m_FileNameList.GetSelItems(m_FileNameList.GetSelCount(),   sels);   
				for(int i = 0; i < m_FileNameList.GetSelCount(); i++)   
				{   
					CString str;
					m_FileNameList.GetText(sels[i], str);
					kf->m_FrameList.push_back(str.GetString());
				}   
				delete []  sels;
			}
		}
	}
}

// ----------------------------------------
void CFrameAnimEditor::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CDialog::OnPaint();

	if(m_Texture.GetData())
	{	
		//m_Texture.Draw(m_AnimPic.GetDC(), 0, 0, width, height, ((index * width) % imgWidth), ((index * width) / imgWidth) * height);
		m_Texture.Draw(m_AnimPic.GetDC(), 0, 0, -1, -1, 0, 0);
	}
	
}

// ----------------------------------------
// ɾ��
void CFrameAnimEditor::OnBnClickedButton1()
{
	int  *sels  =  new  int[m_FileNameList.GetSelCount()]; 	
	for(int i = 0; i < m_FileNameList.GetSelCount(); i++)   
	{   
		m_FileNameList.GetSelItems(m_FileNameList.GetSelCount(),   sels); 	
		m_FileNameList.DeleteString(sels[i]);
		 
		i = -1;
	}   
	delete [] sels;
}

// ----------------------------------------

void CFrameAnimEditor::OnBnClickedButton6()
{
}

// ----------------------------------------
// ͼԪ�������¡�
void CFrameAnimEditor::OnEnChangeEdit3()
{
	
}

// ----------------------------------------
void CFrameAnimEditor::OnBnClickedButton4()
{		
	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
		
	CWinApp* pApp = AfxGetApp();   

	static CString path;
	path = pApp->GetProfileString("OTE�ļ�·������", "ѡ����Ч��ͼ");
	
	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT,
		"��ͼ�ļ�(*.tga)|*.tga|��ͼ�ļ�(*.dds)|*.dds|All Files (*.*)|*.*|",						
		NULL 
		); 
	tpFileDialog->m_ofn.lpstrInitialDir = path;
	int nResponse =tpFileDialog->DoModal();	

	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{
		CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
																				tpFileDialog->GetFileName().GetLength());   
		pApp->WriteProfileString("OTE�ļ�·������", "ѡ����Ч��ͼ", strPath);

		POSITION   pos  =  tpFileDialog->GetStartPosition(); 
		while(pos) 
		{ 				 
			CString szFilePathName = tpFileDialog->GetNextPathName(pos);
			CString   strFilename   =   szFilePathName.Mid(szFilePathName.ReverseFind('\\') + 1);
			m_FileNameList.AddString(strFilename);
		} 			
	}
	delete tpFileDialog;	
}

void CFrameAnimEditor::OnEnChangeFilenameEdit()
{

}

void CFrameAnimEditor::OnCbnSelchangeCombo2()
{
	
}

// ------------------------------------------
void CFrameAnimEditor::OnLbnDblclkList2()
{
	CString str;
	m_FileNameList.GetText(m_FileNameList.GetCurSel(), str);

	std::string locName = OTE::OTEHelper::GetResPath("ResConfig", "FrameTexPath");		
	std::string fileName = locName + str.GetString();

	m_Texture.Free();
	m_Texture.ReadFileTGA(fileName.c_str());	

	this->InvalidateRect(NULL);
}

// ------------------------------------------
// ָ����ȫ����
void CFrameAnimEditor::OnBnClickedButton5()
{
	/*if(CKeyFrameDlg::s_Inst.m_pTrack)
	{
		std::list<OTE::COTEKeyFrame*> lista;
		CKeyFrameDlg::s_Inst.m_pTrack->GetKeyFrameList(&lista);
		
		int index = 0;
		std::list<OTE::COTEKeyFrame*>::iterator it = lista.begin();		
		while(it != lista.end() && index < m_FileNameList.GetCount())
		{
			CString txt;
			m_FileNameList.GetText(index ++ , txt);
			(*it)->m_TextureName = txt.GetString();

			++ it;
		}			
	}*/

	if(CKeyFrameDlg::s_Inst.m_pTrack)
	{
		std::list<OTE::COTEKeyFrame*> lista;
		CKeyFrameDlg::s_Inst.m_pTrack->GetKeyFrameList(&lista);
		
		std::list<OTE::COTEKeyFrame*>::iterator it = lista.begin();		
		while(it != lista.end())
		{
			CString txt;
			m_FileNameList.GetText(m_FileNameList.GetCurSel(), txt);
			(*it)->m_TextureName = txt.GetString();
			(*it)->m_FrameList.clear();
			(*it)->m_FrameList.push_back( txt.GetString() );

			++ it;	
		}			
	}
}

// ------------------------------------------
void CFrameAnimEditor::OnLbnSelchangeList2()
{
	CString str;
	m_FileNameList.GetText(m_FileNameList.GetCurSel(), str);

	std::string locName = OTE::OTEHelper::GetResPath("ResConfig", "FrameTexPath");				
	std::string fileName = locName + str.GetString();

	m_Texture.Free();
	m_Texture.ReadFileTGA(fileName.c_str());	

	this->InvalidateRect(NULL);
}
