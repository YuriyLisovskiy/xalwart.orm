/**
 * transaction.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <xalwart.base/interfaces/orm.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./interfaces.h"
#include "./queries/insert.h"
#include "./queries/select.h"
#include "./queries/update.h"
#include "./queries/delete.h"


__ORM_BEGIN__

// TESTME: Transaction
// TODO: docs for 'Transaction'
class Transaction
{
public:
	inline Transaction() noexcept : sql_builder(nullptr), connection(nullptr)
	{
	}

	explicit inline Transaction(IDatabaseConnection* connection, ISQLQueryBuilder* builder) :
		connection(connection), sql_builder(builder)
	{
		this->check_state();
		this->connection->begin_transaction();
	}

	inline Transaction(const Transaction& other) noexcept
	{
		if (this != &other)
		{
			this->_copy_from(other);
		}
	}

	inline Transaction(Transaction&& other) noexcept
	{
		if (this != &other)
		{
			this->_move_from(std::forward<Transaction>(other));
		}
	}

	inline ~Transaction()
	{
		if (this->connection)
		{
			this->connection->rollback_transaction();
		}
	}

	inline Transaction& operator= (Transaction&& other) noexcept
	{
		if (this != &other)
		{
			this->_move_from(std::forward<Transaction>(other));
		}

		return *this;
	}

	inline Transaction& operator= (const Transaction& other) noexcept
	{
		if (this != &other)
		{
			this->_copy_from(other);
		}

		return *this;
	}

	inline void commit() const
	{
		this->check_state();
		this->connection->end_transaction();
	}

	inline void rollback() const
	{
		this->check_state();
		this->connection->rollback_transaction();
	}

	template <class T>
	inline q::Insert<T> insert()
	{
		this->check_state();
		return q::Insert<T>(this->connection, this->sql_builder);
	}

	template <class T>
	inline q::Select<T> select()
	{
		this->check_state();
		return q::Select<T>(this->connection, this->sql_builder);
	}

	template <class T>
	inline q::Update<T> update()
	{
		this->check_state();
		return q::Update<T>(this->connection, this->sql_builder);
	}

	template <class T>
	inline q::Delete<T> delete_()
	{
		this->check_state();
		return q::Delete<T>(this->connection, this->sql_builder);
	}

protected:
	IDatabaseConnection* connection = nullptr;
	ISQLQueryBuilder* sql_builder = nullptr;

	inline void check_state() const
	{
		require_non_null(this->sql_builder, "SQL builder is nullptr", _ERROR_DETAILS_);
		require_non_null(this->connection, "Database connection is nullptr", _ERROR_DETAILS_);
	}

private:
	void _copy_from(const Transaction& other) noexcept
	{
		this->sql_builder = other.sql_builder;
		this->connection = other.connection;
	}

	void _move_from(Transaction&& other) noexcept
	{
		this->connection = other.connection;
		other.connection = nullptr;

		this->sql_builder = other.sql_builder;
		other.sql_builder = nullptr;
	}
};

__ORM_END__
