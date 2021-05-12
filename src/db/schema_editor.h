/**
 * db/schema_editor.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TODO: description
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./abc.h"
#include "../abc.h"


__ORM_DB_BEGIN__

// TESTME: DefaultSchemaEditor
class DefaultSQLSchemaEditor : public abc::ISchemaEditor
{
protected:
	orm::abc::ISQLDriver* db;

protected:
	[[nodiscard]]
	std::string quote_name(const std::string& s) const override
	{
		return s.starts_with('"') ? s : '"' + s + '"';
	}

	[[nodiscard]]
	virtual std::string sql_on_action_to_string(on_action action) const;

	[[nodiscard]]
	virtual std::string sql_column_constraints(
		const constraints_t& constraints, const std::string& default_value
	) const;

	virtual void sql_column_autoincrement_check(
		sql_column_type type, bool autoincrement, bool primary_key
	) const;

	[[nodiscard]]
	virtual bool sql_column_max_len_check(
		const std::string& name, sql_column_type type,
		const std::optional<size_t>& max_len
	) const;

	[[nodiscard]]
	virtual std::string sql_type_to_string(sql_column_type type) const;

	[[nodiscard]]
	virtual std::string sql_column(const column_state& column) const;

	[[nodiscard]]
	virtual std::string sql_foreign_key(
		const std::string& name,
		const std::string& parent, const std::string& parent_key,
		on_action on_delete, on_action on_update
	) const;

public:
	inline explicit DefaultSQLSchemaEditor(orm::abc::ISQLDriver* db) : db(db)
	{
		if (!this->db)
		{
			throw NullPointerException(
				"DefaultSQLSchemaEditor: database driver is nullptr", _ERROR_DETAILS_
			);
		}
	}

	inline void create_table(const table_state& table) const override
	{
		std::list<std::string> columns;
		for (const auto& col : table.columns)
		{
			auto& c = col.second.constraints;
			columns.push_back(this->sql_column(col.second));
		}

		std::list<std::string> constraints;
		for (const auto& fk : table.foreign_keys)
		{
			auto fk_c = fk.second;
			constraints.push_back(this->sql_foreign_key(
				fk.first, fk_c.to, fk_c.key, fk_c.on_delete, fk_c.on_update
			));
		}

		auto s_constraints = str::join(", ", constraints.begin(), constraints.end());
		auto query = "CREATE TABLE " + table.name + "(" +
			str::join(", ", columns.begin(), columns.end()) +
			(s_constraints.empty() ? "" : ", " + s_constraints) +
		");";
		this->db->run_query(query);
	}

	inline void drop_table(const std::string& name) const override
	{
		this->db->run_query("DROP TABLE " + name + ";");
	}

	inline void create_column(
		const std::string& table_name, const column_state& column
	) const override
	{
		auto& c = column.constraints;
		auto col_string = this->sql_column(column);
		auto query = "ALTER TABLE " + table_name + " ADD COLUMN " + col_string + ";";
		this->db->run_query(query);
	}

	inline void drop_column(
		const table_state& table, const column_state& column
	) const override
	{
		this->db->run_query(
			"ALTER TABLE " + table.name + " DROP COLUMN " + column.name + ";"
		);
	}
};

__ORM_DB_END__
