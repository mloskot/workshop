// $Id$
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "translator.h"
#include "dataset.h"
#include "error.h"
#include "filesystem.h"
#include "mrsidimageinfo.h"
// gdal
#include <gdal.h>
#include <cpl_string.h>
// std
#include <cassert>
#include <cmath>
#include <sstream>
#include <vector>

namespace sid2jp2
{

//
// Register messages used to notify the listener window
// about translation process.
//
UINT WM_SID2JP2_FINISH = ::RegisterWindowMessage(_T("WM_SID2JP2_FINISH"));
UINT WM_SID2JP2_FILE_NEXT = ::RegisterWindowMessage(_T("WM_SID2JP2_FILE_NEXT"));
UINT WM_SID2JP2_TARGET_RATIO = ::RegisterWindowMessage(_T("WM_SID2JP2_TARGET_RATIO"));
UINT WM_SID2JP2_FILE_PROGRESS = ::RegisterWindowMessage(_T("WM_SID2JP2_FILE_PROGRESS"));
UINT WM_SID2JP2_FILE_FAILURE = ::RegisterWindowMessage(_T("WM_SID2JP2_FILE_FAILURE"));

Translator::Translator() :
    m_listener(NULL),
    m_running(false),
    m_driver(NULL),
    m_options(NULL)
{
    // idle
}

Translator::~Translator()
{
    m_listener.Detach();
}

void Translator::Configure(HWND listener, GDALDriverH driver, char** options,
                           std::vector<dataset_t> const& datasets,
                           std::map<std::string, MrSidImageInfo::int64> const& ratios)
{
    assert(::IsWindow(listener));
    assert(NULL != driver);
    assert(NULL != options);

    m_listener.Attach(listener);
    m_driver = driver;
    m_options = options;

    // TODO: Optimize
    m_datasets = datasets;
    m_ratios = ratios;
}

bool Translator::IsTerminating() const
{
    return (!m_running);
}

HWND Translator::GetListener() const
{
    return m_listener;
}

void Translator::Run()
{
    assert(::IsWindow(m_listener));
    assert(NULL != m_driver);
    assert(NULL != m_options);

    m_running = true;

    std::vector<dataset_t>::size_type counter = 0;

    for (std::vector<dataset_t>::const_iterator it = m_datasets.begin(); 
        m_running && it != m_datasets.end();
        ++it)
    {
        dataset_t const* ptrDS = &(*it);
        m_listener.SendMessage(WM_SID2JP2_FILE_NEXT,
                               static_cast<WPARAM>(counter),
                               reinterpret_cast<LPARAM>(ptrDS));

        std::string const& fileInput = it->first;
        std::string const& fileOutput = it->second;

        // Check if the output directory exists, otherwise
        // create it recursively, from output file path.
        fs::create_file_path_recurse(fileOutput);

        // Calculate percentage of image size reduction
        int const ratio = static_cast<int>(m_ratios[fileInput]);
        int const perc = CompressionPercFromRatio(ratio);
        std::ostringstream os;
        os << perc;
        m_options = ::CSLSetNameValue(m_options, "TARGET", os.str().c_str());

        // Translate MrSID file to JPEG200 ECW
        if (!ProcessFile(fileInput.c_str(), fileOutput.c_str()))
        {
            dataset_t const* ptrFailedDS = &(*it);
            m_listener.SendMessage(WM_SID2JP2_FILE_FAILURE,
                                   static_cast<WPARAM>(counter),
                                   reinterpret_cast<LPARAM>(ptrFailedDS));

            m_running = false;
            //error::file_translation_failure(m_hWnd, fileInput);
        }
        
        // Don't count aborted file
        if (m_running)
        {
            ++counter;
        }
    }

    // Notify about translation finish
    m_listener.PostMessage(WM_SID2JP2_FINISH, 0,
        MAKELPARAM(static_cast<WORD>(counter), static_cast<WORD>(m_datasets.size())));

    // Restet internal data.
    // After this step, it's required to call Configure()
    // before calling Run() again.
    Reset();
}

void Translator::Terminate()
{
    m_running = false;
}

void Translator::Reset()
{
    m_listener.Detach();
    m_running = false;
    m_driver = NULL;
    m_options = NULL;
}

bool Translator::ProcessFile(const char* inputFile, const char* outputFile)
{
    ATLASSERT(NULL != m_driver);
    ATLASSERT(NULL != inputFile && NULL != outputFile);

    bool success = false;
    GDALDatasetH srcDS = NULL;
    GDALDatasetH dstDS = NULL;
    
    srcDS = GDALOpen(inputFile, GA_ReadOnly);
    if (NULL != srcDS)
    {
        dstDS = GDALCreateCopy(m_driver, outputFile, srcDS, FALSE, m_options,
                               &TranslationCallback, this);
        if (NULL != dstDS)
        {
            GDALClose(dstDS);
            dstDS = NULL;

            success = true;
        }

        GDALClose(srcDS);
        srcDS = NULL;
    }

    return success;
}

int CPL_STDCALL TranslationCallback(double complete, const char* msg, void* pArg)
{
    assert(NULL != pArg);

    Translator* translator = static_cast<Translator*>(pArg);
    if (translator->IsTerminating())
    {
        // Request GDAL to abort translation
        return false;
    }
    
    // Generate progress info
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

        HWND listener = translator->GetListener();
        assert(::IsWindow(listener));
        ::SendMessage(listener, WM_SID2JP2_FILE_PROGRESS, static_cast<WPARAM>(percent), 0);
    }

    lastComplete = complete;

    // Continue translation
    return true;
}

int Translator::CompressionPercFromRatio(int ratio)
{
    if (0 >= ratio)
    {
        ratio = 1;
    }

    int const percentage = (100 - (100 / ratio));
    return percentage;
}

} // namespace sid2jp2

