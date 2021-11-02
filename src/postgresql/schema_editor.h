/**
 * postgresql/schema_editor.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TODO: description
 */

#pragma once

#ifdef USE_POSTGRESQL

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../db/schema_editor.h"


__ORM_POSTGRESQL_BEGIN__

// TESTME: SchemaEditor
// TODO: docs for 'SchemaEditor'
class SchemaEditor : public db::DefaultSQLSchemaEditor
{
public:
	inline explicit SchemaEditor(IBackend* backend) : db::DefaultSQLSchemaEditor(backend)
	{
	}

protected:
	[[nodiscard]]
	std::string sql_column(const db::ColumnState& column) const override;

	void sql_column_autoincrement_check(
		db::SqlColumnType type, bool autoincrement, bool primary_key
	) const override;
};

__ORM_POSTGRESQL_END__

#endif // USE_POSTGRESQL
