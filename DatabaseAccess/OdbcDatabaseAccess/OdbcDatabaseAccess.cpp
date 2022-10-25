#include <iostream>
#include <iomanip>
#include <string>
#include <locale>
#include <ctime>

#include <soci/soci.h>
#include <soci/odbc/soci-odbc.h>

using std::operator""s;

// https://soci.sourceforge.net/doc/master/
// https://soci.sourceforge.net/doc/master/statements/
// https://soci.sourceforge.net/doc/master/backends/odbc/

import OdbcDatabaseAccess.Models;
import OdbcDatabaseAccess.BasicQueryModule;
import OdbcDatabaseAccess.NorthwindContext;

// #define SqlQuery(...) #__VA_ARGS__

int32_t main()
{
  NorthwindContext context;

  try
  {
    for (auto& table_info : context.Database().TableInformation())
    {
      std::cout << "Table Name: " << table_info.TableName << std::endl;

      for (auto& column_info : table_info.Columns())
      {
        std::cout << "\tType: " << std::to_string(column_info.type) << std::endl;
      }

      std::cout << "\n\n";
    }

    for (auto& [schema_name, table_name, create_date, modify_date] : context.GetTables())
    {
      char create_date_string[26];
      strftime(create_date_string, sizeof(create_date_string), "%c", &create_date);

      char modify_date_string[26];
      strftime(modify_date_string, sizeof(modify_date_string), "%c", &modify_date);

      printf_s("[%s].[%s] C: \"%s\", M: \"%s\"\n", schema_name.c_str(), table_name.c_str(),
        create_date_string, modify_date_string);
    }

    long long AffectedRows = context.Database().ExecuteSql(R"(
      INSERT INTO [dbo].[Region] ([RegionId], [RegionDescription])
      VALUES (:RegionId, :RegionDescription)
    )", soci::use(5, "RegionId"), soci::use("Region #5"s, "RegionDescription"));
  }
  catch (soci::odbc_soci_error const& ex)
  {
    std::cerr << "\n\n"
      << "ODBC Error Code:   " << ex.odbc_error_code() << std::endl
      << "Native Error Code: " << ex.native_error_code() << std::endl
      << "SOCI Message:      " << ex.what() << std::endl
      << "ODBC Message:      " << ex.odbc_error_message() << std::endl;
  }
  catch (std::exception const& ex)
  {
    std::cerr << "Error: " << ex.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
