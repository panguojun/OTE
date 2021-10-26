#include "stdafx.h"

#   ifdef GAME_DEBUG	
	#include "AppFrame.h"

	#include "OTEQTSceneManager.h"
	#include "OTEOgreManager.h"	
	#include "OTEPluginManager.h" 
	#include "OTETilePage.h"
	
	#include "AppFrameDoc.h"
	#include "AppFrameView.h"

	// --------------------------------------------------------
	#ifdef _DEBUG
	#define new DEBUG_NEW 
	#endif

	// --------------------------------------------------------
	using namespace Ogre;
	using namespace OTE;

	// --------------------------------------------------------
	extern std::vector<COTEPluginInterface*>	g_PIMouseEventListeners;
	extern std::vector<COTEPluginInterface*>	g_PIKeyEventListeners;

	// --------------------------------------------------------
	// CAppFrameView

	IMPLEMENT_DYNCREATE(CAppFrameView, CView)

	BEGIN_MESSAGE_MAP(CAppFrameView, CView)
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONDBLCLK()
		ON_WM_LBUTTONUP()
		ON_WM_MOUSEWHEEL()
		ON_WM_RBUTTONDOWN()
		ON_WM_RBUTTONUP()
		ON_WM_MOUSEMOVE()
		ON_WM_KEYUP()
		ON_WM_KEYDOWN()
	END_MESSAGE_MAP()

	// CAppFrameView ����/����

	CAppFrameView::CAppFrameView()
	{		
		
	}

	CAppFrameView::~CAppFrameView()
	{
	}

	BOOL CAppFrameView::PreCreateWindow(CREATESTRUCT& cs)
	{

		return CView::PreCreateWindow(cs);
	}

	
	// CAppFrameView ����

	void CAppFrameView::OnDraw(CDC* /*pDC*/)
	{
		CAppFrameDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		
	}

	// CAppFrameView ���

	#ifdef _DEBUG
	void CAppFrameView::AssertValid() const
	{
		CView::AssertValid();
	}

	void CAppFrameView::Dump(CDumpContext& dc) const
	{
		CView::Dump(dc);
	}

	CAppFrameDoc* CAppFrameView::GetDocument() const // �ǵ��԰汾��������
	{
		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAppFrameDoc)));
		return (CAppFrameDoc*)m_pDocument;
	}
	#endif //_DEBUG	

	// --------------------------------------------------
	void CAppFrameView::OnLButtonDown( UINT nFlags, CPoint point)
	{
		for(unsigned int i = 0; i < g_PIMouseEventListeners.size(); i++)
		{
			if(g_PIMouseEventListeners.at(i)->OnLButtonDown((HWND)m_hWnd, point.x, point.y, 
					::GetKeyState(VK_CONTROL) & 0x80, ::GetKeyState(VK_MENU) & 0x80, ::GetKeyState(VK_SHIFT) & 0x80))
					break;
		}
		
		CView::OnLButtonDown( nFlags, point);
	}

	// --------------------------------------------------
	void CAppFrameView::OnLButtonDblClk( UINT nFlags, CPoint point)
	{
		for(unsigned int i = 0; i < g_PIMouseEventListeners.size(); i++)
		{
			if(g_PIMouseEventListeners.at(i)->OnLeftDClick((HWND)m_hWnd, point.x, point.y, 
					::GetKeyState(VK_CONTROL) & 0x80, ::GetKeyState(VK_MENU) & 0x80, ::GetKeyState(VK_SHIFT) & 0x80))
					break;
		}
		
		CView::OnLButtonDblClk( nFlags, point);
	}	

	// ------------------------------------------------------------
	void CAppFrameView::OnLButtonUp( UINT nFlags, CPoint point)
	{
		for(unsigned int i = 0; i < g_PIMouseEventListeners.size(); i++)
		{
			if(g_PIMouseEventListeners.at(i)->OnLButtonUp((HWND)m_hWnd, point.x, point.y, 
				::GetKeyState(VK_CONTROL) & 0x80, ::GetKeyState(VK_MENU) & 0x80, ::GetKeyState(VK_SHIFT) & 0x80))
					break;
		
		}
		CView::OnLButtonUp( nFlags, point);
	} 

	// ------------------------------------------------------------
	void CAppFrameView::OnRButtonDown( UINT nFlags, CPoint point)
	{
		for(unsigned int i = 0; i < g_PIMouseEventListeners.size(); i++)	
		{
			if(g_PIMouseEventListeners.at(i)->OnRButtonDown((HWND)m_hWnd, point.x, point.y, 
				::GetKeyState(VK_CONTROL) & 0x80, ::GetKeyState(VK_MENU) & 0x80, ::GetKeyState(VK_SHIFT) & 0x80))
				break;
		}
		CView::OnRButtonDown( nFlags, point);	
	} 

	// ------------------------------------------------------------
	void CAppFrameView::OnRButtonUp( UINT nFlags, CPoint point)
	{
		for(unsigned int i = 0; i < g_PIMouseEventListeners.size(); i++)
		{
			if(g_PIMouseEventListeners.at(i)->OnRButtonUp((HWND)m_hWnd, point.x, point.y, 
				::GetKeyState(VK_CONTROL) & 0x80, ::GetKeyState(VK_MENU) & 0x80, ::GetKeyState(VK_SHIFT) & 0x80))
				break;
		}
		CView::OnRButtonUp( nFlags, point);	
	}

	// ------------------------------------------------------------
	void CAppFrameView::OnMouseMove( UINT nFlags, CPoint point)
	{			
		for(unsigned int i = 0; i < g_PIMouseEventListeners.size(); i++)
		{
			if(g_PIMouseEventListeners.at(i)->OnMouseMove((HWND)m_hWnd, point.x, point.y, 
				::GetKeyState(VK_CONTROL) & 0x80, ::GetKeyState(VK_MENU) & 0x80, ::GetKeyState(VK_SHIFT) & 0x80))
				return;
		}

		if(/*(::GetKeyState(VK_MENU) & 0x80) && */(::GetKeyState(VK_RBUTTON) & 0x80))
		{			
			// ------------------------------------------------------------
			// ��ת�����
			// ------------------------------------------------------------
			if(COTEOgreManager::Instance()->m_pCamera->getProjectionType() != Ogre::PT_ORTHOGRAPHIC)
			{
				Ogre::Vector3 camPos = COTEOgreManager::Instance()->m_pCamera->getPosition();							

				// ������ת���� ����תĿ��

				Ogre::Ray r = Ogre::Ray(COTEOgreManager::Instance()->m_pCamera->getPosition(), 
												COTEOgreManager::Instance()->m_pCamera->getDirection());			
				Ogre::Vector3 center; // ���ĵ�
				
				if(!OTE::COTETilePage::GetCurrentPage())
				{
					std::pair<bool,Ogre::Real> result = r.intersects(Ogre::Plane(Ogre::Vector3::UNIT_Y, Ogre::Vector3(0, 0, 0)));
					center = r.getPoint(result.second);
				}
				else
					center = r.getPoint(10.0f);

				Ogre::Vector3 targetPos = center; // Ŀ���

				/*if(SCENE_MGR->GetCurObjSection())
				{
					center = SCENE_MGR->GetCurObjSection()->getParentSceneNode()->getPosition();
					targetPos = r.getPoint( 
						r.intersects(Ogre::Plane(Ogre::Vector3::UNIT_Y, Ogre::Vector3(0, center.y, 0))).second
						);
				}*/
				
				// ȷ����Ŀ�����ת����
				
				{	
					Ogre::Vector3 oc =  camPos - targetPos;
					float camDis = oc.length();
					oc.normalise();

					Ogre::Vector3 tx = Ogre::Vector3::UNIT_Y.crossProduct(oc).normalisedCopy(); // ������x		
					Ogre::Vector3 ty = tx.crossProduct(oc).normalisedCopy(); // ������y
									
					// todo �Ӵ���ʾ�������Ż�ԭ ...	 

					// �õ���ת�Ƕ�	
#ifdef __ZG
					Ogre::Vector2 screenMove(- point.x + m_nMousePosX, - point.y + m_nMousePosY);
#else
					Ogre::Vector2 screenMove(  point.x - m_nMousePosX, point.y - m_nMousePosY);
#endif
					screenMove.x =    screenMove.x * 0.0035f;	
					screenMove.y =  - screenMove.y * 0.0035f;		

					float pitch = (-ty.length() * screenMove.y);
					float yaw   = ( tx.length() * screenMove.x);

					COTEOgreManager::Instance()->m_pCamera->pitch(Ogre::Radian(pitch));
					COTEOgreManager::Instance()->m_pCamera->yaw  (Ogre::Radian(yaw  ));

					Ogre::Vector3 camDir = COTEOgreManager::Instance()->m_pCamera->getDirection();		

					// λ��				

					Ogre::Vector3 newCamPos = targetPos - camDir * camDis; /* ���ջ��ȴ��� ��֤Զ��һ�� */			
					
					COTEOgreManager::Instance()->m_pCamera->setPosition(newCamPos);	

				
				}	
			}

		}

		if((::GetKeyState(VK_MBUTTON) & 0x80) || (::GetKeyState(VK_MENU) & 0x80) && (::GetKeyState(VK_LBUTTON) & 0x80) )
		{			
			// ------------------------------------------------------------
			// ƽ�������
			// ------------------------------------------------------------

			Ogre::Vector3 camPos = COTEOgreManager::Instance()->m_pCamera->getPosition();
			
			// �����������
			Ogre::Vector3 camX = COTEOgreManager::Instance()->m_pCamera->getOrientation() * Ogre::Vector3::UNIT_X;
			Ogre::Vector3 camY = COTEOgreManager::Instance()->m_pCamera->getOrientation() * Ogre::Vector3::UNIT_Y;
			
			// ���������� ��ĻͶӰ�ĵ�2ά����	
			Ogre::Vector2 cX = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_X), camY.dotProduct(Ogre::Vector3::UNIT_X));
			Ogre::Vector2 cY = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_Y), camY.dotProduct(Ogre::Vector3::UNIT_Y));;
			Ogre::Vector2 cZ = Ogre::Vector2(camX.dotProduct(Ogre::Vector3::UNIT_Z), camY.dotProduct(Ogre::Vector3::UNIT_Z));;
			
			Ogre::Vector2 screenMove(point.x - m_nMousePosX, point.y - m_nMousePosY);
			
			Ogre::Ray r = Ogre::Ray(camPos, COTEOgreManager::Instance()->m_pCamera->getDirection());	
			std::pair<bool,Ogre::Real> result = r.intersects(
				Ogre::Plane(Ogre::Vector3::UNIT_Y, Ogre::Vector3(0, 0/*camPos.y - 5.0f*/, 0)));
			
			float camDis = result.second;

			float clipNear = COTEOgreManager::Instance()->m_pCamera->getNearClipDistance();	
			
			// todo �Ӵ���ʾ�������Ż�ԭ ...

			screenMove.x =    screenMove.x * Ogre::Math::Abs(camDis) / clipNear * 0.00007f;	//screen coordnate
			screenMove.y =  - screenMove.y * Ogre::Math::Abs(camDis) / clipNear * 0.0001f;	//screen coordnate

			Ogre::Vector3 move3D = Ogre::Vector3(screenMove.dotProduct(cX), screenMove.dotProduct(cY), screenMove.dotProduct(cZ));

			if(COTEOgreManager::Instance()->m_pCamera->getProjectionType() != Ogre::PT_ORTHOGRAPHIC)
			{
				if(!(::GetKeyState(VK_CONTROL) & 0x80))
				{
					COTEOgreManager::Instance()->m_pCamera->setPosition(
						Ogre::Vector3(
						camPos.x - move3D.x,
						camPos.y, 
						camPos.z - move3D.z)
						);
				}
				else
				{
					COTEOgreManager::Instance()->m_pCamera->setPosition(
						Ogre::Vector3(
						camPos.x - move3D.x,
						camPos.y - move3D.y, 
						camPos.z)
						);
				}
			}
		}

		if((::GetKeyState(VK_MENU) & 0x80) && (::GetKeyState(VK_SHIFT) & 0x80) )
		{
			// ------------------------------------------------------------
			// ������Զ�����
			// ------------------------------------------------------------	
			Ogre::Vector3 old_camPos = COTEOgreManager::Instance()->m_pCamera->getPosition();

			COTEOgreManager::Instance()->m_pCamera->moveRelative(Ogre::Vector3(0, 0, 
				(point.x - m_nMousePosX + point.y - m_nMousePosY) * 1.0) );
		
			if(COTEOgreManager::Instance()->m_pCamera->getPosition().y < 4 || 
				COTEOgreManager::Instance()->m_pCamera->getPosition().y > 256)
			{	

				COTEOgreManager::Instance()->m_pCamera->setPosition(old_camPos);
			}

		}
		m_nMousePosX = point.x;
		m_nMousePosY = point.y;
			
		CView::OnMouseMove( nFlags, point);		
	}

	// --------------------------------------------------
	BOOL CAppFrameView::OnMouseWheel( UINT nFlags, short delta, CPoint point)
	{
		// �༭�����Ϣ��Ӧ
		
		for(unsigned int i = 0; i < g_PIMouseEventListeners.size(); i++)	
		{
			if(g_PIMouseEventListeners.at(i)->OnWheel((HWND)m_hWnd, point.x, point.y, delta, 
				::GetKeyState(VK_CONTROL) & 0x80, ::GetKeyState(VK_MENU) & 0x80, ::GetKeyState(VK_SHIFT) & 0x80))
			{
				return true;
			}
		}

		//if(::GetKeyState(VK_MENU) & 0x80)
		if(COTEOgreManager::Instance()->m_pCamera->getProjectionType() != Ogre::PT_ORTHOGRAPHIC)
		{
			// ���ĵ�
			Ogre::Ray r = Ogre::Ray(COTEOgreManager::Instance()->m_pCamera->getPosition(), 
											COTEOgreManager::Instance()->m_pCamera->getDirection());	
			Ogre::Vector3 center = r.getPoint(10.0f);
			if(center.y < 0)
			{
				std::pair<bool,Ogre::Real> result = r.intersects(Ogre::Plane(Ogre::Vector3::UNIT_Y, Ogre::Vector3(0, 0, 0)));
				center = r.getPoint(result.second);	
			}

			if(SCENE_MGR->GetCurObjSection())
			{
				//OTE_TRACE("SCENE_MGR->GetCurObjSection() : " << SCENE_MGR->GetCurObjSection())
				//center = SCENE_MGR->GetCurObjSection()->getParentSceneNode()->getPosition();
			}

			// ���������
			// ����������ߵ�

			Ogre::Vector3 camOldPos = COTEOgreManager::Instance()->m_pCamera->getPosition();

			// �����Զ���ƶ�		
			
			if(delta < 0) 
			{

				COTEOgreManager::Instance()->m_pCamera->moveRelative(Ogre::Vector3(0, 0, (center - camOldPos).length() * 0.4f) );
							
			}
			if(delta > 0)
			{
				COTEOgreManager::Instance()->m_pCamera->moveRelative(Ogre::Vector3(0, 0, -(center - camOldPos).length() * 0.4f) );
								
			}			
	
			// ����������ߵ�
			
			if(COTEOgreManager::Instance()->m_pCamera->getPosition().y < 3)
			{
				COTEOgreManager::Instance()->m_pCamera->setPosition(camOldPos);
			}		

		}

		return  true;

	} 

	// --------------------------------------------------
	void CAppFrameView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		for(unsigned int i = 0; i < g_PIKeyEventListeners.size(); i++)
		{		
			if(g_PIKeyEventListeners.at(i)->OnKeyUp(nChar, 
				::GetKeyState(VK_CONTROL) & 0x80, ::GetKeyState(VK_MENU) & 0x80, ::GetKeyState(VK_SHIFT) & 0x80))
				break;
		}

		CView::OnKeyUp(nChar, nRepCnt, nFlags);
	}

	// --------------------------------------------------
	void CAppFrameView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		// һЩ���������
		switch(nChar)
		{			
			// ���ݱ༭��Ϣ����Ӧ�Ĳ��

			case VK_F1: // ��������
			{					
				COTEPluginManager::GetInstance()->SendCustomMessage("OM_OBJPROPERTY");			
			}break;

			case VK_F2: // ������Ϊ
			{					
				COTEPluginManager::GetInstance()->SendCustomMessage("OM_OBJACTION");			
			}break;

			case VK_F3: // �������
			{					
				COTEPluginManager::GetInstance()->SendCustomMessage("OM_OBJMATERIAL");			
			}break;

			case VK_F4 :
			{
				// 3D��ʾ/����

				static bool isVisible = true;
				OTE::COTEMagicEntity::SetAllMagicsVisible(isVisible);
				SCENE_MGR->getRootSceneNode()->setVisible(!isVisible);				
				isVisible = !isVisible;
			}break;
			case VK_F5 :
			{
				// ������ʾ/����

				if(OTE::COTETilePage::GetCurrentPage())
				{
					static bool isVisible = true;
					OTE::COTETilePage::GetCurrentPage()->ShowTerrain(!isVisible);
					isVisible = !isVisible;
				}
			}break;			
			case VK_F6 :
			{
				// ��ʾ/�������

				static bool isVisible = true;
				SCENE_MGR->ShowEntities(!isVisible);
				isVisible = !isVisible;
				
			}break;
			case VK_F7 :
			{
				// ��ʾ/������Ч

				static bool isVisible = true;
				OTE::COTEMagicEntity::SetAllMagicsVisible(isVisible);
				isVisible = !isVisible;
				
			}break;
			//case VK_F8 :
			//{
			//	// ������ʾ/���ر�����

			//	if(OTE::COTETilePage::GetCurrentPage())
			//	{
			//		static bool isVisible = true;
			//		OTE::COTETilePage::GetCurrentPage()->ShowTerrainBoundingBox(!isVisible);
			//		isVisible = !isVisible;
			//	}
			//}break;
	
			case VK_F9 :
			{
				SCENE_CAM->setFarClipDistance(SCENE_CAM->getFarClipDistance() + 50);
				if(COTETilePage::GetCurrentPage())
					COTETilePage::GetCurrentPage()->SetMaxDistance(SCENE_CAM->getFarClipDistance());

			}break;		
			case VK_F10 :
			{
				SCENE_CAM->setFarClipDistance(SCENE_CAM->getFarClipDistance() - 50);
				if(COTETilePage::GetCurrentPage())
					COTETilePage::GetCurrentPage()->SetMaxDistance(SCENE_CAM->getFarClipDistance());

			}break;	

			case 192:
			{
				// Plugins 

				if(!SCENE_MGR->m_IsEditor)
				{	
					((CMainFrame*)this->GetParentFrame())->LoadPlugins(".\\OTEPlugins.cfg");
					((CMainFrame*)this->GetParentFrame())->ShowControlBar(&((CMainFrame*)this->GetParentFrame())->m_wndToolBar, true, false); 
					
					::ShowCursor(true);	
					SCENE_MGR->m_IsEditor = true;				
				}
				else
				{
					((CMainFrame*)this->GetParentFrame())->ClearPlugins();			

					while(((CMainFrame*)this->GetParentFrame())->m_wndToolBar.GetToolBarCtrl().GetButtonCount() > 0)
					{
						((CMainFrame*)this->GetParentFrame())->m_wndToolBar.GetToolBarCtrl().DeleteButton(0);
					}
					
					((CMainFrame*)this->GetParentFrame())->ShowControlBar(&((CMainFrame*)this->GetParentFrame())->m_wndToolBar, false, false); 
	
					::ShowCursor(false);	
					SCENE_MGR->m_IsEditor = false;
					
				}

			}break;
			case 'R':
			{
				static bool isWire =false;
				if(!isWire && ::GetKeyState(VK_CONTROL) & 0x80)
				{
					COTEOgreManager::Instance()->m_pCamera->setDetailLevel( Ogre::SDL_WIREFRAME );
					isWire = true;
				}
				else
				{
					COTEOgreManager::Instance()->m_pCamera->setDetailLevel( Ogre::SDL_SOLID );
					isWire = false;
				}

			}break;
		}

		
		for(unsigned int i = 0; i < g_PIKeyEventListeners.size(); i++)
		{
			if(	g_PIKeyEventListeners.at(i)->OnKeyDown(nChar, 
				::GetKeyState(VK_CONTROL) & 0x80, ::GetKeyState(VK_MENU) & 0x80, ::GetKeyState(VK_SHIFT) & 0x80))
				break;
		}


		CView::OnKeyDown(nChar, nRepCnt, nFlags);
	}


#endif	