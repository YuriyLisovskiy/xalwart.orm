/**
 * db/migration.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TODO: description
 */

#pragma once

// C++ libraries.
#include <list>
#include <memory>

// Base libraries.
#include <xalwart.base/interfaces/orm.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../interfaces.h"
#include "./operations/create_table.h"
#include "./operations/drop_table.h"
#include "./operations/rename_table.h"
#include "./operations/add_column.h"
#include "./operations/drop_column.h"
#include "./operations/alter_column.h"
#include "./operations/rename_column.h"
#include "./states.h"
#include "./utility.h"


__ORM_DB_BEGIN__

// TESTME: Migration
// TODO: docs for 'Migration'
class Migration
{
public:
	inline explicit Migration(orm::IBackend* backend, std::string identifier, bool initial=false) :
		identifier(std::move(identifier)), is_initial(initial)
	{
		require_non_null(backend, ce<Migration>("", "driver is not initialized"));
		this->sql_backend = dynamic_cast<orm::ISQLBackend*>(backend);
		if (!this->sql_backend)
		{
			throw ValueError(
				"'xw::orm::db::Migration' requires 'xw::orm::ISQLBackend'-based backend", _ERROR_DETAILS_
			);
		}

		this->sql_schema_editor = this->sql_backend->schema_editor();
		require_non_null(this->sql_schema_editor, ce<Migration>("", "schema editor is not initialized"));
	}

	inline void update_state(ProjectState& state) const
	{
		for (const auto& operation : this->operations)
		{
			operation->update_state(state);
		}
	}

	bool apply(
		ProjectState& state,
		const ISchemaEditor* schema_editor,
		const std::function<void()>& success_callback=nullptr
	) const;

	bool rollback(
		ProjectState& state,
		const ISchemaEditor* schema_editor,
		const std::function<void()>& success_callback=nullptr
	) const;

	[[nodiscard]]
	inline std::string name() const
	{
		return this->identifier;
	}

	[[nodiscard]]
	inline bool initial() const
	{
		return this->is_initial;
	}

	protected:
	std::string identifier;

	std::list<std::shared_ptr<IOperation>> operations;

	// Mark transaction as initial.
	bool is_initial = false;

	// Database driver for running transactions.
	orm::ISQLBackend* sql_backend;

	ISchemaEditor* sql_schema_editor;

	// Operations.
	void create_table(
		const std::string& name,
		const std::function<void(ops::CreateTable&)>& build_columns,
		const std::function<void(ops::CreateTable&)>& build_constraints=nullptr
	);

	inline void drop_table(const std::string& name)
	{
		this->operations.push_back(std::make_shared<ops::DropTable>(name));
	}

	inline void rename_table(const std::string& old_name, const std::string& new_name)
	{
		this->operations.push_back(std::make_shared<ops::RenameTable>(old_name, new_name));
	}

	template <column_migration_type T>
	inline void add_column(const std::string& table_name, const std::string& column_name, const Constraints& c={})
	{
		this->operations.push_back(std::make_shared<ops::AddColumn<T>>(table_name, column_name, c));
	}

	inline void drop_column(const std::string& table_name, const std::string& column_name)
	{
		this->operations.push_back(std::make_shared<ops::DropColumn>(table_name, column_name));
	}

	template <column_migration_type T>
	inline void alter_column(const std::string& table_name, const std::string& column_name, const Constraints& c={})
	{
		this->operations.push_back(std::make_shared<ops::AlterColumn<T>>(table_name, column_name, c));
	}

	inline void rename_column(const std::string& table_name, const std::string& old_name, const std::string& new_name)
	{
		this->operations.push_back(std::make_shared<ops::RenameColumn>(table_name, old_name, new_name));
	}

protected:
	void apply_unsafe(
		orm::IDatabaseConnection* connection,
		ProjectState& state,
		const ISchemaEditor* schema_editor,
		const std::function<void()>& success_callback=nullptr
	) const;

	void rollback_unsafe(
		orm::IDatabaseConnection* connection,
		ProjectState& state,
		const ISchemaEditor* schema_editor,
		const std::function<void()>& success_callback=nullptr
	) const;

	inline void rollback_and_release_connection(
		const std::shared_ptr<orm::IDatabaseConnection>& connection
	) const
	{
		connection->rollback_transaction();
		this->sql_backend->release_connection(connection);
	}
};

__ORM_DB_END__
