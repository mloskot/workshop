///////////////////////////////////////////////////////////////////////////////
// $Id: ogc_wkt_grammar_test.cpp 89 2008-07-25 20:50:50Z mateusz@loskot.net $
//
// Copyright (c) 2006 Mateusz £oskot
//
// Test of Well-Known-Text Grammar implementation
///////////////////////////////////////////////////////////////////////////////
#define BOOST_TEST_MODULE wkt_simple_parser_test
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <string> 
#include "ogc_wkt_grammar.hpp" // Tested module

///////////////////////////////////////////////////////////////////////////////
//
// POINT
//
///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( wkt_point_test_suite );

BOOST_AUTO_TEST_CASE( point_empty_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("POINT EMPTY", gram).full );
}

BOOST_AUTO_TEST_CASE( point_valid_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("POINT (10 10)", gram).full );
	BOOST_CHECK( parse("POINT(15 20)", gram).full );
}

BOOST_AUTO_TEST_CASE( point_invalid_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( !parse("POINT (10)", gram).full );
	BOOST_CHECK( !parse("POINT (10 15 20)", gram).full );
}

BOOST_AUTO_TEST_CASE( point_valid_decimal_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("POINT (0.123 0.456)", gram).full );
	BOOST_CHECK( parse("POINT (10.123 10.456)", gram).full );
}

BOOST_AUTO_TEST_CASE( point_blanks_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("POINT        EMPTY", gram).full );
	BOOST_CHECK( parse("POINT (10 10)", gram).full );
	BOOST_CHECK( parse("POINT (10  10)", gram).full );
	BOOST_CHECK( parse("POINT (10   10)", gram).full );
	BOOST_CHECK( parse("POINT ( 10 10 )", gram).full );
	BOOST_CHECK( parse("POINT (  10 10  )", gram).full );
	BOOST_CHECK( parse("POINT (   10 10   )", gram).full );
	BOOST_CHECK( parse("POINT (   10    10   )", gram).full );
}

BOOST_AUTO_TEST_CASE( point_comma_case )
{
	ogc::wkt::grammar gram;

	// Test of unexpected comma character
	BOOST_CHECK( !parse("POINT (10,10)", gram).full );
	BOOST_CHECK( !parse("POINT (10, 10)", gram).full );
	BOOST_CHECK( !parse("POINT (10 ,10)", gram).full );
	BOOST_CHECK( !parse("POINT (10 , 10)", gram).full );
}

BOOST_AUTO_TEST_CASE( point_parenthesis_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( !parse("POINT 10 10", gram).full );
	BOOST_CHECK( !parse("POINT (10 10", gram).full );
	BOOST_CHECK( !parse("POINT 10 10)", gram).full );
	BOOST_CHECK( !parse("POINT ((10 10))", gram).full );
	BOOST_CHECK( !parse("POINT (((10 10)))", gram).full );
}

BOOST_AUTO_TEST_SUITE_END(); // wkt_point_test_suite

///////////////////////////////////////////////////////////////////////////////
//
// LINESTRING
//
///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( wkt_linestring_test_suite );

BOOST_AUTO_TEST_CASE( linestring_empty_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("LINESTRING EMPTY", gram).full );
}

BOOST_AUTO_TEST_CASE( linestring_valid_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("LINESTRING (10 10, 20 20, 30 40)", gram).full );
	BOOST_CHECK( parse("LINESTRING(0 0, 10 10, 20 25, 50 60)", gram).full );
}

BOOST_AUTO_TEST_CASE( linestring_invalid_case )
{
	ogc::wkt::grammar gram;

	// TODO
	// http://feature.opengeospatial.org/forumbb/viewtopic.php?t=326
	BOOST_CHECK_MESSAGE( !parse("LINESTRING (10 10)", gram).full,
		"TODO: Waiting for clarification of LineString BNF." );
}

