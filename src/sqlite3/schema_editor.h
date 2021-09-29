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
// TODO: docs for 'SchemaEditor'
class SchemaEditor : public db::DefaultSQLSchemaEditor
{
public:
	inline explicit SchemaEditor(abc::IBackend* backend) : db::DefaultSQLSchemaEditor(backend)
	{
	}

	void drop_column(const db::TableState& table, const db::ColumnState& column) const override;

	inline void alter_column(
		const db::TableState& table, const db::ColumnState& old_column, const db::ColumnState& new_column, bool strict
	) const override;

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
			throw ValueError("'autoincrement' is only allowed on an integer primary key", _ERROR_DETAILS_);
		}
	}

	virtual void recreate_table(
		const db::TableState& table, const std::unordered_map<std::string, std::string>& mapping
	) const;
};

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
