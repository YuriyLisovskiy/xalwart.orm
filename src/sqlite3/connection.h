/**
 * sqlite3/connection.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

#ifdef USE_SQLITE3

// C++ libraries.
#include <string>
#include <functional>

// SQLite
#include <sqlite3.h>

// Base libraries.
#include <xalwart.base/abc/orm.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../exceptions.h"


__ORM_SQLITE3_BEGIN__

class SQLite3Connection : public abc::IDatabaseConnection
{
public:
	explicit SQLite3Connection(const char* filename);

	~SQLite3Connection() override;

	[[nodiscard]]
	inline std::string dbms_name() const final
	{
		return "sqlite";
	}

	// Acts like 'run_query_unsafe' but handles errors thrown during
	// query execution to roll back the transaction if it was started.
	//
	// 'sql_query' should be a valid SQL statement.
	// 'row_handler' can be nullptr.
	void run_query(
		const std::string& sql_query,
		const std::function<void(const std::map<std::string, char*>& /* columns */)>& row_handler,
		const std::function<void(const std::vector<char*>& /* columns */)>& vector_handler
	) const override;

	void run_query(const std::string& sql_query, std::string& last_row_id) const override;

	// Throws 'QueryError' if the transaction was already started.
	inline void begin_transaction() const final
	{
		if (this->in_transaction)
		{
			throw QueryError("Transaction is already started", _ERROR_DETAILS_);
		}

		this->in_transaction = true;
		this->run_query_unsafe("BEGIN TRANSACTION;", nullptr, nullptr);
	}

	// If 'end_transaction' ALWAYS should be used after all SQL
	// statements were run if 'begin_transaction' was called.
	//
	// Throws 'QueryError' if the transaction was not started.
	inline void end_transaction() const final
	{
		if (!this->in_transaction)
		{
			throw QueryError("Transaction is not started", _ERROR_DETAILS_);
		}

		this->in_transaction = false;
		this->run_query_unsafe("COMMIT TRANSACTION;", nullptr, nullptr);
	}

	// Throws 'QueryError' if the transaction was not started.
	inline void rollback_transaction() const final
	{
		if (!this->in_transaction)
		{
			throw QueryError("Transaction is not started", _ERROR_DETAILS_);
		}

		this->in_transaction = false;
		this->run_query_unsafe("ROLLBACK TRANSACTION;", nullptr, nullptr);
	}

protected:
	mutable bool in_transaction;

	::sqlite3* db = nullptr;

	// Helper method which throws 'QueryError' with message and
	// location for 'arg' argument name.
	inline void throw_empty_arg(const std::string& arg, int line, const char* function, const char* file) const
	{
		throw QueryError(this->dbms_name() + ": '" + arg + "' is required", line, function, file);
	}

	// Executes SQL query which returns rows as a result.
	// 'row_handler' can be used for building an instance
	// of the object from columns.
	//
	// 'sql_query' should be a valid SQL statement.
	// 'row_handler' can be nullptr.
	virtual void run_query_unsafe(
		const std::string& sql_query,
		std::function<void(const std::map<std::string, char*>& /* columns_as_map */)> map_handler,
		std::function<void(const std::vector<char*>& /* columns_as_vector */)> vector_handler
	) const;
};

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
