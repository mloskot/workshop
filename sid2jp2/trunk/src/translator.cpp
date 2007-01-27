// $Id$
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "translator.h"
#include "dataset.h"
#include "error.h"
#include "filesystem.h"
// gdal
#include <gdal.h>
// std
#include <cassert>
#include <cmath>
#include <vector>


namespace sid2jp2
{

//
// Register messages used to notify the listener window
// about translation process.
//
UINT WM_SID2JP2_START = ::RegisterWindowMessage(_T("WM_SID2JP2_START"));
UINT WM_SID2JP2_STOP = ::RegisterWindowMessage(_T("WM_SID2JP2_STOP"));
UINT WM_SID2JP2_NEXT = ::RegisterWindowMessage(_T("WM_SID2JP2_NEXT"));
UINT WM_SID2JP2_PROGRESS = ::RegisterWindowMessage(_T("WM_SID2JP2_PROGRESS"));

Translator::Translator(HWND listener, GDALDriverH driver, char** options,
                       std::vector<dataset_t> const& datasets)
                       : m_listener(NULL),
                         m_stop(false),
                         m_driver(driver),
                         m_options(options),
                         m_datasets(datasets)
{
    assert(::IsWindow(listener));
    assert(NULL != m_driver);
    assert(NULL != m_options);

    m_listener.Attach(listener);
}

Translator::~Translator()
{
    m_listener.Detach();
}

void Translator::Run()
{
    m_listener.SendMessage(WM_SID2JP2_START);

    bool success = false;
    std::vector<dataset_t>::size_type filesCounter = 0;

    for (std::vector<dataset_t>::const_iterator it = m_datasets.begin(); 
        it != m_datasets.end();
        ++it)
    {
        if (m_stop)
        {
            // Send confirmation request
            break;
        }

        std::string const& fileInput = it->first;
        std::string const& fileOutput = it->second;

        m_listener.SendMessage(WM_SID2JP2_NEXT);

        //// Update progress info
        //ATL::CPath szInputFile(CA2T(fileInput.c_str()));
        //ATL::CPath szOutputFile(CA2T(fileOutput.c_str()));

        //msg.Format(_T("Source (%u/%u): %s"),
        //           (filesCounter + 1), m_files.size(), szInputFile);
        //m_ctlProgressInfo.SetWindowText(msg);
        //msg.Format(_T("Target %s"), szOutputFile);
        //m_ctlProgressFileInfo.SetWindowText(msg);

        // Check if the output directory exists, otherwise
        // create it recursively, from output file path.
        fs::create_file_path_recurse(fileOutput);

        // Translate MrSID file to JPEG200 ECW
        success = ProcessFile(fileInput.c_str(), fileOutput.c_str());
        if (!success)
        {
            // TODO: mloskot - Notify listener about translation failure

            assert(false);

            //error::file_translation_failure(m_hWnd, fileInput);
        }

        ++filesCounter;
    }

    m_listener.SendMessage(WM_SID2JP2_STOP);
}

void Translator::Stop()
{
    m_stop = true;
}

bool Translator::ProcessFile(const char* inputFile, const char* outputFile)
{
    GDALDatasetH hSrcDS = NULL;
    GDALDatasetH hDstDS = NULL;

    ATLASSERT(NULL != m_driver);
    ATLASSERT(NULL != inputFile && NULL != outputFile);

    hSrcDS = GDALOpen(inputFile, GA_ReadOnly);
    if (NULL != hSrcDS)
    {
        hDstDS = GDALCreateCopy(m_driver, outputFile, hSrcDS, FALSE, m_options,
                                &Translator::ProcessingCallback, &m_listener);

        if (NULL != hDstDS)
        {
            GDALClose(hDstDS);

            // SUCCESS
            return TRUE;
        }
    }

    // FAILURE
    return FALSE;
}

int CPL_STDCALL Translator::ProcessingCallback(double complete, const char* msg, void* pArg)
{
    ATLASSERT(NULL != pArg);

    ATL::CWindow listener;
    if (NULL != pArg)
    {
        listener.Attach(*(static_cast<HWND*>(pArg)));
        ATLASSERT(::IsWindow(listener));
    }

    static double lastComplete = -1.0;

    if (lastComplete > complete)
    {
        if (lastComplete >= 1.0)
            lastComplete = -1.0;
        else
            lastComplete = complete;
    }

    if (std::floor(lastComplete * 10) != std::floor(complete * 10))
    {
        int percent = static_cast<int>(std::floor(complete * 100));
        
        // TODO: Testing only
        ATL::CString m;
        m.Format(_T("Percent: %d\n"), percent);
        ::OutputDebugString(m);

        listener.SendMessage(WM_SID2JP2_PROGRESS, 0, static_cast<LPARAM>(percent));

    }

    lastComplete = complete;

    listener.Detach();
    return true;
}


} // namespace sid2jp2

