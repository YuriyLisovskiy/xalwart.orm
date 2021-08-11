/**
 * sqlite3/schema_editor.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./schema_editor.h"

#ifdef USE_SQLITE3


__ORM_SQLITE3_BEGIN__

std::string SchemaEditor::sql_column_constraints(const db::Constraints& cc, const std::string& default_value) const
{
	std::string result;
	if (cc.primary_key)
	{
		result += " PRIMARY KEY";
	}

	if (cc.autoincrement)
	{
		result += " AUTOINCREMENT";
	}

	if (cc.unique)
	{
		result += " UNIQUE";
	}

	if (cc.null.has_value())
	{
		result += cc.null.value() ? " NULL" : " NOT NULL";
	}

	if (!cc.check.empty())
	{
		result += " CHECK (" + cc.check + ")";
	}

	if (!default_value.empty())
	{
		result += " DEFAULT " + default_value;
	}

	return result;
}

std::string SchemaEditor::sql_type_string(db::SqlColumnType type) const
{
	switch (type)
	{
		case db::SqlColumnType::SmallSerial:
			return "SMALLINT";
		case db::SqlColumnType::Serial:
			return "INT";
		case db::SqlColumnType::BigSerial:
			return "BIGINT";
		case db::SqlColumnType::DateTime:
			return "DATETIME";
		default:
			return db::DefaultSQLSchemaEditor::sql_type_string(type);
	}
}

std::string SchemaEditor::sql_column(const db::ColumnState& column) const
{
	auto column_copy = column;
	if (column.constraints.autoincrement)
	{
		switch (column_copy.type)
		{
			case db::SqlColumnType::SmallSerial:
			case db::SqlColumnType::Serial:
			case db::SqlColumnType::BigSerial:
			case db::SqlColumnType::SmallInt:
			case db::SqlColumnType::BigInt:
				column_copy.type = db::SqlColumnType::Int;
				break;
			default:
				break;
		}
	}

	return db::DefaultSQLSchemaEditor::sql_column(column_copy);
}

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
