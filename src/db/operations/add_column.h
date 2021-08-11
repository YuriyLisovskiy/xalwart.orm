/**
 * db/operations/add_column.h
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

// TESTME: AddColumn
// TODO: docs for 'AddColumn'
// Creates a column for table in the database.
template <column_migration_type T>
class AddColumn : public ColumnOperation
{
protected:
	ColumnState column;

public:
	inline AddColumn(
		const std::string& table_name, const std::string& column_name, const Constraints& c={}
	) : ColumnOperation(table_name, column_name)
	{
		if (!c.null && !c.default_.has_value())
		{
			throw orm::MigrationsError(
				"The column '" + column_name + "' ('" + table_name + "' table)"
				" can not be create because it's mandatory (not null) but unknown"
				" how to fill it for the existing rows. Either add the default value"
				" or make the column not mandatory (null).",
				_ERROR_DETAILS_
			);
		}

		this->column = ColumnState::create<T>(column_name, c);
	}

	inline void update_state(ProjectState& state) const override
	{
		auto& table = state.get_table_addr(this->table_name());
		if (table.columns.find(this->name()) != table.columns.end())
		{
			throw ValueError(ce<AddColumn<T>>(
				"update_state",
				"column with name '" + this->name() + "' already exists,"
				" consider altering it instead of creating"
			), _ERROR_DETAILS_);
		}

		table.columns[this->name()] = this->column;
	}

	inline void forward(
		const abc::ISchemaEditor* editor, const ProjectState& from_state, const ProjectState& to_state
	) const override
	{
		auto& to_table = to_state.get_table_addr(this->table_name());
		auto& col = to_table.get_column_addr(this->name());
		auto& from_table = from_state.get_table_addr(this->table_name());
		xw::util::require_non_null(
			editor, ce<AddColumn<T>>("forward", "schema editor is nullptr")
		)->create_column(from_table, col);
	}

	inline void backward(
		const abc::ISchemaEditor* editor, const ProjectState& from_state, const ProjectState& to_state
	) const override
	{
		auto& table = from_state.get_table_addr(this->table_name());
		xw::util::require_non_null(
			editor, ce<AddColumn<T>>("backward", "schema editor is nullptr")
		)->drop_column(table, table.get_column_addr(this->name()));
	}
};

__ORM_DB_OPERATIONS_END__
