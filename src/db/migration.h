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

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../abc.h"
#include "./operations/create_table.h"
#include "./operations/drop_table.h"
#include "./operations/add_column.h"
#include "./state.h"
#include "./utility.h"


__ORM_DB_BEGIN__

// TESTME: Migration
class Migration
{
protected:
	std::string identifier;

	std::list<std::shared_ptr<abc::IOperation>> operations;

	// Mark transaction as initial.
	bool is_initial = false;

	// Database driver for running transactions.
	orm::abc::ISQLDriver* sql_driver;

	abc::ISchemaEditor* sql_schema_editor;

protected:

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

	template <column_migration_type_c T>
	inline void add_column(
		const std::string& table_name, const std::string& column_name, const constraints_t& c={}
	)
	{
		this->operations.push_back(
			std::make_shared<ops::AddColumn<T>>(table_name, column_name, c)
		);
	}

public:
	inline explicit Migration(
		orm::abc::ISQLDriver* driver, std::string identifier, bool initial=false
	) : sql_driver(driver), identifier(std::move(identifier)), is_initial(initial)
	{
		xw::util::require_non_null(
			this->sql_driver, ce<Migration>("", "driver is not initialized")
		);
		this->sql_schema_editor = this->sql_driver->schema_editor();
		xw::util::require_non_null(
			this->sql_schema_editor, ce<Migration>("", "schema editor is not initialized")
		);
	}

	inline void update_state(project_state& state) const
	{
		for (const auto& operation : this->operations)
		{
			operation->update_state(state);
		}
	}

	bool apply(
		project_state& state,
		const abc::ISchemaEditor* schema_editor,
		const std::function<void()>& success_callback=nullptr
	) const;

	bool rollback(
		project_state& state,
		const abc::ISchemaEditor* schema_editor,
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
};

__ORM_DB_END__
