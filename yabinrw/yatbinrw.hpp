// Yet Another Tiny Binary Reader/Writer
//
// Copyright Mateusz Loskot <mateusz@loskot.net> 2009
// Copyright Darin Adler 2000
// Copyright Beman Dawes 2006, 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Original design of unrolled_byte_loops templates taken from Boost C++ Libraries
// (boost/spirit/home/support/detail/integer/endian.hpp)
//

#ifndef YATBINRW_HPP_INCLUDED
#define YATBINRW_HPP_INCLUDED

#include <boost/cstdint.hpp>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_signed.hpp>

#include <cassert>
#include <climits>
#include <algorithm>
#include <iosfwd>

#include <iostream>

#if CHAR_BIT != 8
#error Platforms with CHAR_BIT != 8 are not supported
#endif

namespace yatbinrw
{

// Unrolled loops for loading and storing streams of bytes.

BOOST_STATIC_ASSERT(CHAR_BIT == 8);

template <typename T, std::size_t N, bool Sign = boost::is_signed<T>::value>
struct unrolled_byte_loops
{
    typedef unrolled_byte_loops<T, N - 1, Sign> next;

    static T load_big(const unsigned char* bytes)
    {
        return *(bytes - 1) | (next::load_big(bytes - 1) << 8);
    }

    template <typename Iterator>
    static T load_big(Iterator& bytes)
    {
        return *(bytes - 1) | (next::load_big(--bytes) << 8);
    }

    static T load_little(const unsigned char* bytes)
    {
        return *bytes | (next::load_little(bytes + 1) << 8);
    }

    template <typename Iterator>
    static T load_little(Iterator& bytes)
    {
        return *bytes++ | (next::load_little(bytes) << 8);
    }

    static void store_big(char* bytes, T value)
    {
        *(bytes - 1) = static_cast<char>(value);
        next::store_big(bytes - 1, value >> 8);
    }
    
    static void store_little(char* bytes, T value)
    {
        *bytes = static_cast<char>(value);
        next::store_little(bytes + 1, value >> 8);
    }
};

template <typename T>
struct unrolled_byte_loops<T, 1, false>
{
    static T load_big(const unsigned char* bytes)
    {
        return *(bytes - 1);
    }

    template <typename Iterator>
    static T load_big(Iterator& bytes)
    {
        return *(bytes - 1);
    }

    static T load_little(const unsigned char* bytes)
    {
        return *bytes;
    }
    
    template <typename Iterator>
    static T load_little(Iterator& bytes)
    {
        return *bytes;
    }

    static void store_big(char* bytes, T value)
    {
        *(bytes - 1) = static_cast<char>(value);
    }
    
    static void store_little(char* bytes, T value)
    {
        *bytes = static_cast<char>(value);
    }
};

template <typename T>
struct unrolled_byte_loops<T, 1, true>
{
    static T load_big(const unsigned char* bytes)
    {
        return *reinterpret_cast<const signed char*>(bytes - 1);
    }
    
    template <typename Iterator>
    static T load_big(Iterator& bytes)
    {
        return *reinterpret_cast<const signed char*>(&*(bytes - 1));
    }

    static T load_little(const unsigned char* bytes)
    {
        return *reinterpret_cast<const signed char*>(bytes);
    }

    template <typename Iterator>
    static T load_little(Iterator& bytes)
    {
        return *reinterpret_cast<const signed char*>(&*bytes);
    }
    
    static void store_big(char* bytes, T value)
    {
        *(bytes - 1) = static_cast<char>(value);
    }
    
    static void store_little(char* bytes, T value)
    {
        *bytes = static_cast<char>(value);
    }
};

template <typename T, std::size_t N>
inline T load_big_endian(const void* bytes)
{
    const unsigned char* ptr = static_cast<const unsigned char*>(bytes);
    return unrolled_byte_loops<T, N>::load_big(ptr + N);
}

template <typename T, std::size_t N, typename Iterator>
inline T load_big_endian(Iterator bytes)
{
    Iterator it(bytes + N);
    return unrolled_byte_loops<T, N>::load_big(it);
}

template <typename T, std::size_t N>
inline T load_little_endian(const void* bytes)
{
    const unsigned char* ptr = static_cast<const unsigned char*>(bytes);
    return unrolled_byte_loops<T, N>::load_little(ptr);
}

template <typename T, std::size_t N, typename Iterator>
inline T load_little_endian(Iterator bytes)
{
    return unrolled_byte_loops<T, N>::load_little(bytes);
}

template <typename T, std::size_t N>
inline void store_big_endian(void* bytes, T value)
{
    char* ptr = static_cast<char*>(bytes);
    unrolled_byte_loops<T, N>::store_big(ptr + N, value);
}

template <typename T, std::size_t N>
inline void store_little_endian(void* bytes, T value)
{
    char* ptr = static_cast<char*>(bytes);
    unrolled_byte_loops<T, N>::store_little(ptr, value);
}

} // namespace yatbinrw

#endif // YATBINRW_HPP_INCLUDED
