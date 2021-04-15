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
#include "../driver.h"
#include "./schema_editor.h"


__SQLITE3_BEGIN__

// TESTME: Driver
class Driver : public SQLDriverBase
{
protected:
	::sqlite3* db = nullptr;

public:
	explicit Driver(const char* filename);

	inline ~Driver() override
	{
		sqlite3_close(this->db);
		SQLDriverBase::~SQLDriverBase();
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
		if (!this->schema_editor_)
		{
			this->schema_editor_ = std::make_shared<SchemaEditor>((ISQLDriver*) this);
		}

		return this->schema_editor_.get();
	}

	// execute query
	void execute_query(const std::string& query) const override;

	// insert row(s)
	[[nodiscard]]
	std::string run_insert(const std::string& query) const override;

	// select rows
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

__SQLITE3_END__

#endif // USE_SQLITE3
