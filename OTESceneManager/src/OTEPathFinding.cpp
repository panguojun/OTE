#include "OTEPathFinding.h"
#include "OTECollisionMap.h"

// ---------------------------------------------
using namespace Ogre;
using namespace OTE;

// ---------------------------------------------
// ����

static const int	c_Step			= 4;			// ����

static const int	c_Directions	= 8;			// ��λ

static const int	c_MaxDepth		= 1000;			//������

// c_Directions�������ƶ��任

static const int c_PosArray[][2] = 
{
				{- c_Step,		0		},			// ��
				{+ c_Step,		0		},			// ��	
				{0,				+ c_Step},			// ��
				{0,				- c_Step},			// ��

				{- c_Step,		+ c_Step},			// ����
				{- c_Step,		- c_Step},			// ����
				{+ c_Step,		+ c_Step},			// ����
				{+ c_Step,		- c_Step}			// ����
};

// ---------------------------------------------
// ��̬����

static int*										s_GMap = NULL;		// ��ʷ��¼

static std::list<WayNode_t*>					s_OpenList;			// ���ű�

static int										s_MaxG;				// �����

static int										s_TargetX;			// ���յ����λ��

static int										s_TargetY;			// ���յ����λ��

static COTECollisionMap*						s_ColliMap = NULL;	// ��ײͼ 

// ---------------------------------------------
// �����б���F����

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

	// �����������ֱ�ӷŵ���

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
	{ // ���վ������˵��Զ���жϴ���һ�˿�ʼ����

		if(db + de >= 0)
		{ // ����ĩ��
			
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
		{ // ����ʼ��
			
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
// A* Ѱ·�㷨ʵ��

static bool FindPathImp(std::list<WayNode_t*>::iterator cit, 
									 int x, int y, int tx, int ty, int G, int depth, std::list<HistoryNode_t>& history)
{	
	// �رոýڵ�

	if(	(*cit) )
		delete (*cit);

	s_OpenList.erase( cit );

	// �����ʷ

	s_GMap[y * s_ColliMap->GetWidth() + x] = -1; // -1 ����˵��Ѿ��ر�


	float H = Ogre::Math::Sqrt( float((x - tx) * (x - tx) + (y - ty) * (y - ty)) );	

	// �Ƿ��Ѿ�����
	
	if(H <= c_Step)
	{	
		s_MaxG = G;

		s_TargetX = x;
		s_TargetY = y;

		//OTE_TRACE("Depth: " << depth)
	    return true;
	}

	// ѭ���ӽڵ���ӵ����Žڵ��	

	for(int i = 0; i < c_Directions; i ++)
	{
		int nx = x + c_PosArray[i][0];
		int ny = y + c_PosArray[i][1];

		// �߽���		

		if( nx < 0 || nx >= s_ColliMap->GetWidth() ||
			ny < 0 || ny >= s_ColliMap->GetHeight() )
			continue;

		int nextG = G + c_Step;
		//int nextG = G + (int)sqrt((float)c_PosArray[i][0] * c_PosArray[i][0] + c_PosArray[i][1] * c_PosArray[i][1]);
		int lastG = s_GMap[ ny * s_ColliMap->GetWidth() + nx ];
		
		if(  lastG < 0																	||	// �Ƿ��Ѿ��ر�
			 (lastG > 0 && lastG <= nextG)												||	// ����˵�Ĵ��۲�������			 
			 CCollision2D::HitTest(s_ColliMap, x, y, nx, ny)								// ��ײͼ��ײ
			) 
			continue;		
		

		H = Ogre::Math::Sqrt( float((nx - tx) * (nx - tx) + (ny - ty) * (ny - ty)) );	
				
		s_GMap[ ny * s_ColliMap->GetWidth() + nx ] = nextG;

		InsertWayNodeList(new WayNode_t(nx, ny, nextG, H),
			s_OpenList
			);

	}

	// ���õ��������

	if(depth < c_MaxDepth)
	{
		// ѭ�����п��Žڵ� ֱ���ҵ�����

		std::list<WayNode_t*>::iterator best = s_OpenList.begin();

		while(best != s_OpenList.end())
		{
			// ���Žڵ�, ��������·��	
			
			int bX = (*best)->_X;
			int bY = (*best)->_Y;
			int bF = (*best)->_F;
			int bG = (*best)->_G;

			if( FindPathImp( best, bX, bY, tx, ty, bG, depth + 1, history) ) 
			{ 
				history.push_front( HistoryNode_t(bX, bY, bF, bG) );			

				return true;  // ���Գɹ�!
			}	
			
			// ���ɴ�!!

			//best = s_OpenList.begin();
			return false;
		}
	}
	
	// ���ɴ�!!

	return false;
}

// ---------------------------------------------
// �����Ž��еõ�·��

static void GetBestPath(int sx, int sy, int tx, int ty, 
									   std::list<HistoryNode_t>& axList, std::list<HistoryNode_t>& bestlist)
{
	// ��ӡ

	//OTE_TRACE("A* Ѱ·axList �ڵ����� : " << axList.size())
	
	int		x = s_TargetX;
	int		y = s_TargetY;
	int 	G = s_MaxG - c_Step;
	
	int     lastSide = 0;

	while( (x != sx || y != sy) && G > c_Step) // �Ƿ񵽴���ʼ��
	{			
		for(int index = lastSide; index < lastSide + c_Directions; index ++) // c_Directions��������ӽڵ�
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

	// ������Ŀ��������
	
	Ogre::Vector3 hitPos;

	if((s_TargetX != tx || s_TargetY != ty) && !CCollision2D::HitTest(s_ColliMap, x, y, tx, ty))
	{
		bestlist.push_back(HistoryNode_t(tx, ty, 0, 0));
	}

//	OTE_TRACE("A* Ѱ·bestlist �ڵ����� : " << bestlist.size())

}

// ==============================================
// COTEPathFinding
// ==============================================

void COTEPathFinding::Init(COTECollisionMap* gmap)
{
	Clear();

	s_ColliMap = gmap;	// ��ײͼ

	// ��������ײͼ�ߴ��Ӧ

	s_GMap = new int[s_ColliMap->GetWidth() * s_ColliMap->GetHeight()];

	OTE_TRACE("����s_GMap size: " << s_ColliMap->GetWidth() << " " << s_ColliMap->GetHeight())
}

// ---------------------------------------------
void COTEPathFinding::Clear()
{
	SAFE_DELETE(s_GMap);

	ClearOpenList();
}


// ---------------------------------------------
// Ѱ·

bool COTEPathFinding::FindPath(COTECollisionMap* gmap, int x, int y, int tx, int ty, std::list<HistoryNode_t>& history)
{	
	if(!s_GMap ||
		(s_ColliMap != gmap) // ��ײͼ���л���
		)	
		Init(gmap);	

	// �߽���

	if( x < 0 || x >= s_ColliMap->GetWidth() ||
		y < 0 || y >= s_ColliMap->GetHeight() )
		return false;

	// �Ƿ�ɴ

	if(CCollision2D::HitTest(s_ColliMap, x, y) || CCollision2D::HitTest(s_ColliMap, tx, ty))
	{
		OTE_TRACE("start pos color: " << x << " " << y << " " << s_ColliMap->GetPixel(x, y) << " end pos color: " << tx << " " << ty << " " << s_ColliMap->GetPixel(tx, ty))
		return false;
	}

	// Ѱ·

	//OTE_TRACE("------ A* Ѱ·��ʼ! ------")

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
		//OTE_TRACE("------ A* Ѱ·ʧ��! ------")
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
	
	//OTE_TRACE("------ A* Ѱ·���! ------")

	return true;

}