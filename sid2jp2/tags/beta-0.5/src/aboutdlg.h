// $Id$
/////////////////////////////////////////////////////////////////////////////
// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef ABOUTDLG_H_INCLUDED
#define ABOUTDLG_H_INCLUDED

class AboutDlg : public CDialogImpl<AboutDlg>
{
public:
	enum { IDD = IDD_ABOUTBOX };

	BEGIN_MSG_MAP(AboutDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
};

#endif // #ifndef ABOUTDLG_H_INCLUDED