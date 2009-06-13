// Yet Another Tiny Binary Reader/Writer
//
// Simple test and usage example
//
// Copyright Mateusz Loskot <mateusz@loskot.net> 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "endian.hpp"

#include <cstring>
#include <exception>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/detail/endian.hpp>

namespace {

template <typename T>
void hex_to_bytes(std::string const& hex, T& bytes)
{
    if (0 != hex.size() % 2)
    {
        throw std::runtime_error("invalid lenght of hex string");
    }

    std::string::size_type const nchars = 2;
    std::string::size_type const nbytes = hex.size() / nchars;

    T(nbytes).swap(bytes);

    for (std::string::size_type i = 0; i < nbytes; ++i)
    {
        std::istringstream iss(hex.substr(i * nchars, nchars));
        unsigned int n(0);
        if (!(iss >> std::hex >> n))
        {
            throw std::runtime_error("hex to binary failed");
        }
        bytes[i] = static_cast<typename T::value_type>(n);
    }
}

template <typename T>
void bytes_to_hex(T const& bytes, std::string& hexstr)
{
    typedef typename T::size_type size_type;
    size_type const nbytes = bytes.size();

    const char hexchrs[] = "0123456789ABCDEF";
    char hexbyte[3] = { 0 };

    std::ostringstream oss;
    for (size_type i = 0; i < nbytes; ++i)
    {
        boost::uint8_t byte = static_cast<boost::uint8_t>(bytes[i]);
        hexbyte[0] = hexchrs[(byte >> 4) & 0xf];
        hexbyte[1] = hexchrs[byte & 0xf];
        hexbyte[2] = '\0';
        oss << std::setw(2) << hexbyte;
    }
    hexstr = oss.str();
    
    if (hexstr.size() != nbytes * 2)
    {
        throw std::runtime_error("binary to hex failed");
    }
}

typedef std::vector<unsigned char> byte_array;

} // anonymous namespace

int main()
{
    using namespace mloskot;
    try
    {
        std::string const hex_little("010C0000005839B4C876BEF33F83C0CAA145B61640");
        std::cout << hex_little << std::endl;

        byte_array bytes;
        hex_to_bytes(hex_little, bytes);

        endian::endian_value<boost::uint8_t> ev8;
        ev8.load<endian::little_endian_tag>(bytes.begin());

        std::cout << (int)ev8 << std::endl;

        endian::endian_value<boost::uint32_t> ev32;
        ev32.load<endian::little_endian_tag>(bytes.begin() + 1);

        std::cout << ev32 << std::endl;

        endian::endian_value<double> ev64d1;
        ev64d1.load<endian::little_endian_tag>(bytes.begin() + 1 + 4);

        std::cout << ev64d1 << std::endl;

        endian::endian_value<double> ev64d2;
        ev64d2.load<endian::little_endian_tag>(bytes.begin() + 1 + 4 + 8);

        std::cout << ev64d2 << std::endl;
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
