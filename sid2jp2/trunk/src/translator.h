// $Id$
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
#ifndef SID2JP2_TRANSLATOR_H_INCLUDED
#define SID2JP2_TRANSLATOR_H_INCLUDED

#include "dataset.h"
#include "mrsidimageinfo.h"
// gdal
#include <gdal.h>
// std
#include <map>
#include <string>
#include <vector>
// boost
#include <boost/noncopyable.hpp>


namespace sid2jp2
{
extern UINT WM_SID2JP2_FINISH;
extern UINT WM_SID2JP2_FILE_NEXT;
extern UINT WM_SID2JP2_TARGET_RATIO;
extern UINT WM_SID2JP2_FILE_PROGRESS;
extern UINT WM_SID2JP2_FILE_FAILURE;

class Translator : private boost::noncopyable
{
public:

    Translator();
    ~Translator();
    void Configure(HWND listener, GDALDriverH driver, char** options,
                   std::vector<dataset_t> const& datasets,
                   std::map<std::string, MrSidImageInfo::int64> const& ratios);
    void Run();
    void Terminate();
    bool IsTerminating() const;
    HWND GetListener() const;

private:
    
    ATL::CWindow m_listener;
    bool m_running;
    char** m_options;
    GDALDriverH m_driver;
    
    //std::vector<dataset_t> const& m_datasets;
    std::vector<dataset_t> m_datasets;
    std::map<std::string, MrSidImageInfo::int64> m_ratios;
    
    void Reset();
    bool ProcessFile(const char* inputFile, const char* outputFile);

    // Calculate target size reduction from target compression ratio (N:1).
    // Size reduction is a percentage of the original size.
    int CompressionPercFromRatio(int ratio);

}; // class Translator

int __stdcall TranslationCallback(double complete, const char* msg, void* pArg);

} // namespace sid2jp2

#endif // SID2JP2_TRANSLATOR_H_INCLUDED