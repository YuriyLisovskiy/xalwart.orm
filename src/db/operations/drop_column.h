/**
 * db/operations/drop_column.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TODO: description
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./base.h"
#include "../states.h"


__ORM_DB_OPERATIONS_BEGIN__

// TESTME: DropColumn
// TODO: docs for 'DropColumn'
// Drops a column from table in database.
class DropColumn : public ColumnOperation
{
public:
	inline explicit DropColumn(const std::string& table_name, const std::string& column_name) :
		ColumnOperation(table_name, column_name)
	{
	}

	inline void update_state(ProjectState& state) const override
	{
		auto& table = state.get_table_addr(this->table_name());
		auto& column = table.get_column_addr(this->name());
		table.columns.erase(column.name);
	}

	inline void forward(
		const ISchemaEditor* editor, const ProjectState& from_state, const ProjectState& to_state
	) const override
	{
		const auto& table = from_state.get_table_addr(this->table_name());
		require_non_null(
			editor, ce<DropColumn>("forward", "schema editor is nullptr")
		)->drop_column(table, table.get_column_addr(this->name()));
	}

	inline void backward(
		const ISchemaEditor* editor, const ProjectState& from_state, const ProjectState& to_state
	) const override
	{
		auto& table = to_state.get_table_addr(this->table_name());
		require_non_null(
			editor, ce<DropColumn>("backward", "schema editor is nullptr")
		)->create_column(table, table.get_column_addr(this->name()));
	}
};

__ORM_DB_OPERATIONS_END__
