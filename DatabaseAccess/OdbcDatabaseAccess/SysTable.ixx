module;

#include <string>
#include <ctime>

export module OdbcDatabaseAccess.Models.SysTable;

export struct SysTable
{
  uint64_t ObjectId;
  std::string TableName;
  std::string SchemaName;
  std::string SchemaId;
  std::tm CreateDate;
  std::tm ModifyDate;
};
