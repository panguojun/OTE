// ParticleEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ColorDlg.h"
#include "Plugin_ParticleEditor.h"
#include "ParticleEditDlg.h"
#include <sstream>
#include "EffDoc.h"
#include "SliderEditor.h"
#include "ListViewEditor.h"
#include "Ogre.h"
#include "particleeditdlg.h"

#include "VecEditor.h"

using namespace Ogre;
using namespace OTE;

// CParticleEditDlg 对话框

CParticleEditDlg CParticleEditDlg::s_Inst;

IMPLEMENT_DYNAMIC(CParticleEditDlg, CDialog)
CParticleEditDlg::CParticleEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CParticleEditDlg::IDD, pParent)
{
}

CParticleEditDlg::~CParticleEditDlg()
{
}

void CParticleEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_PorpList);
}


BEGIN_MESSAGE_MAP(CParticleEditDlg, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnLvnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, OnNMDblclkList1)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, OnNMClickList1)
END_MESSAGE_MAP()


// CParticleEditDlg 消息处理程序

void CParticleEditDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	
}

// --------------------------------------------------------
BOOL CParticleEditDlg::OnInitDialog()
{
	bool tRet = CDialog::OnInitDialog();	

	m_PorpList.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_MULTIWORKAREAS);
	m_PorpList.InsertColumn(0, "属性", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 75); 
	m_PorpList.InsertColumn(1, "值", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 130); 
	m_PorpList.InsertColumn(2, "类型", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 30);
	m_PorpList.InsertColumn(3, "用户数据", LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM, 30);
	
	UpdatePropertyList();

	return tRet;
}

// --------------------------------------------------------
// 操作函数

void OnSliderEdit(float v, CString ud)
{
	OTE::COTEParticleSystemRes* psr = CEffDoc::s_Inst.m_ParticleSystem->GetResPtr();
	COTESimpleEmitter* emitter = (COTESimpleEmitter*)psr->m_Emitter;	
	COTEParticleSystemAffector* affector = psr->m_ParticleSystemAffector;
	
	char name[64], gp[64]; 
	sscanf((char*)LPCTSTR(ud), "%s %s", name, gp);
	if(psr)
	{			
		if(strcmp(gp, "发射器属性") == 0)
			emitter->SetVal(name, v);	
		else 
			affector->SetVal(name, v);	

		emitter->Emite(CEffDoc::s_Inst.m_ParticleSystem);
		emitter->Show(CEffDoc::s_Inst.m_ParticleSystem);
		CParticleEditDlg::s_Inst.m_PorpList.InvalidateRect(NULL);		
	}
}

// 更新

void OnUpdateList()
{
	CParticleEditDlg::s_Inst.UpdatePropertyList();
	CParticleEditDlg::s_Inst.m_PorpList.InvalidateRect(NULL);
}

// --------------------------------------------------------
// 向量编辑

void OnVectorEdit(float x, float y, float z)
{
	OTE::COTEParticleSystemRes* psr = CEffDoc::s_Inst.m_ParticleSystem->GetResPtr();
	COTESimpleEmitter* emitter = (COTESimpleEmitter*)psr->m_Emitter;	
	COTEParticleSystemAffector* affector = psr->m_ParticleSystemAffector;
	{
		Ogre::Vector3 vecVal;		
		
		vecVal.x = x;
		vecVal.y = y;
		vecVal.z = z;
		
		char name[64], gp[64]; 
		sscanf((char*)LPCTSTR(CParticleEditDlg::s_Inst.m_VectorEditor.m_UserString), "%s %s", name, gp);
				
		if(strcmp(gp, "发射器属性") == 0)
			emitter->SetVal(name, vecVal);	
		else 
			affector->SetVal(name, vecVal);

		emitter->Emite(CEffDoc::s_Inst.m_ParticleSystem);

		static unsigned int sOldOper = ::GetTickCount();
		if(::GetTickCount() - sOldOper > 500) // 不要太频繁更新
		{
			CParticleEditDlg::s_Inst.UpdatePropertyList();
			CParticleEditDlg::s_Inst.InvalidateRect(NULL);
			sOldOper = ::GetTickCount();
		}
		
	}
}


