// MatLibDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Plugin_MaterialEditor.h"
#include "MatLibDlg.h"
#include "MatLibDlg.h"
#include "OTEMagicManager.h"
#include "OTEShadowRender.h"
#include "OTETrack.h"


using namespace Ogre;
using namespace OTE;

// CMatLibDlg �Ի���

IMPLEMENT_DYNAMIC(CMatLibDlg, CDialog)
CMatLibDlg::CMatLibDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMatLibDlg::IDD, pParent)
{
}

CMatLibDlg::~CMatLibDlg()
{
}

void CMatLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMatLibDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
END_MESSAGE_MAP()

// ------------------------------------------
BOOL CMatLibDlg::OnInitDialog()
{
	BOOL ret = CDialog::OnInitDialog();

	LoadMatLib();

	return ret;
}

// ------------------------------------------
// ���ز��ʿ�
OTE::COTEActorEntity*		g_ObjPtr = NULL;
void CMatLibDlg::LoadMatLib()
{
	g_ObjPtr = (OTE::COTEActorEntity*)MAGIC_MGR->AutoCreateMagic("������������.mesh");
	//g_ObjPtr->setMaterialName();

	WAIT_LOADING_RES(g_ObjPtr)

	COTERenderTexture* rt = COTERenderTexture::Create("matlib_rt_1", 128, 128);

	rt->BindObj(g_ObjPtr);	

	Ogre::TexturePtr mmTex = rt->GetMMTexture();

	mmTex->getBuffer()->lock( HardwareBuffer::HBL_READ_ONLY );
	const PixelBox& pixBox	= mmTex->getBuffer()->getCurrentLock();

	unsigned int* pBuf	= (unsigned int*)pixBox.data;

	pBuf += pixBox.rowPitch * (pixBox.getHeight()/2);
	pBuf += pixBox.getWidth() / 2;
	unsigned int index	= *pBuf;

	mmTex->getBuffer()->unlock();

}
// ------------------------------------------
void CMatLibDlg::OnBnClickedButton2()
{

}
