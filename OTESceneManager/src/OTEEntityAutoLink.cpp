/************************************************************
 ************************************************************

 模块名称： 自动拼接实体
 相关模块： 
						
 描述    ： 这是一种把实体拼在一起 用于制作一些介于物件与地皮之间
            的特殊地貌或者人造环境(城市的街道).
			具体方法就是把实体按照一种规范制作成很多单元(具体看资源)
			然后把这些单元拼接起来形成整体.

 *************************************************************
 *************************************************************/

#include "OTEEntityAutoLink.h"
#include "OTEQTSceneManager.h"
#include "OTETerrainHeightMgr.h"
#include "OTECollisionManager.h"

// ----------------------------------------------
using namespace Ogre;
using namespace OTE;

// ----------------------------------------------
#define SET_UV_AT(ux, uz, u, v, operFlag){UpdateEntities(ux, uz, u, v, operFlag);}
#define GET_UV_AT(ux, uz, u, v) {GetALEntityAtPoint(ux, uz, u, v);}

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 物件拼接策略

       |    
	LT | RT    
   --------- 
    LB | RB	   
 	   |	 

  c_SmpEntUMap : u = fu(u', v')
  c_SmpEntVMap : v = fv(u', v')

  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/
const float c_SmpEntUMap[][20] = {
	{0, 1, 1, 1,    
	 0, 1, 3, 3,   
	 0, 2, 0, 0,   
	 0, 3, 2, 3},	// 左上LT	一组对应关系中的U部分
	
	{1, 1, 2, 3,   
	 3, 1, 2, 3,    
	 1, 3, 2, 2,   
	 3, 1, 1, 3},	// 右上 RT
	
	{0, 3, 1, 3,   
	 0, 1, 3, 1,  
	 0, 1, 1, 0,  
	 2, 1, 2, 3},	// 左下	LB
	
	{0, 3, 2, 3, 
	 2, 1, 2, 3,  
	 1, 1, 2, 2,  
	 0, 3, 2, 1}	// 右下 RB
};

const float c_SmpEntVMap[][20] = {
	{0, 0, 0, 1,    
	 1, 1, 1, 1,   
	 1, 3, 3, 0,   
	 3, 3, 3, 3},	// 左上	一组对应关系中的V部分
	
	{0, 0, 0, 0,  
	 3, 1, 1, 1,    
	 3, 0, 1, 0,    
	 1, 3, 1, 3},	// 右上
	
	{1, 3, 3, 0,    
	 1, 1, 0, 1,   
	 2, 2, 2, 2,   
	 3, 3, 3, 3},	// 左下
	
	{3, 1, 1, 0,    
	 3, 1, 1, 1,   
	 2, 2, 2, 2,   
	 3, 0, 3, 1}	// 右下
};

// ---------------------------------------------------------------------
const float c_DecSmpEntUMap[][20] = {	
	{3, 2, 2, 3,   
	 0, 3, 2, 2,   
	 0, 1, 2, 3,    
	 2, 1, 1, 1},	// 左上
	
	{0, 0, 3, 1,    
	 0, 2, 2, 0,    
	 0, 1, 2, 3,    
	 0, 0, 2, 0},	// 右上
	
	{0, 1, 2, 2,    
	 0, 3, 2, 3,    
	 3, 2, 2, 3,    
	 0, 2, 0, 1},	// 左下
	
	{0, 1, 2, 1,   
	 0, 3, 2, 1,    
	 0, 0, 3, 3,   
	 0, 1, 0, 3}	// 右下
};

const float c_DecSmpEntVMap[][20] = {
	{2, 0, 0, 0,    
	 2, 0, 1, 1,    
	 2, 2, 2, 2,  
	 2, 3, 2, 3},	// 左上
	
	{0, 0, 2, 2,  
	 1, 3, 2, 3, 
	 2, 2, 2, 2, 
	 3, 2, 3, 1},	// 右上
	
	{0, 0, 0, 1, 
	 0, 1, 1, 1,  
	 2, 2, 2, 2,  
	 3, 0, 3, 0},	// 左下
	
	{0, 0, 0, 3,   
	 1, 3, 0, 0,    
	 2, 2, 2, 2,    
	 0, 3, 1, 3}	// 右下
};

// ======================================
// COTEEntityAutoLink
// ======================================
COTEEntityAutoLink COTEEntityAutoLink::s_Inst;
COTEEntityAutoLink*	COTEEntityAutoLink::GetInstance()
{
	return &s_Inst;
}

