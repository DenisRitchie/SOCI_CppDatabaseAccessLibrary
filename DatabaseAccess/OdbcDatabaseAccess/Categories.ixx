module;

#include <cinttypes>
#include <string>

export module OdbcDatabaseAccess.Models:Category;

export struct Category
{
  int32_t CategoryId;
  std::string CategoryName;
  std::string Description;
  std::string Picture;
};
