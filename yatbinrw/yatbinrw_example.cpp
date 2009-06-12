#include "yatbinrw.hpp"

#include <cstring>
#include <exception>
#include <iostream>
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

typedef std::vector<unsigned char> byte_array;

} // anonymous namespace

int main()
{
    try
    {
        std::string const txt("1, 12, 1.234, 1.234, 5.678");

        std::cout << "load: " << txt << std::endl;
        {
            boost::uint8_t n8(0);
            boost::uint32_t n32(0);
            boost::uint64_t n64(0);
            double d64(0);

            std::cout << "\tfrom little-endian:\n";
            {
                std::string const hex_little("010C0000005839B4C876BEF33F83C0CAA145B61640");    
                byte_array bytes;
                hex_to_bytes(hex_little, bytes);

                n8 = yatbinrw::load_little_endian<boost::uint8_t, sizeof(boost::uint8_t)>(bytes.begin());
                std::cout << "\t\tn8 = " << (int)n8 << std::endl;

                n32 = yatbinrw::load_little_endian<boost::uint32_t, sizeof(boost::uint32_t)>(bytes.begin() + 1);
                std::cout << "\t\tn32 = " << n32 << std::endl;

                n64 = yatbinrw::load_little_endian<boost::uint64_t, sizeof(boost::uint64_t)>(bytes.begin() + 1 + 4);
                std::memcpy(&d64, &n64, sizeof(double));
                std::cout << "\t\tn64 = " << n64 << std::endl;
                std::cout << "\t\td64 = " << d64 << std::endl;

                n64 = yatbinrw::load_little_endian<boost::uint64_t, sizeof(boost::uint64_t)>(bytes.begin() + 1 + 4 + 8);
                std::memcpy(&d64, &n64, sizeof(double));
                std::cout << "\t\tn64 = " << n64 << std::endl;
                std::cout << "\t\td64 = " << d64 << std::endl;
            }

            std::cout << "\tfrom big-endian\n";
            {
                std::string const hex_big("01000000013FF3BE76C8B439584016B645A1CAC083");
                byte_array bytes;
                hex_to_bytes(hex_big, bytes);        

                n8 = yatbinrw::load_big_endian<boost::uint8_t, sizeof(boost::uint8_t)>(bytes.begin());
                std::cout << "\t\tn8 = " << (int)n8 << std::endl;

                n32 = yatbinrw::load_big_endian<boost::uint32_t, sizeof(boost::uint32_t)>(bytes.begin() + 1);
                std::cout << "\t\tn32 = " << n32 << std::endl;

                n64 = yatbinrw::load_big_endian<boost::uint64_t, sizeof(boost::uint64_t)>(bytes.begin() + 1 + 4);
                std::memcpy(&d64, &n64, sizeof(double));
                std::cout << "\t\tn64 = " << n64 << std::endl;
                std::cout << "\t\td64 = " << d64 << std::endl;

                n64 = yatbinrw::load_big_endian<boost::uint64_t, sizeof(boost::uint64_t)>(bytes.begin() + 1 + 4 + 8);
                std::memcpy(&d64, &n64, sizeof(double));
                std::cout << "\t\tn64 = " << n64 << std::endl;
                std::cout << "\t\td64 = " << d64 << std::endl;
            }

        } // load
    }
    catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}