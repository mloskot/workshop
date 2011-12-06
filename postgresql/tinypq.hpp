//
// A very tiny C++ wrapper for (subset of) libpq C API
//
// Copyright (C) 2011 Mateusz Loskot <mateusz@loskot.net>
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef TINYPQ_HPP_INCLUDED
#define TINYPQ_HPP_INCLUDED

#ifdef _MSC_VER
#if (_MSC_VER < 1600)
#error "Visual Studio  2010 or later required to support C++11 features.
#endif
#endif

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <libpq-fe.h>

namespace tinypq
{

typedef std::unique_ptr<PGconn, std::function<void(PGconn*)>> connection;
typedef std::unique_ptr<PGresult, std::function<void(PGresult*)>> result;

void require_connection(connection const& conn)
{
    if (PQstatus(conn.get()) != CONNECTION_OK)
        throw std::runtime_error("no connection");
}

void require_tuples(result const& res)
{
    if (PQresultStatus(res.get()) != PGRES_TUPLES_OK)
        throw std::runtime_error("no tuples");
}
connection connect(std::string info)
{
    connection conn(PQconnectdb(info.c_str()), [](PGconn* c) { PQfinish(c); });

    if (PQstatus(conn.get()) != CONNECTION_OK)
        throw std::runtime_error("no connection");

    return conn;
}

result execute(connection const& conn, std::string sql, bool binary = true)
{
    require_connection(conn);

    int const fmt = binary ? 1 : 0;
    result res(PQexecParams(conn.get(), sql.c_str(), 0, 0, 0, 0, 0, fmt), 
        [](PGresult* r) { PQclear(r); });

    if (!(PQresultStatus(res.get()) == PGRES_TUPLES_OK || PQresultStatus(res.get()) == PGRES_COMMAND_OK))
        throw std::runtime_error(PQerrorMessage(conn.get()));

    return res;
}

char* fetch(result const& res, int row, int column)
{
    require_tuples(res);

    if (row < 0)
        throw std::invalid_argument("row position is negative");

    if (column < 0)
        throw std::invalid_argument("column position is negative");

    return PQgetvalue(res.get(), row, column);
}

char* fetch(result const& res, int row, std::string field)
{
    require_tuples(res);

    int const fn = PQfnumber(res.get(), field.c_str());
    return fetch(res, row, fn);
}

} // namespace tinypq

#endif // TINYPQ_HPP_INCLUDED
