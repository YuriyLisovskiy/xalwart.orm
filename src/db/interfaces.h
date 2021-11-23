/**
 * db/interfaces.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Interfaces for 'db' module.
 */

#pragma once

// Base libraries.
#include <xalwart.base/datetime.h>
#include <xalwart.base/interfaces/orm.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./states.h"


__ORM_DB_BEGIN__

// TODO: docs for 'ISchemaEditor'
class ISchemaEditor
{
protected:
	[[nodiscard]]
	virtual std::string quote_name(const std::string& s) const = 0;

public:

	// Creates a new table.
	virtual void create_table(const TableState& table, const IDatabaseConnection* connection) const = 0;

	// Drops the table.
	virtual void drop_table(const std::string& name, const IDatabaseConnection* connection) const = 0;

	// Renames the table.
	virtual void rename_table(
		const TableState& table, const std::string& old_name, const std::string& new_name,
		const IDatabaseConnection* connection
	) const = 0;

	// Creates a new column.
	virtual void create_column(
		const TableState& table, const ColumnState& column, const IDatabaseConnection* connection
	) const = 0;

	// Drops the column.
	virtual void drop_column(
		const TableState& table, const ColumnState& column, const IDatabaseConnection* connection
	) const = 0;

	// If column names are the same, just renames this column,
	// alters column's constraints otherwise.
	virtual void alter_column(
		const TableState& table,
		const ColumnState& from_column,
		const ColumnState& to_column,
		bool strict,
		const IDatabaseConnection* connection
	) const = 0;
};

// TODO: docs for 'IOperation'
class IOperation
{
public:
	virtual void update_state(ProjectState& st) const = 0;

	virtual void forward(
		const ISchemaEditor* editor,
		const ProjectState& from_state, const ProjectState& to_state,
		const IDatabaseConnection* connection
	) const = 0;

	virtual void backward(
		const ISchemaEditor* editor,
		const ProjectState& from_state, const ProjectState& to_state,
		const IDatabaseConnection* connection
	) const = 0;
};

__ORM_DB_END__
