/**
 * abc.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Abstract base classes for ORM library.
 */

#pragma once

// C++ libraries.
#include <string>
#include <map>
#include <functional>
#include <list>

// Base libraries.
#include <xalwart.base/abc/orm.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./queries/conditions.h"
#include "./db/abc.h"


__ORM_ABC_BEGIN__

// TODO: docs for 'SQLQueryBuilder'
class SQLQueryBuilder
{
public:

	// insert
	[[nodiscard]]
	virtual std::string sql_insert(
		const std::string& table_name, const std::string& columns, const std::list<std::string>& rows
	) const = 0;

	// select
	[[nodiscard]]
	virtual std::string sql_select_(
		const std::string& table_name,
		const std::string& columns,
		bool distinct,
		const std::list<q::Join>& joins,
		const q::Condition& where_cond,
		const std::list<q::Ordering>& order_by_cols,
		long int limit,
		long int offset,
		const std::list<std::string>& group_by_cols,
		const q::Condition& having_cond
	) const = 0;

	[[nodiscard]]
	virtual std::string sql_select(
		const std::string& table_name,
		const std::list<std::string>& columns,
		bool distinct,
		const std::list<q::Join>& joins,
		const q::Condition& where_cond,
		const std::list<q::Ordering>& order_by_cols,
		long int limit,
		long int offset,
		const std::list<std::string>& group_by_cols,
		const q::Condition& having_cond
	) const = 0;

	// update
	[[nodiscard]]
	virtual std::string sql_update(
		const std::string& table_name, const std::string& columns_data, const q::Condition& condition
	) const = 0;

	// delete
	[[nodiscard]]
	virtual std::string sql_delete(const std::string& table_name, const q::Condition& where_cond) const = 0;
};

class ConnectionWrapper;

// TODO: docs for 'SQLBackend'
class SQLBackend : public xw::abc::orm::Backend
{
public:
	virtual ConnectionWrapper wrap_connection() = 0;

	// Returns SQL schema editor related to driver.
	[[nodiscard]]
	virtual db::abc::ISchemaEditor* schema_editor() const = 0;

	// Returns SQL query builder related to driver.
	[[nodiscard]]
	virtual SQLQueryBuilder* query_builder() const = 0;
};

// Requests connection from backend and returns it back when
// object is destroyed.
//
// Usage example:
// {
//   {
//     auto connection = backend->wrap_connection();
//     ...
//   } // inner scope: wrapper object will return the connection automatically
// } // global scope
class ConnectionWrapper final
{
public:
	explicit inline ConnectionWrapper(SQLBackend* backend) : _backend(backend)
	{
		require_non_null(this->_backend, "SQL backend is nullptr", _ERROR_DETAILS_);
	}

	inline ~ConnectionWrapper()
	{
		this->release();
	}

	inline xw::abc::orm::DatabaseConnection* connection()
	{
		if (!this->_connection)
		{
			this->_connection = this->_backend->get_connection();
		}

		return require_non_null(this->_connection.get(), _ERROR_DETAILS_);
	}

	inline void release()
	{
		if (this->_connection)
		{
			this->_backend->release_connection(this->_connection);
		}

		this->_connection = nullptr;
	}

private:
	SQLBackend* _backend;
	std::shared_ptr<xw::abc::orm::DatabaseConnection> _connection;
};

__ORM_ABC_END__
