/**
 * postgresql/schema_editor.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./schema_editor.h"

#ifdef USE_POSTGRESQL


__ORM_POSTGRESQL_BEGIN__

std::string SchemaEditor::sql_column(const db::ColumnState& column) const
{
	auto column_copy = column;
	if (column.constraints.autoincrement)
	{
		switch (column_copy.type)
		{
			case db::SqlColumnType::SmallInt:
				column_copy.type = db::SqlColumnType::SmallSerial;
			case db::SqlColumnType::Int:
				column_copy.type = db::SqlColumnType::Serial;
			case db::SqlColumnType::BigInt:
				column_copy.type = db::SqlColumnType::BigSerial;
				break;
			default:
				break;
		}
	}

	return db::DefaultSQLSchemaEditor::sql_column(column_copy);
}

void SchemaEditor::sql_column_autoincrement_check(
	db::SqlColumnType type, bool autoincrement, bool /* primary_key */
) const
{
	if (autoincrement)
	{
		switch (type)
		{
			case db::SqlColumnType::SmallSerial:
			case db::SqlColumnType::Serial:
			case db::SqlColumnType::BigSerial:
				break;
			default:
				throw ValueError(
					"'autoincrement' is only allowed on column with serial type", _ERROR_DETAILS_
				);
		}
	}
}

__ORM_POSTGRESQL_END__

#endif // USE_POSTGRESQL
