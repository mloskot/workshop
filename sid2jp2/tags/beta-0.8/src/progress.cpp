#include "stdafx.h"
#include "progress.h"
#include <cmath>
#include <cstdio>

int __stdcall FileProcessingProgress(double dfComplete, const char* pszMessage, void* pProgressArg)
{
    ::OutputDebugString(_T("."));
    return TRUE;
}
