#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "KeyFrameDlg.h"

#include "ActorEditorMainDlg.h"
#include "ActionEdit.h"
#include "BrowserBone.h"
#include "TransformManager.h"
#include "CustomGen.h"
#include "FramePropty.h"
#include "TrackPropCtrlDlg.h"
#include "OTEMathLib.h"
#include "ColorDlg.h"
#include "KeyTrackBall.h"
#include "OTETrackXmlLoader.h"
#include "OTEActTrackUsage.h"

using namespace Ogre;
using namespace OTE;

// ===============================================
// CKeyFrameDlg
// ===============================================

// �������

bool g_isRecord = false;
#define TRACKBALL_TRACK// {if(!g_isRecord){std::list<OTE::COTEKeyFrame*> pList;m_pTrack->GetKeyFrameList(&pList);	CKeyTrackBall::s_Inst.PushStack(m_KeyList, pList);g_isRecord = true;}}
#define IS_EDITVALID if( !CActorEditorMainDlg::s_pActorEntity || !CActionEdit::s_Inst.m_pAct || !m_pTrack) return;
// ---------------------------------------------------------
CKeyFrameDlg::CKeyFrameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKeyFrameDlg::IDD, pParent)
{	
	m_CurKey = NULL;
	m_CurTimePos = 0;
	m_MinTimePos = 0;
	m_MaxTimePos = 1.0f;
	m_SelectedTimePos = -1;

	m_pTrack = NULL;
	m_pMover = NULL;
	m_TrackID = -1;
	m_bPush = false;
	m_pPathView = NULL;

	m_MarkSpace = 0.01f;
	m_MarkTextSpace = 0.1f;

	m_MouseStateFlag = 0;

	m_IsEdited = false;
}

void CKeyFrameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_BUTTON2, m_KeyIteratorBtn);
	DDX_Control(pDX, IDC_BUTTON1, m_KeyPreBtn);
	DDX_Control(pDX, IDC_BUTTON3, m_KeyNextBtn);

	DDX_Control(pDX, IDC_CHECK2, m_IsBezier);
}

BEGIN_MESSAGE_MAP(CKeyFrameDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_Erase, OnBnClickedErase)
	ON_BN_CLICKED(IDC_BindBone, OnBnBindBone)
	ON_BN_CLICKED(IDC_Save, OnBnClickedSave)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)	
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_CHECK2, OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButton6)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT2, OnEnChangeEdit2)
END_MESSAGE_MAP()


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ���²���
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void CKeyFrameDlg::UpdateMover(OTE::COTEKeyFrame *kf)
{
	Ogre::Vector3 Post = Ogre::Vector3::ZERO;
	Ogre::Vector3 Scale = Ogre::Vector3::UNIT_SCALE;
	Ogre::Quaternion Quat = Ogre::Quaternion::IDENTITY;
	
	COTEActTrackUsage::BindTrans(CActorEditorMainDlg::s_pActorEntity, m_pTrack, kf, Post, Quat, Scale);	

	m_pMover->setVisible(true);
	m_pMover->getParentNode()->setPosition(Post);
	m_pMover->getParentNode()->setOrientation(Quat);
	m_pMover->getParentNode()->setScale(Scale);

	if(CActorEditorMainDlg::s_TransMgr.IsVisible())
		CActorEditorMainDlg::s_TransMgr.SetDummyPos(
					m_pMover->getParentSceneNode()->getPosition());
}

// ---------------------------------------------------------
// ����ƶ� ����֡ʱ��
void CKeyFrameDlg::UpdateTimePosMove(float timePos)
{
	IS_EDITVALID

	SetCurTimePos(timePos);		

	CActionEdit::s_Inst.m_pAct->MoveFrameTo(timePos);

	OTE::COTEKeyFrame *kf = m_pTrack->GetFrameAt(timePos);
	if(kf)
	{	
		UpdateMover(kf);
	}
	else
	{		
		if(m_pMover)
			m_pMover->setVisible(false);
	}	
}
// ---------------------------------------------------------
//��������
void CKeyFrameDlg::UpdateData(const int TrackId)
{
	if(!CActorEditorMainDlg::s_pActorEntity ||
		!CActionEdit::s_Inst.m_pAct)
		return;
	
	// ����	

	ClearSelection();

	SCENE_MGR->ClearCurFocusObj();

	if(m_pMover)
		MAGIC_MGR->DeleteMagic(m_pMover->getName().c_str(), true);

	// ���¹켣����

	m_pTrack = COTETrackManager::GetInstance()->GetTrack(TrackId, COTEActTrackUsage::GetTrackXmlLoader());

	if(!m_pTrack)
	{
		OTE_MSG("���ID�Ĺ켣������ " << "ID = "  << TrackId, "����")
		return;
	}

	m_pTrack->m_AnimMinTime = 0.0f;
	m_pTrack->m_AnimMaxTime = CActionEdit::s_Inst.m_pAct->m_Length;	

	m_TrackID = TrackId;

	// ������������	
	
	m_pMover = OTE::COTEMagicManager::GetSingleton()->CreateMagic(".BBS", "TrackEditTester");
	((COTEBillBoardSet*)m_pMover)->SetColour(Ogre::ColourValue(0.0f, 0.0f, 0.0f, 0.0f));

	SCENE_MGR->AttachObjToSceneNode(m_pMover);

	UpdateTimePosMove(this->m_CurTimePos);

	
	// ���»�������

	m_MinTimePos = m_pTrack->m_AnimMinTime;
	m_MaxTimePos = m_pTrack->m_AnimMaxTime;
	
	m_MarkSpace = (m_MaxTimePos - m_MinTimePos) / 100.0f;
	m_MarkTextSpace = m_MarkSpace * 10.0f;

	// ���½���Ч�� 

	if(m_pTrack->m_IsBezierCurves)
		m_IsBezier.SetCheck(BST_CHECKED);
	else
		m_IsBezier.SetCheck(BST_UNCHECKED);

	GetDlgItem(IDC_BindBone)->EnableWindow(
		CActorEditorMainDlg::s_pActorEntity->hasSkeleton() &&
		CActionEdit::s_Inst.m_pAct->GetMagByInterID(m_pTrack->m_nID) != CActorEditorMainDlg::s_pActorEntity
		);

	// ����2d/3d ֡��ʾЧ��

	UpdataSlider();

	// �Զ�ѡ��һ��

	SelectKeyFromList(m_CurTimePos - 0.01f, m_CurTimePos + 0.01f);

}

