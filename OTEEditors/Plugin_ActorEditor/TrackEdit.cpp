// TrackEdit.cpp : 实现文件
//
#include "stdafx.h"
#include "Plugin_ActorEditor.h"
#include "TrackEdit.h"
#include "ActorEditorMainDlg.h"
#include "ActionEdit.h"
#include "BrowserBone.h"
#include "TransformManager.h"
#include "OTEMathLib.h"


CTrackEdit CTrackEdit::s_Inst;
// CTrackEdit 对话框

IMPLEMENT_DYNAMIC(CTrackEdit, CDialog)
CTrackEdit::CTrackEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CTrackEdit::IDD, pParent)
{
	m_pMManager = OTE::CMagicManager::getManager();
	m_pTManager = OTE::CTrackManager::getManager();
	m_pTrack = NULL;
	m_pMover = NULL;
	m_TrackID = -1;
	m_bPush = false;
	m_pPathView = NULL;
}

CTrackEdit::~CTrackEdit()
{
}

void CTrackEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TimeSlider, m_Slider);
	DDX_Control(pDX, IDC_EditKey, m_BtnEdit1);
	DDX_Control(pDX, IDC_EditKey2, m_BtnEdit2);
}

//销毁自己
void CTrackEdit::DestoryMe()
{
	CActorEditorMainDlg::gTransMgr.HideAllDummies();

	if(m_pTrack != NULL)
	{
		m_pTManager->Delete(m_pTrack->m_nID);
		m_pTrack = NULL;
	}
	if(m_pMover != NULL)
	{
		m_pMManager->Delete(m_pMover->getName().c_str());
		m_pMover = NULL;
	}
	m_TrackID = -1;
	m_bPush = false;

	if(m_pPathView)
	{
		delete m_pPathView;
		m_pPathView = NULL;
	}

	DestroyWindow();
}

//更新数据
void CTrackEdit::UpdateData(const int TrackId)
{
	CActorEditorMainDlg::gTransMgr.HideAllDummies();

	if(CActorEditorMainDlg::g_pActorEntity == NULL || CActionEdit::s_Inst.m_pAct == NULL)
		return;

	m_TrackID = TrackId;
	if(m_pTrack != NULL)
		m_pTManager->Delete(m_pTrack->m_nID);
	if(m_pMover != NULL)
		m_pMManager->Delete(m_pMover->getName().c_str());

	m_pTrack = m_pTManager->getTrack(TrackId);
	//m_pMover = m_pMManager->GetMagic(MagFile, "TrackEditTester");
	m_pMover = m_pMManager->GetMagic("辅助物体.obj", "TrackEditTester");
	if(m_pMover != NULL)
	{
		m_pMover->getParentSceneNode()->setPosition(CActorEditorMainDlg::g_pActorEntity->getParentSceneNode()->getPosition());
		m_pMover->getParentSceneNode()->setOrientation(CActorEditorMainDlg::g_pActorEntity->getParentSceneNode()->getOrientation());
		m_pMover->getParentSceneNode()->setScale(CActorEditorMainDlg::g_pActorEntity->getParentSceneNode()->getScale());
	}

	if((m_pTrack->m_AnimMaxTime - m_pTrack->m_AnimMinTime) < 0.001f)
	{
		m_pTrack->m_AnimMinTime = 0.0f;
		m_pTrack->m_AnimMaxTime = CActionEdit::s_Inst.m_pAct->mLength;
	}
	m_Slider.m_MinAnimTime = m_pTrack->m_AnimMinTime;
	m_Slider.m_MaxAnimTime = m_pTrack->m_AnimMaxTime;

	UpdataSlider();
}

