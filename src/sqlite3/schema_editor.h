/**
 * sqlite3/schema_editor.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TODO: description
 */

#pragma once

#ifdef USE_SQLITE3

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../db/schema_editor.h"


__ORM_SQLITE3_BEGIN__

// TESTME: SchemaEditor
class SchemaEditor : public db::DefaultSQLSchemaEditor
{
protected:
	[[nodiscard]]
	std::string sql_column_constraints(
		const db::constraints_t& constraints, const std::string& default_value
	) const override;

	[[nodiscard]]
	std::string sql_type_to_string(db::sql_column_type type) const override;

	inline void sql_column_autoincrement_check(
		db::sql_column_type type, bool autoincrement, bool primary_key
	) const override
	{
		if (autoincrement && (type != db::INT_T || !primary_key))
		{
			throw ValueError(
				"'autoincrement' is only allowed on an integer primary key",
				_ERROR_DETAILS_
			);
		}
	}

public:
	inline explicit SchemaEditor(orm::abc::ISQLDriver* db) : db::DefaultSQLSchemaEditor(db)
	{
	}

	[[nodiscard]]
	std::string sql_column(const db::column_state& column) const override;

	inline void drop_column(
		const db::table_state& table, const db::column_state& column
	) const override
	{
		auto columns = table.columns;
		columns.erase(column.name);
		auto new_table_name = this->quote_name("new_" + table.name + "_table_to_alter");
		auto columns_list = str::join(
			", ", columns.begin(), columns.end(),
			[](const auto& col) -> auto { return col.first; }
		);
		auto column_definition = str::join(
			", ", columns.begin(), columns.end(),
			[this](const auto& pair) -> auto {
				auto col = pair.second;
				auto c = col.constraints;
				return this->sql_column(col);
			}
		);
		auto query = "PRAGMA foreign_keys=off;"
		" CREATE TABLE " + new_table_name + " (" + column_definition + ");"
		" INSERT INTO " + new_table_name + " (" + columns_list + ")"
		" SELECT " + columns_list +
		" FROM " + table.name + ";"
		" DROP TABLE " + table.name + ";"
		" ALTER TABLE " + new_table_name + " RENAME TO " + table.name + ";"
		" PRAGMA foreign_keys=on;";
		this->db->run_query(query);
	}
};

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
