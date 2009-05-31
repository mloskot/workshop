// $Id$
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "error.h"
#include <cpl_error.h> // CPL
#include <string> // STD

namespace sid2jp2 { namespace error {

void file_translation_failure(HWND hParent, std::string const& file)
{
    ATL::CString szPath(CA2T(file.c_str()));
    ATL::CString szOptional(_T(""));

    std::string gdalError(::CPLGetLastErrorMsg());
    if (!gdalError.empty())
    {
        ATL::CString szError(CA2T(gdalError.c_str()));
        szOptional.Format(_T("\n\rGDAL Error:\n\t%s"), szError);
    }

    ATL::CString msg;
    msg.Format(_T("Translation of '%s' failed!%s"), szPath, szOptional);

    ::MessageBox(hParent, msg,
                 _T("Translation failure!"),
                 MB_OK | MB_ICONERROR);
}

} // namespace error
} // namespace sid2jp2
