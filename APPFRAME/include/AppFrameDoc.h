// AppFrameDoc.h :  CAppFrameDoc ��Ľӿ�
//


#pragma once

class CAppFrameDoc : public CDocument
{
protected: // �������л�����
	CAppFrameDoc();
	DECLARE_DYNCREATE(CAppFrameDoc)

// ����
public:

// ����
public:

// ��д
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CAppFrameDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


