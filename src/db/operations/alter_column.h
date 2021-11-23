/**
 * db/operations/alter_column.h
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
#include "../../exceptions.h"


__ORM_DB_OPERATIONS_BEGIN__

// TESTME: AlterColumn
// TODO: docs for 'AlterColumn'
// Alters a column of the table in database.
template <column_migration_type T>
class AlterColumn : public ColumnOperation
{
protected:
	ColumnState column_data;

public:
	inline AlterColumn(
		const std::string& table_name, const std::string& column_name, const Constraints& c={}
	) : ColumnOperation(table_name, column_name)
	{
		if (!c.null && !c.default_.has_value())
		{
			throw orm::MigrationsError(
				"The column '" + column_name + "' (from '" + table_name + "' table)"
				" can not be alter because it's mandatory (not null) but unknown"
				" how to fill it for the existing rows. Either add the default value"
				" or make the column not mandatory (null).",
				_ERROR_DETAILS_
			);
		}

		this->column_data = ColumnState::create<T>(column_name, c);
	}

	inline void update_state(ProjectState& state) const override
	{
		auto& table = state.get_table_addr(this->table_name());
		const auto& column = table.get_column_addr(this->name());
		table.columns[column.name] = this->column_data;
	}

	inline void forward(
		const ISchemaEditor* editor,
		const ProjectState& from_state, const ProjectState& to_state,
		const IDatabaseConnection* connection
	) const override
	{
		auto& to_table = to_state.get_table_addr(this->table_name());
		auto& from_table = from_state.get_table_addr(this->table_name());
		require_non_null(
			editor, ce<AlterColumn<T>>("forward/backward", "schema editor is nullptr")
		)->alter_column(
			from_table,
			from_table.get_column_addr(this->name()), to_table.get_column_addr(this->name()),
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
		this->forward(editor, from_state, to_state, connection);
	}
};

__ORM_DB_OPERATIONS_END__
