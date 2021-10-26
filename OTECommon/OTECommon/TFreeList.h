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
		{//������������
			delete[] mpData;
			delete[] mpDataPtr;
			mInit = false;
		}
	}
	~TFreeList()
	{
		Finalize();
	}
	//��ʵ��
	FLDataType* NewInstance()
	{
		FreeListAssert( mInit );
		if( mCurIdx == 0 )
			return NULL;
		return mpDataPtr[ --mCurIdx ];
	}
	//ɾ��ʵ��
	void FreeInstance( FLDataType* pInstance )
	{
		FreeListAssert( mInit );
		if( !pInstance )
			return;
		FreeListAssert( ( pInstance >= &mpData[ 0 ] ) && ( pInstance <= &mpData[ mMaxSize - 1 ] ) );
		FreeListAssert( ( mCurIdx < mMaxSize ) );
		mpDataPtr[ mCurIdx++ ] = pInstance;
	}
	//�õ���ǰԪ��ʣ����
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
	//�б��ܳ�
	int mMaxSize;
	//��ǰ�б���ʣ��Ԫ�ص�����
	int mCurSize;
	//ָ�����һ������Ԫ�ص���һ��Ԫ��
	int mCurIdx;
	//�б�����ʱ,������ȫ������
	bool mFreeOnDestroy;
	//�Ƿ��Ѿ���ʼ��
	bool mInit;
};