BOOST_AUTO_TEST_CASE( linestring_valid_decimal_case )
{
	ogc::wkt::grammar gram;
	
	BOOST_CHECK( parse("LINESTRING (10.123 10.123, 20.987 20.987, 30.0005 40.0005)", gram).full );
}

BOOST_AUTO_TEST_CASE( linestring_blanks_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("LINESTRING     EMPTY", gram).full );
	BOOST_CHECK( parse("LINESTRING (10   10, 20 20, 30    40)", gram).full );
	BOOST_CHECK( parse("LINESTRING (10 10, 20   20, 30 40)", gram).full );
	BOOST_CHECK( parse("LINESTRING (   10 10, 20 20, 30 40   )", gram).full );
	BOOST_CHECK( parse("LINESTRING (   10     10, 20  20, 30    40)", gram).full );
}

BOOST_AUTO_TEST_CASE( linestring_comma_case )
{
	ogc::wkt::grammar gram;

	// Test of correct comma usage
	BOOST_CHECK( parse("LINESTRING (10 10,  20 20)", gram).full );
	BOOST_CHECK( parse("LINESTRING (10 10  ,  20 20)", gram).full );
	BOOST_CHECK( parse("LINESTRING (10 10     ,20 20)", gram).full );

	// Test of unexpected comma character
	BOOST_CHECK( !parse("LINESTRING (10, 10, 20, 20)", gram).full );
	BOOST_CHECK( !parse("LINESTRING (10, 10, 20 ,20, 30 40)", gram).full );
	BOOST_CHECK( !parse("LINESTRING (10 10, 20 20, 30 ,40)", gram).full );
	BOOST_CHECK( !parse("LINESTRING ( , 10 10, 20 20, 30 40)", gram).full );
	BOOST_CHECK( !parse("LINESTRING (10 10, 20 20, 30 40 , )", gram).full );
}

BOOST_AUTO_TEST_CASE( linestring_parenthesis_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( !parse("LINESTRING 10 10, 20 20, 30 40", gram).full );
	BOOST_CHECK( !parse("LINESTRING 10 10, 20 20", gram).full );
	BOOST_CHECK( !parse("LINESTRING (10 10, 20 20", gram).full );
	BOOST_CHECK( !parse("LINESTRING 10 10, 20 20)", gram).full );

	BOOST_CHECK( !parse("LINESTRING ((10 10, 20 20, 30 40))", gram).full );
	BOOST_CHECK( !parse("LINESTRING ((10 10), (20 20), (30 40))", gram).full );
	BOOST_CHECK( !parse("LINESTRING (((10 10)), (20 20), (30 40))", gram).full );
	BOOST_CHECK( !parse("LINESTRING ((10 10, 20 20), 30 40)", gram).full );
}

BOOST_AUTO_TEST_SUITE_END(); // wkt_linestring_test_suite

///////////////////////////////////////////////////////////////////////////////
//
// LINEARRING
//
///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( wkt_linearring_test_suite );

BOOST_AUTO_TEST_CASE( linearring_empty_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("LINEARRING EMPTY", gram).full );
}

BOOST_AUTO_TEST_CASE( linearring_valid_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("LINEARRING (10 10, 20 10, 30 20, 10 10)", gram).full );
}

BOOST_AUTO_TEST_CASE( linearring_invalid_case )
{
	ogc::wkt::grammar gram;

	// TODO
	// http://feature.opengeospatial.org/forumbb/viewtopic.php?t=326
	BOOST_CHECK_MESSAGE( !parse("LINEARRING (10 10)", gram).full,
		"TODO: Waiting for clarification of LineString BNF." );
}

BOOST_AUTO_TEST_CASE( linearring_valid_decimal_case )
{
	ogc::wkt::grammar gram;
	
	BOOST_CHECK( parse("LINEARRING (10.123 10.123, 20.987 10.987, 30.0005 20.0005, 10.123 10.123)", gram).full );
}

