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
#include "../state.h"


__ORM_DB_OPERATIONS_BEGIN__

// TESTME: RenameColumn
// Renames a column in the table in database.
class RenameColumn : public ColumnOperation
{
protected:
	std::string new_name_;
	std::string new_name_lower_;

public:
	inline explicit RenameColumn(
		const std::string& table_name,
		const std::string& old_name, std::string new_name
	) : ColumnOperation(table_name, old_name),
		new_name_(std::move(new_name))
	{
		if (this->new_name_.empty())
		{
			throw ValueError(
				ce<RenameColumn>("", "'new_name' can not be empty"),
				_ERROR_DETAILS_
			);
		}

		this->new_name_lower_ = str::lower(this->new_name_);
	}

	[[nodiscard]]
	inline const std::string& new_name() const
	{
		return this->new_name_;
	}

	[[nodiscard]]
	inline std::string new_name_lower() const
	{
		return this->table_name_lower_;
	}

	inline void update_state(project_state& state) const override
	{
		auto& table = state.get_table_addr(this->table_name_lower());
		auto column = table.get_column(this->name_lower());
		if (table.columns.contains(this->new_name_lower()))
		{
			throw ValueError(ce<RenameColumn>(
				"update_state",
				"column with name '" + this->new_name() + "' already exists,"
				" consider altering it instead of creating"
			), _ERROR_DETAILS_);
		}

		table.columns.erase(column.name);
		column.name = this->new_name_lower();
		table.columns[column.name] = column;
	}

	inline void forward(
		const abc::ISchemaEditor* editor,
		const project_state& from_state, const project_state& to_state
	) const override
	{
		const auto& to_table = to_state.get_table_addr(this->table_name_lower());
		const auto& from_table = from_state.get_table_addr(this->table_name_lower());
		xw::util::require_non_null(
			editor, ce<RenameColumn>("forward", "schema editor is nullptr")
		)->alter_column(
			from_table,
			from_table.get_column_addr(this->name_lower()),
			to_table.get_column_addr(this->new_name_lower())
		);
	}

	inline void backward(
		const abc::ISchemaEditor* editor,
		const project_state& from_state, const project_state& to_state
	) const override
	{
		auto& to_table = to_state.get_table_addr(this->table_name_lower());
		auto& from_table = from_state.get_table_addr(this->table_name_lower());
		xw::util::require_non_null(
			editor, ce<RenameColumn>("backward", "schema editor is nullptr")
		)->alter_column(
			from_table,
			from_table.get_column_addr(this->new_name_lower()),
			to_table.get_column_addr(this->name_lower())
		);
	}
};

__ORM_DB_OPERATIONS_END__
