// DlgImgProc.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "StreetView.h"
#include "DlgImgProc.h"

#include <list>
#include "imgMerge.h"

static ThreadParams m_thdparams;


// CDlgImgProc �Ի���

IMPLEMENT_DYNAMIC(CDlgImgProc, CDialog)

CDlgImgProc::CDlgImgProc(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImgProc::IDD, pParent)
{

}

CDlgImgProc::~CDlgImgProc()
{
}

void CDlgImgProc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PROGRESS, m_EditProc);
}


BEGIN_MESSAGE_MAP(CDlgImgProc, CDialog)
	ON_MESSAGE(WM_PROGRESS,onNoteProgress)
	ON_BN_CLICKED(IDC_BTN_CANCEL, &CDlgImgProc::OnBnClickedBtnCancel)
	ON_BN_CLICKED(IDC_BTN_OK, &CDlgImgProc::OnBnClickedBtnOk)
END_MESSAGE_MAP()


// CDlgImgProc ��Ϣ�������

BOOL CDlgImgProc::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	// TODO: �ڴ����ר�ô����/����û���

	return CDialog::Create(lpszTemplateName, pParentWnd);
}


void CDlgImgProc::Combine(std::list<CString> lstImgs)
{
	DWORD dwThreadID=0;
	

	static IplImage* imgCombined=NULL;
	m_thdparams.hParentWnd=this->GetSafeHwnd();
	m_thdparams.lstImgs=lstImgs;
	m_thdparams.IplImageCombined=imgCombined;
	LPVOID lpParam=&m_thdparams;
	hThread=CreateThread(NULL,0,ImgHandleThread,lpParam,0,&dwThreadID);
	if(hThread==NULL)
	{
		MessageBox(_T("Error in creating a thread."),_T("ERROR"),MB_OK);
	}
	
}

LRESULT CDlgImgProc::onNoteProgress(WPARAM wParam, LPARAM lParam)
{
	CString cstr;
	this->m_EditProc.GetWindowTextW(cstr);
	cstr.AppendFormat(_T("%s\r\n"),(LPCTSTR)wParam);
	this->m_EditProc.SetWindowTextW(cstr);
	if(lParam==100)
	{
		if(m_thdparams.IplImageCombined!=NULL)
			cvShowImage("Result",m_thdparams.IplImageCombined);
	}
	return NULL;
}
void CDlgImgProc::OnBnClickedBtnCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD dwExitCode;
	if(hThread!=NULL)
	{
		GetExitCodeThread(hThread,&dwExitCode);
		if(dwExitCode!=STILL_ACTIVE)
			TerminateThread(hThread,1);
	}
	CDialog::OnCancel();
}

void CDlgImgProc::OnBnClickedBtnOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD dwExitCode;
	if(hThread!=NULL)
	{
		GetExitCodeThread(hThread,&dwExitCode);
		if(dwExitCode==STILL_ACTIVE)
		{
			if(MessageBox(_T("Thread is still running, terminate the thread and exit?"),
				_T("Question"),MB_OKCANCEL)==MB_OK)
					TerminateThread(hThread,1);
		}
	}
	CDialog::OnOK();
}
