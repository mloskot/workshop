///////////////////////////////////////////////////////////////////////////////
// $Id$
//
// Implementation of Grammar Rules for Well-Known-Text (WKT) format.
//
// This implementation is based on definition of WKT format available
// in the "OpenGIS Simple Features Specification For SQL (Revision 1.1)"
// by Open Geospatial Consortium, Inc.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef OGC_WKT_GRAMMAR_HPP_INCLUDED
#define OGC_WKT_GRAMMAR_HPP_INCLUDED

#include <boost/spirit/core.hpp> 
#include <boost/spirit/utility/functor_parser.hpp> 
#include <cstddef> // std::ptrdiff_t
#include <iostream>
#include <string>

///////////////////////////////////////////////////////////////////////////////

namespace ogc { 
	namespace wkt {

namespace sp = boost::spirit;

struct error_report_parser
{ 
    error_report_parser(const char *msg)
        : _msg(msg)
    {}

    typedef sp::nil_t result_t; 
    
    template <typename ScannerT> 
    int operator()(ScannerT const& scan, result_t& /*result*/) const 
    { 
        std::cerr << _msg << std::endl; 
        return 0; 
    } 

private: 
    std::string _msg; 
}; 

typedef sp::functor_parser<error_report_parser> error_report_p; 

error_report_p my_grammar::error_missing_semicolon("missing semicolon"); 

struct grammar : public sp::grammar<grammar>
{
    error_report_p error_point_tag_expected; 

    grammar()
        : error_point_tag_expected("POINT tag expected")
    {}

	template <typename ScannerT>
	struct definition
	{
		definition(grammar const& self)
			: empty_txt_p("EMPTY")
		{
			//
			// Definition of Highest Level Rules
			//

			// Geometry Tagged Text
			geometry_tag_r
				=	point_tag_r
				|	linestring_tag_r
				|	linearring_tag_r
				|	polygon_tag_r
				|	multipoint_tag_r
				|	multilinestring_tag_r
				|	multipolygon_tag_r
				|	geometrycollection_tag_r
				;

			// Point Tagged Text
			point_tag_r
				=	("POINT" | error_point_tag_expected)
				>>	*(sp::blank_p) >> point_txt_r
				;

			// LineString Tagged Text
			linestring_tag_r
				=	"LINESTRING"
				>>	*(sp::blank_p) >> linestring_txt_r
				;

			// LinearRing Tagged Text
			linearring_tag_r
				=	"LINEARRING"
				>>	*(sp::blank_p) >> linestring_txt_r
				;

			// Polygon Tagged Text
			polygon_tag_r
				=	"POLYGON"
				>>	*(sp::blank_p) >> polygon_txt_r
				;

			// MultiPoint Tagged Text
			multipoint_tag_r
				=	"MULTIPOINT"
				>>	*(sp::blank_p) >> multipoint_txt_r
				;
			
			// MultiLineString Tagged Text
			multilinestring_tag_r
				=	"MULTILINESTRING"
				>>	*(sp::blank_p) >> multilinestring_txt_r
				;

			// MultiPolygon Tagged Text
			multipolygon_tag_r
				=	"MULTIPOLYGON"
				>>	*(sp::blank_p) >> multipolygon_txt_r
				;

			// GeometryCollection Tagged Text
			geometrycollection_tag_r
				=	"GEOMETRYCOLLECTION"
				>>	*(sp::blank_p) >> geometrycollection_txt_r
				;

			//
			// Definition of Single Geometry Level Rules
			//

			// Point Text
			point_txt_r
				=	empty_txt_p
				|	'(' >> point_r >> ')'
				;

			point_r
				=	*(sp::blank_p)
				>>	x_p
				>>	*(sp::blank_p)
				>>	y_p
				>>	*(sp::blank_p)
				;

			x_p = sp::real_p // Double precision literal
				;

			y_p = sp::real_p // Double precision literal
				;

			// LineString Text
			linestring_txt_r
				=	empty_txt_p 
				|	*(sp::blank_p)
					>>	'('
						>>	point_r
						>>	*(',' >> point_r)
					>>	')'
					>> *(sp::blank_p)
				;

			// Polygon Text
			polygon_txt_r
				=	empty_txt_p 
				|	'(' >> linestring_txt_r
				>>	*(',' >> linestring_txt_r)
				>>	')'
				;

			// MultiPoint Text
			multipoint_txt_r
				=	empty_txt_p 
				|	'(' >>	*(sp::blank_p)
						>>	'('
								>>	*(sp::blank_p)
								>>	point_r
								>>	*(sp::blank_p)
						>>	')'
						>>	*( *(sp::blank_p)
						>>	','
						>>	*(sp::blank_p)
						>>	'('
								>>	point_r
						>>	')')
				>> *(sp::blank_p)
				>>	')'
				;

			// MultiLineString Text
			multilinestring_txt_r
				=	empty_txt_p 
				|	'('
				>>	linestring_txt_r
				>>	*(',' >> linestring_txt_r)
				>>	')'
				;

			// MultiPolygon Text
			multipolygon_txt_r
				=	empty_txt_p 
				|	'(' >> polygon_txt_r >> *(',' >> polygon_txt_r) >> ')'
				;
				
			geometrycollection_txt_r
				=	empty_txt_p
				|	'(' >> geometry_tag_r >> *(',' >> geometry_tag_r) >> ')'
				;

			//
			// Definition of Common Helper Rules
			//

			empty_txt_p = "EMPTY"
				;
		}

		// Highest Level Rules
		sp::rule<ScannerT> geometry_tag_r;
		sp::rule<ScannerT> point_tag_r;
		sp::rule<ScannerT> linestring_tag_r;
		sp::rule<ScannerT> linearring_tag_r;
		sp::rule<ScannerT> polygon_tag_r;
		sp::rule<ScannerT> multipoint_tag_r;
		sp::rule<ScannerT> multilinestring_tag_r;
		sp::rule<ScannerT> multipolygon_tag_r;
		sp::rule<ScannerT> geometrycollection_tag_r;

        // Lowest level components
		sp::rule<ScannerT> point_r;
		sp::rule<ScannerT> x_p;
		sp::rule<ScannerT> y_p;

		// Single Geometry Level Rules
		sp::rule<ScannerT> point_txt_r;
		sp::rule<ScannerT> linestring_txt_r;
		sp::rule<ScannerT> polygon_txt_r;
		sp::rule<ScannerT> multipoint_txt_r;
		sp::rule<ScannerT> multilinestring_txt_r;
		sp::rule<ScannerT> multipolygon_txt_r;
		sp::rule<ScannerT> geometrycollection_txt_r;
	
		// Common Helper Rules
		sp::strlit<> empty_txt_p; // "EMPTY"

		// Public interface
		sp::rule<ScannerT> const& start() const
        {
            return geometry_tag_r;
        }
	};
};

	} // namespace wkt
} // namespace ogc

#endif // #ifndef OGC_WKT_GRAMMAR_HPP_INCLUDED

// EOF
