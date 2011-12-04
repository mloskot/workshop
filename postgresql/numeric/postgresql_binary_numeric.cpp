//
// Copyright (C) 2011 Mateusz Loskot <mateusz@loskot.net>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <libpq-fe.h>
#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
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
    enum 
    {
        NBase = 10000,
        HalfNBase =5000,
        Positive = 0x0000,
        Negative = 0x4000,
        NaN = 0xC000,
        Null =  0xF000,
        MaxPrevision = 1000,
        MinScale = 0,
        MaxScale = MaxPrevision,
        MinSigDgits = 16,
        DecimalDigits = 4
    };

    int weight;
    int sign;
    int display_scale;
    std::vector<short> digits;
};

numeric read_numeric(char* val)
{
    numeric n;

    short* s = reinterpret_cast<short*>(val);
    int const ndigits = ntohs(*s); s++;
    n.weight = ntohs(*s); s++;
    n.sign = ntohs(*s); s++;
    n.display_scale = ntohs(*s); s++;

    n.digits.resize(ndigits);
    for (auto i = 0U; i < n.digits.size(); ++i)
    {
        n.digits[i] = ntohs(*s);
        s++;
    }
    return n;
}

static const int round_powers[4] = {0, 1000, 100, 10};

void round_var(numeric& var, int rscale)
{
	short *digits = &var.digits[0];
	int			ndigits;
    int carry;
	var.display_scale = rscale;

	// decimal digits wanted
	int di = (var.weight + 1) * numeric::DecimalDigits + rscale;

	// If di = 0, the value loses all digits, but could round up to 1 if its
	// first extra digit is >= 5.  If di < 0 the result must be 0.
	if (di < 0)
	{
        var.digits.resize(0);
		var.weight = 0;
        var.sign = numeric::Positive;
	}
	else
	{
		/* NBASE digits wanted */
		ndigits = (di + numeric::DecimalDigits - 1) / numeric::DecimalDigits;

		/* 0, or number of decimal digits to keep in last NBASE digit */
		di %= numeric::DecimalDigits;

        if (ndigits < var.digits.size() ||
			(ndigits == var.digits.size() && di > 0))
		{
			//var->ndigits = ndigits;

			if (di == 0)
                carry = (digits[ndigits] >= numeric::HalfNBase) ? 1 : 0;
			else
			{
				/* Must round within last NBASE digit */
				int			extra,
							pow10;

				pow10 = round_powers[di];
				extra = digits[--ndigits] % pow10;
				digits[ndigits] = digits[ndigits] - (short) extra;
				carry = 0;
				if (extra >= pow10 / 2)
				{
					pow10 += digits[ndigits];
                    if (pow10 >= numeric::NBase)
					{
						pow10 -= numeric::NBase;
						carry = 1;
					}
					digits[ndigits] = (short) pow10;
				}
			}

			// Propagate carry if needed
			while (carry)
			{
				carry += digits[--ndigits];
				if (carry >= numeric::NBase)
				{
					digits[ndigits] = (short) (carry - numeric::NBase);
					carry = 1;
				}
				else
				{
					digits[ndigits] = (short) carry;
					carry = 0;
				}
			}

			if (ndigits < 0)
			{
				//var.digits--;
				//var.ndigits++;
				var.weight++;
			}
		}
	}
}

std::string get_str_from_numeric(numeric& var, int display_scale)
{
    std::string str;
    if (var.sign == numeric::NaN)
	{
		str = "NaN";
		return str;
	}

	// Check if we must round up before printing the value and do so.
    int ndigits = var.digits.size();

    round_var(var, display_scale);
	int i = display_scale + var.weight + 1;
 //   if (i >= 0 && ndigits > i)
	//{
	//	int carry = (var.digits[i] > 4) ? 1 : 0;
 //       ndigits = i;

	//	while (carry)
	//	{
	//		carry += var.digits[--i];
	//		var.digits[i] = carry % 10;
	//		carry /= 10;
	//	}

	//	if (i < 0)
	//	{
	//		// XXX var.digits--;
	//		ndigits++;
	//		var.weight++;
	//	}
	//}
	//else
 //   {
	//	ndigits = (std::max)(0, (std::min)(i, ndigits));
 //   }

	// Allocate space for the result
	str.resize((std::max)(0, display_scale) + (std::max)(0, var.weight) + 4);

	// Output a dash for negative values
    auto cp = str.begin();
    if (var.sign == numeric::Negative)
		*cp++ = '-';

	// Output all digits before the decimal point
	i = (std::max)(var.weight, 0);
	int d = 0;
	while (i >= 0)
	{
		if (i <= var.weight && d < ndigits)
			*cp++ = var.digits[d++] + '0';
		else
			*cp++ = '0';
		i--;
	}

	// If requested, output a decimal point and all the digits that follow it.
	if (display_scale > 0)
	{
		*cp++ = '.';
		while (i >= -display_scale)
		{
			if (i <= var.weight && d < ndigits)
				*cp++ = var.digits[d++] + '0';
			else
				*cp++ = '0';
			i--;
		}
	}

	return str;
}

template <typename T>
void echo(T const& t)
{
    std::for_each(t.cbegin(), t.cend(), [](short d) { std::cout << d; });
    std::cout << std::endl;
}

int main()
{
    try
    {
        std::string conninfo = "dbname=mloskot user=mloskot";
        connection_t conn = connect(conninfo);

        // num20 => NUMERIC(20, 0)
        // num202 => NUMERIC(20, 2)
        result_t res = exec(conn, "SELECT num20, num202 FROM test_data_types LIMIT 1");

        int fn = PQfnumber(res.get(), "num20");
        char* val = PQgetvalue(res.get(), 0, fn);
        auto num20 = read_numeric(val);

        fn = PQfnumber(res.get(), "num202");
        val = PQgetvalue(res.get(), 0, fn);
        auto num202 = read_numeric(val);

        // TODO: write digits to string repr, handle all properties
        //std::string snum20 = get_str_from_numeric(num20, num20.display_scale);
        std::string snum202 = get_str_from_numeric(num202, num202.display_scale);

        return EXIT_SUCCESS;

    }
    catch (std::exception const& e)
    {
        std::clog << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