// --------------------------------------------------------
// 颜色
void OnColorDlgNotify(DWORD cor, DWORD ud)
{
	OTE::COTEParticleSystemRes* psr = CEffDoc::s_Inst.m_ParticleSystem->GetResPtr();
	COTESimpleEmitter* emitter = (COTESimpleEmitter*)psr->m_Emitter;	
	COTEParticleSystemAffector* affector = psr->m_ParticleSystemAffector;
	{
		Ogre::ColourValue corVal;		
	
		corVal.r = GetRValue(cor) / 255.0f;
		corVal.g = GetGValue(cor) / 255.0f;
		corVal.b = GetBValue(cor) / 255.0f;
		
		char name[64], gp[64]; 
		sscanf((char*)LPCTSTR(CColorDlg::s_Inst.m_UserString), "%s %s", name, gp);
				
		if(strcmp(gp, "发射器属性") == 0)
			emitter->SetVal(name, corVal);	
		else 
			affector->SetVal(name, corVal);

		emitter->Emite(CEffDoc::s_Inst.m_ParticleSystem);

		static unsigned int sOldOper = ::GetTickCount();
		if(::GetTickCount() - sOldOper > 500) // 不要太频繁更新
		{
			CParticleEditDlg::s_Inst.UpdatePropertyList();
			CParticleEditDlg::s_Inst.InvalidateRect(NULL);
			sOldOper = ::GetTickCount();
		}
	}
}

