// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// ("Fluent UI")����ʾ������Ϊ�ο������ṩ�� 
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://msdn.microsoft.com/officeui��
//
// ��Ȩ���� (C) Microsoft Corporation
// ��������Ȩ����

// StreetViewDoc.cpp : CStreetViewDoc ���ʵ��
//

#include "stdafx.h"
#include "StreetView.h"

#include "StreetViewDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStreetViewDoc

IMPLEMENT_DYNCREATE(CStreetViewDoc, CDocument)

BEGIN_MESSAGE_MAP(CStreetViewDoc, CDocument)
END_MESSAGE_MAP()


// CStreetViewDoc ����/����

CStreetViewDoc::CStreetViewDoc()
{
	// TODO: �ڴ����һ���Թ������

}

CStreetViewDoc::~CStreetViewDoc()
{
}

BOOL CStreetViewDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CStreetViewDoc ���л�

void CStreetViewDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}


// CStreetViewDoc ���

#ifdef _DEBUG
void CStreetViewDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CStreetViewDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CStreetViewDoc ����
