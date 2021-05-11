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
class DefaultSQLSchemaEditor : public abc::ISQLSchemaEditor
{
protected:
	orm::abc::ISQLDriver* db;

protected:

	[[nodiscard]]
	virtual std::string sql_on_action_to_string(on_action action) const;

	[[nodiscard]]
	virtual std::string sql_column_constraints(
		const std::optional<bool>& null,
		bool primary_key,
		bool unique,
		bool autoincrement,
		const std::string& check,
		const std::string& default_
	) const;

	virtual void sql_column_autoincrement_check(
		sql_column_type type, bool autoincrement, bool primary_key
	) const;

	[[nodiscard]]
	virtual bool sql_column_max_len_check(
		const std::string& name, sql_column_type type,
		const std::optional<size_t>& max_len
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

	[[nodiscard]]
	std::string sql_type_to_string(sql_column_type type) const override;

	[[nodiscard]]
	std::string sql_column(
		sql_column_type type, const std::string& name,
		const std::optional<size_t>& max_len,
		const std::optional<bool>& null,
		bool primary_key,
		bool unique,
		bool autoincrement,
		const std::string& check,
		const std::string& default_
	) const override;

	[[nodiscard]]
	std::string sql_foreign_key(
		const std::string& name,
		const std::string& parent, const std::string& parent_key,
		on_action on_delete, on_action on_update
	) const override;

	inline void create_table(
		const std::string& name,
		const std::list<std::string>& columns,
		const std::list<std::string>& constraints
	) const override
	{
		auto s_constraints = str::join(", ", constraints.begin(), constraints.end());
		auto query = "CREATE TABLE " + name + "(" +
			str::join(", ", columns.begin(), columns.end()) +
			(s_constraints.empty() ? "" : ", " + s_constraints) +
		");";
		this->db->run_query(query);
	}

	inline void drop_table(const std::string& name) const override
	{
		this->db->run_query("DROP TABLE " + name + ";");
	}
};

__ORM_DB_END__
