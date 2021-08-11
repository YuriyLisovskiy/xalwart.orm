/**
 * db/schema_editor.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./schema_editor.h"


__ORM_DB_BEGIN__

std::string DefaultSQLSchemaEditor::sql_on_action_constraint(OnAction action) const
{
	switch (action)
	{
		case OnAction::SetNull:
			return "SET NULL";
		case OnAction::SetDefault:
			return "SET DEFAULT";
		case OnAction::Restrict:
			return "RESTRICT";
		case OnAction::NoAction:
			return "NO ACTION";
		case OnAction::Cascade:
			return "CASCADE";
	}

	throw ValueError(
		"DefaultSQLSchemaEditor > sql_on_action_to_string: unknown SQL 'on action'",
		_ERROR_DETAILS_
	);
}

std::string DefaultSQLSchemaEditor::sql_column_constraints(
	const Constraints& constraints, const std::string& default_value
) const
{
	std::string result;
	if (constraints.primary_key)
	{
		result += " PRIMARY KEY";
	}

	if (constraints.unique)
	{
		result += " UNIQUE";
	}

	if (constraints.null.has_value())
	{
		result += constraints.null.value() ? " NULL" : " NOT NULL";
	}

	if (!constraints.check.empty())
	{
		result += " CHECK (" + constraints.check + ")";
	}

	if (!default_value.empty())
	{
		result += " DEFAULT " + default_value;
	}

	return result;
}

void DefaultSQLSchemaEditor::sql_column_autoincrement_check(
	SqlColumnType type, bool autoincrement, bool primary_key
) const
{
	if (autoincrement)
	{
		bool non_int_type = false;
		switch (type)
		{
			case SqlColumnType::SmallInt:
			case SqlColumnType::Int:
			case SqlColumnType::BigInt:
			case SqlColumnType::SmallSerial:
			case SqlColumnType::Serial:
			case SqlColumnType::BigSerial:
				break;
			default:
				non_int_type = true;
				break;
		}

		if (non_int_type || !primary_key)
		{
			throw ValueError(
				"DefaultSQLSchemaEditor > sql_column_autoincrement_check:"
				" 'autoincrement' is only allowed on an integer (short, int, long long) primary key",
				_ERROR_DETAILS_
			);
		}
	}
}

bool DefaultSQLSchemaEditor::sql_column_max_len_check(
	const std::string& name, SqlColumnType type, const std::optional<size_t>& max_len
) const
{
	if (max_len.has_value())
	{
		if (type != SqlColumnType::VarChar)
		{
			throw ValueError(
				"DefaultSQLSchemaEditor > sql_column_max_len_check:"
				" unable to set 'max_len' constraint for column '" + name +
				"' with type '" + this->sql_type_string(type) + "'",
				_ERROR_DETAILS_
			);
		}
		else
		{
			return true;
		}
	}

	return false;
}

std::string DefaultSQLSchemaEditor::sql_type_string(SqlColumnType type) const
{
	switch (type)
	{
		case SqlColumnType::Bool:
			return "BOOL";
		case SqlColumnType::VarChar:
			return "VARCHAR";
		case SqlColumnType::Text:
			return "TEXT";
		case SqlColumnType::SmallInt:
			return "SMALLINT";
		case SqlColumnType::Int:
			return "INTEGER";
		case SqlColumnType::BigInt:
			return "BIGINT";
		case SqlColumnType::SmallSerial:
			return "SMALLSERIAL";
		case SqlColumnType::Serial:
			return "SERIAL";
		case SqlColumnType::BigSerial:
			return "BIGSERIAL";
		case SqlColumnType::Real:
			return "REAL";
		case SqlColumnType::Double:
			return "DOUBLE";
		case SqlColumnType::Date:
			return "DATE";
		case SqlColumnType::Time:
			return "TIME";
		case SqlColumnType::DateTime:
			return "TIMESTAMP";
	}

	throw ValueError(
		"DefaultSQLSchemaEditor > sql_type_to_string: unknown SQL data type", _ERROR_DETAILS_
	);
}

std::string DefaultSQLSchemaEditor::sql_column(const ColumnState& column) const
{
	if (column.name.empty())
	{
		throw ValueError(
			"DefaultSQLSchemaEditor > sql_column: 'name' can not be empty", _ERROR_DETAILS_
		);
	}

	auto c = column.constraints;
	auto sql_type = this->sql_type_string(column.type);
	if (this->sql_column_max_len_check(column.name, column.type, c.max_len))
	{
		sql_type += "(" + std::to_string(c.max_len.value()) + ")";
	}

	this->sql_column_autoincrement_check(column.type, c.autoincrement, c.primary_key);

	return column.name + " " + sql_type + this->sql_column_constraints(
		c, column.default_value
	);
}

std::string DefaultSQLSchemaEditor::sql_foreign_key(
	const std::string& name,
	const std::string& parent, const std::string& parent_key,
	OnAction on_delete, OnAction on_update
) const
{
	auto result = "FOREIGN KEY(" + name + ") REFERENCES " + parent + "(" + parent_key + ")";
	switch (on_delete)
	{
		case OnAction::NoAction:
			break;
		default:
			result += " ON DELETE " + this->sql_on_action_constraint(on_delete);
			break;
	}

	switch (on_update)
	{
		case OnAction::NoAction:
			break;
		default:
			result += " ON UPDATE " + this->sql_on_action_constraint(on_update);
			break;
	}

	return result;
}

__ORM_DB_END__
