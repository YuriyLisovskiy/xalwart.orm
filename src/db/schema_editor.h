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
	virtual std::string sql_type_to_string(sql_column_type type) const;

	[[nodiscard]]
	virtual std::string sql_on_action_to_string(on_action action) const;

	[[nodiscard]]
	virtual std::string sql_constraints(
		bool null, bool primary_key, bool unique, const std::string& check
	) const;

public:
	inline explicit DefaultSQLSchemaEditor(orm::abc::ISQLDriver* db) : db(db)
	{
		if (!this->db)
		{
			throw core::NullPointerException(
				"xw::orm::db::SchemaEditorBase: database driver is nullptr", _ERROR_DETAILS_
			);
		}
	}

	[[nodiscard]]
	inline std::string sql_data_column(
		sql_column_type type, const std::string& name,
		bool null, bool primary_key, bool unique, const std::string& check
	) const override
	{
		return name + " " + this->sql_type_to_string(type) +
			this->sql_constraints(null, primary_key, unique, check);
	}

	[[nodiscard]]
	std::string sql_text_column(
		sql_column_type type, const std::string& name, long int max_len,
		bool null, bool primary_key, bool unique, const std::string& check
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