BOOST_AUTO_TEST_CASE( linearring_blanks_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("LINEARRING     EMPTY", gram).full );
	BOOST_CHECK( parse("LINEARRING (10   10, 20 10, 30    20, 10 10)", gram).full );
	BOOST_CHECK( parse("LINEARRING (10 10, 20   10, 30  20, 10 10)", gram).full );
	BOOST_CHECK( parse("LINEARRING (   10 10, 20 10, 30 20, 10 10   )", gram).full );
	BOOST_CHECK( parse("LINEARRING (   10     10, 20  10, 30    20, 10    10)", gram).full );
	BOOST_CHECK( parse("LINEARRING (10 10,  20 10,  30 20,  10 10)", gram).full );
	BOOST_CHECK( parse("LINEARRING (10 10  ,  20 10  ,  30 20  ,  10 10)", gram).full );
	BOOST_CHECK( parse("LINEARRING (10 10    ,20 10    ,30 20    ,10 10)", gram).full );
}

BOOST_AUTO_TEST_CASE( linearring_comma_case )
{
	ogc::wkt::grammar gram;

	// Test of incorrectly placed comma character
	BOOST_CHECK( !parse("LINEARRING (10, 10, 20, 10, 30, 20, 10, 10)", gram).full );
	BOOST_CHECK( !parse("LINEARRING (10 10, ,20 ,10, 30 20, 10, ,10)", gram).full );

	// No comma character
	BOOST_CHECK( !parse("LINEARRING (10 10 20 10 30 20 10 10)", gram).full );
}

BOOST_AUTO_TEST_CASE( linearring_parenthesis_case )
{
	ogc::wkt::grammar gram;

	// Test of incorrect parenthesis
	BOOST_CHECK( !parse("LINEARRING 10 10, 20 10, 30 20, 10 10", gram).full );
	BOOST_CHECK( !parse("LINEARRING (10 10, 20 10, 30 20, 10 10", gram).full );
	BOOST_CHECK( !parse("LINEARRING 10 10, 20 10, 30 20, 10 10)", gram).full );

	BOOST_CHECK( !parse("LINEARRING ((10 10, 20 10, 30 20, 10 10))", gram).full );
	BOOST_CHECK( !parse("LINEARRING ((10 10), 20 10, 30 20, 10 10)", gram).full );
	BOOST_CHECK( !parse("LINEARRING ((10 10), (20 10), (30 20), (10 10))", gram).full );
	BOOST_CHECK( !parse("LINEARRING ((10 10, 20 10), 30 20, 10 10)", gram).full );
}

BOOST_AUTO_TEST_SUITE_END(); // wkt_linearring_test_suite

///////////////////////////////////////////////////////////////////////////////
//
// POLYGON
//
///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( wkt_polygon_test_suite );

BOOST_AUTO_TEST_CASE( polygon_empty_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("POLYGON EMPTY", gram).full );
}

BOOST_AUTO_TEST_CASE( polygon_valid_case )
{
	ogc::wkt::grammar gram;

	// A Polygon with 1 exterior ring and 0 interior rings
	BOOST_CHECK( parse("POLYGON ((10 10, 10 20, 20 20, 20 15, 10 10))", gram).full );

	// A Polygon with 1 exterior ring and 1 interior ring
	BOOST_CHECK( parse("POLYGON((0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7, 5 5))", gram).full );
}

BOOST_AUTO_TEST_CASE( polygon_invalid_case )
{
	ogc::wkt::grammar gram;

	// TODO
	// http://feature.opengeospatial.org/forumbb/viewtopic.php?t=326
	BOOST_CHECK_MESSAGE( parse("POLYGON ((10 10))", gram).full,
		"TODO: Waiting for clarification of LineString BNF." );
}

