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

extern UINT WM_SID2JP2_START;
extern UINT WM_SID2JP2_STOP;
extern UINT WM_SID2JP2_NEXT;
extern UINT WM_SID2JP2_PROGRESS;

class Translator : private boost::noncopyable
{
public:

    Translator(HWND listener, GDALDriverH driver, char** options, std::vector<dataset_t> const& datasets);
    ~Translator();
    void Run();
    void Stop();
    static int __stdcall ProcessingCallback(double complete, const char* msg, void* pArg);

private:

    ATL::CWindow m_listener;
    bool m_stop;
    char** m_options;
    GDALDriverH m_driver;
    std::vector<dataset_t> const& m_datasets;
    
    bool ProcessFile(const char* inputFile, const char* outputFile);

}; // class Translator

} // namespace sid2jp2

#endif