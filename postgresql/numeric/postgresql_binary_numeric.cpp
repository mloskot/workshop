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

std::vector<short> read_digits(char* val)
{
    short* s = reinterpret_cast<short*>(val);
    int ndigits = ntohs(*s); s++;
    int weight  = ntohs(*s); s++;
    int sign    = ntohs(*s); s++;
    int dscale  = ntohs(*s); s++;

    std::vector<short> digits(ndigits);
    for (auto i = 0U; i < digits.size(); ++i, s++)
    {
        digits[i] = ntohs(*s);
    }
    return digits;
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
        auto num20 = read_digits(val);

        fn = PQfnumber(res.get(), "num202");
        val = PQgetvalue(res.get(), 0, fn);
        auto num202 = read_digits(val);

        // TODO: write digits to string repr, handle all properties
        echo(num20);
        echo(num202);
    }
    catch (std::exception const& e)
    {
        std::clog << e.what() << std::endl;
    }
}
