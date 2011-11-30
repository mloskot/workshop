#include <libpq-fe.h>
#include <functional>
#include <memory>
typedef std::unique_ptr<PGConn, std::function<void(PGConn*)>> connection_t;
int main()
{
    std::string conninfo;
    connection_t conn(PQconnectdb(conninfo.c_str(), [&](PGConn* c) {PQfinish(c);});
}