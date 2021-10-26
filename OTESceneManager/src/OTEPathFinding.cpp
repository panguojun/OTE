#include "OTEPathFinding.h"
#include "OTECollisionMap.h"

// ---------------------------------------------
using namespace Ogre;
using namespace OTE;

// ---------------------------------------------
// 常数

static const int	c_Step			= 4;			// 步长

static const int	c_Directions	= 8;			// 方位

static const int	c_MaxDepth		= 1000;			//最大深度

// c_Directions个方向移动变换

static const int c_PosArray[][2] = 
{
				{- c_Step,		0		},			// 左
				{+ c_Step,		0		},			// 右	
				{0,				+ c_Step},			// 下
				{0,				- c_Step},			// 上

				{- c_Step,		+ c_Step},			// 左下
				{- c_Step,		- c_Step},			// 左上
				{+ c_Step,		+ c_Step},			// 右下
				{+ c_Step,		- c_Step}			// 右上
};

// ---------------------------------------------
// 静态变量

static int*										s_GMap = NULL;		// 历史纪录

static std::list<WayNode_t*>					s_OpenList;			// 开放表

static int										s_MaxG;				// 最大步骤

static int										s_TargetX;			// 最终到达的位置

static int										s_TargetY;			// 最终到达的位置

static COTECollisionMap*						s_ColliMap = NULL;	// 碰撞图 

// ---------------------------------------------
// 插入列表按照F排序

static void InsertWayNodeList(WayNode_t* wn, std::list<WayNode_t*>& wnlist)
{	
	if(wnlist.empty())
	{
		wnlist.push_back(wn);
		return;
	}

	std::list<WayNode_t*>::iterator begin = wnlist.begin();
	std::list<WayNode_t*>::iterator end = wnlist.end(); -- end;

	float db = wn->_F - (*begin)->_F;
	float de = wn->_F - (*end  )->_F;

	// 如果在两端则直接放到下

	if(db <= 0)
	{
		wnlist.push_front(wn);
		return;
	}
	else if(de >= 0)
	{
		wnlist.push_back(wn);

		return;
	}
	else 	
	{ // 按照距离两端点的远近判断从哪一端开始搜索

		if(db + de >= 0)
		{ // 靠近末端
			
		  while(end != begin)
		  {			
			if(wn->_F > (*end)->_F)
			{
				wnlist.insert( ++ end, wn);
				break;
			}
			else if(wn->_F == (*end)->_F )
			{
				if(wn->_X != (*end)->_X || wn->_Y != (*end)->_Y)
				{
					wnlist.insert( ++ end, wn );
				}
				break;
			}

			-- end;
		  }
		  return;

		}
		else
		{ // 靠近始端
			
		  while(begin != end)
		  {		   
			if(wn->_F < (*begin)->_F)
			{
				wnlist.insert( begin, wn );
				break;
			}
			else if(wn->_F == (*begin)->_F )
			{
				if(wn->_X != (*begin)->_X || wn->_Y != (*begin)->_Y)
				{
					wnlist.insert( begin, wn );
				}
				break;
			}

			++ begin;
		  }

		  return;

		}
	}

}

// ---------------------------------------------
static void ClearOpenList()
{
	std::list<WayNode_t*>::iterator it = s_OpenList.begin();
	while(it != s_OpenList.end())
	{
		delete (*it);
		++ it;
	}
	s_OpenList.clear();
}

// ---------------------------------------------
// A* 寻路算法实现

static bool FindPathImp(std::list<WayNode_t*>::iterator cit, 
									 int x, int y, int tx, int ty, int G, int depth, std::list<HistoryNode_t>& history)
{	
	// 关闭该节点

	if(	(*cit) )
		delete (*cit);

	s_OpenList.erase( cit );

	// 标记历史

	s_GMap[y * s_ColliMap->GetWidth() + x] = -1; // -1 代表此点已经关闭


	float H = Ogre::Math::Sqrt( float((x - tx) * (x - tx) + (y - ty) * (y - ty)) );	

	// 是否已经到达
	
	if(H <= c_Step)
	{	
		s_MaxG = G;

		s_TargetX = x;
		s_TargetY = y;

		//OTE_TRACE("Depth: " << depth)
	    return true;
	}

	// 循环子节点添加到开放节点表	

	for(int i = 0; i < c_Directions; i ++)
	{
		int nx = x + c_PosArray[i][0];
		int ny = y + c_PosArray[i][1];

		// 边界检测		

		if( nx < 0 || nx >= s_ColliMap->GetWidth() ||
			ny < 0 || ny >= s_ColliMap->GetHeight() )
			continue;

		int nextG = G + c_Step;
		//int nextG = G + (int)sqrt((float)c_PosArray[i][0] * c_PosArray[i][0] + c_PosArray[i][1] * c_PosArray[i][1]);
		int lastG = s_GMap[ ny * s_ColliMap->GetWidth() + nx ];
		
		if(  lastG < 0																	||	// 是否已经关闭
			 (lastG > 0 && lastG <= nextG)												||	// 到达此点的代价不是最优			 
			 CCollision2D::HitTest(s_ColliMap, x, y, nx, ny)								// 碰撞图碰撞
			) 
			continue;		
		

		H = Ogre::Math::Sqrt( float((nx - tx) * (nx - tx) + (ny - ty) * (ny - ty)) );	
				
		s_GMap[ ny * s_ColliMap->GetWidth() + nx ] = nextG;

		InsertWayNodeList(new WayNode_t(nx, ny, nextG, H),
			s_OpenList
			);

	}

	// 调用的最大的深度

	if(depth < c_MaxDepth)
	{
		// 循环所有开放节点 直到找到最优

		std::list<WayNode_t*>::iterator best = s_OpenList.begin();

		while(best != s_OpenList.end())
		{
			// 最优节点, 尝试最优路径	
			
			int bX = (*best)->_X;
			int bY = (*best)->_Y;
			int bF = (*best)->_F;
			int bG = (*best)->_G;

			if( FindPathImp( best, bX, bY, tx, ty, bG, depth + 1, history) ) 
			{ 
				history.push_front( HistoryNode_t(bX, bY, bF, bG) );			

				return true;  // 尝试成功!
			}	
			
			// 不可达!!

			//best = s_OpenList.begin();
			return false;
		}
	}
	
	// 不可达!!

	return false;
}

