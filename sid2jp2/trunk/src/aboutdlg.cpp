// $Id$
/////////////////////////////////////////////////////////////////////////////
// aboutdlg.cpp : implementation of the AboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "aboutdlg.h"
#include <gdal.h> // GDAL

LRESULT AboutDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CenterWindow(GetParent());

    const char* pszGDALVersion = ::GDALVersionInfo("RELEASE_NAME");
    ATL::CString msg(pszGDALVersion);

	return TRUE;
}

LRESULT AboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}