COTEEntityAutoLink::COTEEntityAutoLink() : 
m_LastHgt(-1.0f),
m_LastBottomHgt(-1.0f)
{
}

// --------------------------------------	
// 得到这个位置上的实体

COTEEntity* COTEEntityAutoLink::GetALEntityAtPoint(int uX, int uZ, int& u, int& v)
{	
	std::stringstream ent_ss;
	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;	    // entity name

	if( SCENE_MGR->IsEntityPresent(ent_ss.str()) )
	{
		COTEEntity* e = SCENE_MGR->GetEntity(ent_ss.str());	
		std::string mn = e->getMesh()->getName();			
		int tu, tv;
		
		if(sscanf(mn.c_str(), (m_MeshGroupNamePrefix + "_%d_%d").c_str(), &tu, &tv) == 2)
		{
			u = tu;
			v = tv;
			return e;
		}
	}
	else
	{
		Ogre::Vector3 pos;
		GetWorldPosition(uX, uZ, pos);

		if(m_LastBottomHgt < 0 || Ogre::Math::Abs(m_LastBottomHgt - pos.y) < 1.0f)
		{
			// 在外围

			u = 3;
			v = 2;
		}
		else
		{
			// 在内部

			u = 1;
			v = 1;
		}
	}
	return NULL;
}

// --------------------------------------	
// 外部接口
void COTEEntityAutoLink::AutoLink(float X, float Z, unsigned int operFlag)
{
	// 缩放

	COTETilePage* page = COTETilePage::GetCurrentPage();
	if(page)
	{
		float tileSize = page->m_TileSize;
		m_Scale = page->m_Scale * Ogre::Vector3(tileSize, 1.0f, tileSize);
	}
	else
	{
		m_Scale = Ogre::Vector3::UNIT_SCALE;
	}

	// 自动连接

	AutoLink(int(X / m_Scale.x),	 int(Z / m_Scale.z), 0, 0, CTile::LEFT, operFlag);
	AutoLink(int(X / m_Scale.x) + 1, int(Z / m_Scale.z), 0, 0, CTile::RIGHT, operFlag);
	AutoLink(int(X / m_Scale.x),	 int(Z / m_Scale.z) + 1, 0, 0, CTile::LEFTBOTTOM, operFlag);
	AutoLink(int(X / m_Scale.x) + 1, int(Z / m_Scale.z) + 1, 0, 0, CTile::RIGHTBOTTOM, operFlag);

}

// --------------------------------------
// 实现连接

void COTEEntityAutoLink::AutoLink(int uX, int uZ, int u, int v, short cornerType, unsigned int operFlag)
{
	// 有效？

	if(m_LastBottomHgt < 0				|| 
		m_MeshGroupNamePrefix.empty()	||
		m_EntGroupNamePrefix.empty() 
		)
		return;

	if( !(u  >= 0 && u  < 4) ||
		!(v  >= 0 && v  < 4)  )
		return;

	int uInd, vInd;	//存放u v值索引
			
	// 添加一层 

	if(operFlag & AUTOLINK_OPER_ADD)
	{
		GET_UV_AT(uX, uZ, uInd, vInd) 
		if(uInd >= 4 && vInd > 1 && uInd < 8 && vInd < 4)	
		{
			uInd = 1;
			vInd = 1;
		}
		if(uInd < 4 && vInd < 4)
		{ 
			int u_Ind = c_SmpEntUMap[cornerType][uInd + vInd * 4];
			int v_Ind = c_SmpEntVMap[cornerType][uInd + vInd * 4];

			SET_UV_AT(uX, uZ, u_Ind, v_Ind, operFlag)			
		}
		else
		{		
			SET_UV_AT(uX, uZ, u, v, operFlag)
		}
	}
	
	// 删除一层

	else
	{	
		int uInd, vInd;
		GET_UV_AT(uX, uZ, uInd, vInd) 
		if(uInd >= 4 && vInd > 1 && uInd < 8 && vInd < 4)
		{
			uInd = 1;
			vInd = 1;
		}
		if(uInd < 4 && vInd < 4)
		{
			int u_Ind = c_DecSmpEntUMap[cornerType][uInd + vInd * 4];
			int v_Ind = c_DecSmpEntVMap[cornerType][uInd + vInd * 4];
		
			SET_UV_AT(uX, uZ, u_Ind, v_Ind, operFlag)			
		}		
	}
}

