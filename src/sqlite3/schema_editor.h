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


__SQLITE3_BEGIN__

// TESTME: SchemaEditor
class SchemaEditor : public db::DefaultSQLSchemaEditor
{
protected:
	[[nodiscard]]
	std::string sql_type_to_string(db::sql_column_type type) const override;

public:
	inline explicit SchemaEditor(orm::abc::ISQLDriver* db) : db::DefaultSQLSchemaEditor(db)
	{
	}

	[[nodiscard]]
	std::string sql_data_column(
		db::sql_column_type type, const std::string& name,
		bool null, bool primary_key, bool unique, const std::string& check
	) const override;
};

__SQLITE3_END__

#endif // USE_SQLITE3
