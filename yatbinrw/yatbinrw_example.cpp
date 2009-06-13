// Yet Another Tiny Binary Reader/Writer
//
// Simple test and usage example
//
// Copyright Mateusz Loskot <mateusz@loskot.net> 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include "yatbinrw.hpp"

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

#ifndef BOOST_LITTLE_ENDIAN
#error yabinrw example runs on little-endian machines only - no bytes swapping provided
#endif

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
    try
    {
        std::cout << "load:\t";
        {
            std::cout << "1, 12, 1.234, 5.678" << std::endl;

            boost::uint8_t n8(0);
            boost::uint32_t n32(0);
            boost::uint64_t n64(0);
            double ad64(0);
            double bd64(0);

            std::cout << "\tfrom little-endian:\t";
            {
                std::string const hex_little("010C0000005839B4C876BEF33F83C0CAA145B61640");
                std::cout << hex_little << std::endl;

                byte_array bytes;
                hex_to_bytes(hex_little, bytes);

                n8 = yatbinrw::load_little_endian<boost::uint8_t, sizeof(boost::uint8_t)>(bytes.begin());
                std::cout << "\t\tn8 = " << (int)n8 << std::endl;

                n32 = yatbinrw::load_little_endian<boost::uint32_t, sizeof(boost::uint32_t)>(bytes.begin() + 1);
                std::cout << "\t\tn32 = " << n32 << std::endl;

                n64 = yatbinrw::load_little_endian<boost::uint64_t, sizeof(boost::uint64_t)>(bytes.begin() + 1 + 4);
                std::memcpy(&ad64, &n64, sizeof(double));
                std::cout << "\t\tad64 = " << ad64 << " (" << n64 << " )" << std::endl;

                n64 = yatbinrw::load_little_endian<boost::uint64_t, sizeof(boost::uint64_t)>(bytes.begin() + 1 + 4 + 8);
                std::memcpy(&bd64, &n64, sizeof(double));
                std::cout << "\t\tbd64 = " << bd64 << " (" << n64 << " )" << std::endl;
            }

            std::cout << "\tfrom big-endian:\t";
            {
                std::string const hex_big("010000000C3FF3BE76C8B439584016B645A1CAC083");
                std::cout << hex_big << std::endl;

                byte_array bytes;
                hex_to_bytes(hex_big, bytes);        

                n8 = yatbinrw::load_big_endian<boost::uint8_t, sizeof(boost::uint8_t)>(bytes.begin());
                std::cout << "\t\tn8 = " << (int)n8 << std::endl;

                n32 = yatbinrw::load_big_endian<boost::uint32_t, sizeof(boost::uint32_t)>(bytes.begin() + 1);
                std::cout << "\t\tn32 = " << n32 << std::endl;

                n64 = yatbinrw::load_big_endian<boost::uint64_t, sizeof(boost::uint64_t)>(bytes.begin() + 1 + 4);
                std::memcpy(&ad64, &n64, sizeof(double));
                std::cout << "\t\tad64 = " << ad64 << " (" << n64 << " )" << std::endl;

                n64 = yatbinrw::load_big_endian<boost::uint64_t, sizeof(boost::uint64_t)>(bytes.begin() + 1 + 4 + 8);
                std::memcpy(&bd64, &n64, sizeof(double));
                std::cout << "\t\tbd64 = " << bd64 << " (" << n64 << " )" << std::endl;
            }
        } // load

        std::cout << "store:\t";
        {
            boost::uint8_t const n8(1);
            boost::uint32_t const n32(12);
            boost::uint64_t n64(0);
            double const ad64(1.234);
            double const bd64(5.678);

            std::cout << (int)n8 << ", " << n32 << ", " << ad64 << ", " << bd64 << std::endl;

            std::cout << "\tto little-endian:\t";
            {
                byte_array bytes; //empty
                std::back_insert_iterator<byte_array> it(std::back_inserter(bytes));

                yatbinrw::store_little_endian<boost::uint8_t, 1>(it, n8);
                yatbinrw::store_little_endian<boost::uint32_t, 4>(it, n32);

                std::memcpy(&n64, &ad64, sizeof(boost::uint64_t));
                yatbinrw::store_little_endian<boost::uint64_t, 8>(it, n64);

                std::memcpy(&n64, &bd64, sizeof(boost::uint64_t));
                yatbinrw::store_little_endian<boost::uint64_t, 8>(it, n64);

                std::string hex;
                bytes_to_hex(bytes, hex);
                std::cout << hex << std::endl;
            }

            std::cout << "\tto big-endian:";
            {
                byte_array bytes(1 + 4 + 8 + 8); // non-empty

                yatbinrw::store_big_endian<boost::uint8_t, 1>(bytes.begin(), n8);
                yatbinrw::store_big_endian<boost::uint32_t, 4>(bytes.begin() + 1, n32);

                std::memcpy(&n64, &ad64, sizeof(boost::uint64_t));
                yatbinrw::store_big_endian<boost::uint64_t, 8>(bytes.begin() + 1 + 4, n64);

                std::memcpy(&n64, &bd64, sizeof(boost::uint64_t));
                yatbinrw::store_big_endian<boost::uint64_t, 8>(bytes.begin() + 1 + 4 + 8, n64);

                std::string hex;
                bytes_to_hex(bytes, hex);
                std::cout << '\t' << hex << std::endl;
            }

        } // store
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