// --------------------------------------------------------
#define DO_EDITOK(type)  CListViewEditor dlg(type, m_PorpList.GetItemText(tItem, tSubItem), tSubItemRect); if(IDOK == dlg.DoModal() && (!CListViewEditor::s_Text.IsEmpty() && type != 1 || type == 1))
void CParticleEditDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{	
	if(!CEffDoc::s_Inst.m_ParticleSystem)
		return;

	int tItem, tSubItem;

	CRect tSubItemRect;	
	
	Ogre::MaterialPtr mptr = CEffDoc::s_Inst.m_ParticleSystem->getMaterial();

	COTEParticleSystemRes* resPtr = CEffDoc::s_Inst.m_ParticleSystem->GetResPtr();

	COTESimpleEmitter* emitter = (COTESimpleEmitter*)resPtr->m_Emitter;

	COTEParticleSystemAffector* affector = resPtr->m_ParticleSystemAffector;

	if(m_PorpList.GetDClickedItem(tItem, tSubItem, tSubItemRect) && tSubItem == 1)
	{	
		switch(tItem)
		{	
		// 基本属性
		case 2:
			{	
				// material name
				//DO_EDITOK(0)
				{			
					CHAR curDir[256];
					::GetCurrentDirectory(256, curDir);
					   
					CWinApp* pApp = AfxGetApp();   

					static CString path;
					path = pApp->GetProfileString("OTE文件路径配置", "选择特效贴图");
					
					CFileDialog* tpFileDialog=new CFileDialog(true, // TRUE for FileOpen, FALSE for FileSaveAs
						NULL,
						NULL,
						OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
						"贴图文件(*.tga)|*.tga|贴图文件(*.dds)|*.dds|All Files (*.*)|*.*|",						
						NULL 
						); 
					tpFileDialog->m_ofn.lpstrInitialDir = path;
					int nResponse =tpFileDialog->DoModal();	

					::SetCurrentDirectory(curDir);

					if (nResponse == IDOK)
					{
						CString  strPath  = tpFileDialog->GetPathName().Left(tpFileDialog->GetPathName().GetLength() - 
																						tpFileDialog->GetFileName().GetLength());   
						pApp->WriteProfileString("OTE文件路径配置", "选择特效贴图", strPath);
				
						//resPtr->m_MaterialName = atof(CListViewEditor::s_Text);
						Ogre::Pass* pass = mptr->getBestTechnique(0)->getPass(0);
						if(pass->getNumTextureUnitStates() == 0)
						{
							pass->createTextureUnitState();
						}							
						Ogre::TextureUnitState* tus = pass->getTextureUnitState(0);							
						resPtr->m_TextureName = tpFileDialog->GetFileName().GetString();
						tus->setTextureName	(resPtr->m_TextureName);					
					}
					delete tpFileDialog;
				
					UpdatePropertyList();	
				}				
		
			}return;	
		case 3:
			{	
				// poolSize
				DO_EDITOK(0)
				{								
					resPtr->m_PoolSize = atof(CListViewEditor::s_Text);
					resPtr->CreateBuffer();

					UpdatePropertyList();
				}
		
			}return;		

		}

		CString name = m_PorpList.GetItemText(tItem, 0);
		CString typeName = m_PorpList.GetItemText(tItem, 2);
		CString ud = m_PorpList.GetItemText(tItem, 3);

		// float 类型数据用slider编辑

		if(typeName == "float")
		{
			std::map<Ogre::String, std::pair<int, Ogre::Real> >* floats;
			
			if(ud == "发射器属性")
				floats = &emitter->m_floats;
			else
				floats = &affector->m_floats;

			std::map<Ogre::String, std::pair<int, Ogre::Real> >::iterator floatIt = floats->begin();
		
			while(floatIt != floats->end())
			{					
				if(name.GetString() == floatIt->first)
				{					
					CSliderEditor::s_Inst.Show(
						    OnSliderEdit, 
							OnUpdateList, 
							(name + " " + ud), 
							(floatIt->second).second, 
						  /* -Ogre::Math::Abs((floatIt->second).second - ((floatIt->second).second != 0.0f ? (floatIt->second).second * 10.0f : 1.0f))*/
						    0.0f,
							Ogre::Math::Abs((floatIt->second).second + ((floatIt->second).second != 0.0f ? (floatIt->second).second * 10.0f : 1.0f)),
							CRect(CPoint(tSubItemRect.left, tSubItemRect.top), CSize(130, 25))							 
							);
					
					return;
				}

				++ floatIt;
			}	
			return;
		}

		else if(typeName == "Ogre::ColourValue")
		{
			std::map<Ogre::String, std::pair<int, Ogre::ColourValue> >* colors;
			
			if(ud == "发射器属性")
				colors = &emitter->m_colors;
			else
				colors = &affector->m_colors;

			std::map<Ogre::String, std::pair<int, Ogre::ColourValue> >::iterator corIt = colors->begin();
		
			while(corIt != colors->end())
			{					
				if(name.GetString() == corIt->first)
				{
					CColorDlg::s_Inst.Show(true, (corIt->second).second.getAsABGR());
					CColorDlg::s_Inst.SetNotifyFunc(OnColorDlgNotify);	
				
					CColorDlg::s_Inst.m_UserString = name + " " + ud;

					return;
				}

				++ corIt;
			}	
			return;
		}

		else if(typeName == "int")
		{
			std::map<Ogre::String, std::pair<int, int> >* ints;

			if(ud == "发射器属性")
				ints = &emitter->m_ints;
			else
				ints = &affector->m_ints;

			std::map<Ogre::String, std::pair<int, int> >::iterator it = ints->begin();
		
			while(it != ints->end())
			{					
				if(name.GetString() == it->first)
				{
					DO_EDITOK(0)
					{
						(it->second).second = atoi(CListViewEditor::s_Text);
						UpdatePropertyList();
					}

					return;
				}

				++ it;
			}	
			return;
		}	
	
		else if(typeName == "Ogre::Vector3")
		{
			std::map<Ogre::String, std::pair<int, Ogre::Vector3> >* vector3;
			
			if(ud == "发射器属性")
				vector3 = &emitter->m_vector3s;
			else
				vector3 = &affector->m_vector3s;

			std::map<Ogre::String, std::pair<int, Ogre::Vector3> >::iterator vecIt = vector3->begin();
		
			while(vecIt != vector3->end())
			{					
				if(name.GetString() == vecIt->first)
				{
					Ogre::Vector3 vec = (vecIt->second).second;
					m_VectorEditor.m_UserString = name + " " + ud;		
					m_VectorEditor.Show(
						OnVectorEdit, 
						vec.x, vec.y, vec.z, 
					   -Ogre::Math::Abs(vec.length() != 0.0f ? vec.length() * 10.0f : 1.0f), 
						Ogre::Math::Abs(vec.length() != 0.0f ? vec.length() * 10.0f : 1.0f), 
						name);					

					return;
				}

				++ vecIt;
			}	
			return;
		}
	}


}

