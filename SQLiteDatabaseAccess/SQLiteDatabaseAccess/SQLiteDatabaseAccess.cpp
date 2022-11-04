// SQLiteDatabaseAccess.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <iostream>
#include <charconv>
#include <filesystem>
#include <optional>
#include <chrono>
#include <memory>

#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>

using Boolean = int32_t;

struct Exam
{
  uint64_t               ExamId;
  std::string            Name;
  double_t               Price;
  std::tm                CreatedDate;
  std::optional<std::tm> ModifiedDate{ std::nullopt };
  std::optional<std::tm> DeletedDate{ std::nullopt };
  Boolean                IsEdit{ false };
  Boolean                IsDelete{ false };
};

int32_t main()
{
  if (!std::filesystem::exists("Database.db"))
  {
    std::cerr << "Error al abrir el archivo: Database.db\n";
    return EXIT_FAILURE;
  }

  soci::session sql(soci::sqlite3, "Database.db");

  Exam exam;
  std::unique_ptr<soci::statement> statement;

  try
  {
    statement.reset(new soci::statement{ (sql.prepare <<
    R"SQLite(
        Insert Into Exam (Name, Price, CreateDate, IsEdit, IsDelete)
        Values (:Name, :Price, :CreateDate, :IsEdit, :IsDelete)
      )SQLite",
      soci::use(exam.Name, "Name"),
      soci::use(exam.Price, "Price"),
      soci::use(exam.CreatedDate, "CreateDate"),
      soci::use(exam.IsEdit, "IsEdit"),
      soci::use(exam.IsDelete, "IsDelete"))});
  }
  catch (const std::exception& ex)
  {
    std::cerr << "Error: " << ex.what() << std::endl;
    return EXIT_FAILURE;
  }

  while (true)
  {
    std::cout << "Escriba el nombre del examen\n-> ";
    std::getline(std::cin, exam.Name);

    std::cout << "Escriba el precio del examen\n-> ";
    std::cin >> exam.Price;

    std::cin.ignore(std::numeric_limits<int32_t>::max(), '\n');
    std::cin.clear();

    std::time_t current_time = std::chrono::system_clock::to_time_t(
      std::chrono::system_clock::now());

    exam.CreatedDate = *std::localtime(&current_time);

    if (statement->execute(true))
    {
      std::clog << "Los datos se han registrado correctamente.\n";

      std::cout << "¿Desea continuar? (S para Sí, N para No)\n--> ";

      if (std::toupper(std::cin.get()) == 'S')
      {
        std::cin.ignore(std::numeric_limits<int32_t>::max(), '\n');
        std::cin.clear();

        continue;
      }

      break;
    }
    else
    {
      std::cerr << "Error al registrar el examen.\n";
      break;
    }
  }
}