//更新Slider
void CTrackEdit::UpdataSlider()
{
	if(m_pTrack == NULL || m_pMover == NULL)
		return;

	CActionEdit::s_Inst.m_pAct->UpdataMagicPos();
	m_Slider.ClearKeyFrames();

	std::list<OTE::CActKeyFrame*> kl;
	m_pTrack->GetKeyFrameList(&kl);

	std::list<OTE::CActKeyFrame*>::iterator it = kl.begin();

	while(it != kl.end())
	{
		OTE::CActKeyFrame * k = *it;
		if(k)
		{
			KeyFrame_t kf;
			kf.time = k->mTime;
			m_Slider.AddKeyFrame(kf);
		}
		++ it;
	}
	
	m_Slider.Invalidate(); 
	m_Slider.SetFocus();

	UpdateTrackPath();
}

//更新轨道路径
void CTrackEdit::UpdateTrackPath()
{
	if(!m_pPathView)
		m_pPathView = new C3DPathView();

	m_pPathView->ClearNodes();

	std::list<OTE::CActKeyFrame*> kl;
	m_pTrack->GetKeyFrameList(&kl);

	std::list<OTE::CActKeyFrame*>::iterator it = kl.begin();

	while(it != kl.end())
	{
		OTE::CActKeyFrame * k = *it;
		if(k)
		{
			m_pPathView->AddPathNode(k->m_Position);
		}
		++ it;
	}	

	if(kl.size() > 0)
		m_pPathView->Update();
}

BEGIN_MESSAGE_MAP(CTrackEdit, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_Save, OnBnClickedSave)
	ON_BN_CLICKED(IDC_EditKey, OnBnClickedEditKey)
	ON_BN_CLICKED(IDC_Clear, OnBnClickedClear)
	ON_BN_CLICKED(IDC_Erase, OnBnClickedErase)
	ON_BN_CLICKED(IDC_EditKey2, OnBnClickedEditkey2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_TimeSlider, OnNMCustomdrawTimeslider)
END_MESSAGE_MAP()

