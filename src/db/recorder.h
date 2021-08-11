/**
 * db/recorder.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Stores migration records in the database.
 * If a migration is revoked its row is removed from the table.
 * Having a row in the table means a migration is applied.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./models/migration.h"
#include "../abc.h"
#include "../queries/insert.h"
#include "../queries/select.h"
#include "../queries/delete.h"
#include "./operations/create_table.h"


__ORM_DB_BEGIN__

// TESTME: MigrationRecorder
// TODO: docs for 'MigrationRecorder'
class MigrationRecorder
{
protected:
	orm::abc::ISQLDriver* sql_driver;

protected:

	// Ensures if SQL driver is non-nullptr.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	[[nodiscard]]
	inline orm::abc::ISQLDriver* driver() const
	{
		return xw::util::require_non_null(
			this->sql_driver, "xw::orm::db::MigrationRecorder > driver: SQL driver must not be nullptr"
		);
	}

	// Returns select query for `MigrationModel` with SQL driver.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	[[nodiscard]]
	inline q::select<models::Migration> migrations() const
	{
		return q::select<models::Migration>().use(this->driver());
	}

	// Returns `true` if the table of 'MigrationModel' exists,
	// `false` otherwise.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	[[nodiscard]]
	inline bool has_table() const
	{
		auto tables = this->driver()->table_names();
		return xw::util::contains(models::Migration::meta_table_name, tables.begin(), tables.end());
	}

public:

	// Throws `NullPointerException` if `sql_driver` is nullptr.
	inline explicit MigrationRecorder(orm::abc::ISQLDriver* driver)
	{
		this->sql_driver = xw::util::require_non_null(
			driver, "xw::orm::db::MigrationRecorder: SQL driver must not be nullptr"
		);
	}

	// Ensures the table exists and has the correct schema.
	void ensure_schema() const;

	// Returns a `std::list` of `Migration` instances of all
	// applied migrations.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	[[nodiscard]]
	inline std::list<models::Migration> applied_migrations() const
	{
		return this->migrations().all();
	}

	// Records that a migration was applied.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	inline void record_applied(const std::string& name) const
	{
		this->ensure_schema();
		q::insert(models::Migration(name)).use(this->driver()).commit_one();
	}

	// Deletes migration record by `name` from the database.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	inline void record_rolled_back(const std::string& name) const
	{
		this->ensure_schema();
		this->migrations().where(q::c(&models::Migration::name) == name).delete_();
	}

	// Deletes all migration records.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	inline void flush_records() const
	{
		this->migrations().delete_();
	}
};

__ORM_DB_END__
