module;

#include <soci/soci.h>
#include <soci/connection-parameters.h>
#include <soci/odbc/soci-odbc.h>

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <string_view>

export module OdbcDatabaseAccess.BasicQueryModule;

export void TestGetCategories()
{
  try
  {
    soci::connection_parameters parameters(soci::odbc, "Driver={ODBC Driver 17 for SQL Server};Server=DESKTOP-KQTC93V;Database=Northwind;Uid=sa;Pwd=SQLServer;Connection Timeout=30;");
    parameters.set_option(soci::odbc_option_driver_complete, "0" /* SQL_DRIVER_NOPROMPT */);
    soci::session sql_server(parameters);
    std::cout << std::boolalpha << "SqlServer.is_connected(): " << sql_server.is_connected() << std::endl;

    soci::rowset<soci::row> reader = (sql_server.prepare <<
      "SELECT TOP (1000) [CategoryID], [CategoryName], [Description], [Picture] "
      "FROM [Northwind].[dbo].[Categories]");

    std::filesystem::create_directory("bin");

    for (soci::rowset<soci::row>::const_iterator it = reader.begin(); it != reader.end(); ++it)
    {
      auto image = it->get<std::string>(3);

      std::cout
        << "CategoryID:   " << it->get<int32_t>(0) << '\n'
        << "CategoryName: " << it->get<std::string>(1) << '\n'
        << "Description:  " << it->get<std::string>(2) << '\n'
        << "ImageSize:    " << image.size() << std::endl;

      std::ofstream file{ "bin/" + std::to_string(it->get<int32_t>(0)) + "_image.jpg" };
      file.write(image.data(), image.size());

      std::cout << "\n";
    }
  }
  catch (soci::odbc_soci_error const& ex)
  {
    std::cerr
      << "ODBC Error Code:   " << ex.odbc_error_code() << std::endl
      << "Native Error Code: " << ex.native_error_code() << std::endl
      << "SOCI Message:      " << ex.what() << std::endl
      << "ODBC Message:      " << ex.odbc_error_message() << std::endl;
  }
  catch (std::exception const& ex)
  {
    std::cerr << "Some other error: " << ex.what() << std::endl;
  }
}