BOOST_AUTO_TEST_CASE( polygon_valid_decimal_case )
{
	ogc::wkt::grammar gram;

	// A Polygon with 1 exterior ring and 0 interior rings
	BOOST_CHECK( parse("POLYGON ((10.123 10.123, 10.123 20.123, 20.123 20.123, 20.123 15.123, 10.123 10.123))", gram).full );

	// A Polygon with 1 exterior ring and 1 interior ring
	BOOST_CHECK( parse("POLYGON((0.0 0.0,10.12 0.12,10.12 10.12,0.12 10.12,0.12 0.12),(5.789 5.789,7.789 5.789,7.789 7.789,5.789 7.789, 5.789 5.789))", gram).full );
}

BOOST_AUTO_TEST_CASE( polygon_blanks_case )
{
	ogc::wkt::grammar gram;

	// A Polygon with 1 exterior ring and 0 interior rings
	BOOST_CHECK( parse("POLYGON((10    10   , 10 20, 20 20, 20    15, 10     10))", gram).full );

	// A Polygon with 1 exterior ring and 1 interior ring
	BOOST_CHECK( parse("POLYGON  (  (  0  0  ,  10  0  ,  10  10  ,  0  10  ,  0  0  )  ,  (  5  5  ,  7  5  ,  7  7  ,  5  7  ,  5  5  )  )", gram).full );
}

BOOST_AUTO_TEST_CASE( polygon_comma_case )
{
	ogc::wkt::grammar gram;

	// A Polygon with 1 exterior ring and 0 interior rings
	BOOST_CHECK( !parse("POLYGON ( , (10 10, 10 20, 20 20, 20 15, 10 10))", gram).full );
	BOOST_CHECK( !parse("POLYGON ((10, 10, 10 20, 20 20, 20 15, 10 10))", gram).full );
	BOOST_CHECK( !parse("POLYGON ((10 10, ,10 20, 20 20, 20 15, 10 10))", gram).full );
	BOOST_CHECK( !parse("POLYGON ((10 10, 10 20, 20 20, 20 15, 10 10 , ))", gram).full );
	BOOST_CHECK( !parse("POLYGON ((10 10, 10 20, 20 20, 20 15, 10 10) , )", gram).full );
	BOOST_CHECK( !parse("POLYGON ((10 10, 10 ,20, 20 20, 20 ,15, 10 10))", gram).full );

	// A Polygon with 1 exterior ring and 1 interior ring
	BOOST_CHECK( !parse("POLYGON( , (0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7, 5 5))", gram).full );
	BOOST_CHECK( !parse("POLYGON((0 0,10 0,10 10,0 10,0 0)  ,  ,  (5 5,7 5,7 7,5 7, 5 5))", gram).full );

	BOOST_CHECK( !parse("POLYGON((0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7, 5 5) , )", gram).full );
	BOOST_CHECK( !parse("POLYGON( , (0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7, 5 5))", gram).full );
	BOOST_CHECK( !parse("POLYGON((0 , 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7, 5 5))", gram).full );
	BOOST_CHECK( !parse("POLYGON((0 0,10 0,10 10,0 10,0 0),(5 5,7 , 5,7 7,5 7, 5 5))", gram).full );
	BOOST_CHECK( !parse("POLYGON((0 0,10 0,10 10 , , 0 10,0 0),(5 5,7 5,7 7,5 7, 5 5))", gram).full );
}

