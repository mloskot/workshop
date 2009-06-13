// Load/Store values from/to stream of bytes across different endianness.
//
// Copyright Mateusz Loskot <mateusz@loskot.net> 2009
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// Original design of unrolled_byte_loops templates taken from Boost C++ Libraries,
// source boost/spirit/home/support/detail/integer/endian.hpp
// Copyright Darin Adler 2000
// Copyright Beman Dawes 2006, 2009
//
#ifndef MLOSKOT_ENDIAN_HPP_INCLUDED
#define MLOSKOT_ENDIAN_HPP_INCLUDED

#include <boost/config.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/cstdint.hpp>
#include <boost/static_assert.hpp>

#include <cassert>
#include <climits>
#include <cstring>

#if CHAR_BIT != 8
#error Platforms with CHAR_BIT != 8 are not supported
#endif

namespace mloskot { namespace endian {

// Unrolled loops for loading and storing streams of bytes.

template <typename T, std::size_t N, bool Sign = boost::is_signed<T>::value>
struct unrolled_byte_loops
{
    typedef unrolled_byte_loops<T, N - 1, Sign> next;

    template <typename Iterator>
    static T load_forward(Iterator& bytes)
    {
        return *bytes++ | (next::load_forward(bytes) << 8);
    }

    template <typename Iterator>
    static T load_backward(Iterator& bytes)
    {
        return *(bytes - 1) | (next::load_backward(--bytes) << 8);
    }
    
    template <typename Iterator>
    static void store_forward(Iterator& bytes, T value)
    {
        *bytes = static_cast<char>(value);
        next::store_forward(++bytes, value >> 8);
    }

    template <typename Iterator>
    static void store_backward(Iterator& bytes, T value)
    {
        *(bytes - 1) = static_cast<char>(value);
        next::store_backward(--bytes, value >> 8);
    }
};

template <typename T>
struct unrolled_byte_loops<T, 1, false>
{
    template <typename Iterator>
    static T load_forward(Iterator& bytes)
    {
        return *bytes;
    }

    template <typename Iterator>
    static T load_backward(Iterator& bytes)
    {
        return *(bytes - 1);
    }

    template <typename Iterator>
    static void store_forward(Iterator& bytes, T value)
    {
        // typename Iterator::value_type
        *bytes = static_cast<char>(value);
    }

    template <typename Iterator>
    static void store_backward(Iterator& bytes, T value)
    {
        *(bytes - 1) = static_cast<char>(value);
    }
};

template <typename T>
struct unrolled_byte_loops<T, 1, true>
{    
    template <typename Iterator>
    static T load_forward(Iterator& bytes)
    {
        return *reinterpret_cast<const signed char*>(&*bytes);
    }

    template <typename Iterator>
    static T load_backward(Iterator& bytes)
    {
        return *reinterpret_cast<const signed char*>(&*(bytes - 1));
    }
    
    template <typename Iterator>
    static void store_forward(Iterator& bytes, T value)
    {
        BOOST_STATIC_ASSERT(boost::is_signed<typename Iterator::value_type>::value);

        *bytes = static_cast<typename Iterator::value_type>(value);
    }

    template <typename Iterator>
    static void store_backward(Iterator& bytes, T value)
    {
        BOOST_STATIC_ASSERT(boost::is_signed<typename Iterator::value_type>::value);

        *(bytes - 1) = static_cast<typename Iterator::value_type>(value);
    }
};

// Types of endianness

struct big_endian_tag {};
struct little_endian_tag {};

#ifdef BOOST_BIG_ENDIAN
    typedef big_endian_tag native_endian_tag;
#else
    typedef little_endian_tag native_endian_tag;
#endif

namespace detail {

template <typename T, std::size_t N, typename Iterator, typename E>
T load_dispatch(Iterator& bytes, E, E)
{
    return unrolled_byte_loops<T, N>::load_forward(bytes);
}

template <typename T, std::size_t N, typename Iterator, typename E1, typename E2>
T load_dispatch(Iterator& bytes, E1, E2)
{
    std::advance(bytes, N);
    return unrolled_byte_loops<T, N>::load_backward(bytes);
}

template <typename T, std::size_t N, typename Iterator, typename E>
void store_dispatch(Iterator& bytes, T value, E, E)
{
    return unrolled_byte_loops<T, N>::store_forward(bytes, value);
}

template <typename T, std::size_t N, typename Iterator, typename E1, typename E2>
void store_dispatch(Iterator& bytes, T value, E1, E2)
{
    std::advance(bytes, N);
    return unrolled_byte_loops<T, N>::store_backward(bytes, value);
}

template <typename T>
struct endian_value_base
{
    typedef T value_type;
    typedef native_endian_tag endian_type;

    endian_value_base() : value(T()) {}
    explicit endian_value_base(T value) : value(value) {}

    operator T() const
    {
        return value;
    }

protected:
    T value;
};

} // namespace detail

template <typename T, std::size_t N = sizeof(T)>
struct endian_value : public detail::endian_value_base<T>
{
    typedef detail::endian_value_base<T> base;

    endian_value() {}
    explicit endian_value(T value) : base(value) {}

    template <typename E, typename Iterator>
    void load(Iterator bytes)
    {
        base::value = detail::load_dispatch<T, N>(bytes, base::endian_type(), E());
    }

    template <typename E, typename Iterator>
    void store(Iterator bytes)
    {
        detail::store_dispatch<T, N>(bytes, base::value, base::endian_type(), E());
    }
};

template <>
struct endian_value<double, 8> : public detail::endian_value_base<double>
{
    typedef detail::endian_value_base<double> base;

    endian_value() {}
    explicit endian_value(double value) : base(value) {}

    template <typename E, typename Iterator>
    void load(Iterator bytes)
    {
        endian_value<boost::uint64_t, 8> raw;
        raw.load<E>(bytes);

        double& target_value = base::value;
        std::memcpy(&target_value, &raw, sizeof(double));
    }

    template <typename E, typename Iterator>
    void store(Iterator bytes)
    {
        boost::uint64_t raw;
        double const& source_value = base::value;
        std::memcpy(&raw, &source_value, sizeof(boost::uint64_t));

        detail::store_dispatch<boost::uint64_t, sizeof(boost::uint64_t)>(bytes, raw, base::endian_type(), E());
    }
};

}} // namespace mloskot::endian

#endif // MLOSKOT_ENDIAN_HPP_INCLUDED
