// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// ("Fluent UI")，该示例仅作为参考资料提供， 
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://msdn.microsoft.com/officeui。
//
// 版权所有 (C) Microsoft Corporation
// 保留所有权利。

// StreetViewView.cpp : CStreetViewView 类的实现
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
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CStreetViewView::OnFilePrintPreview)
	ON_WM_LBUTTONUP()
	ON_WM_DROPFILES()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CStreetViewView 构造/析构

CStreetViewView::CStreetViewView()
{
	// TODO: 在此处添加构造代码

}

CStreetViewView::~CStreetViewView()
{
}

BOOL CStreetViewView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CStreetViewView 绘制

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
	// TODO: 在此处为本机数据添加绘制代码
}


// CStreetViewView 打印


void CStreetViewView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CStreetViewView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CStreetViewView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CStreetViewView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
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


// CStreetViewView 诊断

#ifdef _DEBUG
void CStreetViewView::AssertValid() const
{
	CView::AssertValid();
}

void CStreetViewView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CStreetViewDoc* CStreetViewView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CStreetViewDoc)));
	return (CStreetViewDoc*)m_pDocument;
}
#endif //_DEBUG


// CStreetViewView 消息处理程序

void CStreetViewView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CView::OnLButtonUp(nFlags, point);
}


void CStreetViewView::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int DropCount=DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,0);//取得被拖动文件的数目
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
      //取得第i个拖动文件名所占字节数
      int NameSize=DragQueryFile(hDropInfo,i,NULL,0)+1;
      if(NameSize>=MAX_PATH)
	  {
		MessageBox(_T("Path goes too deep, the max length is 260 chars"),NULL,MB_OK);
		return;
	  }
      DragQueryFile(hDropInfo,i,FilePath,NameSize);//把文件名拷贝到缓冲区
	  str.AppendFormat(_T("%s\r\n"),FilePath);
	  lstImages.push_front(FilePath);
      }
	str.AppendFormat(_T("Press ok to start combine.\r\n"));
	MessageBox(str,NULL,MB_OK);
	DragFinish(hDropInfo);  //拖放结束后,释放内存

	m_dlgImgProc.ShowWindow(SW_SHOW);
	m_dlgImgProc.Combine(lstImages);
	CView::OnDropFiles(hDropInfo);
}

BOOL CStreetViewView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类
	m_dlgImgProc.Create(MAKEINTRESOURCE(IDD_Combine),this);
	return CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

int CStreetViewView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	DragAcceptFiles(TRUE);
	return 0;
}
