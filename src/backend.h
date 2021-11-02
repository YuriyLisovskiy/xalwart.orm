/**
 * sql_driver.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Default backend implements initialization of default SQL schema editor
 * and default SQL query builder.
 */

#pragma once

// C++ libraries.
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>
#include <functional>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./interfaces.h"


__ORM_BEGIN__

// TESTME: DefaultSQLBackend
// TODO: docs for 'DefaultSQLBackend'
class DefaultSQLBackend : public ISQLBackend
{
public:
	using ConnectionBuilder = std::function<std::shared_ptr<IDatabaseConnection>()>;

	explicit inline DefaultSQLBackend(size_t pool_size, ConnectionBuilder builder) :
		_pool_size(pool_size), _connection_builder(std::move(builder))
	{
		if (pool_size < 1)
		{
			throw ValueError("Pool size should be greater than zero", _ERROR_DETAILS_);
		}

		if (!this->_connection_builder)
		{
			throw NullPointerException("connection builder is nullptr", _ERROR_DETAILS_);
		}
	}

	void create_pool() final;

	inline ConnectionWrapper wrap_connection() final
	{
		return ConnectionWrapper(this);
	}

	// Provides a free connection from pool to access the database.
	// If there is not any connection available, waits for it.
	// This is a blocking operation.
	std::shared_ptr<IDatabaseConnection> get_connection() override;

	// Returns used connection to pool.
	// The code that requested a connection, ALWAYS should return
	// it back after using it, otherwise this connection will be
	// lost.
	void release_connection(const std::shared_ptr<IDatabaseConnection>& connection) override;

	// TESTME: schema_editor
	// Instantiates default schema editor if it was not
	// done yet and returns it.
	[[nodiscard]]
	db::ISchemaEditor* schema_editor() const override;

	// TESTME: sql_builder
	// Instantiates default SQL query builder if it was not
	// done yet and returns it.
	[[nodiscard]]
	ISQLQueryBuilder* sql_builder() const override;

protected:

	// SQL Schema editor related to SQL driver.
	mutable std::shared_ptr<db::ISchemaEditor> sql_schema_editor = nullptr;

	// SQL query builder related to SQL driver.
	mutable std::shared_ptr<ISQLQueryBuilder> sql_query_builder = nullptr;

private:
	std::mutex _mutex;
	std::condition_variable _condition;
	std::queue<std::shared_ptr<IDatabaseConnection>> _connection_pool;
	const size_t _pool_size;
	ConnectionBuilder _connection_builder;
};

__ORM_END__
