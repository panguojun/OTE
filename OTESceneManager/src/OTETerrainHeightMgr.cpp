#include "oteterrainheightmgr.h"
// ----------------------------------------------
using namespace Ogre;

namespace OTE
{

// ----------------------------------------------
COTETerrainHeightMgr COTETerrainHeightMgr::s_Inst;
COTETerrainHeightMgr* COTETerrainHeightMgr::GetInstance()
{
	return &s_Inst;
}


//-----------------------------------------------------------------------
// 光滑边缘
//　x, z, 强度，外圈，内圈，操作类型
void COTETerrainHeightMgr::SetHeightAt(COTETilePage* page, float cx, float cz,float dragHeight,
									 float dragIntensity, float topSize, float outRad, int type)
{	
	//m_LastEditTileList.clear();
    COTETile *t = page->GetTileAt(cx,cz);
	if(t)
	{
		SetHeightAt(t, cx,cz,dragHeight,dragIntensity,topSize,outRad, type);    
	}
}  

//-----------------------------------------------------------------------
// 单点操作
void COTETerrainHeightMgr::SetHeightAt(COTETilePage* page, float x, float z, float y )
{	
	//m_LastEditTileList.clear();
    COTETile *t = page->GetTileAt(x, z);
	if(t)
	{
		SetHeightAt(t, x, z, y);    
	}
}  



// ----------------------------------------------
void COTETerrainHeightMgr::SetHeightAt(COTETile* tile, float x, float z, float height)
{  
    Vector3 start	= tile->m_Bounds.getMinimum();
    Vector3 end		= tile->m_Bounds.getMaximum();
	

    /* Safety catch, if the point asked for is outside
    * of this tile, it will ask the appropriate tile
    */

    if ( x < start.x )
    {
        if ( tile->m_Neighbors[ CTile::WEST ] != 0 )
            return SetHeightAt(tile->m_Neighbors[ CTile::WEST ], x,  z, height );
        else
            x = start.x;
    }

    if ( x > end.x )
    {
        if ( tile->m_Neighbors[ CTile::EAST ] != 0 )
                return   SetHeightAt(tile->m_Neighbors[ CTile::EAST ], x,  z, height );
        else
            x = end.x;
    }

    if ( z < start.z )
    {
        if ( tile->m_Neighbors[ CTile::NORTH ] != 0 )
            return   SetHeightAt(tile->m_Neighbors[ CTile::NORTH ], x,  z, height );
        else
            z = start.z;
    }

    if ( z > end.z )
    {
        if ( tile->m_Neighbors[ CTile::SOUTH ] != 0 )
            return   SetHeightAt(tile->m_Neighbors[ CTile::SOUTH ], x,  z, height );
        else
            z = end.z;
    }
    

    float x_pct = ( x - start.x ) / ( end.x - start.x );
    float z_pct = ( z - start.z ) / ( end.z - start.z );

    float x_pt = x_pct * ( float ) ( tile->m_pParentPage->m_TileSize + 1);
    float z_pt = z_pct * ( float ) ( tile->m_pParentPage->m_TileSize + 1);
    
    int x_index = ( int ) x_pt;
    int z_index = ( int ) z_pt;	

	// If we got to the far right / bottom edge, move one back		
    if (x_index > tile->m_pParentPage->m_TileSize)
    {
        --x_index;
        x_pct = 1.0f;
    }
    else
    {
        // get remainder
        x_pct = x_pt - x_index;
    }
    if (z_index> tile->m_pParentPage->m_TileSize)
    {
        --z_index;
        z_pct = 1.0f;
    }
    else
    {
        z_pct = z_pt - z_index;
    }	 
	float tHeight=height;
	//update in the main buffer
	tile->ManualSetVertexbufferHgtAt(x_index,z_index,tHeight); 
	//===============deal with border leaks problem here==================

    if(x_index==0 && z_index==0)///west and north
	{
		if(tile->m_Neighbors[ CTile::WEST ]!=0)//west		
			tile->m_Neighbors[ CTile::WEST ]->ManualSetVertexbufferHgtAt(tile->m_pParentPage->m_TileSize,z_index,tHeight);  
		if(tile->m_Neighbors[ CTile::NORTH ]!=0)//north
		    tile->m_Neighbors[ CTile::NORTH ]->ManualSetVertexbufferHgtAt(x_index,tile->m_pParentPage->m_TileSize,tHeight); 
		if(tile->m_Neighbors[ CTile::WEST ]!=0 && tile->m_Neighbors[ CTile::NORTH ]!=0)	//west and north  		
			tile->m_Neighbors[ CTile::WEST ]->m_Neighbors[ CTile::NORTH ]->ManualSetVertexbufferHgtAt(tile->m_pParentPage->m_TileSize,tile->m_pParentPage->m_TileSize,tHeight); 
		return;
	}
    if(x_index==tile->m_pParentPage->m_TileSize && z_index==0)///east and north
	{
		if(tile->m_Neighbors[ CTile::EAST ]!=0)//east
			tile->m_Neighbors[ CTile::EAST ]->ManualSetVertexbufferHgtAt(0,z_index,tHeight); 
		if(tile->m_Neighbors[ CTile::NORTH ]!=0)//north
		    tile->m_Neighbors[ CTile::NORTH ]->ManualSetVertexbufferHgtAt(x_index,tile->m_pParentPage->m_TileSize,tHeight);  
		if(tile->m_Neighbors[ CTile::EAST ]!=0 && tile->m_Neighbors[ CTile::NORTH ]!=0)//east and north				  
			tile->m_Neighbors[ CTile::EAST ]->m_Neighbors[ CTile::NORTH ]->ManualSetVertexbufferHgtAt(0,tile->m_pParentPage->m_TileSize,tHeight); 
		return;	  
	}
	if(x_index==tile->m_pParentPage->m_TileSize && z_index==tile->m_pParentPage->m_TileSize)///east and south
	{
		if(tile->m_Neighbors[ CTile::EAST ]!=0)//east
			tile->m_Neighbors[ CTile::EAST ]->ManualSetVertexbufferHgtAt(0,z_index,tHeight); 
		if(tile->m_Neighbors[ CTile::SOUTH ]!=0)//south
		    tile->m_Neighbors[ CTile::SOUTH ]->ManualSetVertexbufferHgtAt(x_index,0,tHeight);  
		if(tile->m_Neighbors[ CTile::EAST ]!=0 && tile->m_Neighbors[ CTile::SOUTH ]!=0)//east and south				  
			tile->m_Neighbors[ CTile::EAST ]->m_Neighbors[ CTile::SOUTH ]->ManualSetVertexbufferHgtAt(0,0,tHeight); 
		return;	  
	}
	if(x_index==0 && z_index==tile->m_pParentPage->m_TileSize)///west and south
	{
	    if(tile->m_Neighbors[ CTile::WEST ]!=0)//west		
		    tile->m_Neighbors[ CTile::WEST ]->ManualSetVertexbufferHgtAt(tile->m_pParentPage->m_TileSize,z_index,tHeight);  
		if(tile->m_Neighbors[ CTile::SOUTH ]!=0)//south
		    tile->m_Neighbors[ CTile::SOUTH ]->ManualSetVertexbufferHgtAt(x_index,0,tHeight);  
		if(tile->m_Neighbors[ CTile::WEST ]!=0 && tile->m_Neighbors[ CTile::SOUTH ]!=0)//east and south				  
			tile->m_Neighbors[ CTile::WEST ]->m_Neighbors[ CTile::SOUTH ]->ManualSetVertexbufferHgtAt(tile->m_pParentPage->m_TileSize,0,tHeight); 
		return;	  
	}
    //not at the corner
	if(x_index==0 && tile->m_Neighbors[ CTile::WEST ]!=0)//west		
		tile->m_Neighbors[ CTile::WEST ]->ManualSetVertexbufferHgtAt(tile->m_pParentPage->m_TileSize,z_index,tHeight);  
	if(z_index==0 && tile->m_Neighbors[ CTile::NORTH ]!=0)//north
		tile->m_Neighbors[ CTile::NORTH ]->ManualSetVertexbufferHgtAt(x_index,tile->m_pParentPage->m_TileSize,tHeight); 

	if(x_index==tile->m_pParentPage->m_TileSize && tile->m_Neighbors[ CTile::EAST ]!=0)//east
		tile->m_Neighbors[ CTile::EAST ]->ManualSetVertexbufferHgtAt(0,z_index,tHeight); 
	if(z_index==tile->m_pParentPage->m_TileSize && tile->m_Neighbors[ CTile::SOUTH ]!=0)//south
		tile->m_Neighbors[ CTile::SOUTH ]->ManualSetVertexbufferHgtAt(x_index,0,tHeight);  		  
	
}

// ----------------------------------------------
void COTETerrainHeightMgr::SetHeightAt(COTETile* tile, int i, int j)
{	
	static bool  bVisited=0;	
  
	static CRectA<float>  rect;

	if(!bVisited) ///calculate terrain size by height	
	{
		bVisited = 1;			
		
		int endx = i + tile->m_pParentPage->m_TileSize + 1;
		int endz = j + tile->m_pParentPage->m_TileSize + 1;

		rect.sx = i * tile->m_pParentPage->m_Scale.x;
		rect.sz = j * tile->m_pParentPage->m_Scale.z;
		rect.ex = (endx + tile->m_pParentPage->m_UnitSize) * tile->m_pParentPage->m_Scale.x;
		rect.ez = (endz + tile->m_pParentPage->m_UnitSize) * tile->m_pParentPage->m_Scale.z;

	}

	//loop through tiles :
    SetTileHeightAt(tile, rect, CTile::HERE, 0, 0, eTerrain_Edit_Smooth);     
	bVisited=0; 	
  
}


// -----------------------------------------------------------------------
// (x，z，强度，外圈，内圈，操作类型)
void COTETerrainHeightMgr::SetHeightAt(COTETile* tile, float xo, float zo,float dragHeight, float dragIntensity,float topSize, float outRad, int type )
{
    static bool  bVisited=0;	
	static float x=0.0f,z=0.0f;   
	static CRectA<float>  rect;
	static float rr=0.0f;

	if(!bVisited)///calculate terrain size by height	
	{
		bVisited = 1;	

		// fix
		float tHeight=(dragHeight>0.0f? dragHeight:-dragHeight);

		if(type == eTerrain_Edit_ManualHeight)	//复制高度时特殊处理
			outRad /= 3.0f;
		rr = Math::Sqrt(outRad);
 		rect.sx = xo-rr,rect.sz=zo-rr;
		rect.ex = xo+rr,rect.ez=zo+rr;
		
		// get the start tile
		x=rect.sx,z=rect.sz;
	}

	// the tile's border

    Vector3 start	= tile->m_Bounds.getMinimum();
    Vector3 end		= tile->m_Bounds.getMaximum();

    if ( x < start.x )
    {
        if ( tile->m_Neighbors[ CTile::WEST ] != 0 )
            return SetHeightAt( tile->m_Neighbors[ CTile::WEST ], xo,zo, dragHeight,dragIntensity,topSize, outRad,type);
        else
            x = start.x;
    }

    if ( x > end.x )
    {
        if ( tile->m_Neighbors[ CTile::EAST ] != 0 )
            return SetHeightAt(tile->m_Neighbors[ CTile::EAST ], xo,zo, dragHeight,dragIntensity,topSize, outRad,type);
        else
            x = end.x;
    }

    if ( z < start.z )
    {
        if ( tile->m_Neighbors[ CTile::NORTH ] != 0 )
            return  SetHeightAt(tile->m_Neighbors[ CTile::NORTH ], xo,zo, dragHeight,dragIntensity,topSize, outRad,type);
        else
            z = start.z;
    }

    if ( z > end.z )
    {
        if ( tile->m_Neighbors[ CTile::SOUTH ] != 0 )
            return  SetHeightAt(tile->m_Neighbors[ CTile::SOUTH ], xo,zo, dragHeight,dragIntensity,topSize, outRad,type);
        else
            z = end.z;
    }
	
	//loop through tiles :
	SetTileHeightAt(tile, rect, CTile::HERE, topSize, dragIntensity, type);     
	bVisited=0;   
}

// ----------------------------------------------------------------------
void COTETerrainHeightMgr::SetTileHeightAt(COTETile* tile, CRectA<float>& rect, int dire,float topSize, float intensity, int type)
{
	if(dire == CTile::HERE || dire == CTile::EAST )
        proTerrain(tile, rect,topSize,intensity,type);
	
	// go through the tile map

	int tdire = dire;
	if(tdire == CTile::HERE || tdire == CTile::EAST)
	{		
		tdire = CTile::EAST;
		if ( tile->m_Neighbors[ CTile::EAST ] == 0 || !tile->m_Neighbors[ CTile::EAST ]->isInRect(rect) )//change dir?
		{
			tdire = CTile::SOUTH;
			if( tile->m_Neighbors[ CTile::SOUTH ] == 0 || !tile->m_Neighbors[ CTile::SOUTH ]->isInRect(rect))
				return;//finished?
		}
	}
	else if(tdire == CTile::SOUTH)
	{
		if( tile->m_Neighbors[ CTile::WEST ] == 0 || !tile->m_Neighbors[ CTile::WEST ]->isInRect(rect))
			tdire = CTile::HERE;		
		else
			tdire = CTile::WEST;	
		
	}
	else if(tdire == CTile::WEST)
	{
		if( tile->m_Neighbors[ CTile::WEST ] == 0 || !tile->m_Neighbors[ CTile::WEST ]->isInRect(rect))
			tdire = CTile::HERE; 	
    }			
    //
	if(tdire == CTile::HERE)
		return SetTileHeightAt(tile, rect,tdire,topSize,intensity,type); 
	else
		return SetTileHeightAt(tile->m_Neighbors[ tdire ], rect,tdire,topSize,intensity,type); 
	
}

//----------------------------------------------------------------------
float Paraboloid(float x, float z, float y0, float a)
{
	return y0 - a * (x * x + z * z);
}

float Linear(float x, float y0, float k)
{
	if(x < 0)
		return y0 + k * x;
	else
		return y0 - k * x;
}

//----------------------------------------------------------------------
/* 
	this stuff is the "heart of the matter"  
	includeing all the raise/low,flat,smooth...
	options.
*/
void COTETerrainHeightMgr::proTerrain(COTETile* tile, CRectA<float>& rect, float topSize, float intensity, int type)
{
#ifndef GAME_DEBUG1
	return;
#endif
	// 记录操作 

	std::list<COTETile*>::iterator it = m_LastEditTileList.begin();
	while(it != m_LastEditTileList.end())
	{
		if(*(it ++) == tile) break;		
	}
	if(it == m_LastEditTileList.end())
		m_LastEditTileList.push_back(tile);


	//get index
	int sx_index,sz_index,ex_index,ez_index;
    tile->GetPctIndex(rect.sx,rect.sz,sx_index,sz_index);
    tile->GetPctIndex(rect.ex,rect.ez,ex_index,ez_index);

	//recalculation     
    switch (type)
	{
	case eTerrain_Edit_Raise:/* raise*/
		{	
			//Set heights in a ovel way 
			float xo=rect.getCenterX<float>();
			float zo=rect.getCenterZ<float>();
			float size=rect.getHalfSize<float>(); 
	
			float y0=intensity*size*size;
	        
			for(int j=sz_index;j<ez_index+1;j++)
			for (int i=sx_index;i<ex_index+1;i++)
			{
				float xi=tile->_vertex( i, j, 0 );
				float zi=tile->_vertex( i, j, 2 );
				if((xi-xo)*(xi-xo)+(zi-zo)*(zi-zo)<size*size)
				{
					float yy=0.0f;
					if((xi-xo)*(xi-xo)+(zi-zo)*(zi-zo)<=topSize*topSize)
					{
						float tTopHalfSize=(topSize<size? topSize:size);
						yy=Paraboloid(tTopHalfSize,0,y0,intensity);
					}
					else
						yy=Paraboloid(xi-xo,zi-zo,y0,intensity);

					//update the buffer
					yy=(yy > 0.0f? yy : 0.0f);	


					// 更新

					tile->SetPosBufHeightAt(i, j, tile->m_pPositionBuffer[tile->_index(i, j) * 3 + 1] + yy / tile->m_pParentPage->m_Scale.y, tile->m_pPositionBuffer);

				}
			}	
			}break;
		case eTerrain_Edit_Low:/* low */
			{	
			//Set heights in a ovel way 
			float xo=rect.getCenterX<float>();
			float zo=rect.getCenterZ<float>();
			float size=rect.getHalfSize<float>(); 
	
			float y0=intensity*size*size;
            
			for(int j=sz_index;j<ez_index+1;j++)
			for (int i=sx_index;i<ex_index+1;i++)
			{
				float xi=tile->_vertex( i,j, 0 );
				float zi=tile->_vertex( i,j, 2 );
				if((xi-xo)*(xi-xo)+(zi-zo)*(zi-zo)<size*size)
				{
					float yy=0.0f;
					if((xi-xo)*(xi-xo)+(zi-zo)*(zi-zo)<=topSize*topSize)
					{
						float tTopHalfSize=(topSize<size? topSize:size);
						yy=Paraboloid(tTopHalfSize,0,y0,intensity);
					}
					else
						yy=Paraboloid(xi-xo,zi-zo,y0,intensity);

		            //update the buffer			
					yy=(yy > 0.0f? yy : 0.0f);


					tile->SetPosBufHeightAt(i, j, tile->m_pPositionBuffer[tile->_index(i, j) * 3 + 1] - yy / tile->m_pParentPage->m_Scale.y, tile->m_pPositionBuffer);				

				}
			}	
			}break;
	case eTerrain_Edit_Flat:/* flat */
			{				
				float xo=rect.getCenterX<float>();
				float zo=rect.getCenterZ<float>();
				float size=rect.getHalfSize<float>(); 				                
				
				float yy0=tile->GetHeightAt(xo,zo);                
	             		
				for(int j=sz_index;j<ez_index+1;j++)
				for (int i=sx_index;i<ex_index+1;i++)
				{
					float xi=tile->_vertex( i,j, 0 );
					float yi=tile->_vertex( i,j, 1 );
					float zi=tile->_vertex( i,j, 2 );

					float yy=yi;	

					if((xi-xo)*(xi-xo)+(zi-zo)*(zi-zo)<size*size)
					{	
						yy+=(yy0-yy)/(tile->m_pParentPage->m_TileSize);				
					}

					//update the buffer	
					tile->SetPosBufHeightAt(i, j, /*m_pPositionBuffer[_index(i, j) * 3 + 1] + */yy, tile->m_pPositionBuffer);				
				}	
			}break;
	case eTerrain_Edit_Smooth:/* smooth */
			{	
				float intensity = 4.0f;
			float xo=rect.getCenterX<float>();
			float zo=rect.getCenterZ<float>();
			float size=rect.getHalfSize<float>(); 

			for(int j=sz_index; j<ez_index;j++)
			for(int i=sx_index; i<ex_index;i++)
			{
				float xi=tile->_vertex( i,j, 0 );
				float yi=tile->_vertex( i,j, 1 );
				float zi=tile->_vertex( i,j, 2 );   
				float yy=yi;
				//do if in the ovel area
				if((xi-xo)*(xi-xo)+(zi-zo)*(zi-zo)<size*size)
				{
					float ny=tile->_vertex( i,j-1 < 0 ? 0 : j-1, 1 ); 
					float sy=tile->_vertex( i,j+1, 1 ); 
					float ey=tile->_vertex( i+1,j, 1 );
					float wy=tile->_vertex( i-1 < 0 ? 0 : i-1, j, 1 );

					/*
						* deal with border problem here
						*/

					if(i==0 && j==0 )///west and north     
					{	
						if(tile->m_Neighbors[ CTile::WEST ] && tile->m_Neighbors[ CTile::NORTH ])// * * *
						{											 // * * *
								                                        // * * * 
							wy=tile->m_Neighbors[ CTile::WEST ]->_vertex(tile->m_pParentPage->m_TileSize - 1,0, 1 ); 							
							ny=tile->m_Neighbors[ CTile::NORTH ]->_vertex(0,tile->m_pParentPage->m_TileSize - 1,1); 
							sy=tile->_vertex( i,j+1, 1 ); 
							ey=tile->_vertex( i+1,j, 1 );	
							yy+=((ny+sy+ey+wy)/intensity-yy)/intensity; 
							//SET THE THE NEIGHBRERS TO AVOID LEAK!
							tile->m_Neighbors[ CTile::WEST ]->ManualSetVertexbufferHgtAt(tile->m_pParentPage->m_TileSize , 0 , yy);
							tile->m_Neighbors[ CTile::NORTH ]->ManualSetVertexbufferHgtAt(0 , tile->m_pParentPage->m_TileSize,yy);
							tile->m_Neighbors[ CTile::WEST ]->m_Neighbors[ CTile::NORTH ]->ManualSetVertexbufferHgtAt(tile->m_pParentPage->m_TileSize , tile->m_pParentPage->m_TileSize,yy);
						}
						else if(tile->m_Neighbors[ CTile::WEST ])//* * *
						{                          //  *   
 							wy=tile->m_Neighbors[ CTile::WEST ]->_vertex(tile->m_pParentPage->m_TileSize - 1, 0,1); 							
							ny=tile->_vertex(0,0,1); 
							sy=tile->_vertex( i,j+1, 1 ); 
							ey=tile->_vertex( i+1,j, 1 );	
							yy+=((ny+sy+ey+wy)/intensity-yy)/intensity; 
						
							tile->m_Neighbors[ CTile::WEST ]->ManualSetVertexbufferHgtAt(tile->m_pParentPage->m_TileSize , 0 , yy);
						}
						else if(tile->m_Neighbors[ CTile::NORTH ])// *  
						{                           // * *
								                    // *
							wy=tile->_vertex(0,0,1);							
							ny=tile->m_Neighbors[ CTile::NORTH ]->_vertex(0,tile->m_pParentPage->m_TileSize - 1,1); 
							sy=tile->_vertex( i,j+1, 1 ); 
							ey=tile->_vertex( i+1,j, 1 );	
							yy+=((ny+sy+ey+wy)/intensity-yy)/intensity; 
						
							tile->m_Neighbors[ CTile::NORTH ]->ManualSetVertexbufferHgtAt(0 , tile->m_pParentPage->m_TileSize,yy);
						}
						else//reach the west and north corner haha  // *  *
						{                                           // *
							wy=tile->_vertex(0, 0, 1 ); 							
							ny=tile->_vertex(0, 0, 1 ); 
							sy=tile->_vertex( i,j+1, 1 ); 
							ey=tile->_vertex( i+1,j, 1 );	
							yy+=((ny+sy+ey+wy)/intensity-yy)/intensity; 
						}
							
					}	
					else if(i==0 )///west
					{
						if( tile->m_Neighbors[ CTile::WEST ])	
						{
							wy=tile->m_Neighbors[ CTile::WEST ]->_vertex(tile->m_pParentPage->m_TileSize - 1,j, 1 );
							ny=tile->_vertex( i,j-1, 1 ); 
							sy=tile->_vertex( i,j+1, 1 ); 
							ey=tile->_vertex( i+1,j, 1 );
							yy+=((ny+sy+ey+wy)/intensity-yy)/intensity;  
							//SET THE THE NEIGHBRERS TO AVOID LEAK!
							tile->m_Neighbors[ CTile::WEST ]->ManualSetVertexbufferHgtAt(tile->m_pParentPage->m_TileSize , j , yy);
						}
						else//reach the west border  // *  
						{                            // *
							wy=tile->_vertex(0, j, 1 ); 							
							ny=tile->_vertex( i,j-1, 1 ); 
							sy=tile->_vertex( i,j+1, 1 ); 
							ey=tile->_vertex( i+1,j, 1 );
							yy+=((ny+sy+ey+wy)/intensity-yy)/intensity; 
						}
					}	
					else if(j==0 )///NORTH
					{
						if(tile->m_Neighbors[ CTile::NORTH ])
						{
							ny=tile->m_Neighbors[ CTile::NORTH ]->_vertex(i,tile->m_pParentPage->m_TileSize - 1, 1 );
							sy=tile->_vertex( i,j+1, 1 ); 
							ey=tile->_vertex( i+1,j, 1 );
							wy=tile->_vertex( i-1,j, 1 );
							yy+=((ny+sy+ey+wy)/intensity-yy)/intensity;  
							//SET THE THE NEIGHBRERS TO AVOID LEAK!
							tile->m_Neighbors[ CTile::NORTH ]->ManualSetVertexbufferHgtAt(i,tile->m_pParentPage->m_TileSize , yy);		
						}
						else
						{															
							ny=tile->_vertex( i,0, 1 );
							sy=tile->_vertex( i,j+1, 1 ); 
							ey=tile->_vertex( i+1,j, 1 );
							wy=tile->_vertex( i-1,j, 1 );
							yy+=((ny+sy+ey+wy)/intensity-yy)/intensity; 
						}						    
					}	
					else if(i>0 && j>0)//other
					{ 	
				
				    		ny=tile->_vertex( i,j-1, 1 ); 
							sy=tile->_vertex( i,j+1, 1 ); 
							ey=tile->_vertex( i+1,j, 1 );
							wy=tile->_vertex( i-1,j, 1 );
							yy+=((ny+sy+ey+wy)/intensity-yy)/intensity;
					
					}							
					
					//update PositionBuffer				
					if(yy>=-MIN_FLOAT && yy < MAX_FLOAT)/*bug*/
					{
						tile->ManualSetVertexbufferHgtAt(i, j, yy);
					}
				}	
			}	
		}break;
		case eTerrain_Edit_SingleVertex: { // single vertex 
           
			float xo=rect.getCenterX<float>();
			float zo=rect.getCenterZ<float>();
			float size=rect.getHalfSize<float>()*20.0f;
            float yy=0.0f;
			for(int j = sz_index;  j < ez_index + 1; j ++)
			for(int i = sx_index;  i < ex_index + 1; i ++)
			{
				float xi=tile->_vertex( i,j, 0 );
				float zi=tile->_vertex( i,j, 2 );
				if((xi-xo)*(xi-xo)+(zi-zo)*(zi-zo)<size*size)
				{
			
					if((xi-xo)*(xi-xo) + (zi-zo)*(zi-zo) <= topSize*topSize)
					{				
						yy=1.0f*intensity;											
					}								
				}
				
				if(yy>=-MIN_FLOAT && yy<MAX_FLOAT)/*bug*/
				{

					tile->SetPosBufHeightAt(i, j, tile->m_pPositionBuffer[tile->_index(i, j) * 3 + 1] + yy, tile->m_pPositionBuffer);				
				}

			}		
		}break;
		case eTerrain_Edit_ManualHeight:/* ManualHeight */
		{				
			float xo=rect.getCenterX<float>();
			float zo=rect.getCenterZ<float>();
			float size=rect.getHalfSize<float>(); 				                
			
			float yy0 = intensity;                
	             	
			for(int j = sz_index; j < ez_index + 1; j++)
			for(int i = sx_index; i < ex_index + 1; i++)
			{
				float xi=tile->_vertex( i,j, 0 );
				float yi=tile->_vertex( i,j, 1 );
				float zi=tile->_vertex( i,j, 2 );

				float yy=yi;	

				if((xi-xo)*(xi-xo)+(zi-zo)*(zi-zo) < size*size)
				{	
					//yy+=(yy0-yy)/(tile->m_pParentPage->m_TileSize);	
					yy = yy0;
				}


				//update the buffer	
				tile->SetPosBufHeightAt(i, j, /*m_pPositionBuffer[_index(i, j) * 3 + 1] + */yy, tile->m_pPositionBuffer);				
			}	
		}break;	
		case eTerrain_Edit_ManualHeight_Rect:/* ManualHeight rect */
		{				
			float xo=rect.getCenterX<float>();
			float zo=rect.getCenterZ<float>();
			float size=rect.getHalfSize<float>(); 				                
			
			float yy0 = intensity;                
	             	
			for(int j = sz_index; j < ez_index + 1; j++)
			for(int i = sx_index; i < ex_index + 1; i++)
			{
				float xi=tile->_vertex( i,j, 0 );
				float yi=tile->_vertex( i,j, 1 );
				float zi=tile->_vertex( i,j, 2 );

				float yy=yi;	

				if(Ogre::Math::Abs(xi-xo) < size && Ogre::Math::Abs(zi-zo) < size)
				{					
					yy = yy0;
				}

				//update the buffer	
				tile->SetPosBufHeightAt(i, j, yy, tile->m_pPositionBuffer);				
			}	
		}break;		

		default:
				{}
	} 
    //update in the main buffer     
	CRectA<int> tnRect(sx_index,sz_index,ex_index+1,ez_index+1);
	tile->UpdateVertexHgtbuffer(tnRect); 

	// 更新法线

	if(type == eTerrain_Edit_ManualHeight_Rect) // 悬崖情况法线不更新
		//tile->AverageVertexNormal();
	//else
		tile->ClearVertexNormal();

}

//----------------------------------------------------------------------
void COTETerrainHeightMgr::SetTileHeightAt(COTETile* tile, float height)
{
	for(int j = 0; j < tile->m_pParentPage->m_TileSize + 1; j++)
	for(int i = 0; i < tile->m_pParentPage->m_TileSize + 1; i++)
	{
		//update the buffer	
		tile->SetPosBufHeightAt(i, j, height, tile->m_pPositionBuffer);				
	}	

	CRectA<int> rect(0, 0, tile->m_pParentPage->m_TileSize + 1, tile->m_pParentPage->m_TileSize + 1);
	tile->UpdateVertexHgtbuffer(rect); 
}

//----------------------------------------------------------------------
void COTETerrainHeightMgr::ExportHeightData(COTETile* tile, int startx, int startz, TileCreateData_t* pData)
{
	COTETilePage* page = tile->m_pParentPage;
	float* pSysPos = tile->m_pPositionBuffer;
	Ogre::Vector3* pSysNormal = tile->m_pNormalBuffer;

	int endx = startx + page->m_TileSize + 1;
    int endz = startz + page->m_TileSize + 1;

	int startUX = startx / tile->m_pParentPage->m_UnitSize;
	int startUZ = startz / tile->m_pParentPage->m_UnitSize;
	
	int j = startz;
    for ( int uZ = 0; uZ < (endz - startz - 1) / tile->m_pParentPage->m_UnitSize; uZ ++ )
    {
		int i = startx;
        for ( int uX = 0; uX < (endx - startx - 1) / tile->m_pParentPage->m_UnitSize; uX ++ )
        {     
			
			CTerrainUnit* tu = (CTerrainUnit*)tile->m_pTerrainUnitMap->m_Tiles[uX][uZ];			
			
			int uvInfOffset = uX + startUX + (uZ + startUZ) * ( (page->m_PageSizeX) / tile->m_pParentPage->m_UnitSize);
			CTerrainUnitData* pUnitData = pData->uvData + uvInfOffset;
			
			// 一个单元
			
			Real height = 0;
			
			for(int jj = 0; jj < tile->m_pParentPage->m_UnitSize + 1; jj ++)
			for(int ii = 0; ii < tile->m_pParentPage->m_UnitSize + 1; ii ++)
			{			
				*pSysPos++;	// x
				height = *pSysPos++;// y
				*pSysPos++;	// z	

				float scaleHeight = (height / page->m_Scale.y * 255.0);
				int iscaleHeight = int(scaleHeight);
				if(scaleHeight - iscaleHeight> 0.45)
					iscaleHeight = iscaleHeight + 1;
				else
					iscaleHeight = iscaleHeight;


				pData->heightData[(j + jj) * (page->m_PageSizeX + 1) + (i + ii)] = iscaleHeight;//(height / page->m_Scale.y) * 255;
				pData->normal[(j + jj) * (page->m_PageSizeX + 1) + (i + ii)] = *pSysNormal++;

			}
   
			i += tile->m_pParentPage->m_UnitSize;
        }

		j += tile->m_pParentPage->m_UnitSize;
    }

}



//----------------------------------------------------------------------
void COTETerrainHeightMgr::updateTerrainAllHeight(COTETilePage* page, TileCreateData_t* createData)
{
	int q = 0;
	for ( int j = 0; j < page->m_PageSizeZ; j += ( page->m_TileSize ) )	//此循环仿创建地形顶点数据时的循环
	{
		int p = 0;

		for ( int i = 0; i < page->m_PageSizeX; i += ( page->m_TileSize ) )
		{
			COTETile* tile = page->m_pPage->m_Tiles[ p ][ q ];
			if(!tile)
				break;      

			updateTerrainAllHeight(tile, i, j, createData);
			
			p++;
		}
		q++;
	}
}

// ----------------------------------------------------
void COTETerrainHeightMgr::updateTerrainAllHeight(COTETile* tile, int i, int j, TileCreateData_t* createData)
{
	const VertexElement* poselem = tile->m_Terrain->vertexDeclaration->findElementBySemantic(VES_POSITION);  
	const VertexElement* normail = tile->m_Terrain->vertexDeclaration->findElementBySemantic(VES_NORMAL); 
	
	float* pSysPos = tile->m_pPositionBuffer;
	Ogre::Vector3* pSysNormal = tile->m_pNormalBuffer;

	  
	int bufSize = tile->m_pParentPage->m_MainBuffer->getVertexSize() * tile->GetTileVertexCount();
    unsigned char* pBase0 = static_cast<unsigned char*>(tile->m_pParentPage->m_MainBuffer->lock(
									tile->m_IndexInPage * bufSize, bufSize,HardwareBuffer::HBL_DISCARD));

	int endx = i + tile->m_pParentPage->m_TileSize + 1;
	int endz = j + tile->m_pParentPage->m_TileSize + 1;

	int z = j;
	for(int jj=0; jj<(endz - j - 1)/tile->m_pParentPage->m_UnitSize; jj++)
	{
		int x = i;
		for(int ii = 0; ii<(endx - i - 1)/tile->m_pParentPage->m_UnitSize; ii++)
		{
			for(int jjj = 0; jjj<(tile->m_pParentPage->m_UnitSize+1); jjj++)
			{
				for(int iii = 0; iii<(tile->m_pParentPage->m_UnitSize+1); iii++)
				{	
					float *pPos, *pNormal;
					
					poselem->baseVertexPointerToElement(pBase0, &pPos);
					normail->baseVertexPointerToElement(pBase0, &pNormal);

					*pSysPos++;		*pPos++;
					*pSysPos++ = *pPos++ =(float)createData->heightData[(z + jjj) * (tile->m_pParentPage->m_PageSizeX + 1) + (x + iii)] / 255.0f * tile->m_pParentPage->m_Scale.y;
					*pSysPos++;		*pPos++;

					(*pSysNormal).x = *pNormal++ = createData->normal[(z + jjj) * (tile->m_pParentPage->m_PageSizeX + 1) + (x + iii)].x;
					(*pSysNormal).y = *pNormal++ = createData->normal[(z + jjj) * (tile->m_pParentPage->m_PageSizeX + 1) + (x + iii)].y;
					(*pSysNormal).z = *pNormal++ = createData->normal[(z + jjj) * (tile->m_pParentPage->m_PageSizeX + 1) + (x + iii)].z;
					pSysNormal++;

					pBase0 += tile->m_pParentPage->m_MainBuffer->getVertexSize();
				}
			}
			x += tile->m_pParentPage->m_UnitSize;
		}
		z += tile->m_pParentPage->m_UnitSize;
	}

	tile->m_pParentPage->m_MainBuffer->unlock();  
}


//----------------------------------------------------------------------
void COTETerrainHeightMgr::getVertexHeightData(COTETilePage* page, TileCreateData_t * createData)
{
	int q = 0;
    for ( int j = 0; j < page->m_PageSizeZ; j += ( page->m_TileSize ) )
    {
        int p = 0;
        for ( int i = 0; i < page->m_PageSizeX; i += ( page->m_TileSize ) )
        {	            
			// Attach it to the page
			COTETile* tile = page->m_pPage->m_Tiles[ p ][ q ];
			if(tile)
				ExportHeightData(tile, i, j, createData);        

            p++;
        }
        q++;
    }
	
}


}