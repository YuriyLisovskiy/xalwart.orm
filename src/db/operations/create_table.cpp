/**
 * db/operations/create_table.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./create_table.h"


__ORM_DB_OPERATIONS_BEGIN__

void CreateTable::forward(
	const abc::ISQLSchemaEditor* editor,
	const project_state& from_state, const project_state& to_state
) const
{
	auto table = to_state.tables.at(this->name_lower());
	std::list<std::string> columns_list;
	for (const auto& col : table.columns)
	{
		auto c = std::get<2>(col.second);
		columns_list.push_back(
			xw::util::require_non_null(editor)->sql_column(
				std::get<0>(col.second), col.first,
				c.max_len, c.null, c.primary_key, c.unique,
				c.autoincrement, c.check, std::get<1>(col.second)
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
		editor, "CreateTable > forward: schema editor is nullptr"
	)->create_table(this->table_name, columns_list, constraints_list);
}

__ORM_DB_OPERATIONS_END__
