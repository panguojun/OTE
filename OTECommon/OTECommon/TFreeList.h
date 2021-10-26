#pragma once
#ifndef FreeListAssert
#define FreeListAssert( p ) if( !p ) __asm int 3;
#endif

// *******************************************
// TFreeList
// *******************************************
template< class FLDataType >
class TFreeList
{
public:
	TFreeList( int vMaxSize )
	{
		mpDataPtr = new FLDataType*[ vMaxSize ];
		mpData = new FLDataType[ vMaxSize ];
		FreeListAssert( mpData );
		FreeListAssert( mpDataPtr );
		mMaxSize = vMaxSize;
		mCurIdx = mMaxSize;
		mFreeOnDestroy = true;
		Init();

	}
	TFreeList( FLDataType* pData, FLDataType** pDataPtr, int vMaxSize )
	{
		Init( pData, pDataPtr, vMaxSize );
	}
	TFreeList()
	{
		mpDataPtr = NULL;
		mpData = NULL;
		mMaxSize = 0;
		mCurIdx = 0;
		mFreeOnDestroy = false;
		mInit = false;
	}
	void Init( FLDataType* pData, FLDataType** pDataPtr, int vMaxSize )
	{
		mpDataPtr = pDataPtr;
		mpData = pData;
		FreeListAssert( mpData );
		FreeListAssert( mpDataPtr );
		mMaxSize = vMaxSize;
		mCurIdx = mMaxSize;
		mFreeOnDestroy = false;
		Init();
	}
	void Finalize()
	{
		if( mFreeOnDestroy && mInit )
		{//销毁所有数据
			delete[] mpData;
			delete[] mpDataPtr;
			mInit = false;
		}
	}
	~TFreeList()
	{
		Finalize();
	}
	//新实例
	FLDataType* NewInstance()
	{
		FreeListAssert( mInit );
		if( mCurIdx == 0 )
			return NULL;
		return mpDataPtr[ --mCurIdx ];
	}
	//删除实例
	void FreeInstance( FLDataType* pInstance )
	{
		FreeListAssert( mInit );
		if( !pInstance )
			return;
		FreeListAssert( ( pInstance >= &mpData[ 0 ] ) && ( pInstance <= &mpData[ mMaxSize - 1 ] ) );
		FreeListAssert( ( mCurIdx < mMaxSize ) );
		mpDataPtr[ mCurIdx++ ] = pInstance;
	}
	//得到当前元素剩余量
	int GetCurSize()
	{
		FreeListAssert( mInit );
		return mCurIdx;
	}
	int GetMaxSize()
	{
		FreeListAssert( mInit );
		return mMaxSize;
	}
	FLDataType* GetData()
	{
		return mpData;
	}
private:
	void Init()
	{
		for( int k = 0; k < mMaxSize ;++k )
		{
			mpDataPtr[ k ] = &mpData[ k ];
		}
		mInit = true;
	}
private:
	FLDataType* mpData;
	FLDataType** mpDataPtr;
	//列表总长
	int mMaxSize;
	//当前列表内剩余元素的数量
	int mCurSize;
	//指向最后一个可用元素的下一个元素
	int mCurIdx;
	//列表销毁时,将数据全部销毁
	bool mFreeOnDestroy;
	//是否已经初始化
	bool mInit;
};