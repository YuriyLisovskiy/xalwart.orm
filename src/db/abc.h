/**
 * db/abc.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Abstract base classes for `db` module.
 */

#pragma once

// Core libraries.
#include <xalwart.core/datetime.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./state.h"


__ORM_DB_ABC_BEGIN__

class ISchemaEditor
{
protected:
	[[nodiscard]]
	virtual std::string quote_name(const std::string& s) const = 0;

public:

	// Creates a new table.
	virtual void create_table(const table_state& table) const = 0;

	// Drops the table.
	virtual void drop_table(const std::string& name) const = 0;

	// Creates a new column.
	virtual void create_column(
		const table_state& table, const column_state& column
	) const = 0;

	// Drops the column.
	virtual void drop_column(
		const table_state& table, const column_state& column
	) const = 0;

	// If column names are the same, just renames this column,
	// alters column's constraints otherwise.
	virtual void alter_column(
		const table_state& table,
		const column_state& from_column, const column_state& to_column
	) const = 0;
};

class IOperation
{
public:
	virtual void update_state(project_state& st) const = 0;

	virtual void forward(
		const ISchemaEditor* editor,
		const project_state& from_state, const project_state& to_state
	) const = 0;

	virtual void backward(
		const ISchemaEditor* editor,
		const project_state& from_state, const project_state& to_state
	) const = 0;
};

__ORM_DB_ABC_END__
