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
		const std::optional<bool>& null,
		bool primary_key,
		bool unique,
		bool autoincrement,
		const std::string& check,
		const std::string& default_
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
	std::string sql_column(
		db::sql_column_type type, const std::string& name,
		const std::optional<size_t>& max_len,
		const std::optional<bool>& null,
		bool primary_key,
		bool unique,
		bool autoincrement,
		const std::string& check,
		const std::string& default_
	) const override;
};

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
