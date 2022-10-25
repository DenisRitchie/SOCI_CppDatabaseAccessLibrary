module;

#include <soci/soci.h>

#include <string>
#include <string_view>

export module OdbcDatabaseAccess.SqlServer:SqlDataType;

namespace std
{
  export [[nodiscard]] inline constexpr string_view to_string(const soci::data_type data_type) noexcept
  {
    switch (data_type)
    {
      case soci::data_type::dt_string:             return "String";
      case soci::data_type::dt_date:               return "Date";
      case soci::data_type::dt_double:             return "Double";
      case soci::data_type::dt_integer:            return "Int32";
      case soci::data_type::dt_long_long:          return "Int64";
      case soci::data_type::dt_unsigned_long_long: return "UInt64";
      case soci::data_type::dt_blob:               return "Blod";
      case soci::data_type::dt_xml:                return "Xml";
      default:                                     return "Unknown";
    }
  }
}
