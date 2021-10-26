#include "stdafx.h"
#include "Plugin_MaterialEditor.h"
#include "ShaderFXEditor.h"
#include "MaterialEditorMainDlg.h"

#include "OgreHighLevelGpuProgramManager.h"
#include "OgreD3D9HLSLProgram.h"
#include "OgreGpuProgram.h"

#include "OTEShader.h"
#include "OTEQTSceneManager.h"
#include ".\shaderfxeditor.h"

// -----------------------------------------------
#	define	TRACE_MSG(msg) {std::stringstream __ss__; __ss__ << msg << "\n"; m_ParamList.AddString( __ss__.str().c_str() ); }

// -----------------------------------------------
using namespace Ogre;
using namespace OTE;

// -----------------------------------------------

IMPLEMENT_DYNAMIC(CShaderFXEditor, CDialog)
CShaderFXEditor::CShaderFXEditor(CWnd* pParent /*=NULL*/)
	: CDialog(CShaderFXEditor::IDD, pParent)
{
}

CShaderFXEditor::~CShaderFXEditor()
{
}

void CShaderFXEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ParamList);
}


BEGIN_MESSAGE_MAP(CShaderFXEditor, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButton5)
END_MESSAGE_MAP()

// --------------------------------------------------------
// 指定shader到材质
// 检查匹配　
// --------------------------------------------------------

void CShaderFXEditor::TestVS(const std::string& hlslName)
{
	if(!CMaterialEditorMainDlg::s_Inst.m_pMat)
		return;

	Ogre::Pass* pass = CMaterialEditorMainDlg::s_Inst.m_pMat->getTechnique(0)->getPass(0);
	
	// 确保 在使用shader之前 把纹理邦定到对应层的坐标
	for(unsigned int i = 0; i < pass->getNumTextureUnitStates(); ++ i)
	{
		pass->getTextureUnitState(i)->setTextureCoordSet(i);
	}

	// 创建 shader

	GpuProgramPtr gpptr = Ogre::HighLevelGpuProgramManager::getSingleton().getByName(hlslName);
	if(gpptr.isNull())
	{
		gpptr = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
			hlslName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			"hlsl", GPT_VERTEX_PROGRAM);	
		((D3D9HLSLProgram*)gpptr.getPointer())->setEntryPoint("vs_main");
		Ogre::DataStreamPtr ds = Ogre::ResourceGroupManager::getSingleton().openResource(hlslName);
	
		((D3D9HLSLProgram*)gpptr.getPointer())->setSource(ds->getAsString());
		((D3D9HLSLProgram*)gpptr.getPointer())->setTarget("vs_2_0");
		gpptr->loadFromSource();
	}
	if(!gpptr->isSupported())
	{
		OTE_MSG_ERR("Shader版本本机不支持！")
		return;
	}

	m_ParamList.ResetContent();

	std::list<COTEShaderParams::ParamElm_t> paramList;
	COTEShaderParams::PopulateParameterNames((D3D9HLSLProgram*)gpptr.getPointer(), paramList);
	std::list<COTEShaderParams::ParamElm_t>::iterator it = paramList.begin();

	int errorCnt = 0;
	while(it != paramList.end())
	{
		if((*it).second.empty())
		{
			++ it;
			continue;
		}

		Ogre::StringVector params;
		params.resize(2);
		params[1] = (*it).second;

	    bool match = MaterialSerializer::ProcessAutoProgramParam((*it).first, "param_named_auto", 
					params, NULL);

		if(match)
			TRACE_MSG((*it).second + "(match)")
		else
		{
			errorCnt ++;
			TRACE_MSG((*it).second)	
		}

		++ it;
	}
	
	if(errorCnt != 0)
		OTE_MSG_ERR("有" << errorCnt <<"个常量名称不匹配！")
	else
	{
		pass->setVertexProgram(hlslName);

		Ogre::GpuProgramParametersSharedPtr gppPtr = pass->getVertexProgramParameters(); 
		std::list<COTEShaderParams::ParamElm_t>::iterator it = paramList.begin();		
		while(it != paramList.end())
		{
			Ogre::StringVector params;
			params.resize(2);
			params[1] = (*it).second;
			MaterialSerializer::ProcessAutoProgramParam(
									(*it).first, 
									"param_named_auto", 
									params, 
									gppPtr.getPointer(),
									_SETUP_PARAMS
									);
			++ it;
		}
	}
}