// ---------------------------------------------------
//����Slider

void CKeyFrameDlg::UpdataSlider()
{
	if(!m_pTrack)
		return;	
	
	std::list<OTE::COTEKeyFrame*> kl;
	m_pTrack->GetKeyFrameList(&kl);

	std::list<OTE::COTEKeyFrame*>::iterator it = kl.begin();
	std::vector<Key_t>::iterator klit =  m_KeyList.begin();
	
	while(it != kl.end())
	{
		OTE::COTEKeyFrame* k = *it;
		if(k)
		{
			if(klit == m_KeyList.end())
			{
				m_KeyList.push_back( Key_t(k->m_Time, RGB(255, 255, 0), k) );
				klit =  m_KeyList.end();
			}
			else
			{				
				(*klit) = Key_t(k->m_Time, RGB(255, 255, 0), k);				
				++ klit;
			}	
		}		
		++ it;
	}

	if(m_KeyList.size() > kl.size()) 
		m_KeyList.erase(klit, m_KeyList.end());

	// ����3D�켣��ʾ

	UpdateTrackPath();

}

// ---------------------------------------------------
//���¹��·��

void CKeyFrameDlg::UpdateTrackPath()
{
	if(!m_pMover)
		return;

	if(!m_pPathView)
		m_pPathView = new C3DPathView("Dummy.Track");

	m_pPathView->ClearNodes();

	std::list<OTE::COTEKeyFrame*> kl;
	m_pTrack->GetKeyFrameList(&kl);

	std::list<OTE::COTEKeyFrame*>::iterator it = kl.begin();

	while(it != kl.end())
	{
		const OTE::COTEKeyFrame* k = *it;
		if(k)
		{	
			Ogre::Vector3 Post = Ogre::Vector3::ZERO;
			Ogre::Vector3 Scale = Ogre::Vector3::UNIT_SCALE;
			Ogre::Quaternion Quat = Ogre::Quaternion::IDENTITY;
			
			COTEActTrackUsage::BindTrans(CActorEditorMainDlg::s_pActorEntity, m_pTrack, k, Post, Quat, Scale);	

			m_pPathView->AddPathNode(Post);
		}
		++ it;
	}	

	m_pPathView->Update();

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ����������
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// ����ƶ�

void CKeyFrameDlg::OnMouseMove(UINT nFlags, CPoint point)
{		
	if((m_MouseStateFlag & MOUSESTATE_FLAG_LBSLIDERDOWN) &&
		::GetKeyState(VK_LBUTTON) & 0x80 && 
		(m_MouseButtonDownPos.x - point.x) * (m_MouseButtonDownPos.x - point.x) + (m_MouseButtonDownPos.y - point.y) * (m_MouseButtonDownPos.y - point.y) > 2.0f
		)
	{
		float timePos = WindowPosToTimePos(int(point.x / m_MarkSpace) * m_MarkSpace);			

		if((m_MouseStateFlag & MOUSESTATE_FLAG_KEYITLOCKED) && !(::GetKeyState(VK_CONTROL) & 0x80))
		{	
			UpdateTimePosMove(timePos);
		}

		// �ڻ����� ֡��ʾһ���Ĳ���

		if(m_OperationPaneRect.PtInRect(point))
		{
			// �ƶ�ѡ��֡���ı�����ʱ�����ϵ�λ��

			if(m_CurKey)
			{
				m_CurKey->Time = timePos;
				
				OTE::COTEKeyFrame* k = (OTE::COTEKeyFrame*)m_CurKey->pUserData;
				if(k)			
					k->m_Time = m_CurKey->Time;
				
			}	
			else
			{
				m_CurKey = NULL;
				ClearSelection();	
			}
			
			// �ı���Чʱ��			
			
			if(m_InureTimeLocked && CActionEdit::s_Inst.m_pAct)					
				CActionEdit::s_Inst.m_pAct->m_InureTime = timePos;	
			
			// ˢ����ʾ

			InvalidateRect(&CRect(point.x - 20, m_TimeLinePaneRect.top, point.x + 20, m_TimeLinePaneRect.bottom));
			
		}

	}

	// ����

	if((::GetKeyState(VK_LBUTTON) & 0x80) && (::GetKeyState(VK_SHIFT) & 0x80))
	{		
		if(!(m_MouseStateFlag & MOUSESTATE_FLAG_MOUSELOCKED))
			CloneKey(m_CurKey ? m_CurKey->Time : m_CurTimePos);

		m_MouseStateFlag |= MOUSESTATE_FLAG_MOUSELOCKED;
	}


	CDialog::OnMouseMove(nFlags, point);
}


// ---------------------------------------------------------
// ������

void CKeyFrameDlg::OnLButtonDown(UINT nFlags, CPoint point)
{	
	if(!CActionEdit::s_Inst.m_pAct)
		return;

	// ���״̬ 

	m_MouseStateFlag |= MOUSESTATE_FLAG_LBSLIDERDOWN;

	if(m_KeyIteratorRect.PtInRect(point))
		m_MouseStateFlag |= MOUSESTATE_FLAG_KEYITLOCKED;
	else
		m_MouseStateFlag &= ~MOUSESTATE_FLAG_KEYITLOCKED;
	

	CRect rt = m_TimeLinePaneRect;// rt.bottom = rt.top + 20; // �߶�
	if(rt.PtInRect(point))
	{
		// ֡��ѡ

		float timePos1 = WindowPosToTimePos(point.x - 5);
		float timePos2 = WindowPosToTimePos(point.x + 5);
		
		if(!(::GetKeyState(VK_CONTROL) & 0x80))
		{
			ClearSelection();
		}

		SelectKeyFromList(timePos1, timePos2);			

		if(!(::GetKeyState(VK_CONTROL) & 0x80))
		{
			// ��ǰһ֡����

			if(m_CurKey)
			{
				CActorEditorMainDlg::s_TransMgr.ShowDummyA();

				UpdateTimePosMove(m_CurKey->Time);
					
				// ����������

				if(!CFramePropty::s_Inst.m_hWnd)
					CFramePropty::s_Inst.Create(IDD_FRM_PP, this);
				CFramePropty::s_Inst.UpdateFramePropertyList(this->GetCurrentKey());

				// ������ɫ

				if(CColorDlg::s_Inst.m_hWnd && CColorDlg::s_Inst.IsWindowVisible())
				{
					OTE::COTEKeyFrame *kf = m_pTrack->GetFrameAt(m_CurKey->Time);
					if(kf)			
						CColorDlg::s_Inst.Show(true, kf->m_Colour.getAsABGR());
				}

			}
			else
			{		
				UpdateTimePosMove((timePos1 + timePos2) / 2.0f);
			}
		}

		// ������ʾ

		InvalidateRect(&m_TimeLinePaneRect);

		CActionEdit::s_Inst.m_pAct->UpdateTrackMagicPos();

		UpdateTrackPath();
		
	}
	else
	{
		ClearSelection();
	}
	
	// ��Чʱ�� 

	if(m_InureTimeRect.PtInRect(point))
	{
		m_InureTimeLocked = true;
	}	

	// ��¼���λ��

	m_MouseButtonDownPos = point;
	
	// ������˼�¼״̬(���Լ�����¼)

	g_isRecord = false;

	CDialog::OnLButtonDown(nFlags, point);	
}

// ---------------------------------------------------------
void CKeyFrameDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_MouseStateFlag &= ~MOUSESTATE_FLAG_KEYITLOCKED;
	m_MouseStateFlag &= ~MOUSESTATE_FLAG_MOUSELOCKED;
	
	m_InureTimeLocked = false;

	InvalidateRect(&m_TimeLinePaneRect);
	InvalidateRect(&m_OperationPaneRect);

	CDialog::OnLButtonUp(nFlags, point);
}

