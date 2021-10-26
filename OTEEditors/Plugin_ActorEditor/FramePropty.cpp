// FramePropty.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "FramePropty.h"
#include "KeyFrameDlg.h"
#include "ListViewEditor.h"
#include "ColorDlg.h"
#include "ActionEdit.h"
#include "OTEMathLib.h"
#include "FrameAnimEditor.h"
#include "KeyTrackBall.h"
#include "ActorEditorMainDlg.h"

// CFramePropty �Ի���
CFramePropty	CFramePropty::s_Inst;

// --------------------------------------------------------
IMPLEMENT_DYNAMIC(CFramePropty, CDialog)
CFramePropty::CFramePropty(CWnd* pParent /*=NULL*/)
	: CDialog(CFramePropty::IDD, pParent)
{
}

CFramePropty::~CFramePropty()
{
}

void CFramePropty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_PorpList);
}

BEGIN_MESSAGE_MAP(CFramePropty, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnLvnItemchangedList1)
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, OnHdnItemdblclickList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclkList1)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST1, OnLvnKeydownList1)
END_MESSAGE_MAP()


// --------------------------------------------------------
BOOL CFramePropty::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();

	m_PorpList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_MULTIWORKAREAS);
	m_PorpList.InsertColumn(0, "����", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 70); 
	m_PorpList.InsertColumn(1, "ֵ", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 130); 

	return tRet;
}
	

void CFramePropty::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

// --------------------------------------------------------
/// ���ָ���

// ��ɫ
void OnColorDlgNotify(DWORD cor, DWORD ud)
{
	if(ud == 0)
	{
		OTE::COTEKeyFrame* kf = CKeyFrameDlg::s_Inst.GetCurrentKey();

		if(kf)
		{
			kf->m_Colour.r = GetRValue(cor) / 255.0f;
			kf->m_Colour.g = GetGValue(cor) / 255.0f;
			kf->m_Colour.b = GetBValue(cor) / 255.0f;

			if(CActionEdit::s_Inst.m_pAct)
				CActionEdit::s_Inst.m_pAct->UpdateTrackMagicPos();
		}
	}
	else
	{
		std::list<OTE::COTEKeyFrame*> lista;
		CKeyFrameDlg::s_Inst.m_pTrack->GetKeyFrameList(&lista);
		
		std::list<OTE::COTEKeyFrame*>::iterator it = lista.begin();		
		while(it != lista.end())
		{		
			(*it)->m_Colour.r = GetRValue(cor) / 255.0f;
			(*it)->m_Colour.g = GetGValue(cor) / 255.0f;
			(*it)->m_Colour.b = GetBValue(cor) / 255.0f;

			++ it;	
		}	

		if(CActionEdit::s_Inst.m_pAct)
			CActionEdit::s_Inst.m_pAct->UpdateTrackMagicPos();
	}
}

// --------------------------------------------------
// ����
void OnNumberDlgNotify(int ptrOffset, const char* val)
{
	OTE::COTEKeyFrame* kf = CKeyFrameDlg::s_Inst.GetCurrentKey();

	if(kf)
	{
		/**((float*)(kf + ptrOffset))*/kf->m_Time = atof(val);

		if(CActionEdit::s_Inst.m_pAct)
			CActionEdit::s_Inst.m_pAct->UpdateTrackMagicPos();
	}
}

// --------------------------------------------------
// �����༭

void OnSliderEdit(float v, CString ud)
{
	OTE::COTEKeyFrame* kf = CKeyFrameDlg::s_Inst.GetCurrentKey();
	if(kf)
	{
		if(     ud == "X����")
		{
			kf->m_Position.x = v;
		}
		else if(ud == "Y����")
		{
			kf->m_Position.y = v;
		}
		else if(ud == "Z����")
		{
			kf->m_Position.z = v;
		}

		else if(ud == "X��ת")
		{
			kf->m_Angles.x = v;
			OTE::MathLib::EulerToQuaternion(kf->m_Angles, kf->m_Quaternion);
		}
		else if(ud == "Y��ת")
		{
			kf->m_Angles.y = v;
			OTE::MathLib::EulerToQuaternion(kf->m_Angles, kf->m_Quaternion);
		}
		else if(ud == "Z��ת")
		{
			kf->m_Angles.z = v;
			OTE::MathLib::EulerToQuaternion(kf->m_Angles, kf->m_Quaternion);
		}

		else if(ud == "X����")
		{
			kf->m_Scaling.x = v;		
		}
		else if(ud == "Y����")
		{
			kf->m_Scaling.y = v;		
		}
		else if(ud == "Z����")
		{
			kf->m_Scaling.z = v;		
		}
		else if(ud == "����")
		{
			kf->m_Scaling.x = v; kf->m_Scaling.y = v; kf->m_Scaling.z = v;
		}
		else if(ud == "U�ƶ�")
		{
			kf->m_UV.x = v;		
		}
		else if(ud == "V�ƶ�")
		{
			kf->m_UV.y = v;		
		}
		else if(ud == "UV��ת")
		{
			kf->m_UVRot = v;		
		}
		else if(ud == "U����")
		{
			kf->m_UVScl.x = v;		
		}
		else if(ud == "V����")
		{
			kf->m_UVScl.y = v;		
		}

		if(CActionEdit::s_Inst.m_pAct)
		{
			CActionEdit::s_Inst.m_pAct->UpdateTrackMagicPos();
			CKeyFrameDlg::s_Inst.UpdateMover(kf);			
		}
	}

}

