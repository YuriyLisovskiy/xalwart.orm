/**
 * sqlite3/schema_editor.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./schema_editor.h"

#ifdef USE_SQLITE3


__ORM_SQLITE3_BEGIN__

std::string SchemaEditor::sql_type_to_string(db::sql_column_type type) const
{
	switch (type)
	{
		case db::SMALL_SERIAL_T:
			return "SMALLINT";
		case db::SERIAL_T:
			return "INT";
		case db::BIG_SERIAL_T:
			return "BIGINT";
		default:
			return db::DefaultSQLSchemaEditor::sql_type_to_string(type);
	}
}

std::string SchemaEditor::sql_data_column(
	db::sql_column_type type, const std::string& name,
	bool null, bool primary_key, bool unique, const std::string& check
) const
{
	auto result = name + " ";
	bool auto_increment = false;
	switch (type)
	{
		case db::SMALL_SERIAL_T:
		case db::SERIAL_T:
		case db::BIG_SERIAL_T:
			result += "INTEGER";
			auto_increment = true;
			break;
		default:
			result += this->sql_type_to_string(type);
	}

	if (primary_key)
	{
		result += " PRIMARY KEY";
	}

	if (auto_increment)
	{
		result += " AUTOINCREMENT";
	}

	if (unique)
	{
		result += " UNIQUE";
	}

	if (!null)
	{
		result += " NOT NULL";
	}

	if (!check.empty())
	{
		result += " CHECK (" + check + ")";
	}

	return result;
}

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