// ---------------------------------------------------------
// ����Ҽ�����

void CKeyFrameDlg::OnRButtonDown(UINT nFlags, CPoint point)
{	
	float timePos1 = WindowPosToTimePos(point.x - 5);
	float timePos2 = WindowPosToTimePos(point.x + 5);

	Key_t* key = SelectKeyFromList(timePos1, timePos2);	
	if(!key)
		return;

	// �ӻ���ɾ��һ֡

	float stime = key->Time;
	if(stime >= 0.0f)
	{
		TRACKBALL_TRACK

		std::list<OTE::COTEKeyFrame*> pList;
		m_pTrack->GetKeyFrameList(&pList);
		CKeyTrackBall::s_Inst.PushStack(m_KeyList, pList);

		m_IsEdited = true;
		m_pTrack->RemoveKeyFrameAt(stime);
		RemoveKeyFromList(stime);
		UpdataSlider();
		
		CRect rt = m_OperationPaneRect; rt.left -= 10; rt.bottom += 30; 
		InvalidateRect(&rt);
	}

	CDialog::OnRButtonDown(nFlags, point);
}


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ������������ƶ�����
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void CKeyFrameDlg::OnSceneMouseMove(int x, int y, bool isShift)
{	
	IS_EDITVALID

	static CPoint oldPos = CPoint(x, y);

	if((m_MouseStateFlag & MOUSESTATE_FLAG_KEYITLOCKED)		||
		!(m_MouseStateFlag & MOUSESTATE_FLAG_EDITPREPARED)	|| 		
		(!m_bPush && !m_CurKey) 
		)
	{
		oldPos = CPoint(x, y);
		return;
	}
		
	if(::GetKeyState(VK_LBUTTON) & 0x80)
	{
		Ogre::Vector2 screenMove(x - oldPos.x, y - oldPos.y);

		// �ƶ�
		if(CActorEditorMainDlg::s_TransMgr.GetEditType() == 
									CTransformManager::ePOSITION && m_pMover != NULL)
		{
			TRACKBALL_TRACK

			Ogre::Vector3 ov, nv;
			ov = m_pMover->getParentNode()->getWorldPosition();	

			if(SCENE_CAM->getProjectionType() == Ogre::PT_ORTHOGRAPHIC)
			{
				// ǰ��ͼ
				
				Ogre::Vector3 dMove;
				if((DWORD)SCENE_CAM->getUserObject() & 0xF0000000)
				{
					dMove =	Ogre::Vector3(screenMove.x, - screenMove.y, 0);								
				}

				// ����ͼ

				else if((DWORD)SCENE_CAM->getUserObject() & 0x0F000000)
				{
					dMove =	Ogre::Vector3(screenMove.x, 0, screenMove.y);							
				}

				// ����ͼ

				else if((DWORD)SCENE_CAM->getUserObject() & 0x00F00000)
				{
					dMove =	Ogre::Vector3(0, - screenMove.y, - screenMove.x);					
				}	

				dMove /= 300.0f;

				nv = ov + dMove;
			}
			else
			{				
				CActorEditorMainDlg::s_TransMgr.GetViewportMove(ov, ov/*temp*/, Ogre::Vector2(x - oldPos.x, y - oldPos.y), nv);
			}			
			
			m_pMover->getParentNode()->setPosition(nv);

			CActorEditorMainDlg::s_TransMgr.SetDummyPos(nv);
			
			if(m_SelKeys.size() > 0)
			{
				std::list<Key_t*>::iterator it = m_SelKeys.begin();
				while( it != m_SelKeys.end() )
				{					
					OTE::COTEKeyFrame *kf1 = m_pTrack->GetKeyFrameAt((*it)->Time);
					OTE::COTEKeyFrame *kf2 = m_pTrack->GetKeyFrameAt(m_CurKey->Time);	

					if(kf1 && kf2)
						SetKey((*it)->Time, CKeyFrameDlg::eTrans, kf1->m_Position - kf2->m_Position, kf1->m_Angles, false);	
			

					++ it;
				}
			}
			else
			{
				SetKey(m_CurKey ? m_CurKey->Time : m_CurTimePos, CKeyFrameDlg::eTrans, Ogre::Vector3::ZERO, Ogre::Vector3::ZERO, false);	
			}		
		}

		//��ת

		else if(CActorEditorMainDlg::s_TransMgr.GetEditType() == CTransformManager::eROTATION && m_pMover != NULL)
		{	
			TRACKBALL_TRACK

			if(SCENE_CAM->getProjectionType() == Ogre::PT_ORTHOGRAPHIC)
			{
				
				if(m_CurKey)
				{
					OTE::COTEKeyFrame *kf = m_pTrack->GetKeyFrameAt(m_CurKey->Time);
					if(kf)
					{
						// ǰ��ͼ						
					
						if((DWORD)SCENE_CAM->getUserObject() & 0xF0000000)
						{
							kf->m_Angles.z += screenMove.y;														
						}

						// ����ͼ

						else if((DWORD)SCENE_CAM->getUserObject() & 0x0F000000)
						{
							kf->m_Angles.y += screenMove.y;						
						}

						// ����ͼ

						else if((DWORD)SCENE_CAM->getUserObject() & 0x00F00000)
						{
							kf->m_Angles.x += screenMove.y;						
						}

						OTE::MathLib::EulerToQuaternion(kf->m_Angles, kf->m_Quaternion);
					}
				}
			
			}
			else

			// ���淽��

			{
				Ogre::Vector3 ov;
				ov = m_pMover->getParentNode()->getWorldPosition();

				Ogre::Vector3 angles;
				Ogre::Quaternion oq, nq;
				oq =  m_pMover->getParentNode()->getWorldOrientation();

				CActorEditorMainDlg::s_TransMgr.GetViewportRotate(ov, Ogre::Vector2(x - oldPos.x, y - oldPos.y), oq, nq, &angles);
			
				m_pMover->getParentNode()->setOrientation(nq);	

				CActorEditorMainDlg::s_TransMgr.SetDummyPos(ov);
			
				SetKey(m_CurKey ? m_CurKey->Time : m_CurTimePos, CKeyFrameDlg::eRot, Ogre::Vector3::ZERO, angles, false);

			}

		}

		//����

		else if(CActorEditorMainDlg::s_TransMgr.GetEditType() == CTransformManager::eSCALING && m_pMover != NULL)
		{
			TRACKBALL_TRACK

			Ogre::Vector3 ov;
			Ogre::Vector3 os, ns;

			ov = m_pMover->getParentNode()->getWorldPosition();

			os = m_pMover->getParentNode()->getScale();
			CActorEditorMainDlg::s_TransMgr.GetViewportScaling(ov, Ogre::Vector2(x - oldPos.x, y - oldPos.y), os, ns);
			
			if(isShift)
			{
				float x = (ns.x - os.x) / os.x;
				float y = (ns.y - os.y) / os.y;
				float z = (ns.z - os.z) / os.z;
				
				float f = abs(x)>abs(y) ? x:y;
				f = abs(f)>abs(z) ? f:z;

				ns.x = os.x * (1.0f + f);
				ns.y = os.y * (1.0f + f);
				ns.z = os.z * (1.0f + f);
			}
				
			m_pMover->getParentNode()->setScale(ns);
					
			CActorEditorMainDlg::s_TransMgr.SetDummyPos(ov);
			
			SetKey(m_CurKey ? m_CurKey->Time : m_CurTimePos, CKeyFrameDlg::eScl, Ogre::Vector3::ZERO, Ogre::Vector3::ZERO, false);		
					
		}

		if(CActionEdit::s_Inst.m_pAct)
			CActionEdit::s_Inst.m_pAct->UpdateTrackMagicPos();
	}

	oldPos = CPoint(x, y);
}

