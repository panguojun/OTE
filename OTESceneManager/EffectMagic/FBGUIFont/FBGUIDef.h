#pragma once

 // 错误码
#define GUI_ERR_OUTOFCODEPAGE	-1			// 内码空间溢出
#define	GUI_ERR_SUCCESS			0			// 调用成功
#define GUI_ERR_INVALIDFONT		1			// 无效字体
#define GUI_ERR_INVALIDVP		2			// 无效视口

// 字体输出控制
#define GUI_DT_SINGLELINE		0x00000001	
#define GUI_DT_LEFT				0x00000002
#define GUI_DT_RIGHT			0x00000004
#define GUI_DT_TOP				0x00000008
#define	GUI_DT_BOTTOM			0x00000010
#define GUI_DT_VCENTER			0x00000020
#define GUI_DT_HCENTER			0x00000040

// 控件状态
#define GUI_CTL_VISIBLE			0x00000001
#define GUI_CTL_ENABLE			0x00000003
#define GUI_CTL_FOCUS			0x00000004
#define GUI_CTL_HOVER			0x00000008
#define GUI_CTL_ACTIVE			0x00000010
#define GUI_CTL_CLICKED			0x00000020
#define GUI_CTL_DISABLE			0x00000040

// 控件风格
#define GUI_CSF_NORMAL			0x00000000	// 不可改变控件大小,没有关闭按钮
#define GUI_CSF_TABSTOP			0x00000001	// 可以通过Tab获得焦点
#define GUI_CSF_CONTAINER		0x00000002	// 容器控件
#define GUI_CSF_TOPMOST			0x00000004	// 最上面

#define GUI_MBS_NONE			0x00000000
#define GUI_MBS_OK				0x01000000
#define GUI_MBS_CANCEL			0x02000000

// 对话框风格
#define GUI_DSF_BORDER			0x00010000	// 是否有边
#define GUI_DSF_CAPTION			0x00020000	// 窗口标题
#define GUI_DSF_SYSMENU			0x00080000	// 有关闭按钮
#define GUI_DSF_CANMOVE			0x00100000	// 可以移动
#define	GUI_DSF_HSCROLL			0x00200000	// 是否显示横向滚动条
#define	GUI_DSF_VSCROLL			0x00400000	// 是否显示纵向滚动条

#define GUI_CR_CAPTION			0x00000001
#define GUI_CR_CLIENT			0x00000002
#define GUI_CR_VSCROLL			0x00000003
#define GUI_CR_HSCROLL			0x00000004
#define GUI_CR_PAGE				0x00000005

// 输入框风格
#define GUI_ESF_AUTOHSCROLL		0x01000000	// 可以自动横滚
#define	GUI_ESF_AUTOVSCROLL		0x02000000	// 可以自动纵滚, 必须和ESF_MUTILINE配合使用
#define	GUI_ESF_MUTILINE		0x04000000	// 多行编辑
#define	GUI_ESF_SINGLELINE		0x08000000	// 单行编辑, 该标记不能和ESF_MUTILINE,ESF_AUTOVSCROLL同时使用
#define GUI_ESF_READONLY		0x40000000	// 只读不可写
#define GUI_ESF_PASSWORD		0x80000000	// 密码输入框

// 滚动条风格
#define GUI_SBF_HORZ			0x00010000	// 横向滚动条
#define GUI_SBF_VERT			0x00020000	// 纵向滚动条

// 滚动条区域
#define GUI_SBR_PAGEDOWN		0x00000000	// 上翻页区域
#define GUI_SBR_PAGEUP			0x00000001	// 下翻页区域
#define GUI_SBR_CURSOR			0x00000002	// 光标区域
#define GUI_SBR_BLANKDOWN		0x00000003	// 上空白区域
#define GUI_SBR_BLANKUP			0x00000004	// 下空白区域
#define GUI_SBR_NONE			0x00000005	// 空区域


// 单列表控件风格
#define GUI_LCS_MOUSEOVER		0x0001000	// 是否渲染鼠标跟随

// 多列表控件风格
#define GUI_LCSFMT_LEFT			0x00010000	// 左对齐
#define GUI_LCSFMT_CENTER		0x00020000	// 居中
#define GUI_LCSFMT_RIGHT		0x00040000	// 右对齐

#define GUI_MSF_POPUPMENU		0x00000001