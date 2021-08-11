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
#include "../db/utility.h"


__ORM_SQLITE3_BEGIN__

// TESTME: SchemaEditor
class SchemaEditor : public db::DefaultSQLSchemaEditor
{
protected:
	[[nodiscard]]
	inline std::string sql_drop_table(const std::string& name) const override
	{
		return "DROP TABLE " + name;
	}

	[[nodiscard]]
	std::string sql_column(const db::ColumnState& column) const override;

	[[nodiscard]]
	std::string sql_column_constraints(
		const db::Constraints& constraints, const std::string& default_value
	) const override;

	[[nodiscard]]
	std::string sql_type_string(db::SqlColumnType type) const override;

	inline void sql_column_autoincrement_check(
		db::SqlColumnType type, bool autoincrement, bool primary_key
	) const override
	{
		if (autoincrement && (type != db::SqlColumnType::Int || !primary_key))
		{
			throw ValueError(
				"'autoincrement' is only allowed on an integer primary key",
				_ERROR_DETAILS_
			);
		}
	}

	virtual inline void recreate_table(
		const db::TableState& table, const std::unordered_map<std::string, std::string>& mapping
	) const
	{
		auto columns_definition = str::join(
			", ", table.columns.begin(), table.columns.end(),
			[this](const auto& pair) -> auto { return this->sql_column(pair.second); }
		);
		auto fks_definition = str::join(
			", ", table.foreign_keys.begin(), table.foreign_keys.end(),
			[this](const auto& x) -> auto
			{
				const auto& c = x.second;
				return this->sql_foreign_key(x.first, c.to, c.key, c.on_delete, c.on_update);
			}
		);
		auto insert_columns = str::join(
			", ", mapping.begin(), mapping.end(),
			[this](const auto& x) -> auto { return this->quote_name(x.first); }
		);
		auto select_columns = str::join(
			", ", mapping.begin(), mapping.end(),
			[](const auto& x) -> auto { return x.second; }
		);
		auto table_name = this->quote_name(table.name);
		auto new_table_name = this->quote_name("new_" + table.name + "_table_to_alter");
		auto query = "PRAGMA foreign_keys=off;"
			" CREATE TABLE " + new_table_name + " (" +
				columns_definition + (fks_definition.empty() ? "" : (", " + fks_definition)) + ");"
			" INSERT INTO " + new_table_name + " (" + insert_columns + ")"
			" SELECT " + select_columns +
			" FROM " + table_name + ";"
			" DROP TABLE " + table_name + ";"
			" ALTER TABLE " + new_table_name + " RENAME TO " + table_name + ";"
			" PRAGMA foreign_keys=on;";
		this->execute(query);
	}

public:
	inline explicit SchemaEditor(orm::abc::ISQLDriver* db) : db::DefaultSQLSchemaEditor(db)
	{
	}

	inline void drop_column(const db::TableState& table, const db::ColumnState& column) const override
	{
		auto table_copy = table;
		table_copy.columns.erase(column.name);
		if (table_copy.foreign_keys.contains(column.name))
		{
			table_copy.foreign_keys.erase(column.name);
		}

		std::unordered_map<std::string, std::string> mapping;
		for (const auto& col : table_copy.columns)
		{
			mapping[col.second.name] = this->quote_name(col.second.name);
		}

		this->recreate_table(table_copy, mapping);
	}

	inline void alter_column(
		const db::TableState& table,
		const db::ColumnState& old_column, const db::ColumnState& new_column,
		bool strict
	) const override
	{
		auto table_copy = table;
		std::unordered_map<std::string, std::string> mapping;
		for (const auto& column : table_copy.columns)
		{
			mapping[column.second.name] = this->quote_name(column.second.name);
		}

		table_copy.columns.erase(old_column.name);
		mapping.erase(old_column.name);
		table_copy.columns[new_column.name] = new_column;
		auto old_null = old_column.constraints.null;
		auto new_null = new_column.constraints.null;
		if (old_null.has_value() && old_null.value() && !(new_null.has_value() && new_null.value()))
		{
			mapping[new_column.name] = "coalesce(" +
				this->quote_name(old_column.name) + ", " +
				(new_column.default_value.empty() ? "NULL" : new_column.default_value) +
			")";
		}
		else
		{
			mapping[new_column.name] = this->quote_name(old_column.name);
		}

		if (table_copy.foreign_keys.contains(old_column.name) && old_column.name != new_column.name)
		{
			auto fk = table_copy.foreign_keys[old_column.name];
			table_copy.foreign_keys.erase(old_column.name);
			table_copy.foreign_keys[new_column.name] = fk;
		}

		this->recreate_table(table_copy, mapping);
	}
};

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
