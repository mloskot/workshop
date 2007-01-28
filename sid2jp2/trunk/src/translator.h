// $Id$
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
#ifndef SID2JP2_TRANSLATOR_H_INCLUDED
#define SID2JP2_TRANSLATOR_H_INCLUDED

#include "dataset.h"
// gdal
#include <gdal.h>
// std
#include <vector>
// boost
#include <boost/noncopyable.hpp>


namespace sid2jp2
{

extern UINT WM_SID2JP2_FILE_NEXT;
extern UINT WM_SID2JP2_FILE_PROGRESS;
extern UINT WM_SID2JP2_FILE_FAILURE;

class Translator : private boost::noncopyable
{
public:

    Translator(HWND listener, GDALDriverH driver, char** options, std::vector<dataset_t> const& datasets);
    ~Translator();
    void Run();
    void Terminate();
    bool IsTerminating() const;
    HWND GetListener() const;

private:
    
    ATL::CWindow m_listener;
    bool m_running;
    char** m_options;
    GDALDriverH m_driver;
    std::vector<dataset_t> const& m_datasets;
    
    bool ProcessFile(const char* inputFile, const char* outputFile);

}; // class Translator

int __stdcall TranslationCallback(double complete, const char* msg, void* pArg);

} // namespace sid2jp2

#endif