BOOST_AUTO_TEST_CASE( polygon_parenthesis_case )
{
	ogc::wkt::grammar gram;

	// Test of incorretly placed parenthesis

	// A Polygon with 1 exterior ring and 0 interior rings
	BOOST_CHECK( !parse("POLYGON ( 10 10, 10 20, 20 20, 20 15, 10 10))", gram).full );
	BOOST_CHECK( !parse("POLYGON ((10 10, 10 20, 20 20, 20 15, 10 10 )", gram).full );
	BOOST_CHECK( !parse("POLYGON (10 10, 10 20, 20 20, 20 15, 10 10)", gram).full );

	BOOST_CHECK( !parse("POLYGON (((10 10, 10 20, 20 20, 20 15, 10 10)))", gram).full );
	BOOST_CHECK( !parse("POLYGON ((10 10, 10 20), 20 20, 20 15, 10 10)", gram).full );
	BOOST_CHECK( !parse("POLYGON (((10 10), (10 20), (20 20), (20 15), (10 10)))", gram).full);

	BOOST_CHECK_MESSAGE( !parse("POLYGON ((10 10), (10 20), (20 20), (20 15), (10 10))", gram).full,
		"CHECK: MULTIPOINT is considered as valid POLYGON?");	

	// A Polygon with 1 exterior ring and 1 interior ring
	BOOST_CHECK( !parse("POLYGON ( 0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7, 5 5))", gram).full );
	BOOST_CHECK( !parse("POLYGON ((0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7, 5 5 )", gram).full );
	BOOST_CHECK( !parse("POLYGON ( 0 0,10 0,10 10,0 10,0 0),(5 5,7 5,7 7,5 7, 5 5 )", gram).full );
	BOOST_CHECK( !parse("POLYGON ((0 0,10 0,10 10,0 10,0 0), 5 5,7 5,7 7,5 7, 5 5))", gram).full );
	BOOST_CHECK( !parse("POLYGON ((0 0,10 0,10 10,0 10,0 0 ,(5 5,7 5,7 7,5 7, 5 5))", gram).full );
	BOOST_CHECK( !parse("POLYGON ( 0 0,10 0,10 10,0 10,0 0 ,(5 5,7 5,7 7,5 7, 5 5))", gram).full );
	BOOST_CHECK( !parse("POLYGON (((0 0),(10 0),(10 10),(0 10),(0 0)),((5 5),(7 5),(7 7),(5 7),(5 5)))", gram).full );
}

BOOST_AUTO_TEST_SUITE_END(); // wkt_polygon_test_suite

///////////////////////////////////////////////////////////////////////////////
//
// MULTIPOINT
//
///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( wkt_multipoint_test_suite );

BOOST_AUTO_TEST_CASE( multipoint_empty_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("MULTIPOINT EMPTY", gram).full );
}

BOOST_AUTO_TEST_CASE( multipoint_valid_case)
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("MULTIPOINT ((10 10), (20 20))", gram).full );
	BOOST_CHECK( parse("MULTIPOINT((0 0), (20 20), (60 60))", gram).full );
}

BOOST_AUTO_TEST_CASE( multipoint_invalid_case)
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( !parse("MULTIPOINT (((10 10), (20 20)))", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT (((10 10)), ((20 20)))", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT (10 10, 20 20)", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT (10 10), (20 20)", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT 10 10, 20 20", gram).full );
}

BOOST_AUTO_TEST_CASE( multipoint_valid_decimal_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("MULTIPOINT ((10.123 10.123), (0.002 0.002))", gram).full );
	BOOST_CHECK( parse("MULTIPOINT((0.0 0.0), (20.123456789 20.123456789), (60.100 60.100))", gram).full );
}

BOOST_AUTO_TEST_CASE( multipoint_invalid_decimal_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( !parse("MULTIPOINT (((10.123 10.123), (0.002 0.002)))", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT (((10.123 10.123)), ((0.002 0.002)))", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT (10.123 10.123, 0.002 0.002)", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT (10.123 10.123), (0.002 0.002)", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT 10.123 10.123, 0.002 0.002", gram).full );
}

BOOST_AUTO_TEST_CASE( multipoint_blanks_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("MULTIPOINT ( (10 10), (20 20) )", gram).full );
	BOOST_CHECK( parse("MULTIPOINT(( 10 10 ), ( 20 20 ))", gram).full );
	BOOST_CHECK( parse("MULTIPOINT ((10    10), (20    20))", gram).full );
	BOOST_CHECK( parse("MULTIPOINT ((10    10), (   20 20   ))", gram).full );
	BOOST_CHECK( parse("MULTIPOINT ( ( 10 10 ) , ( 20 20 ) )", gram).full );
}

