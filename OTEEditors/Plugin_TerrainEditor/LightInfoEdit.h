#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <string>
#include <vector>
#include "RenderLight.h"
#include "EditListCtrl.h"


// CLightInfoEdit 对话框


class CLightInfoEdit : public CDialog
{
	DECLARE_DYNAMIC(CLightInfoEdit)

public:	

	static CLightInfoEdit s_Inst;

	CLightInfoEdit(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLightInfoEdit();
	
	std::string m_strCurrentHeightMapFileName;

	// 对话框数据
	enum { IDD = ID_LightEdit };	

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();

	//virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
	//	AFX_CMDHANDLERINFO* pHandlerInfo);

	DECLARE_MESSAGE_MAP()

protected:
	/*unsigned*/ int m_Y;
	/*unsigned*/ int m_Cb;
	/*unsigned*/ int m_Cr;

	int m_lightType;

	float m_aryColor[4];	//用于传递颜色给调色窗口

private :
	void GetLightColorList(float* ary, int arySize, int item);
	
public :
	//时间列表添加全部时间成员
	void SunInfoAddAllItem();

	void SetLightCorrelationObject(int index);


	void UpdateSunInfo(CString strTime);

	void UpdateSunInfo();


	void updateLightInfoList(int i, int action);//更新列表
	void updateLightInfoList(CString strTime, int cType, CString strValue);	

	int getAlterObject(int item, int subitem);

	void setLightInfo(int i);

	void initSilder();

	void silderBindColor(float fbrightness);
	void silderBindColor();

	//更新与灯光信息有关的所有控件及列表数据
	//lightType:所要修改灯光颜色的标识(灯光列表中行号)
	void updateLightColorGroupControler(int lightType, unsigned int r, unsigned int g, unsigned int b, unsigned int a);

	void updateBrightnessLabel();

	void convertColorToYCC(unsigned int r, unsigned int g, unsigned int b);

	void convertColorToRGB(/*unsigned*/ int& r, /*unsigned*/ int& g, /*unsigned*/ int& b, /*unsigned*/ int& a);

	void convertColorFloatToInt(float fr, float fg, float fb, float fa, unsigned int& r, unsigned int& g, unsigned int& b, unsigned int& a);

	void setSilderCorrelation(unsigned int r, unsigned int g, unsigned int b);	//设置亮度滑块要用的相关操作

	//光时间点选择改变时同时更新灯光颜色
	void changeSunTimeThenChangeLightColor();

	//清空时间点列所有值
	void clearLightinfoList();

public :
	void InitLightColorList();


public:

	void OnKeyDown(unsigned int keyCode, bool isShifDwon);

	CEdit txtAngeToSun;
	afx_msg void OnBnClickedSunpos();

	CEditListCtrl m_SunTimeNodeList;
	CEditListCtrl m_LightColorList;

	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButton1();
	
public:

	enum{map512=0, map128=1};
	int m_lightmapSize;
	CButton m_radLightMap256;

	afx_msg void OnBnClickedRadio512();
	afx_msg void OnBnClickedRadio256();
	afx_msg void OnBnClickedBtnsavelight();
	// 亮度调节控件
	CSliderCtrl m_SilderBrightness;
	afx_msg void OnNMCustomdrawSliderbrightness(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList2(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic m_lblBrightness;
	afx_msg void OnLvnItemchangedList2(NMHDR *pNMHDR, LRESULT *pResult);
};
