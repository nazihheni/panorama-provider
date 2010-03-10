#pragma once
#include "afxwin.h"
#include "imgMatch.h"
// CDlgImgProc �Ի���

class CDlgImgProc : public CDialog
{
	DECLARE_DYNAMIC(CDlgImgProc)

public:
	CDlgImgProc(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgImgProc();

// �Ի�������
	enum { IDD = IDD_Combine };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
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