// ����

void OnSliderEditorOK()
{
	CFramePropty::s_Inst.UpdateFramePropertyList(CKeyFrameDlg::s_Inst.GetCurrentKey());	
}

// --------------------------------------------------------
#define DO_EDITOK(type)  CListViewEditor dlg(type, m_PorpList.GetItemText(tItem, tSubItem), tSubItemRect); if(IDOK == dlg.DoModal() && (!CListViewEditor::s_Text.IsEmpty() && type != 1 || type == 1))

void CFramePropty::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	OTE::COTEKeyFrame* kf = CKeyFrameDlg::s_Inst.GetCurrentKey();
	if(!kf)
		return;

	int tItem, tSubItem;
	CRect tSubItemRect;	

	if(m_PorpList.GetDClickedItem(tItem, tSubItem, tSubItemRect) && tSubItem == 1)
	{	
		// ׼����� 

		std::list<OTE::COTEKeyFrame*> pList;
		CKeyFrameDlg::s_Inst.m_pTrack->GetKeyFrameList(&pList);
		CKeyTrackBall::s_Inst.PushStack(CKeyFrameDlg::s_Inst.m_KeyList, pList);

		switch(tItem)
		{	
		case 0:
			{		
				// ʱ��
				DO_EDITOK(0)
				{								
					kf->m_Time = atof(CListViewEditor::s_Text);
					UpdateFramePropertyList(kf);
				}

			}break;	
		case 2:
			{		
				// X����			
				{
					CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK,
						"X����", kf->m_Position.x, -5.0f, 5.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)) , 100.0f );
				}
			}break;
		case 3:
			{		
				// Y����			
				{
					CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK,
						"Y����", kf->m_Position.y, -5.0f, 5.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)) , 100.0f );
				}
			}break;
	    case 4:
			{		
				// Z����
				//DO_EDITOK(0)
				{
					CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK,
						"Z����", kf->m_Position.z, -5.0f, 5.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)) , 100.0f );
				}
			}break;
	    case 6:
			{		
				// ��X��ת
				//DO_EDITOK(0)
				{	
					CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK,
						"X��ת", kf->m_Angles.x, 0, 360.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)), 20.0f );
				}
				
			}break;
	    case 7:
			{		
				// ��Y��ת
				//DO_EDITOK(0)
				{	
					CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK,
						"Y��ת", kf->m_Angles.y, 0, 360.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)), 20.0f );
				}
			

			}break;
	    case 8:
			{		
				// ��Z��ת
				//DO_EDITOK(0)
				{							
					CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK,
						"Z��ת", kf->m_Angles.z, 0, 360.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)), 20.0f );
				}
			
			}break;
	   
		case 10:
			{		
				// X����
				//DO_EDITOK(0)
				{
					CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK,
						"X����", kf->m_Scaling.x, 0, 10.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)) , 100.0f );
				}
			}break;
		case 11:
			{		
				// Y����
				//DO_EDITOK(0)
				{
					CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK,
						"Y����", kf->m_Scaling.y, 0, 10.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)) , 100.0f );
				}
			}break;
	    case 12:
			{		
				// Z����
				//DO_EDITOK(0)
				{
					CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK,
						"Z����", kf->m_Scaling.z, 0, 10.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)) , 100.0f );
				}
			}break;
	    case 13:
			{		
				// ����
				CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK,
					    "����", (kf->m_Scaling.x + kf->m_Scaling.y + kf->m_Scaling.z) / 3.0f, 0, 10.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)) , 100.0f );
				
	
			}break;
		case 15:
			{		
				// U
				//DO_EDITOK(0)
				{		
					CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK,
						"U�ƶ�", kf->m_UV.x, -5.0f, 5.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)) );

				}
			}break;		
		case 16:
			{		
				// V
				//DO_EDITOK(0)
				{							
					CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK,
						"V�ƶ�", kf->m_UV.y, -5.0f, 5.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)) );


				}
			}break;	
		case 17:
			{		
				// R
				//DO_EDITOK(0)
				{		
					CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK,
						"UV��ת", kf->m_UVRot, 0, 360.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)), 0.1f );

				}
			}break;	
		
		case 18:
			{		
				// SU
				//DO_EDITOK(0)
				{					
					CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK, 
						"U����", kf->m_UVScl.x, 0.1f, 5.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)) );
				}
			}break;		
		case 19:
			{		
				// SV
				//DO_EDITOK(0)
				{							
					CSliderEditor::s_Inst.Show(OnSliderEdit, OnSliderEditorOK,
						"V����", kf->m_UVScl.y, 0.1f, 5.0f, CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25)) );

				}
			}break;	
		case 21:
			{		
				// Tex
				//DO_EDITOK(1)
				{							
					/*kf->m_TextureName = CListViewEditor::s_Text;
					*/
					
					CHAR curDir[256];
					::GetCurrentDirectory(256, curDir);
					   
					CWinApp* pApp = AfxGetApp();   

					static CString path;
					path = pApp->GetProfileString("OTE�ļ�·������", "ѡ����Ч��ͼ");
					
					CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
						NULL,
						NULL,
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						"��ͼ�ļ�(*.tga)|*.tga|��ͼ�ļ�(*.dds)|*.dds|All Files (*.*)|*.*|",						
						NULL 
						); 
					tpFileDialog->m_ofn.lpstrInitialDir = path;
					strcpy(tpFileDialog->m_ofn.lpstrFile, kf->m_TextureName.c_str());
					int nResponse =tpFileDialog->DoModal();	

					::SetCurrentDirectory(curDir);

					if (nResponse == IDOK)
					{
						CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
																						tpFileDialog->GetFileName().GetLength());   
						pApp->WriteProfileString("OTE�ļ�·������", "ѡ����Ч��ͼ", strPath);

						kf->m_TextureName = tpFileDialog->GetFileName();					
					}
					delete tpFileDialog;

					UpdateFramePropertyList(kf);
				}
			}break;	
		case 23:
			{
				CColorDlg::s_Inst.Show(true, kf->m_Colour.getAsABGR());
				CColorDlg::s_Inst.SetNotifyFunc(OnColorDlgNotify);			
			}
			break;	
		case 24:
			{		
				// A
				DO_EDITOK(0)
				{							
					kf->m_Colour.a = atof(CListViewEditor::s_Text);
					UpdateFramePropertyList(kf);
				}
			}break;	

		case 26:
			{	
				// �ӳ�
				DO_EDITOK(0)
				{							
					kf->m_DelayTime = atof(CListViewEditor::s_Text);
					UpdateFramePropertyList(kf);
				}
			}break;

		case 28:
			{	
				// �ɼ���
				DO_EDITOK(0)
				{							
					kf->m_Visible = atof(CListViewEditor::s_Text);
					UpdateFramePropertyList(kf);
				}
			}break;
		case 29:
			{
				// ������
				if(!CFrameAnimEditor::s_Inst.m_hWnd)
					CFrameAnimEditor::s_Inst.Create(IDD_FRAMEANIM_DLG);				
				CFrameAnimEditor::s_Inst.ShowWindow(SW_SHOW);
				CFrameAnimEditor::s_Inst.Update();
						
			}break;
		case 30:
			{	
				// Ѱַģʽ
				DO_EDITOK(0)
				{							
					CKeyFrameDlg::s_Inst.m_pTrack->m_TexAddrMode = (Ogre::TextureUnitState::TextureAddressingMode)atoi(CListViewEditor::s_Text);
					UpdateFramePropertyList(kf);
				}
			}break;
			
		}	
	}

}

