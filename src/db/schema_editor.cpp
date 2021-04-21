/**
 * db/schema_editor.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./schema_editor.h"


__ORM_DB_BEGIN__

std::string DefaultSQLSchemaEditor::sql_type_to_string(sql_column_type type) const
{
	switch (type)
	{
		case BOOL_T:
			return "BOOL";
		case CHAR_T:
			return "CHAR";
		case VARCHAR_T:
			return "VARCHAR";
		case TEXT_T:
			return "TEXT";
		case SMALLINT_T:
			return "SMALLINT";
		case INT_T:
			return "INTEGER";
		case BIGINT_T:
			return "BIGINT";
		case SMALL_SERIAL_T:
			return "SMALLSERIAL";
		case SERIAL_T:
			return "SERIAL";
		case BIG_SERIAL_T:
			return "BIGSERIAL";
		case REAL_T:
			return "REAL";
		case DOUBLE_T:
			return "DOUBLE";
		case DATE_T:
			return "DATE";
		case TIME_T:
			return "TIME";
		case DATETIME_T:
			return "TIMESTAMP";
	}

	throw ValueError("unknown SQL data type", _ERROR_DETAILS_);
}

std::string DefaultSQLSchemaEditor::sql_on_action_to_string(on_action action) const
{
	switch (action)
	{
		case SET_NULL:
			return "SET NULL";
		case SET_DEFAULT:
			return "SET DEFAULT";
		case RESTRICT:
			return "RESTRICT";
		case NO_ACTION:
			return "NO ACTION";
		case CASCADE:
			return "CASCADE";
	}

	throw ValueError("unknown SQL 'on [action]'", _ERROR_DETAILS_);
}

std::string DefaultSQLSchemaEditor::sql_constraints(
	bool null, bool primary_key, bool unique, const std::string& check
) const
{
	std::string result;
	if (primary_key)
	{
		result += " PRIMARY KEY";
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

std::string DefaultSQLSchemaEditor::sql_foreign_key(
	const std::string& name,
	const std::string& parent, const std::string& parent_key,
	on_action on_delete, on_action on_update
) const
{
	auto result = "FOREIGN KEY(" + name + ") REFERENCES " + parent + "(" + parent_key + ")";
	switch (on_delete)
	{
		case NO_ACTION:
			break;
		default:
			result += " ON DELETE " + this->sql_on_action_to_string(on_delete);
			break;
	}

	switch (on_update)
	{
		case NO_ACTION:
			break;
		default:
			result += " ON UPDATE " + this->sql_on_action_to_string(on_update);
			break;
	}

	return result;
}

std::string DefaultSQLSchemaEditor::sql_text_column(
	sql_column_type type, const std::string& name, long int max_len,
	bool null, bool primary_key, bool unique, const std::string& check
) const
{
	switch (type)
	{
		case CHAR_T:
		case VARCHAR_T:
		case TEXT_T:
			break;
		default:
			throw ValueError("invalid text column type", _ERROR_DETAILS_);
	}

	return name + " " + this->sql_type_to_string(type) + (
		type == TEXT_T || max_len < 0 ? "" : "(" + std::to_string(max_len) + ")"
	) + this->sql_constraints(null, primary_key, unique, check);
}

__ORM_DB_END__