// --------------------------------------
// 更新拼接的实体模型
// 模型元素名称服从　%s_IndexU_IndexV格式
// --------------------------------------
void COTEEntityAutoLink::UpdateEntities(int uX, int uZ, int u, int v, unsigned int operFlag)
{
	std::stringstream ss;
	ss << m_MeshGroupNamePrefix << "_" << u << "_" << v << ".mesh"; // mesh name
	std::stringstream ent_ss;
	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;	    // entity name	
	
	if( SCENE_MGR->IsEntityPresent(ent_ss.str()) )
	{
		SCENE_MGR->RemoveEntity(ent_ss.str());
	}	

	// 悬崖底部高度

	Ogre::Vector3 pos;
	GetWorldPosition(uX, uZ, pos);			
	
	//m_LastBottomHgt < 0 ? m_LastBottomHgt = pos.y : NULL;

	// 创建 或旋转

	COTEActorEntity* ae = NULL;
	if(CHECK_RES(ss.str())) // 存在吗？
	{
		ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
	}

	else	
	
	//////////// 如果对应的资源没有找到则用已有资源拼 ////////////

	{
		/// 从[00]旋转得到[20],[02],[22]
		if(u == 2 && v == 0)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 0 << "_" << 0 << ".mesh";
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(90.0f));				
		}
		if(u == 0 && v == 2)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 0 << "_" << 0 << ".mesh";
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(-90.0f));				
		}
		if(u == 2 && v == 2)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 0 << "_" << 0 << ".mesh";
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(180.0f));				
		}

		/// 从[10]旋转得到[01],[12],[21]
		if(u == 0 && v == 1)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 1 << "_" << 0 << ".mesh";	
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(-90.0f));				
		}
		if(u == 1 && v == 2)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 1 << "_" << 0 << ".mesh";	
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(180.0f));			
		}
		if(u == 2 && v == 1)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 1 << "_" << 0 << ".mesh";	
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(-90.0f));				
		}

		/// 从[30]旋转得到[31],[33],[32]
		if(u == 3 && v == 1)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 3 << "_" << 0 << ".mesh";	
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(90.0f));				
		}
		if(u == 3 && v == 3)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 3 << "_" << 0 << ".mesh";	
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(180.0f));			
		}
		if(u == 2 && v == 3)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 3 << "_" << 0 << ".mesh";	
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(-90.0f));				
		}

		/// 从[03]旋转得到[13]
		if(u == 1 && v == 3)
		{
			ss.str("");		ss << m_MeshGroupNamePrefix << "_" << 0 << "_" << 3 << ".mesh";	
			ent_ss.str("");	ent_ss << m_EntGroupNamePrefix << "_" << uX << "_" << uZ;
			ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());
			ae->getParentSceneNode()->yaw(Degree(90.0f));				
		}		

	}

	// 位置
	
	if( u == 1 && v == 1 && (operFlag & AUTOLINK_UPDATETERRAIN) )
	{			
		// 调整 terrain tile 的高度

		COTETile* tile = COTETilePage::s_CurrentPage->GetTileAt(uX, uZ);
		if(tile)
		{
			float size = (COTETilePage::s_CurrentPage->m_TileSize + 1) * COTETilePage::s_CurrentPage->m_Scale.x * 2.0f;
			float uhgt = COTETilePage::s_CurrentPage->m_Scale.y / 255.0f;

			COTETerrainHeightMgr::GetInstance()->SetHeightAt(
				COTETilePage::s_CurrentPage, 
				pos.x,				//x坐标
				pos.z,				//z坐标
				m_LastHgt - uhgt,	//y高度
				m_LastHgt - uhgt,	//强度
				size,				//内圈
				size,				//外圈
				COTETerrainHeightMgr::eTerrain_Edit_ManualHeight_Rect);	//计算类型			
		}	

		// 边缘连接问题

		UpdateBorder(uX, uZ, -1, 0, AUTOLINK_OPER_ADD);		// 左
		UpdateBorder(uX, uZ, 1, 0,  AUTOLINK_OPER_ADD);		// 右
		UpdateBorder(uX, uZ, 0, -1, AUTOLINK_OPER_ADD);		// 上
		UpdateBorder(uX, uZ, 0, 1,  AUTOLINK_OPER_ADD);		// 下
	}

	else	
	{	
		// 清理已有的

		if( !(operFlag & AUTOLINK_OPER_ADD) && 
			 (operFlag & AUTOLINK_UPDATETERRAIN)
			)
		{
			// 清理  terrain tile 的高度
			
			COTETile* tile = COTETilePage::s_CurrentPage->GetTileAt(uX, uZ);
			if(tile)
			{
				float size = (COTETilePage::s_CurrentPage->m_TileSize + 1) * COTETilePage::s_CurrentPage->m_Scale.x * 2.5f;
				
				COTETerrainHeightMgr::GetInstance()->SetHeightAt(
					COTETilePage::s_CurrentPage, 
					pos.x,				//x坐标
					pos.z,				//z坐标
					m_LastBottomHgt,	//y高度
					m_LastBottomHgt,	//强度
					size,				//内圈
					size,				//外圈
					COTETerrainHeightMgr::eTerrain_Edit_ManualHeight_Rect);	//计算类型					
			}	

			UpdateBorder(uX, uZ, -1,  0,  0);  // 左
			UpdateBorder(uX, uZ,  1,  0,  0);  // 右
			UpdateBorder(uX, uZ,  0, -1,  0);  // 上
			UpdateBorder(uX, uZ,  0,  1,  0);  // 下		
		}
		
		if(ae)
		{			
			float yscale = m_Scale.x; /*temp*/
			ae->getParentSceneNode()->scale(m_Scale.x, yscale, m_Scale.z);
			
			if(operFlag & AUTOLINK_RAISE)
				m_LastHgt < 0 ? m_LastHgt = m_LastBottomHgt + ae->getBoundingBox().getMaximum().y * yscale : NULL;	
			else
				m_LastHgt < 0 ? m_LastHgt = m_LastBottomHgt - ae->getBoundingBox().getMinimum().y * yscale : NULL;	
			
			pos.y = m_LastBottomHgt;
			ae->getParentSceneNode()->setPosition(pos);	
		}
	}
}

