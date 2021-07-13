/**
 * sqlite3/schema_editor.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./schema_editor.h"

#ifdef USE_SQLITE3


__ORM_SQLITE3_BEGIN__

std::string SchemaEditor::sql_column_constraints(
	const db::constraints_t& cc, const std::string& default_value
) const
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

std::string SchemaEditor::sql_type_string(db::sql_column_type type) const
{
	switch (type)
	{
		case db::SMALL_SERIAL_T:
			return "SMALLINT";
		case db::SERIAL_T:
			return "INT";
		case db::BIG_SERIAL_T:
			return "BIGINT";
		case db::DATETIME_T:
			return "DATETIME";
		default:
			return db::DefaultSQLSchemaEditor::sql_type_string(type);
	}
}

std::string SchemaEditor::sql_column(const db::column_state& column) const
{
	auto column_copy = column;
	if (column.constraints.autoincrement)
	{
		switch (column_copy.type)
		{
			case db::SMALL_SERIAL_T:
			case db::SERIAL_T:
			case db::BIG_SERIAL_T:
			case db::SMALLINT_T:
			case db::BIGINT_T:
				column_copy.type = db::INT_T;
				break;
			default:
				break;
		}
	}

	return db::DefaultSQLSchemaEditor::sql_column(column_copy);
}

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
