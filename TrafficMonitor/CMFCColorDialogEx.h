#pragma once


// CMFCColorDialogEx

class CMFCColorDialogEx : public CMFCColorDialog
{
	//DECLARE_DYNAMIC(CMFCColorDialogEx)

public:
	CMFCColorDialogEx(COLORREF clrInit = 0, DWORD dwFlags = 0 /* reserved */, CWnd* pParentWnd = NULL, HPALETTE hPal = NULL);
	virtual ~CMFCColorDialogEx();

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();

};


