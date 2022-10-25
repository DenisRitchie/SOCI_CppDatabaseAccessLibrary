module;

#include <soci/soci.h>
#include <soci/odbc/soci-odbc.h>
#include <soci/connection-parameters.h>

#include <type_traits>
#include <vector>
#include <utility>
#include <memory>
#include <tuple>
#include <functional>
#include <concepts>
#include <ctime>

export module OdbcDatabaseAccess.SqlServer:OdbcContext;

import OdbcDatabaseAccess.Models.SysTable;
import :StatementIterator;

export namespace OdbcDatabaseAccess
{
  class OdbcContext;
  class DatabaseFacade;
  class TableInfoList;
  class ColumnInfoList;

  class TableInfo : public SysTable
  {
  public:
    friend class TableInfoList;
    friend class ColumnInfoList;

  public:
    TableInfo(DatabaseFacade& database_facade);
    ColumnInfoList Columns();

  private:
    DatabaseFacade* m_database_facade;
  };

  class TableInfoList
  {
  public:
    TableInfoList(DatabaseFacade& database_facade);
    statement_iterator<TableInfo> begin();
    std::default_sentinel_t end();

  private:
    DatabaseFacade* m_database_facade;
  };


  class ColumnInfoList
  {
  public:
    ColumnInfoList(const TableInfo& table_info);
    statement_iterator<soci::column_info> begin();
    std::default_sentinel_t end();

  private:
    TableInfo m_table_info;
  };


  class DatabaseFacade
  {
  public:
    friend class TableInfo;
    friend class TableInfoList;
    friend class ColumnInfoList;
    friend class OdbcContext;

  public:
    DatabaseFacade(const DatabaseFacade&) = delete;
    DatabaseFacade(DatabaseFacade&&) = delete;
    DatabaseFacade& operator=(const DatabaseFacade&) = delete;
    DatabaseFacade& operator=(DatabaseFacade&&) = delete;
    ~DatabaseFacade() = default;

    [[nodiscard]] TableInfoList TableInformation();

    template<typename ...BindTypes>
    long long ExecuteSql(const std::string& query, BindTypes&&... exchange_value);

    std::weak_ptr<soci::transaction> BeginTransaction();
    void CommitTransaction();
    void RollbackTransaction();

  private:
    DatabaseFacade(OdbcContext& odbc_context);

  private:
    std::shared_ptr<soci::transaction> m_transaction;
    OdbcContext& m_odbc_context;
  };


  class OdbcContext
  {
  public:
    friend class TableInfo;
    friend class TableInfoList;
    friend class ColumnInfoList;
    friend class DatabaseFacade;

  public:
    OdbcContext(const OdbcContext&) = delete;
    OdbcContext(OdbcContext&&) = delete;
    OdbcContext& operator=(const OdbcContext&) = delete;
    OdbcContext& operator=(OdbcContext&&) = delete;
    virtual ~OdbcContext() = default;

    DatabaseFacade& Database() const noexcept;

  protected:
    OdbcContext(const std::string_view connection_string);

  protected:
    DatabaseFacade m_database_facade;
    soci::connection_parameters m_parameters;
    soci::session m_sql;
  };

  ///////////////////////////////////////// DatabaseFacade /////////////////////////////////////////

  DatabaseFacade::DatabaseFacade(OdbcContext& odbc_context) : m_odbc_context{ odbc_context }
  {
  }

  TableInfoList DatabaseFacade::TableInformation() { return *this; }

  template<typename ...BindTypes>
  long long DatabaseFacade::ExecuteSql(const std::string& query, BindTypes&&... exchange_value)
  {
    soci::statement statement(m_odbc_context.m_sql);

    (statement.exchange(std::forward<BindTypes>(exchange_value)), ...);

    statement.alloc();
    statement.prepare(query);
    statement.define_and_bind();
    statement.execute(true);

    return statement.get_affected_rows();
  }

  std::weak_ptr<soci::transaction> DatabaseFacade::BeginTransaction()
  {
    m_transaction = std::make_shared<soci::transaction>(m_odbc_context.m_sql);
    return m_transaction;
  }

  void DatabaseFacade::CommitTransaction()
  {
    m_transaction->commit();
    m_transaction.reset();
  }

  void DatabaseFacade::RollbackTransaction()
  {
    m_transaction->rollback();
    m_transaction.reset();
  }

  ///////////////////////////////////////// OdbcContext /////////////////////////////////////////

  OdbcContext::OdbcContext(const std::string_view connection_string)
    : m_parameters(soci::odbc, { connection_string.data(), connection_string.size() })
    , m_database_facade(*this)
  {
    m_parameters.set_option(soci::odbc_option_driver_complete, "0" /* SQL_DRIVER_NOPROMPT */);
    m_sql.open(m_parameters);
  }

  DatabaseFacade& OdbcContext::Database() const noexcept
  {
    return const_cast<DatabaseFacade&>(this->m_database_facade);
  }

  ///////////////////////////////////////// TableInfo /////////////////////////////////////////

  TableInfo::TableInfo(DatabaseFacade& database_facade) : m_database_facade{ &database_facade }
  {
  }

  ColumnInfoList TableInfo::Columns() { return *this; }

  ///////////////////////////////////////// TableInfos /////////////////////////////////////////

  TableInfoList::TableInfoList(DatabaseFacade& database_facade) : m_database_facade{ &database_facade }
  {
  }

  statement_iterator<TableInfo> TableInfoList::begin()
  {
    statement_iterator<TableInfo> table_iterator;
    table_iterator.SetValue(new TableInfo{ *this->m_database_facade });

    table_iterator.SetStatement((
      this->m_database_facade->m_odbc_context.m_sql.prepare_table_names(),
      soci::into(table_iterator.Value().TableName)));

    return table_iterator;
  }

  std::default_sentinel_t TableInfoList::end()
  {
    return std::default_sentinel;
  }

  ///////////////////////////////////////// ColumnInfos /////////////////////////////////////////

  ColumnInfoList::ColumnInfoList(const TableInfo& table_info) : m_table_info{ table_info }
  {
  }

  statement_iterator<soci::column_info> ColumnInfoList::begin()
  {
    statement_iterator<soci::column_info> column_iterator;
    column_iterator.SetValue(new soci::column_info);

    column_iterator.SetStatement((
      this->m_table_info.m_database_facade->m_odbc_context.m_sql.prepare_column_descriptions(this->m_table_info.TableName),
      soci::into(column_iterator.Value())));

    return column_iterator;
  }

  std::default_sentinel_t ColumnInfoList::end()
  {
    return std::default_sentinel;
  }
}
