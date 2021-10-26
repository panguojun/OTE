#pragma once

 // ������
#define GUI_ERR_OUTOFCODEPAGE	-1			// ����ռ����
#define	GUI_ERR_SUCCESS			0			// ���óɹ�
#define GUI_ERR_INVALIDFONT		1			// ��Ч����
#define GUI_ERR_INVALIDVP		2			// ��Ч�ӿ�

// �����������
#define GUI_DT_SINGLELINE		0x00000001	
#define GUI_DT_LEFT				0x00000002
#define GUI_DT_RIGHT			0x00000004
#define GUI_DT_TOP				0x00000008
#define	GUI_DT_BOTTOM			0x00000010
#define GUI_DT_VCENTER			0x00000020
#define GUI_DT_HCENTER			0x00000040

// �ؼ�״̬
#define GUI_CTL_VISIBLE			0x00000001
#define GUI_CTL_ENABLE			0x00000003
#define GUI_CTL_FOCUS			0x00000004
#define GUI_CTL_HOVER			0x00000008
#define GUI_CTL_ACTIVE			0x00000010
#define GUI_CTL_CLICKED			0x00000020
#define GUI_CTL_DISABLE			0x00000040

// �ؼ����
#define GUI_CSF_NORMAL			0x00000000	// ���ɸı�ؼ���С,û�йرհ�ť
#define GUI_CSF_TABSTOP			0x00000001	// ����ͨ��Tab��ý���
#define GUI_CSF_CONTAINER		0x00000002	// �����ؼ�
#define GUI_CSF_TOPMOST			0x00000004	// ������

#define GUI_MBS_NONE			0x00000000
#define GUI_MBS_OK				0x01000000
#define GUI_MBS_CANCEL			0x02000000

// �Ի�����
#define GUI_DSF_BORDER			0x00010000	// �Ƿ��б�
#define GUI_DSF_CAPTION			0x00020000	// ���ڱ���
#define GUI_DSF_SYSMENU			0x00080000	// �йرհ�ť
#define GUI_DSF_CANMOVE			0x00100000	// �����ƶ�
#define	GUI_DSF_HSCROLL			0x00200000	// �Ƿ���ʾ���������
#define	GUI_DSF_VSCROLL			0x00400000	// �Ƿ���ʾ���������

#define GUI_CR_CAPTION			0x00000001
#define GUI_CR_CLIENT			0x00000002
#define GUI_CR_VSCROLL			0x00000003
#define GUI_CR_HSCROLL			0x00000004
#define GUI_CR_PAGE				0x00000005

// �������
#define GUI_ESF_AUTOHSCROLL		0x01000000	// �����Զ����
#define	GUI_ESF_AUTOVSCROLL		0x02000000	// �����Զ��ݹ�, �����ESF_MUTILINE���ʹ��
#define	GUI_ESF_MUTILINE		0x04000000	// ���б༭
#define	GUI_ESF_SINGLELINE		0x08000000	// ���б༭, �ñ�ǲ��ܺ�ESF_MUTILINE,ESF_AUTOVSCROLLͬʱʹ��
#define GUI_ESF_READONLY		0x40000000	// ֻ������д
#define GUI_ESF_PASSWORD		0x80000000	// ���������

// ���������
#define GUI_SBF_HORZ			0x00010000	// ���������
#define GUI_SBF_VERT			0x00020000	// ���������

// ����������
#define GUI_SBR_PAGEDOWN		0x00000000	// �Ϸ�ҳ����
#define GUI_SBR_PAGEUP			0x00000001	// �·�ҳ����
#define GUI_SBR_CURSOR			0x00000002	// �������
#define GUI_SBR_BLANKDOWN		0x00000003	// �Ͽհ�����
#define GUI_SBR_BLANKUP			0x00000004	// �¿հ�����
#define GUI_SBR_NONE			0x00000005	// ������


// ���б�ؼ����
#define GUI_LCS_MOUSEOVER		0x0001000	// �Ƿ���Ⱦ������

// ���б�ؼ����
#define GUI_LCSFMT_LEFT			0x00010000	// �����
#define GUI_LCSFMT_CENTER		0x00020000	// ����
#define GUI_LCSFMT_RIGHT		0x00040000	// �Ҷ���

#define GUI_MSF_POPUPMENU		0x00000001