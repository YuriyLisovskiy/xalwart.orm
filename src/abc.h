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

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./queries/conditions.h"
#include "./db/abc.h"


__ORM_ABC_BEGIN__

class ISQLQueryBuilder
{
public:

	// insert
	[[nodiscard]]
	virtual std::string sql_insert(
		const std::string& table_name,
		const std::string& columns,
		const std::list<std::string>& rows
	) const = 0;

	// select
	[[nodiscard]]
	virtual std::string sql_select_(
		const std::string& table_name,
		const std::string& columns,
		bool distinct,
		const std::list<q::join_t>& joins,
		const q::condition_t& where_cond,
		const std::list<q::ordering>& order_by_cols,
		long int limit,
		long int offset,
		const std::list<std::string>& group_by_cols,
		const q::condition_t& having_cond
	) const = 0;

	[[nodiscard]]
	virtual std::string sql_select(
		const std::string& table_name,
		const std::list<std::string>& columns,
		bool distinct,
		const std::list<q::join_t>& joins,
		const q::condition_t& where_cond,
		const std::list<q::ordering>& order_by_cols,
		long int limit,
		long int offset,
		const std::list<std::string>& group_by_cols,
		const q::condition_t& having_cond
	) const = 0;

	// update
	[[nodiscard]]
	virtual std::string sql_update(
		const std::string& table_name,
		const std::string& columns_data,
		const q::condition_t& condition
	) const = 0;

	// delete
	[[nodiscard]]
	virtual std::string sql_delete(
		const std::string& table_name, const q::condition_t& where_cond
	) const = 0;
};

class ISQLDriver
{
public:
	virtual ~ISQLDriver() = default;

	// Returns the name of SQL driver.
	[[nodiscard]]
	virtual std::string name() const = 0;

	// Returns SQL schema editor related to driver.
	[[nodiscard]]
	virtual db::abc::ISQLSchemaEditor* schema_editor() const = 0;

	// Returns SQL query builder related to driver.
	[[nodiscard]]
	virtual ISQLQueryBuilder* query_builder() const = 0;

	[[nodiscard]]
	virtual std::vector<std::string> table_names() const = 0;

	// Runs any SQL query.
	virtual void run_query(const std::string& query) const = 0;

	// insert row(s)

	virtual void run_insert(const std::string& query) const = 0;

	// Returns last inserted row id.
	virtual void run_insert(
		const std::string& query, std::string& last_row_id
	) const = 0;

	// select rows

	// In function 'handler_row(void*, void*)':
	// - `container` is initial container which is passed here as
	//   the second parameter;
	// - `row_map` is row of type std::map<std::string, char*>
	//   which contains pairs (column_name, column_value).
	virtual void run_select(
		const std::string& query,
		void* container,
		void(*handle_row)(void* container, void* row_map)
	) const = 0;

	// update row(s)
	virtual void run_update(const std::string& query, bool batch) const = 0;

	// delete row(s)
	virtual void run_delete(const std::string& query) const = 0;

	// transaction
	virtual bool run_transaction(const std::function<bool()>& func) const = 0;
};

__ORM_ABC_END__