// ---------------------------------------------------
//�������̧�� 
void CKeyFrameDlg::OnSceneLButtonUp(int x, int y)
{
	m_MouseStateFlag &= ~MOUSESTATE_FLAG_EDITPREPARED;
	m_MouseStateFlag &= ~MOUSESTATE_FLAG_LBSLIDERDOWN;
	InvalidateRect(&m_TimeLinePaneRect);
}

// ---------------------------------------------------
void CKeyFrameDlg::OnSceneLButtonDown(int x, int y)
{	
	m_MouseStateFlag |= MOUSESTATE_FLAG_EDITPREPARED;
	m_MouseStateFlag &= ~MOUSESTATE_FLAG_LBSLIDERDOWN;

	g_isRecord = false;	
}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ��������
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// ������Ⱦ�ص�

void OnKeyFrameDlgSceneRender(int ud)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	static bool s_isKeyDown = false;
	if(CKeyFrameDlg::s_Inst.m_hWnd && CKeyFrameDlg::s_Inst.IsWindowVisible())
	{
		if(::GetKeyState(VK_CONTROL) & 0x80 && ::GetKeyState('Z') & 0x80)
		{
			if(!s_isKeyDown)
			{
				std::list<OTE::COTEKeyFrame*> pList;
				if(CKeyTrackBall::s_Inst.PopStack(CKeyFrameDlg::s_Inst.m_KeyList, pList))
				{
					CKeyFrameDlg::s_Inst.m_pTrack->SetKeyFrameList(&pList);
					CKeyFrameDlg::s_Inst.UpdateData(CKeyFrameDlg::s_Inst.m_pTrack->m_nID);

					CRect rt = CKeyFrameDlg::s_Inst.m_OperationPaneRect;
					CKeyFrameDlg::s_Inst.InvalidateRect(&rt);

					if(CActionEdit::s_Inst.m_pAct)
						CActionEdit::s_Inst.m_pAct->UpdateTrackMagicPos();

					if( CFramePropty::s_Inst.m_hWnd && 
						CFramePropty::s_Inst.IsWindowVisible() &&
						CKeyFrameDlg::s_Inst.GetCurrentKey() )
					{
						CFramePropty::s_Inst.UpdateFramePropertyList(CKeyFrameDlg::s_Inst.GetCurrentKey());
					}

				}
			}
			s_isKeyDown = true;
		}
		else
		{
			s_isKeyDown = false;
		}
	}

}