// ------------------------------------------------------
void CShaderFXEditor::TestPS(const std::string& hlslName)
{	
	if(!CMaterialEditorMainDlg::s_Inst.m_pMat)
		return;

	Ogre::Pass* pass = CMaterialEditorMainDlg::s_Inst.m_pMat->getTechnique(0)->getPass(0);

	// 确保 在使用shader之前 把纹理邦定到对应层的坐标
	for(unsigned int i = 0; i < pass->getNumTextureUnitStates(); ++ i)
	{
		pass->getTextureUnitState(i)->setTextureCoordSet(i);
	}

	// 创建 shader

	GpuProgramPtr gpptr = Ogre::HighLevelGpuProgramManager::getSingleton().getByName(hlslName);
	if(gpptr.isNull())
	{
		gpptr = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram(
			hlslName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			"hlsl", GPT_FRAGMENT_PROGRAM);	
		((D3D9HLSLProgram*)gpptr.getPointer())->setEntryPoint("ps_main");
		Ogre::DataStreamPtr ds = Ogre::ResourceGroupManager::getSingleton().openResource(hlslName);
	
		((D3D9HLSLProgram*)gpptr.getPointer())->setSource(ds->getAsString());
		((D3D9HLSLProgram*)gpptr.getPointer())->setTarget("ps_2_0");
		gpptr->loadFromSource();
	}
	if(!gpptr->isSupported())
	{
		OTE_MSG_ERR("Shader版本本机不支持！")
		return;
	}

	m_ParamList.ResetContent();

	std::list<COTEShaderParams::ParamElm_t> paramList;
	COTEShaderParams::PopulateParameterNames((D3D9HLSLProgram*)gpptr.getPointer(), paramList);
	std::list<COTEShaderParams::ParamElm_t>::iterator it = paramList.begin();

	int errorCnt = 0;
	while(it != paramList.end())
	{
		if((*it).second.empty())
		{
			++ it;
			continue;
		}
		Ogre::StringVector params;
		params.resize(2);
		params[1] = (*it).second;

	    bool match = MaterialSerializer::ProcessAutoProgramParam((*it).first, "param_named_manual", 
					params, NULL);

		if(match)			
			TRACE_MSG((*it).second + "(match)")
		else
		{
			errorCnt ++;
			TRACE_MSG((*it).second)		
		}

		++ it;
	}
	
	if(errorCnt != 0)
		OTE_MSG_ERR("有" << errorCnt <<"个常量名称不匹配！")
	else
	{
		pass->setFragmentProgram(hlslName);

		Ogre::GpuProgramParametersSharedPtr gppPtr = pass->getFragmentProgramParameters(); 
		std::list<COTEShaderParams::ParamElm_t>::iterator it = paramList.begin();		
		while(it != paramList.end())
		{
			Ogre::StringVector params;
			params.resize(2);
			params[1] = (*it).second;
			MaterialSerializer::ProcessAutoProgramParam(
									(*it).first, 
									"param_named_manual", 
									params, 
									gppPtr.getPointer(),
									_SETUP_PARAMS
									);
			++ it;
		}
	}
}


// --------------------------------------------------------
// 打开
// --------------------------------------------------------
void CShaderFXEditor::OnBnClickedButton2()
{
	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"vertex shader 文件(*.vs.hlsl)|*.vs.hlsl|All Files (*.*)|*.*|",
		 NULL 
		); 
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{		
		TestVS(tpFileDialog->GetFileName().GetString());	
	}

	delete tpFileDialog;

}
// --------------------------------------------------------
void CShaderFXEditor::OnBnClickedButton3()
{
	CHAR curDir[256];
	::GetCurrentDirectory(256, curDir);
	CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
		 NULL,
		 NULL,
		 OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"pixel shader 文件(*.ps.hlsl)|*.ps.hlsl|All Files (*.*)|*.*|",
		 NULL 
		); 
	int nResponse =tpFileDialog->DoModal();
	::SetCurrentDirectory(curDir);

	if (nResponse == IDOK)
	{
		TestPS(tpFileDialog->GetFileName().GetString());		
	}

	delete tpFileDialog;

	//OnOK();
}

// --------------------------------------------------------
void CShaderFXEditor::OnLbnSelchangeList1()
{
	// TODO: 在此添加控件通知处理程序代码
}

void CShaderFXEditor::OnBnClickedButton4()
{
	OnOK();
}

void CShaderFXEditor::OnBnClickedButton5()
{
	OnCancel();
}
