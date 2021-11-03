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
#include "../interfaces.h"
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
	inline explicit MigrationRecorder(orm::ISQLBackend* backend)
	{
		this->sql_backend = require_non_null(backend, "SQL backend must not be nullptr", _ERROR_DETAILS_);
	}

	// Ensures the table exists and has the correct schema.
	void ensure_schema(const IDatabaseConnection* connection=nullptr) const;

	// Returns a `std::list` of `Migration` instances of all
	// applied migrations.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	[[nodiscard]]
	inline std::list<models::Migration> applied_migrations(
		const IDatabaseConnection* connection=nullptr
	) const
	{
		return this->migrations(connection).all();
	}

	// Records that a migration was applied.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	inline void record_applied(const std::string& name, const IDatabaseConnection* connection) const
	{
		this->ensure_schema(connection);
		q::Insert<models::Migration>(connection, this->backend()->sql_builder())
			.model(models::Migration(name))
			.commit_one();
	}

	// Deletes migration record by `name` from the database.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	inline void record_rolled_back(
		const std::string& name, const IDatabaseConnection* connection
	) const
	{
		this->ensure_schema(connection);
		this->migrations(connection).where(q::c(&models::Migration::name) == name).delete_();
	}

	// Deletes all migration records.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	inline void flush_records(const IDatabaseConnection* connection) const
	{
		this->migrations(connection).delete_();
	}

protected:
	orm::ISQLBackend* sql_backend;

	// Ensures if SQL backend is non-nullptr.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	[[nodiscard]]
	inline orm::ISQLBackend* backend() const
	{
		return require_non_null(
			this->sql_backend, "xw::orm::db::MigrationRecorder > backend: SQL backend must not be nullptr"
		);
	}

	// Returns select query for `MigrationModel` with SQL driver.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	[[nodiscard]]
	inline q::Select<models::Migration> migrations(const orm::IDatabaseConnection* connection) const
	{
		auto wrapper = this->backend()->wrap_connection();
		const IDatabaseConnection* db = connection ? connection : wrapper.connection();
		return q::Select<models::Migration>(db, this->backend()->sql_builder());
	}

	// Returns `true` if the table of 'MigrationModel' exists,
	// `false` otherwise.
	//
	// Throws `NullPointerException` if `sql_driver` is nullptr.
	[[nodiscard]]
	inline bool has_table(const IDatabaseConnection* connection=nullptr) const
	{
		auto tables = this->backend()->get_table_names(connection);
		return xw::util::contains(models::Migration::meta_table_name, tables.begin(), tables.end());
	}
};

__ORM_DB_END__