BOOST_AUTO_TEST_CASE( multipoint_comma_case )
{
	ogc::wkt::grammar gram;

	// Incorrect comma placement
	BOOST_CHECK( !parse("MULTIPOINT ((10 ,10), (20 20))", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT ((10 10), (20, 20))", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT ((10 10), ,(20 20))", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT ((10 10) (20 20))", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT ( , (10 10), (20 20))", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT ((10 10), (20 20) , )", gram).full );
}

BOOST_AUTO_TEST_CASE( multipoint_parenthesis_case )
{
	ogc::wkt::grammar gram;
	BOOST_CHECK( !parse("MULTIPOINT (10 10), (20 20))", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT ((10 10), (20 20)", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT ((10 10, 20 20))", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT (10 10, (20 20))", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT (10 10, 20 20)", gram).full );
	BOOST_CHECK( !parse("MULTIPOINT (10 10), (20 20)", gram).full );
}

BOOST_AUTO_TEST_SUITE_END(); // wkt_multipoint_test_suite

///////////////////////////////////////////////////////////////////////////////
//
// MULTILINESTRING
//
///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( wkt_multilinestring_test_suite );

BOOST_AUTO_TEST_CASE( multilinestring_empty_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("MULTILINESTRING EMPTY", gram).full );
}

BOOST_AUTO_TEST_CASE( multilinestring_valid_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("MULTILINESTRING ((10 10, 20 20),(15 15, 30 15))", gram).full );
}

BOOST_AUTO_TEST_CASE( multilinestring_invalid_case )
{
	ogc::wkt::grammar gram;

	// TODO
	// http://feature.opengeospatial.org/forumbb/viewtopic.php?t=326
	BOOST_CHECK_MESSAGE( !parse("MULTILINESTRING ((10 10))", gram).full,
		"TODO: Waiting for clarification of LineString BNF." );
}

BOOST_AUTO_TEST_CASE( multilinestring_valid_decimal_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("MULTILINESTRING ((10.123 10.123, 20.123 20.123),(15.001 15.001, 0.030 0.015))", gram).full );
}

BOOST_AUTO_TEST_CASE( multilinestring_blanks_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("MULTILINESTRING (  (  10 10, 20 20  ),(  15 15, 30 15  )  )", gram).full );
	BOOST_CHECK( parse("MULTILINESTRING ((10 10  ,  20 20)  ,  (15 15  ,  30 15))", gram).full );
	BOOST_CHECK( parse("MULTILINESTRING ((10   10, 20   20),(15   15, 30   15))", gram).full );
	BOOST_CHECK( parse("MULTILINESTRING (  (  10  10  ,  20  20  )  ,  (  15  15  ,  30  15  )  )", gram).full );
}

BOOST_AUTO_TEST_CASE( multilinestring_comma_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( !parse("MULTILINESTRING ((10, 10, 20, 20),(15, 15, 30, 15))", gram).full );
	BOOST_CHECK( !parse("MULTILINESTRING ((10 10, ,20 20),(15 15, ,30 15))", gram).full );
	BOOST_CHECK( !parse("MULTILINESTRING ((10 10, 20 20), ,(15 15, 30 15))", gram).full );
	BOOST_CHECK( !parse("MULTILINESTRING ((10 10, 20 20)(15 15, 30 15))", gram).full );
	BOOST_CHECK( !parse("MULTILINESTRING ((10 10 20 20),(15 15 30 15))", gram).full );
	BOOST_CHECK( !parse("MULTILINESTRING (,(10 10, 20 20),(15 15, 30 15),)", gram).full );
	BOOST_CHECK( !parse("MULTILINESTRING ((,10 10, 20 20,),(,15 15, 30 15,))", gram).full );
}