// --------------------------------------
// 更新边缘
void COTEEntityAutoLink::UpdateBorder(int uX, int uZ, int offsetX, int offsetZ, unsigned int operFlag)
{
	std::stringstream ent_ss;	ent_ss	<< m_EntGroupNamePrefix << "_border_" << offsetX << offsetZ << "_" << uX << "_" << uZ;

	if( SCENE_MGR->IsEntityPresent(ent_ss.str()) )
	{		
		SCENE_MGR->RemoveEntity(ent_ss.str());		
	}

	if(!(operFlag & AUTOLINK_OPER_ADD))
		return;

	Ogre::Vector3 pos;
	GetWorldPosition(uX, uZ, pos);	

	int u, v;
	if(GetALEntityAtPoint(uX + offsetX, uZ + offsetZ, u, v))
	{		
		// 等价物

		if( (offsetX == -1 && offsetZ == 0 ) && (u == 2 && v == 3 || u == 3 && v == 3) ){ u = 0; v = 1; }
		if( (offsetX == 0  && offsetZ == -1) && (u == 3 && v == 1 || u == 3 && v == 3) ){ u = 1; v = 0; }
		if( (offsetX == 1  && offsetZ == 0 ) && (u == 3 && v == 1 || u == 3 && v == 0) ){ u = 2; v = 1; }
		if( (offsetX == 0  && offsetZ == 1 ) && (u == 3 && v == 0 || u == 2 && v == 3) ){ u = 1; v = 2; }

		// 创建边缘

		if( u == 0 && v == 1 ||
			u == 1 && v == 0 ||
			u == 2 && v == 1 ||
			u == 1 && v == 2)
		{
			std::stringstream ss;	ss << m_MeshGroupNamePrefix << "_border_" << u << "_" << v << ".mesh";	
			pos.y = m_LastBottomHgt;

			COTEActorEntity* ae = SCENE_MGR->CreateEntity(ss.str(), ent_ss.str());	
			ae->getParentSceneNode()->setPosition(pos);		
			ae->getParentSceneNode()->scale(m_Scale.x, m_Scale.x/*temp*/, m_Scale.z);
		}		
	}
}

// --------------------------------------
void COTEEntityAutoLink::GetWorldPosition(int uX, int uZ, Ogre::Vector3& position)
{
	position.x = float(uX) * m_Scale.x + m_Scale.x * 0.5f;
	position.z = float(uZ) * m_Scale.z + m_Scale.z * 0.5f;

	if(COTETilePage::GetCurrentPage())
		position.y = COTETilePage::GetCurrentPage()->GetHeightAt(position.x, position.z);
	else
		position.y = 0;	 // temp!
}