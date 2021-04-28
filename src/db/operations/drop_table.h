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
		const abc::ISQLSchemaEditor* editor,
		const project_state& from_state, const project_state& to_state
	) const override
	{
		// TODO: remove all references to this table!
		xw::util::require_non_null(
			editor, "DropTable > forward: schema editor is nullptr"
		)->drop_table(this->table_name);
	}

	inline void backward(
		const abc::ISQLSchemaEditor* editor,
		const project_state& from_state, const project_state& to_state
	) const override
	{
		auto table = to_state.tables.at(this->name_lower());
		std::list<std::string> columns_list;
		for (const auto& col : table.columns)
		{
			auto col_c = std::get<2>(col.second);
			columns_list.push_back(
				xw::util::require_non_null(editor)->sql_column(
					std::get<0>(col.second), col.first,
					col_c.max_len, col_c.null, col_c.primary_key, col_c.unique,
					col_c.autoincrement, col_c.check, std::get<1>(col.second)
				)
			);
		}

		std::list<std::string> constraints_list;
		for (const auto& fk : table.foreign_keys)
		{
			auto fk_c = fk.second;
			constraints_list.push_back(editor->sql_foreign_key(
				fk.first, fk_c.to, fk_c.key, fk_c.on_delete, fk_c.on_update
			));
		}

		xw::util::require_non_null(
			editor, "DropTable > backward: schema editor is nullptr"
		)->create_table(this->table_name, columns_list, constraints_list);
	}
};

__ORM_DB_OPERATIONS_END__