// --------------------------------------------------------
#define SET_PROPERTY_ITEM(label, content, typeName, ud)		{ss.str("");ss << content; m_PorpList.InsertItem(item_count, label);m_PorpList.SetItemText(item_count, 1, ss.str().c_str());m_PorpList.SetItemText(item_count, 2, typeName);m_PorpList.SetItemText(item_count, 3, ud);item_count ++;}	
#define SET_VEC_PROPERTY_ITEM(label, content, typeName, ud) {ss.str("");ss << content.x << " " << content.y << " " << content.z; m_PorpList.InsertItem(item_count, label);m_PorpList.SetItemText(item_count, 1, ss.str().c_str());m_PorpList.SetItemText(item_count, 2, typeName);m_PorpList.SetItemText(item_count, 3, ud);item_count ++;}	
#define SET_COR_PROPERTY_ITEM(label, content, typeName, ud) {ss.str("");ss << content.r << " " << content.g << " " << content.b << " " << content.a; m_PorpList.InsertItem(item_count, label);m_PorpList.SetItemText(item_count, 1, ss.str().c_str());m_PorpList.SetItemText(item_count, 2, typeName);m_PorpList.SetItemText(item_count, 3, ud);item_count ++;}	

#define SET_PROPERTY_GROUP(type, oplist, ud)	 {std::map<Ogre::String, std::pair<int, type> >::iterator it = oplist.begin();	while(it != oplist.end()){	SET_PROPERTY_ITEM(it->first.c_str(),		it->second.second, #type, ud)	++ it;}}	
#define SET_VEC_PROPERTY_GROUP(type, oplist, ud) {std::map<Ogre::String, std::pair<int, type> >::iterator it = oplist.begin();	while(it != oplist.end()){	SET_VEC_PROPERTY_ITEM(it->first.c_str(),	it->second.second, #type, ud)	++ it;}}	
#define SET_COR_PROPERTY_GROUP(type, oplist, ud) {std::map<Ogre::String, std::pair<int, type> >::iterator it = oplist.begin();	while(it != oplist.end()){	SET_COR_PROPERTY_ITEM(it->first.c_str(),	it->second.second, #type, ud)	++ it;}}	


// --------------------------------------------------------
void CParticleEditDlg::UpdatePropertyList()
{
	if(!CEffDoc::s_Inst.m_ParticleSystem)
		return;

	int item_count = 0;

	std::stringstream ss;
	m_PorpList.DeleteAllItems();

	COTEParticleSystemRes* resPtr = CEffDoc::s_Inst.m_ParticleSystem->GetResPtr();

	SET_PROPERTY_ITEM("基本属性", "-----------------", "", "")

		SET_PROPERTY_ITEM("materialName",				resPtr->m_MaterialName, "string", "基本属性")
		SET_PROPERTY_ITEM("textureName",				resPtr->m_TextureName, "string", "基本属性")
		SET_PROPERTY_ITEM("poolSize",					resPtr->m_PoolSize, "int", "基本属性")
		//SET_PROPERTY_ITEM("liftTime",					resPtr->m_LiftTime, "float", "基本属性")		

	COTESimpleEmitter* emitter = (COTESimpleEmitter*)resPtr->m_Emitter;	
	
	SET_PROPERTY_ITEM("发射器属性", "-----------------", "", "")	
		
		SET_PROPERTY_GROUP(int, emitter->m_ints, "发射器属性")
		
		SET_PROPERTY_GROUP(float, emitter->m_floats, "发射器属性")
		
		SET_VEC_PROPERTY_GROUP(Ogre::Vector3, emitter->m_vector3s, "发射器属性")

		SET_COR_PROPERTY_GROUP(Ogre::ColourValue, emitter->m_colors, "发射器属性")

	COTEParticleSystemAffector* affector = resPtr->m_ParticleSystemAffector;

	SET_PROPERTY_ITEM("影响器属性", "-----------------", "", "")		
	{
		SET_PROPERTY_GROUP(int, affector->m_ints, "影响器属性")

		SET_PROPERTY_GROUP(float, affector->m_floats, "影响器属性")
		
		SET_VEC_PROPERTY_GROUP(Ogre::Vector3, affector->m_vector3s, "影响器属性")

		SET_COR_PROPERTY_GROUP(Ogre::ColourValue, affector->m_colors, "影响器属性")
	}

	// 更新
	emitter->Emite(CEffDoc::s_Inst.m_ParticleSystem);

	this->m_PorpList.Invalidate();
}

// --------------------------------------------------------
void CParticleEditDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{	
	CParticleEditDlg::s_Inst.Invalidate();
}
