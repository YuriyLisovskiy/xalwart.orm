/**
 * db/operations/drop_table.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./base.h"
#include "../states.h"


__ORM_DB_OPERATIONS_BEGIN__

// TESTME: DropTable
// TODO: docs for 'DropTable'
// Drops a table from the database.
class DropTable : public TableOperation
{
public:
	inline explicit DropTable(const std::string& name) : TableOperation(name)
	{
	}

	inline void update_state(ProjectState& state) const override
	{
		state.tables.erase(this->name());
	}

	inline void forward(
		const ISchemaEditor* editor,
		const ProjectState& from_state, const ProjectState& to_state,
		const IDatabaseConnection* connection
	) const override
	{
		auto table = from_state.get_table(this->name());
		require_non_null(
			editor, ce<DropTable>("forward", "schema editor is nullptr")
		)->drop_table(table.name, connection);
	}

	inline void backward(
		const ISchemaEditor* editor,
		const ProjectState& from_state, const ProjectState& to_state,
		const IDatabaseConnection* connection
	) const override
	{
		auto table = to_state.tables.at(this->name());
		require_non_null(
			editor, ce<DropTable>("backward", "schema editor is nullptr")
		)->create_table(table, connection);
	}
};

__ORM_DB_OPERATIONS_END__
