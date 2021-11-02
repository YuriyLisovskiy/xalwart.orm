/**
 * db/schema_editor.cpp
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 */

#include "./schema_editor.h"


__ORM_DB_BEGIN__

void DefaultSQLSchemaEditor::create_table(const TableState& table) const
{
	std::list<ColumnState> columns;
	for (const auto& col : table.columns)
	{
		columns.push_back(col.second);
	}

	std::list<std::tuple<std::string, ForeignKeyConstraints>> foreign_keys;
	for (const auto& fk : table.foreign_keys)
	{
		foreign_keys.emplace_back(fk.first, fk.second);
	}

	this->execute(this->sql_create_table(table, columns, foreign_keys));
}

void DefaultSQLSchemaEditor::alter_column(
	const TableState& table, const ColumnState& old_column, const ColumnState& new_column, bool strict
) const
{
	// Was column renamed?
	if (old_column.name != new_column.name)
	{
		this->execute(this->sql_rename_column(table, old_column, new_column));
	}

	std::list<std::string> actions;
	std::list<std::string> null_actions;
	std::list<std::string> post_actions;

	// Was type changed?
	if (old_column.type != new_column.type)
	{
		auto [partial_sql, extra_actions] = this->partial_sql_alter_column_type(table, old_column, new_column);
		actions.push_back(partial_sql);
		post_actions.insert(post_actions.end(), extra_actions.begin(), extra_actions.end());
	}

	// When changing a column NULL constraint to NOT NULL with a given
	// default value, we need to do 4 steps:
	//  1) add a default for new incoming writes;
	//  2) update existing NULL rows with new default;
	//  3) replace NULL constraints with NOT NULL;
	//  4 drop the default again.
	// Was default changed?
	bool needs_db_default = false;
	auto old_null = old_column.constraints.null;
	auto new_null = new_column.constraints.null;
	bool has_change_from_null_to_not_null = old_null.has_value() && old_null.value() &&
		(!new_null.has_value() || !new_null.value());
	if (has_change_from_null_to_not_null)
	{
		if (
			!this->skip_default(new_column) &&
			old_column.default_value != new_column.default_value &&
			!new_column.default_value.empty()
		)
		{
			needs_db_default = true;
			actions.push_back(this->partial_sql_alter_column_default(table, old_column, new_column, false));
		}
	}

	// Was hull changed?
	if (old_null != new_null)
	{
		auto fragment = this->partial_sql_alter_column_null(table, old_column, new_column);
		if (!fragment.empty())
		{
			null_actions.push_back(fragment);
		}
	}

	auto four_way_default_alteration = !new_column.default_value.empty() &&
		has_change_from_null_to_not_null;
	if (!actions.empty() || !null_actions.empty())
	{
		if (!four_way_default_alteration)
		{
			actions.insert(actions.end(), null_actions.begin(), null_actions.end());
		}

		// TODO: check if driver supports combined alters, i.e. separated by comma.

		// Apply actions.
		for (const auto& partial_sql : actions)
		{
			this->execute(this->sql_alter_column(table, partial_sql));
		}

		if (four_way_default_alteration)
		{
			// Update existing rows with default value.
			this->execute(this->sql_update_with_default(table, new_column));

			// Since we didn't run a NOT NULL change before we need to do it now.
			for (const auto& partial_sql : null_actions)
			{
				this->execute(this->sql_alter_column(table, partial_sql));
			}
		}
	}

	if (!post_actions.empty())
	{
		for (const auto& sql : post_actions)
		{
			this->execute(sql);
		}
	}

	// If primary_key changed to False, delete the primary key constraint.
	if (old_column.constraints.primary_key && !new_column.constraints.primary_key)
	{
		this->delete_primary_key(table, strict);
	}

	// Added a unique?
	if (this->unique_should_be_added(old_column, new_column))
	{
		this->execute(this->sql_create_unique(table, {new_column}));
	}

	// TODO: implement logic for index.

//	if (needs_db_default)
//	{
//		auto partial_sql = this->partial_sql_alter_column_default(table, old_column, new_column, true);
//		this->execute(this->sql_alter_column(table, partial_sql));
//	}
}

