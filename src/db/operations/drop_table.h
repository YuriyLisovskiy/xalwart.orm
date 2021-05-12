/**
 * db/operations/drop_table.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// Core libraries.
#include <xalwart.core/utility.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./base.h"
#include "../state.h"


__ORM_DB_OPERATIONS_BEGIN__

// TESTME: DropTable
// Drops a table from the database.
class DropTable : public TableOperation
{
public:
	inline explicit DropTable(const std::string& name) : TableOperation(name)
	{
	}

	inline void update_state(project_state& state) const override
	{
		state.tables.erase(this->name_lower());
	}

	inline void forward(
		const abc::ISchemaEditor* editor,
		const project_state& from_state, const project_state& to_state
	) const override
	{
		auto table = from_state.get_table(this->name_lower());
		xw::util::require_non_null(
			editor, ce<DropTable>("forward", "schema editor is nullptr")
		)->drop_table(table.name);
	}

	inline void backward(
		const abc::ISchemaEditor* editor,
		const project_state& from_state, const project_state& to_state
	) const override
	{
		auto table = to_state.tables.at(this->name_lower());
		xw::util::require_non_null(
			editor, ce<DropTable>("backward", "schema editor is nullptr")
		)->create_table(table);
	}
};

__ORM_DB_OPERATIONS_END__