// --------------------------------------------------------
// ��������
void CFramePropty::UpdateFramePropertyList(OTE::COTEKeyFrame* kf)
{
	if(!kf)
		return;

	std::stringstream ss;
	
	ss << kf->m_Time;	
	m_PorpList.DeleteAllItems();
	m_PorpList.InsertItem(0, "ʱ��");
	m_PorpList.SetItemText(0, 1, ss.str().c_str());

	m_PorpList.InsertItem(1, "");
	m_PorpList.SetItemText(1, 1, "");

	ss.str("");
	ss << kf->m_Position.x;	
	m_PorpList.InsertItem(2, "ƽ��X");
	m_PorpList.SetItemText(2, 1, ss.str().c_str());

	ss.str("");
	ss << kf->m_Position.y;	
	m_PorpList.InsertItem(3, "ƽ��Y");
	m_PorpList.SetItemText(3, 1, ss.str().c_str());

	ss.str("");
	ss << kf->m_Position.z;	
	m_PorpList.InsertItem(4, "ƽ��Z");
	m_PorpList.SetItemText(4, 1, ss.str().c_str());

	m_PorpList.InsertItem(5, "");
	m_PorpList.SetItemText(5, 1, "");
	
	ss.str("");
	ss << kf->m_Angles.x;
	m_PorpList.InsertItem(6, "��תX");	
	m_PorpList.SetItemText(6, 1, ss.str().c_str());

	ss.str("");
		
	ss << kf->m_Angles.y;
	m_PorpList.InsertItem(7, "��תY");
	m_PorpList.SetItemText(7, 1, ss.str().c_str());

	ss.str("");

	ss << kf->m_Angles.z;
	m_PorpList.InsertItem(8, "��תZ");
	m_PorpList.SetItemText(8, 1, ss.str().c_str());

	m_PorpList.InsertItem(9, "");
	m_PorpList.SetItemText(9, 1, "");

	ss.str("");
	ss << kf->m_Scaling.x;	
	m_PorpList.InsertItem(10, "����X");
	m_PorpList.SetItemText(10, 1, ss.str().c_str());

	ss.str("");
	ss << kf->m_Scaling.y;	
	m_PorpList.InsertItem(11, "����Y");
	m_PorpList.SetItemText(11, 1, ss.str().c_str());

	ss.str("");
	ss << kf->m_Scaling.z;	
	m_PorpList.InsertItem(12, "����Z");
	m_PorpList.SetItemText(12, 1, ss.str().c_str());

	ss.str("");
	ss << (kf->m_Scaling.x + kf->m_Scaling.y + kf->m_Scaling.z) / 3.0f;	
	m_PorpList.InsertItem(13, "����");
	m_PorpList.SetItemText(13, 1, ss.str().c_str());
	
	m_PorpList.InsertItem(14, "");
	m_PorpList.SetItemText(14, 1, "");

	ss.str("");
	ss << kf->m_UV.x;	
	m_PorpList.InsertItem(15, "��ͼ�ƶ�U");
	m_PorpList.SetItemText(15, 1, ss.str().c_str());

	ss.str("");
	ss << kf->m_UV.y;	
	m_PorpList.InsertItem(16, "��ͼ�ƶ�V");
	m_PorpList.SetItemText(16, 1, ss.str().c_str());

	ss.str("");
	ss << kf->m_UVRot;	
	m_PorpList.InsertItem(17, "��ͼ��תR");
	m_PorpList.SetItemText(17, 1, ss.str().c_str());

	ss.str("");
	ss << kf->m_UVScl.x;	
	m_PorpList.InsertItem(18, "��ͼ����U");
	m_PorpList.SetItemText(18, 1, ss.str().c_str());

	ss.str("");
	ss << kf->m_UVScl.y;	
	m_PorpList.InsertItem(19, "��ͼ����V");
	m_PorpList.SetItemText(19, 1, ss.str().c_str());

	m_PorpList.InsertItem(20, "");
	m_PorpList.SetItemText(20, 1, "");

	ss.str("");
	ss << kf->m_TextureName;	
	m_PorpList.InsertItem(21, "�����ļ�");
	m_PorpList.SetItemText(21, 1, ss.str().c_str());

	m_PorpList.InsertItem(22, "");
	m_PorpList.SetItemText(22, 1, "");

	ss.str("");
	ss << "...";	
	m_PorpList.InsertItem(23, "��ɫ");
	m_PorpList.SetItemText(23, 1, ss.str().c_str());

	ss.str("");
	ss << kf->m_Colour.a;	
	m_PorpList.InsertItem(24, "A");
	m_PorpList.SetItemText(24, 1, ss.str().c_str());

	m_PorpList.InsertItem(25, "");
	m_PorpList.SetItemText(25, 1, "");

	ss.str("");
	ss << kf->m_DelayTime;	
	m_PorpList.InsertItem(26, "�ӳ�");
	m_PorpList.SetItemText(26, 1, ss.str().c_str());

	m_PorpList.InsertItem(27, "");
	m_PorpList.SetItemText(27, 1, "");

	ss.str("");
	ss << kf->m_Visible;	
	m_PorpList.InsertItem(28, "�ɼ���");
	m_PorpList.SetItemText(28, 1, ss.str().c_str());

	ss.str("");
	//ss << kf->m_FrameUV.x << ", " << kf->m_FrameUV.y;	
	m_PorpList.InsertItem(29, "������");
	m_PorpList.SetItemText(29, 1, ss.str().c_str());

	ss.str("");
	ss << CKeyFrameDlg::s_Inst.m_pTrack->m_TexAddrMode;	
	m_PorpList.InsertItem(30, "Ѱַģʽ");
	m_PorpList.SetItemText(30, 1, ss.str().c_str());		

	if(CActionEdit::s_Inst.m_pAct)
			CActionEdit::s_Inst.m_pAct->UpdateTrackMagicPos();

}


void CFramePropty::OnHdnItemdblclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}

void CFramePropty::OnClose()
{
	CDialog::OnClose();
}

void CFramePropty::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

}


void CFramePropty::OnLvnKeydownList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	
}
