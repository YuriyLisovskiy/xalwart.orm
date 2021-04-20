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
#include "./operations/table.h"


__ORM_DB_BEGIN__

// TESTME: Migration
class Migration
{
protected:
	std::string identifier;

	std::list<std::shared_ptr<abc::IOperation>> operations;

	bool initial = false;

	// Whether to wrap the whole migration in a transaction.
	// Only has an effect on database drivers which support transactional DDL.
	bool atomic = true;

	// Database driver for running transactions.
	orm::abc::ISQLDriver* sql_driver;

	abc::ISQLSchemaEditor* sql_schema_editor;

public:
	inline explicit Migration(
		orm::abc::ISQLDriver* driver, std::string identifier, bool initial=false
	) : sql_driver(driver), identifier(std::move(identifier)), initial(initial)
	{
		if (!this->sql_driver)
		{
			throw NullPointerException(
				"xw::orm::db::Migration: SQL driver is not initialized",
				_ERROR_DETAILS_
			);
		}

		this->sql_schema_editor = this->sql_driver->schema_editor();
		if (!this->sql_schema_editor)
		{
			throw NullPointerException(
				"xw::orm::db::Migration: SQL schema editor is nullptr",
				_ERROR_DETAILS_
			);
		}
	}

	bool up(abc::ISQLSchemaEditor* schema_editor) const;

	bool down(abc::ISQLSchemaEditor* schema_editor) const;

	[[nodiscard]]
	inline bool is_initial() const
	{
		return this->initial;
	}

	[[nodiscard]]
	inline bool is_atomic() const
	{
		return this->atomic;
	}

	// Operations.
	void create_table(
		const std::string& name,
		const std::function<void(ops::CreateTableOperation&)>& build_columns,
		const std::function<void(ops::CreateTableOperation&)>& build_constraints=nullptr
	)
	{
		ops::CreateTableOperation table_op(name, this->sql_schema_editor);
		if (!build_columns)
		{
			throw NullPointerException(
				"xw::orm::db::Migration: columns builder must be initialized",
				_ERROR_DETAILS_
			);
		}

		build_columns(table_op);
		if (build_constraints)
		{
			build_constraints(table_op);
		}

		this->operations.push_back(std::make_shared<ops::CreateTableOperation>(table_op));
	}
};

__ORM_DB_END__
