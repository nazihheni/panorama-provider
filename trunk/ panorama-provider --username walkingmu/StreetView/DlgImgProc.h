#pragma once
#include "afxwin.h"
#include "imgMatch.h"
// CDlgImgProc 对话框

class CDlgImgProc : public CDialog
{
	DECLARE_DYNAMIC(CDlgImgProc)

public:
	CDlgImgProc(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgImgProc();

// 对话框数据
	enum { IDD = IDD_Combine };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg LRESULT onNoteProgress(WPARAM wParam, LPARAM lParam); 
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_EditProc;
	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	void Combine(std::list<CString> lstImgs);
	afx_msg void OnBnClickedBtnCancel();
private:
	HANDLE hThread;
public:
	IplImage* m_imgCombined;
	afx_msg void OnBnClickedBtnOk();
};
