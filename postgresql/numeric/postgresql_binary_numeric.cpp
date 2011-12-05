//
// Copyright (C) 2011 Mateusz Loskot <mateusz@loskot.net>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <libpq-fe.h>
#include <cassert>
#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
// Windows
#include <float.h>
#include <winsock2.h> // ntohs
typedef std::unique_ptr<PGconn, std::function<void(PGconn*)>> connection_t;
typedef std::unique_ptr<PGresult, std::function<void(PGresult*)>> result_t;

connection_t connect(std::string info)
{
    connection_t conn(PQconnectdb(info.c_str()), [](PGconn* c) { PQfinish(c); });
    if (PQstatus(conn.get()) != CONNECTION_OK)
        throw std::runtime_error("no connection");
    return conn;
}

result_t exec(connection_t& conn, std::string sql, bool binary = true)
{
    result_t res(PQexecParams(conn.get(), sql.c_str(), 0, 0, 0, 0, 0, binary ? 1 : 0), [](PGresult* r) { PQclear(r); });
    if (!(PQresultStatus(res.get()) == PGRES_TUPLES_OK || PQresultStatus(res.get()) == PGRES_COMMAND_OK))
        throw std::runtime_error(PQerrorMessage(conn.get()));
    return res;
}

struct numeric
{
    enum sign_mode
    {
        positive = 0x0000,
        negative = 0x4000,
        NaN = 0xC000,
        null =  0xF000
    };

    enum precision_range
    {
        max_precision = 1000,
        min_scale = 0,
        max_scale = max_precision,
    };

    std::vector<short> digits;
    sign_mode mode;
    int sign;
    int exp;
};

double floor_div(double dividend, double divisor)
{
    if (divisor == 0.0)
        throw std::invalid_argument("division by zero");

    double mod = fmod(dividend, divisor);

    // In float-point arithmetic, dividend - mod is an approximation.
    // Thus after division the div may not be an exact integral value, although
    // it will always be very close to one.
    double div = (dividend - mod) / divisor;
    if (mod != 0)
    {
        // Ensure the remainder has the same sign as the denominator.
        if ((divisor < 0) != (mod < 0))
        {
            mod += divisor;
            div -= 1.0;
        }
    }
    else
    {
        // The remainder is zero, ensure it has the same sign as the denominator.
        mod = _copysign(0.0, divisor);
    }

    // Floor quotient to nearest integral value.
    double quotient(0);
    if (div)
    {
        quotient = floor(div);
        if (div - quotient > 0.5)
            quotient += 1.0;
    }
    else
    {
        // Division result is zero - get the same sign as the true quotient */
        quotient = _copysign(0.0, dividend / divisor);
    }

    return quotient;
}

numeric read_numeric(char* val)
{
    enum { decimal_digits_per_nbase = 4 }; // decimal digits per word

    short* s = reinterpret_cast<short*>(val);
    u_short const words_count = ntohs(*s); s++;
    signed short first_word_weight = ntohs(*s); s++;
    u_short const sign = ntohs(*s); s++;
    u_short display_scale = ntohs(*s); s++;
    assert(display_scale >= 0);
    std::clog << "X: nwords=" << words_count << " weight=" << first_word_weight << " sign=" << sign << " dscale=" << display_scale << std::endl;
    
    numeric n;

    if (sign == numeric::positive)
    {
        n.mode = numeric::positive;
        n.sign = 0;
    }
    else if (sign == numeric::negative)
    {
        n.mode = numeric::negative;
        n.sign = 1;
    }
    else if (sign == numeric::NaN)
    {
        n.mode = numeric::NaN;
        return n;
    }
    else
    {
        throw std::invalid_argument("invalid sign");
    }

    std::vector<short> digits;
    int exp = 0;
    if (words_count == 0)
    {
        exp = -display_scale;
        digits.push_back(0);
    }
    else
    {
        assert(words_count > 0);
        std::vector<short> words(words_count);
        for (auto i = 0U; i < words_count; ++i)
        {
            words[i] = ntohs(*s);
            s++;
        }

        std::array<short, 4> const shifts = { 1000, 100, 10, 1 };
        for(auto it(words.cbegin()); it != words.cend(); ++it)
        {
            for (std::size_t si = 0; si < shifts.size(); ++si)
            {
                double const fd  = floor_div(*it, shifts[si]);
                short const digit = static_cast<int>(fd) % 10;
                assert(digit < 10);
                if (!digits.empty() || digit != 0)
                    digits.push_back(digit);
            }
        }

        // There are weight + 1 digits before the decimal point.
        exp = (first_word_weight + 1 - words_count) * decimal_digits_per_nbase;
        if (0 < exp && exp < 20)
        {
            std::fill_n(std::back_inserter(digits), exp, 0);
            exp = 0;
        }

        if (display_scale > 0)
        {
            int const zerofill = display_scale + exp;
            if (zerofill < 0)
            {
                auto last = digits.cbegin() + (digits.size() + zerofill);
                digits.erase(last, digits.cend());
                exp -= zerofill;
            }
            else if (zerofill > 0)
            {
                std::fill_n(std::back_inserter(digits), zerofill, 0);
                exp -= zerofill;
            }
        }
    }

    assert(!digits.empty());

    n.digits = digits;
    n.exp = exp;
    return n;
}

template <typename T>
void echo(std::string col, numeric const& n, T value)
{
    std::cout << col << ":\tsign=" << n.sign << "\texp=" << n.exp << "\tdigits=";
    std::for_each(n.digits.cbegin(), n.digits.cend(), [](short d) { std::cout << d; });
    std::cout << "\tvalue=" << value << std::endl;
}

int main()
{
    try
    {
        std::string conninfo = "dbname=mloskot user=mloskot";
        connection_t conn = connect(conninfo);
        result_t res = exec(conn, "SELECT dec, num, num20, num202, num63, num05, numneg20, numneg202, numneg63, numneg05 FROM test_data_types LIMIT 1");

        std::array<char const*, 10> cols = 
        { "dec", "num", "num20", "num202", "num63", "num05", "numneg20", "numneg202", "numneg63", "numneg05" };

        std::for_each(cols.cbegin(), cols.cend(), [&res](char const* col)
        {
            int fn = PQfnumber(res.get(), col);
            char* val = PQgetvalue(res.get(), 0, fn);
            auto n = read_numeric(val);

            // make a decimal value
            std::string scoeff;
            scoeff.reserve(n.digits.size());
            std::for_each(n.digits.cbegin(), n.digits.cend(), [&scoeff](short c) { scoeff += std::to_string(c); });
            long long const coeff = std::stoll(scoeff);
            double const value = std::pow(-1.0, n.sign) * coeff * std::pow(10, n.exp);

            echo(col, n, value);
        });

        return EXIT_SUCCESS;

    }
    catch (std::exception const& e)
    {
        std::clog << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