std::string DefaultSQLSchemaEditor::sql_create_table(
	const TableState& table,
	const std::list<ColumnState>& columns,
	const std::list<std::tuple<std::string, ForeignKeyConstraints>>& fks
) const
{
	auto s_columns = str::join(
		", ", columns.begin(), columns.end(), [this](const auto& col) -> auto { return this->sql_column(col); }
	);
	auto s_constraints = str::join(
		", ", fks.begin(), fks.end(), [this](const auto& fk) -> auto
		{
			auto fk_c = std::get<1>(fk);
			return this->sql_foreign_key(
				std::get<0>(fk), fk_c.to, fk_c.key, fk_c.on_delete, fk_c.on_update
			);
		}
	);
	return "CREATE TABLE \"" + table.name + "\"(" + s_columns +
		(s_constraints.empty() ? "" : ", " + s_constraints) + ")";
}

std::string DefaultSQLSchemaEditor::sql_create_unique(
	const TableState& table, const std::list<ColumnState>& cols
) const
{
	auto str_columns = str::join(", ", cols.begin(), cols.end(),
		[](const auto& col) -> auto { return col.name; }
	);
	return this->sql_alter_column(
		table,
		"ADD CONSTRAINT " + this->create_unique_name(table, cols, "_unique") +
		" UNIQUE (" + str_columns + ")"
	);
}

std::string DefaultSQLSchemaEditor::partial_sql_alter_column_default(
	const TableState& table, const ColumnState& old_col, const ColumnState& new_col, bool drop
) const
{
	// !IMPORTANT!
	// When some database drivers does not take defaults as a
	// parameter, the default 'sql_column_default()' method will
	// not fulfill this requirement. Maybe it should be overwritten.
	auto default_ = this->partial_sql_column_default(new_col);
	auto col_name = this->quote_name(new_col.name);
	if (drop)
	{
		return "ALTER COLUMN " + col_name + " DROP DEFAULT";
	}

	return "ALTER COLUMN " + col_name + " SET DEFAULT " + default_;
}

std::string DefaultSQLSchemaEditor::partial_sql_alter_column_null(
	const TableState& table, const ColumnState& old_col, const ColumnState& new_col
) const
{
	auto null = new_col.constraints.null;
	auto col_name = this->quote_name(new_col.name);
	if (null.has_value() && null.value())
	{
		return "ALTER COLUMN " + col_name + " DROP NOT NULL";
	}

	return "ALTER COLUMN " + col_name + " SET NOT NULL";
}

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
		"xw::orm::db::DefaultSQLSchemaEditor > sql_on_action_to_string: unknown SQL 'on action'", _ERROR_DETAILS_
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
				"xw::orm::db::DefaultSQLSchemaEditor > sql_column_autoincrement_check:"
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
				"xw::orm::db::DefaultSQLSchemaEditor > sql_column_max_len_check:"
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

void DefaultSQLSchemaEditor::delete_primary_key(const TableState& table, bool strict) const
{
	auto constraint_names = this->constraint_names(table, true);
	auto cn_size = constraint_names.size();
	if (strict && cn_size != 1)
	{
		throw MigrationsError(
			"Got wrong number " + std::to_string(cn_size) +
			" of primary key constraints for '" + table.name + "'",
			_ERROR_DETAILS_
		);
	}

	for (const auto& constraint_name : constraint_names)
	{
		this->execute(this->sql_delete_primary_key(table, constraint_name));
	}
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
		"xw::orm::db::DefaultSQLSchemaEditor > sql_type_to_string: unknown SQL data type", _ERROR_DETAILS_
	);
}

std::string DefaultSQLSchemaEditor::sql_column(const ColumnState& column) const
{
	if (column.name.empty())
	{
		throw ValueError(
			"xw::orm::db::DefaultSQLSchemaEditor > sql_column: 'name' can not be empty", _ERROR_DETAILS_
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
