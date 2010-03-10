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

// StreetViewView.cpp : CStreetViewView ���ʵ��
//

#include "stdafx.h"
#include "StreetView.h"

#include "StreetViewDoc.h"
#include "StreetViewView.h"
#include "DlgImgProc.h"

#include "sift.h"
#include "imgfeatures.h"
#include "kdtree.h"
#include "utils.h"
#include "xform.h"

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include <list>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStreetViewView

IMPLEMENT_DYNCREATE(CStreetViewView, CView)

BEGIN_MESSAGE_MAP(CStreetViewView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CStreetViewView::OnFilePrintPreview)
	ON_WM_LBUTTONUP()
	ON_WM_DROPFILES()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CStreetViewView ����/����

CStreetViewView::CStreetViewView()
{
	// TODO: �ڴ˴���ӹ������

}

CStreetViewView::~CStreetViewView()
{
}

BOOL CStreetViewView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CStreetViewView ����

void CStreetViewView::OnDraw(CDC* /*pDC*/)
{
	CStreetViewDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CDC* pDC=GetDC();
	CRect crect;
	GetClientRect(crect);
	CString str("Drag two or more jpg or bmp files here, this program will combine these pictures if relevant");
	pDC->DrawText(str,crect,DT_CENTER|DT_VCENTER);
	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CStreetViewView ��ӡ


void CStreetViewView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CStreetViewView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CStreetViewView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CStreetViewView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void CStreetViewView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CStreetViewView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}


// CStreetViewView ���

#ifdef _DEBUG
void CStreetViewView::AssertValid() const
{
	CView::AssertValid();
}

void CStreetViewView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CStreetViewDoc* CStreetViewView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CStreetViewDoc)));
	return (CStreetViewDoc*)m_pDocument;
}
#endif //_DEBUG


// CStreetViewView ��Ϣ�������

void CStreetViewView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CView::OnLButtonUp(nFlags, point);
}


void CStreetViewView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	int DropCount=DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,0);//ȡ�ñ��϶��ļ�����Ŀ
    if (DropCount==1) 
      {
       MessageBox(_T("At least two files should be dragged!"),_T("ERROR"),MB_ICONERROR);
       return;
      }
	std::list<CString> lstImages(16);

	CString str(_T("File to be combined:\r\n"));
	TCHAR FilePath[MAX_PATH];
	for(int i=0;i< DropCount;i++)
	{
      //ȡ�õ�i���϶��ļ�����ռ�ֽ���
      int NameSize=DragQueryFile(hDropInfo,i,NULL,0)+1;
      if(NameSize>=MAX_PATH)
	  {
		MessageBox(_T("Path goes too deep, the max length is 260 chars"),NULL,MB_OK);
		return;
	  }
      DragQueryFile(hDropInfo,i,FilePath,NameSize);//���ļ���������������
	  str.AppendFormat(_T("%s\r\n"),FilePath);
	  lstImages.push_front(FilePath);
      }
	str.AppendFormat(_T("Press ok to start combine.\r\n"));
	MessageBox(str,NULL,MB_OK);
	DragFinish(hDropInfo);  //�ϷŽ�����,�ͷ��ڴ�

	m_dlgImgProc.ShowWindow(SW_SHOW);
	m_dlgImgProc.Combine(lstImages);
	CView::OnDropFiles(hDropInfo);
}

BOOL CStreetViewView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: �ڴ����ר�ô����/����û���
	m_dlgImgProc.Create(MAKEINTRESOURCE(IDD_Combine),this);
	return CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

int CStreetViewView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	DragAcceptFiles(TRUE);
	return 0;
}