// ---------------------------------------------
// 从最优解中得到路径

static void GetBestPath(int sx, int sy, int tx, int ty, 
									   std::list<HistoryNode_t>& axList, std::list<HistoryNode_t>& bestlist)
{
	// 打印

	//OTE_TRACE("A* 寻路axList 节点数量 : " << axList.size())
	
	int		x = s_TargetX;
	int		y = s_TargetY;
	int 	G = s_MaxG - c_Step;
	
	int     lastSide = 0;

	while( (x != sx || y != sy) && G > c_Step) // 是否到达起始点
	{			
		for(int index = lastSide; index < lastSide + c_Directions; index ++) // c_Directions个方向的子节点
		{
			int i = index % c_Directions;	

			int nextG = G - c_Step;
			//int nextG = G - (int)sqrt((float)c_PosArray[i][0] * c_PosArray[i][0] + c_PosArray[i][1] * c_PosArray[i][1]);
			std::list<HistoryNode_t>::iterator it = axList.begin();	

			while(it != axList.end())
			{				
				if( (*it)._X == x + c_PosArray[i][0] && 
					(*it)._Y == y + c_PosArray[i][1] && 
					(*it)._G == G )
				{
					bestlist.push_front(*it);	
					
					s_GMap[ (y + c_PosArray[i][1]) * s_ColliMap->GetWidth() + x + c_PosArray[i][0] ] = 1;

					x += c_PosArray[i][0];
					y += c_PosArray[i][1];	

					G = nextG;

					if(G < c_Step)
					{
						return;
					}

					lastSide = i;
					index = lastSide + c_Directions;
					//i = c_Directions;

					break;
				}

				++ it;			
			}	
		}	
	}

	// 把最后的目标点添加上
	
	Ogre::Vector3 hitPos;

	if((s_TargetX != tx || s_TargetY != ty) && !CCollision2D::HitTest(s_ColliMap, x, y, tx, ty))
	{
		bestlist.push_back(HistoryNode_t(tx, ty, 0, 0));
	}

//	OTE_TRACE("A* 寻路bestlist 节点数量 : " << bestlist.size())

}

// ==============================================
// COTEPathFinding
// ==============================================

void COTEPathFinding::Init(COTECollisionMap* gmap)
{
	Clear();

	s_ColliMap = gmap;	// 碰撞图

	// 精度与碰撞图尺寸对应

	s_GMap = new int[s_ColliMap->GetWidth() * s_ColliMap->GetHeight()];

	OTE_TRACE("创建s_GMap size: " << s_ColliMap->GetWidth() << " " << s_ColliMap->GetHeight())
}

// ---------------------------------------------
void COTEPathFinding::Clear()
{
	SAFE_DELETE(s_GMap);

	ClearOpenList();
}


// ---------------------------------------------
// 寻路

bool COTEPathFinding::FindPath(COTECollisionMap* gmap, int x, int y, int tx, int ty, std::list<HistoryNode_t>& history)
{	
	if(!s_GMap ||
		(s_ColliMap != gmap) // 碰撞图被切换了
		)	
		Init(gmap);	

	// 边界检查

	if( x < 0 || x >= s_ColliMap->GetWidth() ||
		y < 0 || y >= s_ColliMap->GetHeight() )
		return false;

	// 是否可达？

	if(CCollision2D::HitTest(s_ColliMap, x, y) || CCollision2D::HitTest(s_ColliMap, tx, ty))
	{
		OTE_TRACE("start pos color: " << x << " " << y << " " << s_ColliMap->GetPixel(x, y) << " end pos color: " << tx << " " << ty << " " << s_ColliMap->GetPixel(tx, ty))
		return false;
	}

	// 寻路

	//OTE_TRACE("------ A* 寻路开始! ------")

	history.clear(); 
	ClearOpenList();

	memset(s_GMap, 0, sizeof(int) * s_ColliMap->GetWidth() * s_ColliMap->GetHeight());
	
	s_TargetX = x;
	s_TargetY = y;

	float H = Ogre::Math::Sqrt( float((x  - tx) * (x - tx) + (y  - ty) * (y - ty)) );

	InsertWayNodeList(new WayNode_t(x, y, 0, H), s_OpenList);
	
	std::list<HistoryNode_t> axResultList;
	if(!FindPathImp(s_OpenList.begin(), x, y, tx, ty, 0, 0, axResultList))
	{
		//OTE_TRACE("------ A* 寻路失败! ------")
		return false;
	}

	GetBestPath(x, y, tx, ty, axResultList, history);	

	/*OTE_TRACE("----------------------")
	OTE_TRACE("X " << x << " Y " << y)	
	std::list<HistoryNode_t>::iterator it = history.begin();
	while(it != history.end())
	{
		OTE_TRACE("X " << (it)->_X << " Y " << (it)->_Y)
		++ it;
	}*/
	
	//OTE_TRACE("------ A* 寻路完成! ------")

	return true;

}