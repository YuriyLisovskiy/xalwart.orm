/**
 * db/operations/rename_column.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TODO: description
 */

#pragma once

// Module definitions.
#include <utility>

#include "./_def_.h"

// Orm libraries.
#include "./base.h"
#include "../states.h"


__ORM_DB_OPERATIONS_BEGIN__

// TESTME: RenameColumn
// TODO: docs for 'RenameColumn'
// Renames a column in the table in database.
class RenameColumn : public ColumnOperation
{
protected:
	std::string new_name_;

public:
	inline explicit RenameColumn(
		const std::string& table_name, const std::string& old_name, std::string new_name
	) : ColumnOperation(table_name, old_name), new_name_(std::move(new_name))
	{
		if (this->new_name_.empty())
		{
			throw ValueError(ce<RenameColumn>("", "'new_name' can not be empty"), _ERROR_DETAILS_);
		}
	}

	[[nodiscard]]
	inline const std::string& new_name() const
	{
		return this->new_name_;
	}

	inline void update_state(ProjectState& state) const override
	{
		auto& table = state.get_table_addr(this->table_name());
		auto column = table.get_column(this->name());
		if (table.columns.contains(this->new_name()))
		{
			throw ValueError(ce<RenameColumn>(
				"update_state",
				"column with name '" + this->new_name() + "' already exists,"
				" consider altering it instead of creating"
			), _ERROR_DETAILS_);
		}

		table.columns.erase(column.name);
		column.name = this->new_name();
		table.columns[column.name] = column;
	}

	inline void forward(
		const ISchemaEditor* editor,
		const ProjectState& from_state, const ProjectState& to_state,
		const IDatabaseConnection* connection
	) const override
	{
		const auto& to_table = to_state.get_table_addr(this->table_name());
		const auto& from_table = from_state.get_table_addr(this->table_name());
		require_non_null(
			editor, ce<RenameColumn>("forward", "schema editor is nullptr")
		)->alter_column(
			from_table,
			from_table.get_column_addr(this->name()), to_table.get_column_addr(this->new_name()),
			false,
			connection
		);
	}

	inline void backward(
		const ISchemaEditor* editor,
		const ProjectState& from_state, const ProjectState& to_state,
		const IDatabaseConnection* connection
	) const override
	{
		auto& to_table = to_state.get_table_addr(this->table_name());
		auto& from_table = from_state.get_table_addr(this->table_name());
		require_non_null(
			editor, ce<RenameColumn>("backward", "schema editor is nullptr")
		)->alter_column(
			from_table,
			from_table.get_column_addr(this->new_name()), to_table.get_column_addr(this->name()),
			false,
			connection
		);
	}
};

__ORM_DB_OPERATIONS_END__
