module;

#include <soci/soci.h>

#include <utility>
#include <memory>

export module OdbcDatabaseAccess.SqlServer:StatementIterator;

export namespace OdbcDatabaseAccess
{
  template <typename T>
  class statement_iterator
  {
  public:
    T& Value() noexcept
    {
      return *this->m_value.get();
    }

    void SetValue(T* value)
    {
      this->m_value.reset(value);
    }

    void SetStatement(const soci::statement& statement) noexcept
    {
      this->m_statement.reset(new soci::statement(statement));
      this->m_statement->execute();
      this->m_done = this->m_statement->fetch();
    }

    /*
    template <typename Self>
    auto&& Value(this Self&& self) noexcept
    {
      return *self.m_value;
    }

    template <typename Self>
    auto&& operator->(this Self&& self) noexcept
    {
      return self.m_value.operator->();
    }

    template <typename Self>
    auto&& operator*(this Self&& self) noexcept
    {
      return self.m_value.operator*();
    }
    */

    T* operator->() noexcept
    {
      return this->m_value.get();
    }

    const T* operator->() const noexcept
    {
      return this->m_value.get();
    }

    T& operator*() noexcept
    {
      return *this->m_value.get();
    }

    const T& operator*() const noexcept
    {
      return *this->m_value.get();
    }

    statement_iterator& operator++() noexcept
    {
      m_done = m_statement->fetch();
      return *this;
    }

  public:
    template <typename U>
    friend inline bool operator!=(const statement_iterator<U>& iterator, std::default_sentinel_t)
    {
      return iterator.m_done;
    }

    template <typename U>
    friend inline bool operator!=(std::default_sentinel_t, const statement_iterator<U>& iterator)
    {
      return iterator.m_done;
    }

  private:
    bool m_done{ false };
    std::shared_ptr<T> m_value{ nullptr };
    std::shared_ptr<soci::statement> m_statement{ nullptr };
  };
}