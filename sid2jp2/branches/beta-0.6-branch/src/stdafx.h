// $Id$
//
// TODO - copyright note
//
#ifndef STDAFX_H_INCLUDED
#define STDAFX_H_INCLUDED

// Change these values to use different versions
#define WINVER		0x0400
//#define _WIN32_WINNT	0x0400
#define _WIN32_IE	0x0400
#define _RICHEDIT_VER	0x0100

//
// ATL headers
//
#include <atlbase.h>
#include <atlstr.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlwin.h>
#include <atlpath.h>

//
// WTL headers
//
#include <atlframe.h>
#include <atlcrack.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atldlgs.h>
#include <atlddx.h>
#include <atlmisc.h>

#ifdef _MSC_VER
#pragma warning(disable:4251 4275)
#endif // _MSC_VER

#endif // #ifndef STDAFX_H_INCLUDED