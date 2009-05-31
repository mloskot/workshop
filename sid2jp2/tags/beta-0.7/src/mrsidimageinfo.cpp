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

int MrSidImageInfo::GetXSize() const
{
    assert(NULL != m_ds);
    return GDALGetRasterXSize(m_ds);
}

int MrSidImageInfo::GetYSize() const
{
    assert(NULL != m_ds);
    return GDALGetRasterYSize(m_ds);
}

int MrSidImageInfo::GetRasterCount() const
{
    assert(NULL != m_ds);
    return GDALGetRasterCount(m_ds);
}

int MrSidImageInfo::GetNominalSize() const
{
    assert(NULL != m_ds);
    return (GetXSize() * GetYSize() * GetRasterCount());
}

int MrSidImageInfo::GetPhysicalSize() const
{
    assert(NULL != m_ds);
    return fs::get_file_size(m_file);
}

int MrSidImageInfo::GetCompressionRatio() const
{
    assert(NULL != m_ds);
    return static_cast<int>(GetNominalSize() / GetPhysicalSize());
}

} // namespace sid2jp2