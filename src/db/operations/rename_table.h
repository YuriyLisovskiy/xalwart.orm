/**
 * db/operations/rename_table.h
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

// TESTME: RenameTable
// TODO: docs for 'RenameTable'
// Renames a table in the database.
class RenameTable : public TableOperation
{
protected:
	mutable std::string new_table_name;

public:
	inline explicit RenameTable(const std::string& old_name, std::string new_name) :
		TableOperation(old_name), new_table_name(std::move(new_name))
	{
	}

	[[nodiscard]]
	std::string new_name() const
	{
		return this->new_table_name;
	}

	[[nodiscard]]
	std::string old_name() const
	{
		return this->table_name;
	}

	inline void update_state(ProjectState& state) const override
	{
		auto table = state.get_table(this->old_name());
		state.tables.erase(table.name);
		table.name = this->new_name();
		state.tables[table.name] = table;
	}

	inline void forward(
		const ISchemaEditor* editor,
		const ProjectState& from_state, const ProjectState& to_state,
		const IDatabaseConnection* connection
	) const override
	{
		const auto& new_table = to_state.get_table_addr(this->new_name());
		const auto& old_table = from_state.get_table_addr(this->old_name());
		require_non_null(
			editor, ce<RenameTable>("forward/backward", "schema editor is nullptr")
		)->rename_table(new_table, old_table.name, new_table.name, connection);
	}

	inline void backward(
		const ISchemaEditor* editor,
		const ProjectState& from_state, const ProjectState& to_state,
		const IDatabaseConnection* connection
	) const override
	{
		std::swap(this->new_table_name, this->table_name);
		this->forward(editor, from_state, to_state, connection);
		std::swap(this->new_table_name, this->table_name);
	}
};

__ORM_DB_OPERATIONS_END__
