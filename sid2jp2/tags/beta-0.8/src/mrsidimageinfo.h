// $Id$
/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

// gdal
#include <gdal.h>
// std
#include <string>


namespace sid2jp2
{

class MrSidImageInfo
{
public:

    MrSidImageInfo(std::string const& file);
    ~MrSidImageInfo();
    
    int GetXSize() const;
    int GetYSize() const;
    int GetRasterCount() const;

    /// Get nominal size of MrSID image.
    /// It is the size of an image, in bytes, defined as:
    /// width * height * number of bands * number of bytes per sample).
    int GetNominalSize() const;

    /// Get physical size of MrSID image.
    /// It is the size of an image, in bytes, as defined by the file size
    /// required to represent the image on disk. See also nominal image size.
    int GetPhysicalSize() const;

    /// Get input compression ratio of MrSID image.
    /// The ratio is amount of reduction in file size, expressed as the ratio
    /// of the nominal file size to the target size.
    int GetCompressionRatio() const;

private:

    std::string m_file;

    GDALDatasetH m_ds;

}; // class ImageInfo

} // namespace sid2jp2