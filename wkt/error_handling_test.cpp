// $Id: error_handling_test.cpp 89 2008-07-25 20:50:50Z mateusz@loskot.net $
#include "ogc_wkt_grammar.hpp"
#include <boost/spirit/core.hpp>
#include <string>
#include <iostream>


int main()
{
    std::string wkt("POINT 10 20)");
    ogc::wkt::grammar gram;


    if (parse(wkt.c_str(), gram).full)
        std::cout << "OK\n";
    else
        std::cout << "DUPA\n";

    return 0;
}
