module;

#include <soci/soci.h>

#include <chrono>
#include <ctime>
#include <compare>
#include <string>
#include <vector>
#include <tuple>

export module OdbcDatabaseAccess.NorthwindContext;
import OdbcDatabaseAccess.SqlServer;

export class NorthwindContext : public OdbcDatabaseAccess::OdbcContext
{
public:
  NorthwindContext() : OdbcContext("Driver={ODBC Driver 17 for SQL Server};Server=DESKTOP-KQTC93V;Database=Northwind;Uid=sa;Pwd=SQLServer;Connection Timeout=30;")
  {
  }

  auto GetTables()
  {
    std::tuple<std::string, std::string, std::tm, std::tm> values;
    std::vector<decltype(values)> table_names;

    soci::statement statement = (m_sql.prepare << R"query(
      select 
       schema_name(t.schema_id) as schema_name,
       t.name as table_name,
       t.create_date,
       t.modify_date
      from sys.tables t
      order by schema_name, table_name;
    )query"
      , soci::into(std::get<0>(values))
      , soci::into(std::get<1>(values))
      , soci::into(std::get<2>(values))
      , soci::into(std::get<3>(values)));

    statement.execute();

    while (statement.fetch())
    {
      table_names.push_back(std::move(values));
    }

    return table_names;
  }
};
