/**
 * sqlite3/schema_editor.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./schema_editor.h"

#ifdef USE_SQLITE3


__ORM_SQLITE3_BEGIN__

std::string SchemaEditor::sql_column_constraints(
	const std::optional<bool>& null,
	bool primary_key,
	bool unique,
	bool autoincrement,
	const std::string& check,
	const std::string& default_
) const
{
	std::string result;
	if (primary_key)
	{
		result += " PRIMARY KEY";
	}

	if (autoincrement)
	{
		result += " AUTOINCREMENT";
	}

	if (unique)
	{
		result += " UNIQUE";
	}

	if (null.has_value())
	{
		result += null.value() ? " NULL" : " NOT NULL";
	}

	if (!check.empty())
	{
		result += " CHECK (" + check + ")";
	}

	if (!default_.empty())
	{
		result += " DEFAULT " + default_;
	}

	return result;
}

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
		case db::DATETIME_T:
			return "DATETIME";
		default:
			return db::DefaultSQLSchemaEditor::sql_type_to_string(type);
	}
}

std::string SchemaEditor::sql_column(
	db::sql_column_type type, const std::string& name,
	const std::optional<size_t>& max_len,
	const std::optional<bool>& null,
	bool primary_key,
	bool unique,
	bool autoincrement,
	const std::string& check,
	const std::string& default_
) const
{
	if (autoincrement)
	{
		switch (type)
		{
			case db::SMALL_SERIAL_T:
			case db::SERIAL_T:
			case db::BIG_SERIAL_T:
			case db::SMALLINT_T:
			case db::BIGINT_T:
				type = db::INT_T;
				break;
			default:
				break;
		}
	}

	return db::DefaultSQLSchemaEditor::sql_column(
		type, name, max_len, null, primary_key, unique, autoincrement, check, default_
	);
}

__ORM_SQLITE3_END__

#endif // USE_SQLITE3
