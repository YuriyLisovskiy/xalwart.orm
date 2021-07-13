/**
 * db/schema_editor.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./schema_editor.h"


__ORM_DB_BEGIN__

std::string DefaultSQLSchemaEditor::sql_on_action_constraint(on_action action) const
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

	throw ValueError(
		"DefaultSQLSchemaEditor > sql_on_action_to_string: unknown SQL 'on action'",
		_ERROR_DETAILS_
	);
}

std::string DefaultSQLSchemaEditor::sql_column_constraints(
	const constraints_t& constraints, const std::string& default_value
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
	sql_column_type type, bool autoincrement, bool primary_key
) const
{
	if (autoincrement)
	{
		bool non_int_type = false;
		switch (type)
		{
			case SMALLINT_T:
			case INT_T:
			case BIGINT_T:
			case SMALL_SERIAL_T:
			case SERIAL_T:
			case BIG_SERIAL_T:
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
	const std::string& name, sql_column_type type,
	const std::optional<size_t>& max_len
) const
{
	if (max_len.has_value())
	{
		if (type != VARCHAR_T)
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

std::string DefaultSQLSchemaEditor::sql_type_string(sql_column_type type) const
{
	switch (type)
	{
		case BOOL_T:
			return "BOOL";
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

	throw ValueError(
		"DefaultSQLSchemaEditor > sql_type_to_string: unknown SQL data type",
		_ERROR_DETAILS_
	);
}

std::string DefaultSQLSchemaEditor::sql_column(const column_state& column) const
{
	if (column.name.empty())
	{
		throw ValueError(
			"DefaultSQLSchemaEditor > sql_column: 'name' can not be empty",
			_ERROR_DETAILS_
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
	on_action on_delete, on_action on_update
) const
{
	auto result = "FOREIGN KEY(" + name + ") REFERENCES " + parent + "(" + parent_key + ")";
	switch (on_delete)
	{
		case NO_ACTION:
			break;
		default:
			result += " ON DELETE " + this->sql_on_action_constraint(on_delete);
			break;
	}

	switch (on_update)
	{
		case NO_ACTION:
			break;
		default:
			result += " ON UPDATE " + this->sql_on_action_constraint(on_update);
			break;
	}

	return result;
}

__ORM_DB_END__
