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

    // Get GDAL version info
    const char* pszGDALVersion = ::GDALVersionInfo("RELEASE_NAME");
    ATL::CString gdalInfo;
    gdalInfo = CA2T(pszGDALVersion);

    // Display GDAL version info
    WTL::CStatic ctlInfo;
    ctlInfo.Attach(GetDlgItem(IDC_GDALINFO));
    ATLASSERT(NULL != ctlInfo);
    ctlInfo.SetWindowText(gdalInfo);
    ctlInfo.Detach();

	return TRUE;
}

LRESULT AboutDlg::OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	EndDialog(wID);
	return 0;
}