// ---------------------------------------------------------
// CKeyFrameDlg ��Ϣ�������
CKeyFrameDlg CKeyFrameDlg::s_Inst;

BOOL CKeyFrameDlg::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();
	
	Reset();

	SCENE_MGR->AddListener(COTEQTSceneManager::eRenderCB, OnKeyFrameDlgSceneRender);

	return ret;
}

// ---------------------------------------------------------
void CKeyFrameDlg::Reset()
{
	// ��ʼλ��
	
	CRect rect;
	GetClientRect(rect);

	m_KeyIteratorRect.SetRect(0, 0, 120, 20);
	m_TimeLinePaneRect.SetRect(rect.left + 10, rect.top + 20, rect.right - 10, rect.top + 70 );
	m_OperationPaneRect.SetRect(rect.left, rect.top, rect.right, rect.top + 70 );

	SetCurTimePos(0.0f);
}

// ---------------------------------------------------------
void CKeyFrameDlg::OnPaint() 
{
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������

	CRect   rect;  
	GetClientRect(rect);

	if(!m_MemDC.m_hDC)
	{
		m_MemDC.CreateCompatibleDC((CDC*)&dc); 		
		m_MemBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
		m_MemDC.SelectObject(&m_MemBitmap);
	}

	m_MemDC.FillSolidRect(0, 0, rect.Width(), rect.Height(), RGB(55, 70, 70));   

	DrawKeyIterator(&m_MemDC);
	
	DrawKeyMarkList(&m_MemDC);

	DrawKeyList(&m_MemDC); 

	if(CActionEdit::s_Inst.m_pAct)
		DrawInureTimeMark(&m_MemDC, 
			CActionEdit::s_Inst.m_pAct->m_InureTime, RGB(55, 255, 55));

	dc.BitBlt(0, 0, rect.right, rect.bottom, &m_MemDC, 0, 0, SRCCOPY);   

	CDialog::OnPaint();

}

// ---------------------------------------------------------
//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��

HCURSOR CKeyFrameDlg::OnQueryDragIcon()
{
	return NULL;
}

// ---------------------------------------------------------
// ��ǰ

void CKeyFrameDlg::OnBnClickedButton3()
{
	m_CurTimePos += m_MarkSpace;
	
	m_CurTimePos = m_CurTimePos > m_MaxTimePos ? m_MaxTimePos : m_CurTimePos;

	SetCurTimePos(m_CurTimePos);

	InvalidateRect(NULL);

}

// ---------------------------------------------------------
// ���

void CKeyFrameDlg::OnBnClickedButton1()
{
	m_CurTimePos -= m_MarkSpace;

	m_CurTimePos = m_CurTimePos < m_MinTimePos ? m_MinTimePos : m_CurTimePos;	

	SetCurTimePos(m_CurTimePos);

	InvalidateRect(NULL);
}

// ---------------------------------------------------------
// �Զ��崴�� 

void CKeyFrameDlg::OnBnClickedButton2()
{
	if(!CCustomGen::s_Inst.m_hWnd)
		CCustomGen::s_Inst.Create(IDD_CUSTOMCREATE, NULL);
	CCustomGen::s_Inst.ShowWindow(SW_SHOW);
}

// ---------------------------------------------------------
// ���õ�ǰʱ��

void CKeyFrameDlg::SetCurTimePos(float time)
{
	time = time < m_MinTimePos ? m_MinTimePos : time;
	time = time > m_MaxTimePos ? m_MaxTimePos : time;

	m_CurTimePos = time;

	SetKeyBtnsPos(time);
	
	UpdateKeyBtnPosTxt();
}

// ---------------------------------------------------------
// �����α�λ��

void CKeyFrameDlg::SetKeyBtnsPos(float time)
{
	CPoint pos = TimePosToWindowPos(time);

	CRect rect;	
	
	int KeyIteratorWidth = m_KeyIteratorRect.Width();
	m_KeyIteratorRect.SetRect(pos.x - KeyIteratorWidth / 2, pos.y - 30, 
		pos.x - KeyIteratorWidth / 2 + KeyIteratorWidth, pos.y);
	
	m_KeyPreBtn.GetWindowRect(rect);	
	m_KeyPreBtn.MoveWindow(pos.x - rect.Width() - KeyIteratorWidth / 2, pos.y - 30, rect.Width(), 30);

	m_KeyNextBtn.GetWindowRect(rect);	
	m_KeyNextBtn.MoveWindow(pos.x + KeyIteratorWidth / 2, pos.y - 30, rect.Width(), 30);

}

