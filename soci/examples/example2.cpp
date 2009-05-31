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

struct PhonebookEntry
{
    std::string name;
    std::string phone;
};

namespace soci
{

template <data_type Type>
struct soci_data_type_tag { typedef std::string value; };
template <>
struct soci_data_type_tag<dt_string> { typedef std::string value; };
template <>
struct soci_data_type_tag<dt_double> { typedef double value; };
template <>
struct soci_data_type_tag<dt_integer> { typedef int value; };
template <>
struct soci_data_type_tag<dt_unsigned_long> { typedef unsigned long value; };
template <>
struct soci_data_type_tag<dt_long_long> { typedef long long value; };

// basic type conversion on many values (ORM)
template<>
struct type_conversion<PhonebookEntry>
{
    typedef soci::values base_type;

    static void from_base(values const &v, indicator /* ind */, PhonebookEntry &pe)
    {
        if (dt_string == v.get_properties("name").get_data_type())
        {
            pe.name = v.get<soci_data_type_tag<dt_string>::value>("name");
        }

        if (dt_string == v.get_properties("phone").get_data_type())
        {
            pe.phone = v.get<soci_data_type_tag<dt_string>::value>("phone", "<NULL>");
        }
    }

    static void to_base(PhonebookEntry const &pe, values &v, indicator &ind)
    {
        v.set("name", pe.name);
        v.set("phone", pe.phone, pe.phone.empty() ? i_null : i_ok);
        ind = i_ok;
    }
}; 
} // namespace soci

int main()
{
    try
    {
        soci::session sql(soci::postgresql, "dbname=soci_test user=mloskot");

        PhonebookEntry p1;

        int id(1);
        sql << "select * from phonebook where id = :id", soci::into(p1), soci::use(id);
        cout << p1.name << ": " << p1.phone << endl;
    }
    catch (exception const &e)
    {
        cerr << "Error: " << e.what() << '\n';
    }
}
