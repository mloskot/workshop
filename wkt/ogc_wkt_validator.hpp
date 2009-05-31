///////////////////////////////////////////////////////////////////////////////
// $Id: ogc_wkt_validator.hpp 89 2008-07-25 20:50:50Z mateusz@loskot.net $
//
// Implementation of Grammar Rules for Well-Known-Text (WKT) format.
//
// This implementation is based on definition of WKT format available
// in the "OpenGIS Simple Features Specification For SQL (Revision 1.1)"
// by Open Geospatial Consortium, Inc.
//
///////////////////////////////////////////////////////////////////////////////
#ifndef OGC_WKT_VALIDATOR_HPP_INCLUDED
#define OGC_WKT_VALIDATOR_HPP_INCLUDED

#include <boost/spirit/core.hpp>
#include <string>
#include "ogc_wkt_grammar.hpp" // ogc::wkt::grammar

///////////////////////////////////////////////////////////////////////////////

namespace ogc { 
	namespace wkt {

template <typename G = ogc::wkt::grammar>
class simple_validator
{
public:

	simple_validator() {}

	bool check(std::string const& wkt)
	{
		return boost::spirit::parse(wkt.c_str(), grammar_).full;
	}

private:

	G grammar_;

	// NonCopyable type
	simple_validator(simple_validator const&);
	simple_validator& operator=(simple_validator const&);
};

	} // namespace wkt
} // namespace ogc

#endif // #ifndef OGC_WKT_VALIDATOR_HPP_INCLUDED

// EOF

