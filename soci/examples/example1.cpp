#ifdef _MSC_VER
#pragma comment(lib, "libsoci_core-vc80-d-3_0.lib")
#pragma comment(lib, "libsoci_postgresql-vc80-d-3_0.lib")
#endif

// soci
#include <soci.h>
#include <soci-postgresql.h>
// std
#include <exception>
#include <iostream>
#include <istream>
#include <ostream>
#include <string>

using namespace std;

bool getName(string &name)
{
    cout << "Enter name: ";
    return !!(cin >> name);
}

int main()
{
    try
    {
        soci::session sql(soci::postgresql, "dbname=soci_test user=mloskot");

        int count(0);
        sql << "select count(*) from phonebook", soci::into(count);

        cout << "We have " << count << " entries in the phonebook.\n";

        string name;
        while (getName(name))
        {
            string phone;
            soci::indicator ind;
            sql << "select phone from phonebook where name = :name", soci::into(phone, ind), soci::use(name);

            std::clog << sql.get_last_query() << std::endl;

            if (ind == soci::i_ok)
            {
                cout << "The phone number is " << phone << '\n';
            }
            else
            {
                cout << "There is no phone for " << name << '\n';
            }
        }
    }
    catch (exception const &e)
    {
        cerr << "Error: " << e.what() << '\n';
    }
}