// ---------------------------------------------------------
void CKeyFrameDlg::DrawRect(CDC* hdc, const CRect& rect, COLORREF cor, bool isFill)
{	    
	if(isFill)
	{
		CBrush brush(cor);
		hdc->FillRect(rect, &brush);
	}
	else
	{
		hdc->Draw3dRect(rect, cor, cor);
	}
}


// ---------------------------------------------------------
void CKeyFrameDlg::DrawKey(CDC* hdc, float time, COLORREF cor)
{	
	CPoint pos = TimePosToWindowPos(time);

	DrawRect(hdc, CRect(pos.x - 3, pos.y, pos.x + 3/*���/2*/, pos.y + 20 /*�߶�*/), cor, true);
	
}
// ---------------------------------------------------------
void CKeyFrameDlg::DrawEndMark(CDC* hdc, float time, COLORREF cor)
{	
	CPoint pos = TimePosToWindowPos(time);

	DrawRect(hdc, CRect(pos.x - 3, pos.y, pos.x + 3, pos.y + 50), cor, true);
	
}
// ---------------------------------------------------------
void CKeyFrameDlg::DrawInureTimeMark(CDC* hdc, float time, COLORREF cor)
{	
	CPoint pos = TimePosToWindowPos(time);
	m_InureTimeRect = CRect(pos.x - 3, pos.y + 35, pos.x + 3, m_TimeLinePaneRect.bottom);
	DrawRect(hdc, m_InureTimeRect, cor, true);
	
}

// ---------------------------------------------------------
void CKeyFrameDlg::DrawKeyMark(CDC* hdc, float time, bool showText)
{	
	CPoint pos = TimePosToWindowPos(time);	
	
	hdc->MoveTo(pos);

	if(!showText)
		hdc->LineTo(pos.x, pos.y + 10);  

	if(showText)
	{
		hdc->LineTo(pos.x, pos.y + 30);  

		char buff[32];
		sprintf(buff, "%.2f", time);

		SetBkMode (hdc->m_hDC, TRANSPARENT);	
		hdc->TextOut(pos.x - 10, pos.y + 30, buff);
	}
	
}

// ---------------------------------------------------------
CPoint CKeyFrameDlg::TimePosToWindowPos(float time)
{
	float percent = (time - m_MinTimePos) / (m_MaxTimePos - m_MinTimePos);

	return CPoint(m_TimeLinePaneRect.Width() * percent + m_TimeLinePaneRect.left,	m_TimeLinePaneRect.top);

}

// ---------------------------------------------------------
float CKeyFrameDlg::WindowPosToTimePos(int winPos)
{
	float timePos = float(winPos - m_TimeLinePaneRect.left) / m_TimeLinePaneRect.Width() * (m_MaxTimePos - m_MinTimePos); 

	timePos < 0 ? timePos = 0 : NULL;
	timePos > m_MaxTimePos ? timePos = m_MaxTimePos : NULL;	

	return timePos;
}

// ---------------------------------------------------------
// �����α�

void CKeyFrameDlg::DrawKeyIterator(CDC* hdc)
{
	DrawRect(hdc, m_TimeLinePaneRect, RGB(50, 100, 100));
		
	DrawRect(hdc, m_KeyIteratorRect, RGB(100, 50, 50));
	
	CPoint pos = TimePosToWindowPos(m_CurTimePos);

	DrawRect(hdc, 
		CRect(pos.x - 5, m_KeyIteratorRect.bottom, pos.x + 5, m_KeyIteratorRect.bottom + 30), 
		RGB(0, 0, 255), false);

}

// ---------------------------------------------------------
// �������б�

void CKeyFrameDlg::DrawKeyList(CDC* hdc)
{
	std::vector<Key_t>::iterator it = m_KeyList.begin();
	while(it != m_KeyList.end())
	{
		if((*it).state == 1)
		{
			DrawKey(hdc, (*it).Time, RGB(255, 0, 0));
		}
		else
		{			
			DrawKey(hdc, (*it).Time, (*it).Cor);
		}

		++ it;
	}
}

// ---------------------------------------------------------
// ���ƿ̶��б�

void CKeyFrameDlg::DrawKeyMarkList(CDC* hdc)
{	
	static HPEN pen = CreatePen (PS_SOLID, 0, RGB(20, 20, 20)); 
	hdc->SelectObject(pen);

	int totalCnt = (m_MaxTimePos - m_MinTimePos) / m_MarkSpace;
	float timePos = m_MinTimePos;

	for(int count = 0; count <= totalCnt; count ++)
	{		
		if(count % 10  == 0)
		{
			DrawKeyMark(hdc, timePos, true);		
		}
		else
		{			
			DrawKeyMark(hdc, timePos);
		}

		timePos += m_MarkSpace;
	}	

}

// ---------------------------------------------------------
// ���̲���

void CKeyFrameDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//if(nChar == VK_DELETE)
	//{
	//	RemoveKeyFromList(m_SelectedTimePos);
	//}

	CDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}


// ---------------------------------------------------------
void CKeyFrameDlg::OnDestroy()
{
	CDialog::OnDestroy();

	if(m_MemDC.m_hDC)
	{
		m_MemBitmap.DeleteObject();
		m_MemDC.DeleteDC();	
	}	

	DestoryMe(false);
}

// ---------------------------------------------------------
void CKeyFrameDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	m_TimeLinePaneRect.SetRect(20, 0 + 20, cx - 20, cy);
}

// ---------------------------------------------------------
// ��ǰ֡ʱ��
void CKeyFrameDlg::OnEnChangeEdit1()
{
	CString str;
	this->GetDlgItemText(IDC_EDIT1, str);
	SetCurTimePos(atof( (char*)LPCTSTR(str) ));

	InvalidateRect(NULL);
}