BOOST_AUTO_TEST_CASE( multilinestring_parenthesis_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( !parse("MULTILINESTRING (((10 10, 20 20)))", gram).full );
	BOOST_CHECK( !parse("MULTILINESTRING (((10 10, 20 20),(15 15, 30 15)))", gram).full );
	BOOST_CHECK( !parse("MULTILINESTRING (((10 10), (20 20)),((15 15), (30 15)))", gram).full );
	BOOST_CHECK( !parse("MULTILINESTRING (((10 10, 20 20,15 15, 30 15)))", gram).full );
	BOOST_CHECK( !parse("MULTILINESTRING (10 10, 20 20,15 15, 30 15)", gram).full );
	BOOST_CHECK( !parse("MULTILINESTRING (10 10, 20 20),(15 15, 30 15)", gram).full );
	BOOST_CHECK( !parse("MULTILINESTRING (10 10), (20 20),(15 15), (30 15)", gram).full );

	BOOST_CHECK_MESSAGE( !parse("MULTILINESTRING (((10 10, 20 20)),((15 15, 30 15)))", gram).full,
		"CHECK: MULTIPOLYGON syntax seems valid for MULTILINESTRING!");
}

BOOST_AUTO_TEST_SUITE_END(); // wkt_multilinestring_test_suite

///////////////////////////////////////////////////////////////////////////////
//
// MULTIPOLYGON
//
///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( wkt_multipolygon_test_suite );

BOOST_AUTO_TEST_CASE( multipolygon_empty_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("MULTIPOLYGON EMPTY", gram).full );
}

BOOST_AUTO_TEST_CASE( multipolygon_valid_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("MULTIPOLYGON (((10 10, 10 20, 20 20, 20 15, 10 10)),((60 60, 70 70, 80 60, 60 60 ) ))", gram).full );
	BOOST_CHECK( parse("MULTIPOLYGON(((0 0,10 0,10 10,0 10,0 0)),((5 5,7 5,7 7,5 7, 5 5)))", gram).full );
}

BOOST_AUTO_TEST_CASE( multipolygon_invalid_case )
{
	ogc::wkt::grammar gram;

	// TODO
	// http://feature.opengeospatial.org/forumbb/viewtopic.php?t=326
	BOOST_CHECK_MESSAGE( !parse("MULTIPOLYGON (((10 10)))", gram).full,
		"TODO: Waiting for clarification of LineString BNF." );
}

BOOST_AUTO_TEST_CASE( multipolygon_valid_decimal_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("MULTIPOLYGON(((0.0 0.0,10.12 0.12,10.12 10.13,0.14 10.14,0.0 0.0)),((5.01 5.01,7.02 5.02,7 7,5 7, 5.01 5.01)))", gram).full );
}

BOOST_AUTO_TEST_SUITE_END(); // wkt_multipolygon_test_suite

///////////////////////////////////////////////////////////////////////////////
//
// GEOMETRYCOLLECTION
//
///////////////////////////////////////////////////////////////////////////////

BOOST_AUTO_TEST_SUITE( wkt_geometrycollection_test_suite );

BOOST_AUTO_TEST_CASE( geometrycollection_empty_case )
{
	ogc::wkt::grammar gram;

	BOOST_CHECK( parse("GEOMETRYCOLLECTION EMPTY", gram).full );
}

BOOST_AUTO_TEST_CASE( geometrycollection_valid_case )
{
	ogc::wkt::grammar gram;

	// TODO - does not work
	// A GeometryCollection consisting of two Point values and one LineString:
	//BOOST_CHECK( parse("GEOMETRYCOLLECTION(POINT(10 10), POINT(30 30), LINESTRING(15 15, 20 20))", gram).full );
	//BOOST_CHECK( parse("GEOMETRYCOLLECTION(POINT(10 10)))", gram).full );
}

BOOST_AUTO_TEST_SUITE_END(); // wkt_geometrycollection_test_suite

// EOF
