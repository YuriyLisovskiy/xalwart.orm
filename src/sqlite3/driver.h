/**
 * sqlite3/driver.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Common methods for accessing the 'sqlite3' database.
 */

#pragma once

#ifdef USE_SQLITE3

// SQLite
#include <sqlite3.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../sql_driver.h"
#include "./schema_editor.h"


__ORM_SQLITE3_BEGIN__

// TESTME: Driver
class Driver : public DefaultSQLDriver
{
protected:
	::sqlite3* db = nullptr;

protected:
	// Helper method which throws 'QueryError' with message and
	// location for 'arg' argument name.
	inline void throw_empty_arg(
		const std::string& arg, int line, const char* function, const char* file
	) const
	{
		throw QueryError(this->name() + ": '" + arg + "' is required", line, function, file);
	}

public:
	explicit Driver(const char* filename);

	inline ~Driver() override
	{
		sqlite3_close(this->db);
		DefaultSQLDriver::~DefaultSQLDriver();
	}

	[[nodiscard]]
	inline std::string name() const override
	{
		return "sqlite3";
	}

	// Instantiates SQLite3 schema editor if it was not
	// done yet and returns it.
	[[nodiscard]]
	inline db::abc::ISchemaEditor* schema_editor() const override
	{
		if (!this->sql_schema_editor)
		{
			this->sql_schema_editor = std::make_shared<SchemaEditor>((ISQLDriver*) this);
		}

		return this->sql_schema_editor.get();
	}

	[[nodiscard]]
	std::vector<std::string> table_names() const override;

	// execute query
	void run_query(const std::string& query) const override;

	// insert row(s)
	void run_insert(const std::string& query) const override;

	// Sets `last_row_id` to an id of last inserted row.
	void run_insert(
		const std::string& query, std::string& last_row_id
	) const override;

	// select rows

	// In function 'handler_row(void*, void*)':
	// - `container` is initial container which is passed here as
	//   the second parameter;
	// - `row_map` is row of type std::map<std::string, char*>
	//   which contains pairs (column_name, column_value).
	void run_select(
		const std::string& query,
		void* container,
		void(*handle_row)(void* container, void* row_map)
	) const override;

	// update row(s)
	void run_update(const std::string& query, bool batch) const override;

	// delete row(s)
	void run_delete(const std::string& query) const override;

	// wrap 'func()' call in transaction
	bool run_transaction(const std::function<bool()>& func) const override;
};

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