// ---------------------------------------------------------
void CKeyFrameDlg::UpdateKeyBtnPosTxt()
{return; // temp!
	char buff[32];
	sprintf(buff, "%f", m_CurTimePos);
	this->SetDlgItemText(IDC_EDIT1, buff);
}

// ---------------------------------------------------------
// ɾ���ؼ���
void CKeyFrameDlg::OnBnClickedErase()
{
	CActorEditorMainDlg::s_TransMgr.HideAllDummies();

	IS_EDITVALID

	float stime = m_CurTimePos;
	if(m_CurKey)
		stime = m_CurKey->Time;

	if(stime >= 0.0f)
	{		
		m_pTrack->RemoveKeyFrameAt(stime);
		RemoveKeyFromList(stime);

		UpdataSlider();		
		InvalidateRect(&m_TimeLinePaneRect);

		m_IsEdited = true;
	}
}

// ---------------------------------------------------------
// ��������
void CKeyFrameDlg::OnBnBindBone()
{
	if(!CActorEditorMainDlg::s_pActorEntity || 
		!CActorEditorMainDlg::s_pActorEntity->hasSkeleton())
		return;
	
	CActorEditorMainDlg::s_TransMgr.HideAllDummies();

	if(m_pTrack!= NULL && CBrowserBone::s_Inst.m_hWnd)
	{
		CBrowserBone::s_Inst.isAddHook = false;
        CBrowserBone::s_Inst.isAddTrack = true;
		CBrowserBone::s_Inst.ShowWindow(SW_SHOW);
		CBrowserBone::s_Inst.UpdataBoneList();
		
		m_IsEdited = true;
	}
}

// ---------------------------------------------------------
// ����
void CKeyFrameDlg::OnBnClickedSave()
{
	CActorEditorMainDlg::s_TransMgr.HideAllDummies();

	if(m_TrackID > 0 && m_pTrack != NULL)
	{
		COTEActTrackUsage::GetTrackXmlLoader()->TrackWriteXml(m_TrackID);
		COTEActTrackUsage::GetTrackXmlLoader()->SaveFile();

		m_IsEdited = false;
	}
}

// ---------------------------------------------------
//����
void CKeyFrameDlg::DestoryMe(bool sendCloseMsg)
{
	if(m_IsEdited)
	{
		if(m_TrackID > 0 && m_pTrack != NULL &&
			 IDYES == AfxMessageBox("�Ƿ�Ҫ�����޸ģ�", MB_YESNO, 0))
		{			
			COTEActTrackUsage::GetTrackXmlLoader()->TrackWriteXml(m_TrackID);
			COTEActTrackUsage::GetTrackXmlLoader()->SaveFile();		
		}
	}

	// ���� 

	SCENE_MGR->ClearCurFocusObj();

	CActorEditorMainDlg::s_TransMgr.HideAllDummies();

	if(m_pMover != NULL)
	{
		MAGIC_MGR->DeleteMagic(m_pMover->getName().c_str());
		m_pMover = NULL;
	}	
	if(m_pPathView)
	{
		delete m_pPathView;
		m_pPathView = NULL;
	}	

	m_pTrack = NULL;	
	m_TrackID = -1;

	m_bPush = false;		

	// ������ش���

	if(CFramePropty::s_Inst.m_hWnd)
	{
		CFramePropty::s_Inst.ShowWindow(SW_HIDE);
	}

	if(m_hWnd && sendCloseMsg)
		this->SendMessage(WM_CLOSE);
}

// ---------------------------------------------------
// ����/�Զ���ӹؼ�֡

void CKeyFrameDlg::SetKey(float stime, 
							  eTRS_TYPE tsrType, 
							  const Ogre::Vector3& dOffset, 
							  const Ogre::Vector3& angles, 
							  bool m_bAutoAdd)
{
	IS_EDITVALID
	
	if(stime >= 0.0f)
	{
		OTE::COTEKeyFrame *kf = m_pTrack->GetKeyFrameAt(stime);		
		
		if(!kf)
		{	
			// �Զ����� 

			if(!m_bAutoAdd)
				return;
			
			// ���׼��

			g_isRecord = false;	TRACKBALL_TRACK

			kf = COTEKeyFrame::CreateNewKeyFrame();	
			OTE::COTEKeyFrame *f = m_pTrack->GetFrameAt(stime);				
			if(f)
				*kf = *f;		

			kf->m_Time = stime;
			m_pTrack->AddKeyFrame(kf);	
		}	
		else
		{		
			// ����֡ 

			kf->m_Time = stime;

			Ogre::Vector3 Post = Ogre::Vector3::ZERO;
			Ogre::Vector3 Scale = Ogre::Vector3::UNIT_SCALE;
			Ogre::Quaternion Quat = Ogre::Quaternion::IDENTITY;
			Ogre::Vector3 Angles = angles;
			
			COTEActTrackUsage::GetKeyFrameDTrans(
						CActorEditorMainDlg::s_pActorEntity, 
						m_pMover->getParentSceneNode(),
						m_pTrack, Post, Quat, Scale);		

			if(tsrType == eAll || tsrType == eTrans)
			{
				kf->m_Position		= Post + dOffset;			
			}
			if(tsrType == eAll || tsrType == eRot)
			{	
				if(tsrType == eRot)
				{
					kf->m_Quaternion = Quat;
					OTE::MathLib::QuaternionToEuler(Quat, kf->m_Angles);					
				}
				else
				{
					// todo
				}
			}
			if(tsrType == eAll || tsrType == eScl)
			{
				kf->m_Scaling = Scale;				
			}
		}
		
		// ������ʾ

		UpdataSlider();		

		m_IsEdited = true;
	}
}

