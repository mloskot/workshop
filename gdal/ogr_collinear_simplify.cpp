#include <ogrsf_frmts.h>
#include <cassert>
#include <cstdio>
#include <cmath>
#include <limits>
#include <vector>

inline double calculate_slope(OGRRawPoint const& p1, OGRRawPoint const& p2)
{
    //      dy     y2 - y1
    // m = ---- = ---------
    //      dx     x2 - x1
    return ((p2.y - p1.y) / (p2.x - p1.x));
}

void remove_collinear_points(std::vector<OGRRawPoint> const& line,
                            std::vector<OGRRawPoint>& simple,
                            double const tolerance)
{
    typedef std::vector<OGRRawPoint> points_t;
    simple.reserve(line.size());

    // First and last points are always inserted.
    double m = 0;
    simple.push_back(line[0]);
    m = calculate_slope(line[0], line[1]);

    // Simplify excluding last point, always inserted
    points_t::size_type const size = line.size() - 1;
    for (points_t::size_type i = 1; i < size; ++i)
    {
        OGRRawPoint const& a = line[i];
        OGRRawPoint const& b = line[i + 1];
        double mt = calculate_slope(a, b);

        double const diff = m - mt;
        if (!((diff <= tolerance) && (diff >= -tolerance)))
        {
            simple.push_back(a);
            m = mt;
        }
    }

    // Insert last point
    simple.push_back(line[line.size() - 1]);
}

int main()
{
    OGRRegisterAll();

    const char* pszFile = "D:\\data\\mass\\ldtrai\\ldtraia1.shp";
    const char* pszlayer = "ldtraia1";

    // Open Shapefile layer
    OGRDataSource* poDS = NULL;
    poDS = OGRSFDriverRegistrar::Open(pszFile, FALSE);
    assert(NULL != poDS && "Can not open input shapefile!");
    OGRLayer* poLayer = NULL;
    poLayer = poDS->GetLayerByName(pszlayer);

    // Get sample feature of given FID
    const int fid = 0;
    OGRFeature* poFeature = NULL;
    poFeature = poLayer->GetFeature(fid);
    assert(NULL != poFeature);

    // Get linestring geometry of feature
    OGRGeometry* poGeom = NULL;
    poGeom = poFeature->GetGeometryRef();    
    OGRLineString* poLine = NULL;
    poLine = static_cast<OGRLineString*>(poGeom);

    // Copy linestring points to simple array
    std::vector<OGRRawPoint> linePoints(poLine->getNumPoints());
    poLine->getPoints(&linePoints[0], NULL);

    // Simplify linestring by removing collinear or close to collinear points.
    // Original points stay untouched, but simplified version is built as new array
    std::vector<OGRRawPoint> simple;
    double tolerance = 0.1; // std::numeric_limits<double>::epsilon(); // Smallest tolerance as possible
    remove_collinear_points(linePoints, simple, tolerance);

    // Summary
    printf("Before: %u (%u)\n", linePoints.size(), linePoints.capacity());
    printf("After : %u (%u)\n", simple.size(), simple.capacity());

    OGRFeature::DestroyFeature(poFeature);
    OGRDataSource::DestroyDataSource( poDS );
}

