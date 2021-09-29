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
public:

	// Throws `NullPointerException` if `sql_backend` is nullptr.
	inline explicit MigrationRecorder(orm::abc::ISQLBackend* backend)
	{
		this->sql_backend = require_non_null(backend, "SQL backend must not be nullptr", _ERROR_DETAILS_);
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
		auto wrapper = this->backend()->wrap_connection();
		return this->migrations(wrapper.connection()).all();
	}

	// Records that a migration was applied.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	inline void record_applied(const std::string& name) const
	{
		this->ensure_schema();
		auto* backend = this->backend();
		auto wrapper = backend->wrap_connection();
		q::Insert<models::Migration>(wrapper.connection(), backend->sql_builder())
			.model(models::Migration(name))
			.commit_one();
	}

	// Deletes migration record by `name` from the database.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	inline void record_rolled_back(const std::string& name) const
	{
		this->ensure_schema();
		auto wrapper = this->sql_backend->wrap_connection();
		this->migrations(wrapper.connection()).where(q::c(&models::Migration::name) == name).delete_();
	}

	// Deletes all migration records.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	inline void flush_records() const
	{
		auto wrapper = this->sql_backend->wrap_connection();
		this->migrations(wrapper.connection()).delete_();
	}

protected:
	orm::abc::ISQLBackend* sql_backend;

	// Ensures if SQL backend is non-nullptr.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	[[nodiscard]]
	inline orm::abc::ISQLBackend* backend() const
	{
		return require_non_null(
			this->sql_backend, "xw::orm::db::MigrationRecorder > backend: SQL backend must not be nullptr"
		);
	}

	// Returns select query for `MigrationModel` with SQL driver.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	[[nodiscard]]
	inline q::Select<models::Migration> migrations(orm::abc::IDatabaseConnection* connection) const
	{
		return q::Select<models::Migration>(connection, this->backend()->sql_builder());
	}

	// Returns `true` if the table of 'MigrationModel' exists,
	// `false` otherwise.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	[[nodiscard]]
	inline bool has_table() const
	{
		auto tables = this->backend()->get_table_names();
		return xw::util::contains(models::Migration::meta_table_name, tables.begin(), tables.end());
	}
};

__ORM_DB_END__