// CTrackEdit 消息处理程序
//拖动进度条
void CTrackEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(CActorEditorMainDlg::g_pActorEntity == NULL || CActionEdit::s_Inst.m_pAct == NULL)
		return;

	// CActionEdit::s_Inst.m_pAct->stop();

	m_Slider.SetPos(m_Slider.GetPos());

	CActionEdit::s_Inst.m_pAct->MoveFrameTo(m_Slider.GetCurSliderTime());

	OTE::CActKeyFrame *kf = m_pTrack->GetFrameAt(m_Slider.GetCurSliderTime());
	if(kf != NULL)
	{
		Ogre::Vector3 Post = Ogre::Vector3::ZERO;
		Ogre::Vector3 Scale = Ogre::Vector3::UNIT_SCALE;
		Ogre::Quaternion Quat = Ogre::Quaternion::IDENTITY;

		if(m_pTrack->m_AttBoneName.empty())
		{
			Post = kf->m_Position + CActorEditorMainDlg::g_pActorEntity->getParentNode()->getWorldPosition();
			
			Quat = kf->m_Quaternion * CActorEditorMainDlg::g_pActorEntity->getParentNode()->getWorldOrientation();

            Scale = kf->m_Scaling * CActorEditorMainDlg::g_pActorEntity->getParentNode()->getScale();
		}
		else
		{
			Ogre::Node *nd = CActorEditorMainDlg::g_pActorEntity->getSkeleton()->getBone(m_pTrack->m_AttBoneName);
			
			Post = kf->m_Position + nd->getWorldPosition();
			
			Quat = kf->m_Quaternion * nd->getWorldOrientation();

            Scale = kf->m_Scaling * nd->getScale();
		}

		m_pMover->setVisible(true);
		m_pMover->getParentNode()->setPosition(Post);
		m_pMover->getParentNode()->setOrientation(Quat);
		m_pMover->getParentNode()->setScale(Scale);
	}
	else
	{
		m_pMover->setVisible(false);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

// ----------------------------------------------
//保存轨迹
void CTrackEdit::OnBnClickedSave()
{
	CActorEditorMainDlg::gTransMgr.HideAllDummies();

	if(m_TrackID > 0 && m_pTrack != NULL)
	{
		m_pTManager->SaveTrack(m_TrackID);
		m_pTManager->SaveAs();
	}
}
// ----------------------------------------------
//绑定骨骼
void CTrackEdit::OnBnClickedClear()
{
	CActorEditorMainDlg::gTransMgr.HideAllDummies();

	if(!CActorEditorMainDlg::g_pActorEntity || !CActorEditorMainDlg::g_pActorEntity->hasSkeleton())
		return;

	if(m_pTrack!= NULL && CBrowserBone::s_Inst.m_hWnd)
	{
		CBrowserBone::s_Inst.isAddHook = false;
        CBrowserBone::s_Inst.isAddTrack = true;
		CBrowserBone::s_Inst.ShowWindow(SW_SHOW);
		CBrowserBone::s_Inst.UpdataBoneList();
	}
}
// ----------------------------------------------
//删除关键帧
void CTrackEdit::OnBnClickedErase()
{
	CActorEditorMainDlg::gTransMgr.HideAllDummies();

	if(CActorEditorMainDlg::g_pActorEntity == NULL || CActionEdit::s_Inst.m_pAct == NULL || m_pTrack == NULL)
		return;

	float stime = m_Slider.getFoucsKeyFrameTime();
	if(stime >= 0.0f)
	{
		m_pTrack->RemoveKeyFrameAt(stime);
		UpdataSlider();
	}
}
// ----------------------------------------------
//编辑关键帧1
void CTrackEdit::OnBnClickedEditKey()
{
	CActorEditorMainDlg::gTransMgr.HideAllDummies();

	if(CActorEditorMainDlg::g_pActorEntity == NULL || CActionEdit::s_Inst.m_pAct == NULL || m_pTrack == NULL)
		return;

	m_bPush = true;
	m_BtnEdit2.SetState(true);
	m_BtnEdit1.ShowWindow(SW_HIDE);
	m_BtnEdit2.ShowWindow(SW_SHOW);
}
// ----------------------------------------------
//编辑关键帧2
void CTrackEdit::OnBnClickedEditkey2()
{
	CActorEditorMainDlg::gTransMgr.HideAllDummies();

	if(CActorEditorMainDlg::g_pActorEntity == NULL || CActionEdit::s_Inst.m_pAct == NULL || m_pTrack == NULL)
		return;

	m_bPush = false;
	m_BtnEdit1.ShowWindow(SW_SHOW);
	m_BtnEdit2.ShowWindow(SW_HIDE);
}
// ----------------------------------------------
//自动添加关键帧
void CTrackEdit::AutoAddKey()
{
	if(CActorEditorMainDlg::g_pActorEntity == NULL || CActionEdit::s_Inst.m_pAct == NULL || m_pTrack == NULL)
		return;

	float stime = m_Slider.GetCurSliderTime();
	if(stime >= 0.0f)
	{
		OTE::CActKeyFrame *kf = m_pTrack->GetKeyFrameAt(stime);
		bool isNewCreated = false;
		if(kf == NULL)
		{
			kf = new OTE::CActKeyFrame();
			kf->mTime = stime;
			m_pTrack->AddKeyFrame(kf);
			isNewCreated = true;
		}

		kf->mTime = stime;

		Ogre::Vector3 Post = Ogre::Vector3::ZERO;
		Ogre::Vector3 Scale = Ogre::Vector3::UNIT_SCALE;
		Ogre::Quaternion Quat = Ogre::Quaternion::IDENTITY;

		if(m_pTrack->m_AttBoneName.empty())
		{
			Post = m_pMover->getParentNode()->getWorldPosition() - 
				CActorEditorMainDlg::g_pActorEntity->getParentNode()->getWorldPosition();
			
			Quat = m_pMover->getParentNode()->getWorldOrientation() * 
				CActorEditorMainDlg::g_pActorEntity->getParentNode()->getWorldOrientation().Inverse();

            Scale = m_pMover->getParentNode()->getScale() / 
				CActorEditorMainDlg::g_pActorEntity->getParentNode()->getScale();
		}
		else
		{
			Ogre::Node *nd = CActorEditorMainDlg::g_pActorEntity->getSkeleton()->getBone(m_pTrack->m_AttBoneName);
			
			Post = m_pMover->getParentNode()->getWorldPosition() - nd->getWorldPosition();
			
			Quat = m_pMover->getParentNode()->getWorldOrientation() * nd->getWorldOrientation().Inverse();

            Scale = m_pMover->getParentNode()->getScale() / nd->getScale();
		}

		kf->m_Position = Post;
		kf->m_Quaternion = Quat;
		OTE::MathLib::EulerToQuaternion(kf->m_Angles, kf->m_Quaternion);
		kf->m_Scaling = Scale;
		if(isNewCreated)
			UpdataSlider();		
	}
}
// ----------------------------------------------
//处理鼠标移动
void CTrackEdit::OnMouseMove(int x, int y, bool isShift)
{
	static CPoint oldPos = CPoint(x, y);	
	if(::GetKeyState(VK_LBUTTON) & 0x80)
	{		
		// 移动
		if(CActorEditorMainDlg::gTransMgr.GetEditType() == 
									CTransformManager::ePOSITION && m_pMover != NULL)
		{
			Ogre::Vector3 ov, nv;
			ov = m_pMover->getParentNode()->getWorldPosition();	
			CActorEditorMainDlg::gTransMgr.GetViewportMove(CActorEditorMainDlg::gSceneMgr, ov, ov/*temp*/, Ogre::Vector2(x - oldPos.x, y - oldPos.y), nv);
			m_pMover->getParentNode()->setPosition(nv);

			CActorEditorMainDlg::gTransMgr.SetDummyPos(nv);

			AutoAddKey();		
		
		}
		//旋转
		else if(CActorEditorMainDlg::gTransMgr.GetEditType() == CTransformManager::eROTATION && m_pMover != NULL)
		{
			Ogre::Vector3 ov;
			ov = m_pMover->getParentNode()->getWorldPosition();

			Ogre::Quaternion oq, nq;
			oq =  m_pMover->getParentNode()->getWorldOrientation();
			CActorEditorMainDlg::gTransMgr.GetViewportRotate(CActorEditorMainDlg::gSceneMgr, ov, Ogre::Vector2(x - oldPos.x, y - oldPos.y), oq, nq);
			m_pMover->getParentNode()->setOrientation(nq);		
			
			CActorEditorMainDlg::gTransMgr.SetDummyPos(ov);

			AutoAddKey();		
		}
		//缩放
		else if(CActorEditorMainDlg::gTransMgr.GetEditType() == CTransformManager::eSCALING && m_pMover != NULL)
		{
			Ogre::Vector3 ov;
			Ogre::Vector3 os, ns;

			ov = m_pMover->getParentNode()->getWorldPosition();

			os = m_pMover->getParentNode()->getScale();
			CActorEditorMainDlg::gTransMgr.GetViewportScaling(CActorEditorMainDlg::gSceneMgr, ov, Ogre::Vector2(x - oldPos.x, y - oldPos.y), os, ns);
			
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
					
			CActorEditorMainDlg::gTransMgr.SetDummyPos(ov);

			AutoAddKey();		
		}

		if(CActionEdit::s_Inst.m_pAct)
			CActionEdit::s_Inst.m_pAct->UpdataMagicPos();
	}
	oldPos = CPoint(x, y);


}
// ----------------------------------------------
void CTrackEdit::OnNMCustomdrawTimeslider(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

// -----------------------------------------------------
BOOL CTrackEdit::OnInitDialog()
{
	bool ret = CDialog::OnInitDialog();

	//m_KeyFrameDlg.Create(IDD_KEYFRAMEDLG_DIALOG, NULL);
	//m_KeyFrameDlg.ShowWindow(SW_SHOW);

	return ret;
}	
