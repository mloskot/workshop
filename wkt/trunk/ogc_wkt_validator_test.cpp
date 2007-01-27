///////////////////////////////////////////////////////////////////////////////
// $Id$
//
//Author
// Copyright (c) 2006 Mateusz ¿oskot
//
// Test Suite of Well-Known-Text Validator
///////////////////////////////////////////////////////////////////////////////
#define BOOST_TEST_MODULE wkt_simple_parser_test
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <string> 
#include "ogc_wkt_validator.hpp" // Tested module
#include "ogc_wkt_grammar.hpp"
///////////////////////////////////////////////////////////////////////////////

// Grammar Definiton passed explicitely
//typedef ogc::wkt::simple_validator<ogc::wkt::grammar> wkt_validator;

// Grammar Definiton passed implicitly, as template default value
typedef ogc::wkt::simple_validator<> wkt_validator;

BOOST_AUTO_TEST_SUITE( wkt_simple_validator_test_suite );

BOOST_AUTO_TEST_CASE( check_valid_point_case )
{
	wkt_validator valid;
	
	BOOST_CHECK( valid.check("POINT (10 10)") );
}

BOOST_AUTO_TEST_CASE( check_valid_linestring_case )
{
	wkt_validator valid;
	
	BOOST_CHECK( valid.check("LINESTRING(0 0, 10 10, 20 25, 50 60)") );
}

BOOST_AUTO_TEST_CASE( check_valid_polygon_case )
{
	wkt_validator valid;
	
	BOOST_CHECK( valid.check("POLYGON((0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7, 5 5))") );
}

BOOST_AUTO_TEST_CASE( check_valid_multipoint_case )
{
	wkt_validator valid;
	
	BOOST_CHECK( valid.check("MULTIPOINT((0 0), (20 20), (60 60))") );
}

BOOST_AUTO_TEST_CASE( check_valid_multilinestring_case )
{
	wkt_validator valid;
	
	BOOST_CHECK( valid.check("MULTILINESTRING((10 10, 20 20), (15 15, 30 15))") );
}

BOOST_AUTO_TEST_CASE( check_valid_multipolygon_case )
{
	wkt_validator valid;
	
	BOOST_CHECK( valid.check("MULTIPOLYGON(((0 0,10 0,10 10,0 10,0 0)),((5 5,7 5,7 7,5 7, 5 5)))") );
}

BOOST_AUTO_TEST_CASE( check_valid_geometrycollection_case )
{
	wkt_validator valid;
	
	BOOST_CHECK_MESSAGE( valid.check("GEOMETRYCOLLECTION(POINT(10 10), POINT(30 30), LINESTRING(15 15, 20 20))"),
		"TODO: This issue is waiting for fix in the grammar definition");
}

BOOST_AUTO_TEST_SUITE_END(); // wkt_validator_test_suite 

// EOF