// ---------------------------------------------------
//clone�ؼ�֡
void CKeyFrameDlg::CloneKey(float stime)
{
	SetKey(stime + 0.02f);	
	OTE::COTEKeyFrame *kf = m_pTrack->GetKeyFrameAt(stime + 0.02f);
	OTE::COTEKeyFrame *kf0 = m_pTrack->GetKeyFrameAt(stime);
	if(kf && kf0)
	{
		memcpy(kf, kf0, sizeof(OTE::COTEKeyFrame));	
		SelectKeyFromList(stime + 0.02f, stime + 0.02f);
	}

	m_IsEdited = true;
}

// ---------------------------------------------------
//�Զ���ӹؼ�֡
void CKeyFrameDlg::SetKey(float stime, 
						  const Ogre::Vector3& pos, const Ogre::Vector3& scl, const Ogre::Vector3& angles)
{
	IS_EDITVALID	

	if(stime >= 0.0f)
	{
		OTE::COTEKeyFrame *kf = m_pTrack->GetKeyFrameAt(stime);
		if(!kf)
		{
			kf = COTEKeyFrame::CreateNewKeyFrame();
			kf->m_Time = stime;
			m_pTrack->AddKeyFrame(kf);
		}

		kf->m_Time = stime;	

		kf->m_Position		= pos;
		kf->m_Angles	    = angles;
		OTE::MathLib::EulerToQuaternion(kf->m_Angles, kf->m_Quaternion);
		//kf->m_Quaternion	= q;
		kf->m_Scaling		= scl;

		UpdataSlider();	

		m_IsEdited = true;
	}
}

// ---------------------------------------------------
OTE::COTEKeyFrame* CKeyFrameDlg::GetKey(float stime)
{
	if(!m_pTrack)
		return NULL;

	return  m_pTrack->GetKeyFrameAt(stime);
	
}
// ---------------------------------------------------
OTE::COTEKeyFrame* CKeyFrameDlg::GetCurrentKey()
{
	if(m_CurKey)
		return GetKey(m_CurKey->Time);
	else
		return GetKey(GetCurTimePos());		
}

// ---------------------------------------------------
void CKeyFrameDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);	
}

// ---------------------------------------------------
// �������Ա༭

void CKeyFrameDlg::OnBnClickedButton4()
{
	if(!CTrackPropCtrlDlg::s_Inst.m_hWnd)
		CTrackPropCtrlDlg::s_Inst.Create(IDD_TRACKCTRLDLG, NULL);

	CTrackPropCtrlDlg::s_Inst.ShowWindow(SW_SHOW);
}

// ---------------------------------------------------
// ��ӹؼ���

void CKeyFrameDlg::OnBnClickedButton5()
{
	SetKey(m_CurTimePos);	

	SelectKeyFromList(m_CurTimePos - 0.01f, m_CurTimePos + 0.01f);

	UpdateTimePosMove(this->m_CurTimePos);

	InvalidateRect(&m_TimeLinePaneRect);

}

// ---------------------------------------------------
// ˫��

void CKeyFrameDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_CurKey)
	{
		SetCurTimePos(m_CurKey->Time);
	
		// ������

		if(!CFramePropty::s_Inst.m_hWnd)
			CFramePropty::s_Inst.Create(IDD_FRM_PP, this);
		CFramePropty::s_Inst.ShowWindow(SW_SHOW);

		CFramePropty::s_Inst.UpdateFramePropertyList(this->GetCurrentKey());
	}	

	CDialog::OnLButtonDblClk(nFlags, point);
}

// ---------------------------------------------------
// �Ƿ��ñ�������

void CKeyFrameDlg::OnBnClickedCheck2()
{
	m_pTrack->m_IsBezierCurves = (m_IsBezier.GetCheck() == BST_CHECKED);
}

// -------------------------------------------
// ����
void CKeyFrameDlg::OnEndPlugin()
{
	SCENE_MGR->RemoveListener(OnKeyFrameDlgSceneRender);

	m_IsEdited = false;
	DestoryMe();

}

// -------------------------------------------
// �պϹ�����Զ�����ĩ��

void CKeyFrameDlg::OnBnClickedButton6()
{
	IS_EDITVALID	

	COTEKeyFrame* kf = COTEKeyFrame::CreateNewKeyFrame();
	
	COTEKeyFrame* fkf = m_pTrack->GetFirstKey();
	COTEKeyFrame* ekf = m_pTrack->GetEndKey();

	*kf = *fkf;

	float dTime = 0;
	if(m_pTrack->GetFrameNum() > 0)
		dTime = m_pTrack->m_AnimMaxTime / m_pTrack->GetFrameNum();

	kf->m_Time = ekf->m_Time + dTime;
	kf->m_Time > m_pTrack->m_AnimMaxTime ? kf->m_Time = m_pTrack->m_AnimMaxTime : NULL;

	m_pTrack->AddKeyFrame(kf);

	UpdataSlider();	

	InvalidateRect(&m_TimeLinePaneRect);
}

// -------------------------------------
void CKeyFrameDlg::OnTimer(UINT nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}

// -------------------------------------
// �������
// ��ʱ�������ж�ʱ�䳤�ȴ��棬��������ȹ���

void CKeyFrameDlg::OnEnChangeEdit2()
{
	IS_EDITVALID

	CString tText;
	GetDlgItemText(IDC_EDIT2, tText);
	float length = atof(tText.GetString());

	if(length > 0.0f)
	{
		CActionEdit::s_Inst.m_pAct->m_Length = length;
	}
	else
	{
		CActionEdit::s_Inst.m_pAct->m_Length = 2.0f;
	}

	this->UpdateData(this->m_TrackID);
	this->InvalidateRect(NULL); 
}
