// $Id$
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "mrsidimageinfo.h"
#include "filesystem.h"
// std
#include <cassert>
#include <stdexcept>
#include <string>
// winstl
#include <winstl/filesystem_traits.hpp>

namespace sid2jp2
{

MrSidImageInfo::MrSidImageInfo(std::string const& file) : m_ds(NULL)
{
    m_ds = GDALOpen(file.c_str(), GA_ReadOnly);
    if (NULL == m_ds)
    {
        throw std::runtime_error("Can not open dataset '" + file + "'");
    }

    m_file = file;
}

MrSidImageInfo::~MrSidImageInfo()
{
    if (NULL != m_ds)
    {
        GDALClose(m_ds);
    }
}

MrSidImageInfo::int64 MrSidImageInfo::GetXSize() const
{
    assert(NULL != m_ds);
    return GDALGetRasterXSize(m_ds);
}

MrSidImageInfo::int64 MrSidImageInfo::GetYSize() const
{
    assert(NULL != m_ds);
    return GDALGetRasterYSize(m_ds);
}

MrSidImageInfo::int64 MrSidImageInfo::GetRasterCount() const
{
    assert(NULL != m_ds);
    return GDALGetRasterCount(m_ds);
}

MrSidImageInfo::int64 MrSidImageInfo::GetBytesPerSample() const
{
    assert(NULL != m_ds);

    // CreateCopy uses data type of the first band
    GDALRasterBandH band = GDALGetRasterBand(m_ds , 1);
    assert(NULL != band);

    GDALDataType type = GDALGetRasterDataType(band);
    return (GDALGetDataTypeSize(type) / 8);
}

MrSidImageInfo::int64 MrSidImageInfo::GetNominalSize() const
{
    assert(NULL != m_ds);
    
    int64 const width = GetXSize();
    int64 const height = GetYSize();
    int64 const nbands = GetRasterCount();
    int64 const bps = GetBytesPerSample();
    int64 const ns = (width * height * nbands * bps);

    return ns;
}

MrSidImageInfo::int64 MrSidImageInfo::GetPhysicalSize() const
{
    assert(NULL != m_ds);
    return fs::get_file_size(m_file);
}

MrSidImageInfo::int64 MrSidImageInfo::GetCompressionRatio() const
{
    assert(NULL != m_ds);
    return static_cast<int>(GetNominalSize() / GetPhysicalSize());
}

} // namespace sid2jp2