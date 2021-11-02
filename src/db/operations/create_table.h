/**
 * db/operations/create_table.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TODO: description
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Base libraries.
#include <xalwart.base/exceptions.h>

// Orm libraries.
#include "./base.h"
#include "../states.h"


__ORM_DB_OPERATIONS_BEGIN__

// TESTME: CreateTable
// TODO: docs for 'CreateTable'
// Creates a table in the database.
class CreateTable : public TableOperation
{
protected:
	std::unordered_map<std::string, ColumnState> columns{};
	std::unordered_map<std::string, ForeignKeyConstraints> foreign_keys{};

public:
	inline explicit CreateTable(const std::string& name) : TableOperation(name)
	{
	}

	inline void update_state(ProjectState& state) const override
	{
		state.tables[this->name()] = TableState{
			.name = this->name(), .columns = this->columns, .foreign_keys = this->foreign_keys
		};
	}

	inline void forward(
		const ISchemaEditor* editor, const ProjectState& from_state, const ProjectState& to_state
	) const override
	{
		auto table = to_state.get_table(this->name());
		require_non_null(
			editor, ce<CreateTable>("forward", "schema editor is nullptr")
		)->create_table(table);
	}

	inline void backward(
		const ISchemaEditor* editor, const ProjectState& from_state, const ProjectState& to_state
	) const override
	{
		auto table = from_state.get_table(this->name());
		require_non_null(
			editor, ce<CreateTable>("backward", "schema editor is nullptr")
		)->drop_table(table.name);
	}

	template <column_migration_type T>
	inline void column(const std::string& name, const Constraints& c={})
	{
		this->columns[name] = ColumnState::create<T>(name, c);
	}

	inline void foreign_key(const std::string& name, const ForeignKeyConstraints& c)
	{
		this->foreign_keys[name] = c;
	}
};

__ORM_DB_OPERATIONS_END__
