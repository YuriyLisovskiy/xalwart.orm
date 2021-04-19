/**
 * db/recorder.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TODO: description
 */

#pragma once

// C++ libraries.
// TODO

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../abc.h"
#include "../model.h"
#include "../queries/insert.h"
#include "../queries/select.h"
#include "../queries/delete.h"
#include "./operations/table.h"


__DB_BEGIN__

// TESTME: MigrationModel
class MigrationModel : public orm::Model
{
public:
	size_t id{};
	std::string name;

	static constexpr const char* meta_table_name = "xalwart_migrations";

	MigrationModel() = default;

	inline MigrationModel(std::string name) : name(std::move(name))
	{
	}

	[[nodiscard]]
	std::string __str__() const override
	{
		if (this->is_null())
		{
			return "null";
		}

		return "Migration " + this->name;
	}

	inline static const std::tuple meta_columns = {
		make_pk_column_meta("id", &MigrationModel::id),
		make_column_meta("name", &MigrationModel::name)
	};

	inline void __set_attr__(const char* attr_name, const void* data) override
	{
		this->set_attribute_for<MigrationModel>(MigrationModel::meta_columns, attr_name, data);
	}

	[[nodiscard]]
	inline std::shared_ptr<const Object> __get_attr__(const char* attr_name) const override
	{
		return this->get_attribute_from<MigrationModel>(MigrationModel::meta_columns, attr_name);
	}
};

// TESTME: MigrationRecorder
class MigrationRecorder
{
protected:
	orm::abc::ISQLDriver* connection;

protected:
	void ensure_connection() const
	{
		if (!this->connection)
		{
			throw core::NullPointerException(
				"database connection is not initialized", _ERROR_DETAILS_
			);
		}
	}

public:
	inline explicit MigrationRecorder(orm::abc::ISQLDriver* connection) : connection(connection)
	{
		this->ensure_connection();
	}

	[[nodiscard]]
	inline q::select<MigrationModel> migrations() const
	{
		this->ensure_connection();
		return q::select<MigrationModel>().use(this->connection);
	}

	// Return `true` if the table of 'MigrationModel' exists,
	// `false` otherwise.
	[[nodiscard]]
	inline bool has_table() const
	{
		// TODO: has_table()
		return false;
	}

	// Ensure the table exists and has the correct schema.
	inline void ensure_schema() const
	{
		// If the table's there, then its schema was never changed
		// in the codebase.
		if (this->has_table())
		{
			return;
		}

		// Create the table.
		this->ensure_connection();
		auto editor = this->connection->schema_editor();
		CreateTableOperation table(MigrationModel::meta_table_name, editor);
		table.Int("id", {.primary_key=true, .autoincrement=true});
		table.String("name", {.max_len=255, .unique=true});
		table.up(editor);
	}

	// Return a `std::map` mapping `name` to `Migration` instances
	// for all applied migrations.
	[[nodiscard]]
	inline std::map<std::string, MigrationModel> applied_migrations() const
	{
		std::map<std::string, MigrationModel> mapping;

		// If table does not exists, then no migrations are applied.
		if (this->has_table())
		{
			auto migrations = this->migrations().all();
			for (auto& migration : migrations)
			{
				mapping[migration.name] = std::move(migration);
			}
		}

		return mapping;
	}

	// Note that a migration was applied.
	void record_applied(const std::string& name) const
	{
		this->ensure_schema();
		MigrationModel migration(name);
		q::insert(migration).use(this->connection).commit_one();
	}

	// Note that a migration was revoked.
	void record_revoked(const std::string& name) const
	{
		this->ensure_schema();
		auto migration = this->migrations().where(q::c(&MigrationModel::name) == name).first();
		if (!migration.is_null())
		{
			q::delete_(migration).use(this->connection).commit();
		}
	}

	// Delete all migration records. Can be useful for testing migrations.
	void flush() const
	{
		auto migrations = this->migrations().all();
		auto it = migrations.begin();
		auto end = migrations.end();
		if (it != end)
		{
			auto query = q::delete_(*it++);
			while (it != end)
			{
				query.model(*it++);
			}

			query.use(this->connection).commit();
		}
	}
};

__DB_END__
