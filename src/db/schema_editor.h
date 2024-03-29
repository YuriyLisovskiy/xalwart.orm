/**
 * db/schema_editor.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TODO: description
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./interfaces.h"
#include "../interfaces.h"


__ORM_DB_BEGIN__

// TESTME: DefaultSchemaEditor
// TODO: docs for 'DefaultSchemaEditor'
class DefaultSQLSchemaEditor : public ISchemaEditor
{
public:
	void create_table(
		const TableState& table, const IDatabaseConnection* connection
	) const override;

	inline void drop_table(
		const std::string& name, const IDatabaseConnection* connection
	) const override
	{
		this->execute(this->sql_drop_table(name), connection);
	}

	inline void rename_table(
		const TableState& table,
		const std::string& old_name,
		const std::string& new_name,
		const IDatabaseConnection* connection
	) const override
	{
		if (old_name == new_name)
		{
			// TODO: !IMPORTANT! Check if driver ignores case
			//  and if names are the same in lower-case.
			return;
		}

		this->execute(this->sql_rename_table(old_name, new_name), connection);
	}

	inline void create_column(
		const TableState& table, const ColumnState& column, const IDatabaseConnection* connection
	) const override
	{
		this->execute(this->sql_add_column(table, column), connection);
	}

	inline void drop_column(
		const TableState& table, const ColumnState& column, const IDatabaseConnection* connection
	) const override
	{
		this->execute(this->sql_drop_column(table, column), connection);
	}

	void alter_column(
		const TableState& table,
		const ColumnState& old_column,
		const ColumnState& new_column,
		bool strict,
		const IDatabaseConnection* connection
	) const override;

protected:
	// SQL builders.
	[[nodiscard]]
	virtual std::string sql_create_table(
		const TableState& table,
		const std::list<ColumnState>& columns,
		const std::list<std::tuple<std::string, ForeignKeyConstraints>>& fks
	) const;

	[[nodiscard]]
	virtual inline std::string sql_drop_table(const std::string& name) const
	{
		return "DROP TABLE " + this->quote_name(name) + " CASCADE";
	}

	[[nodiscard]]
	virtual inline std::string sql_rename_table(const std::string& old_name, const std::string& new_name) const
	{
		return "ALTER TABLE " + this->quote_name(old_name) + " RENAME TO " + this->quote_name(new_name);
	}

	[[nodiscard]]
	virtual inline std::string sql_add_column(const TableState& table, const ColumnState& column) const
	{
		return "ALTER TABLE " + this->quote_name(table.name) + " ADD COLUMN " + this->sql_column(column);
	}

	[[nodiscard]]
	virtual inline std::string sql_drop_column(const TableState& table, const ColumnState& column) const
	{
		return "ALTER TABLE " + this->quote_name(table.name) + " DROP COLUMN " + column.name;
	}

	[[nodiscard]]
	virtual inline std::string sql_update_with_default(const TableState& table, const ColumnState& col) const
	{
		auto col_name = this->quote_name(col.name);
		return "UPDATE " + this->quote_name(table.name) + " SET " +
	       col_name + " = " + col.default_value + " WHERE " + col_name + " IS NULL";
	}

	[[nodiscard]]
	virtual inline std::string sql_alter_column(const TableState& table, const std::string& actions) const
	{
		return "ALTER TABLE " + this->quote_name(table.name) + " " + actions;
	}

	[[nodiscard]]
	virtual inline std::string sql_delete_constraint(const TableState& table, const std::string& name) const
	{
		return this->sql_alter_column(table, "DROP CONSTRAINT " + name);
	}

	// Hook to specialize column renaming for different drivers.
	[[nodiscard]]
	virtual inline std::string sql_rename_column(
		const TableState& table, const ColumnState& old_col, const ColumnState& new_col
	) const
	{
		return this->sql_alter_column(table, "RENAME COLUMN " + old_col.name + " TO " + new_col.name);
	}

	[[nodiscard]]
	virtual inline std::string sql_delete_primary_key(
		const TableState& table, const std::string& constraint_name
	) const
	{
		return this->sql_delete_constraint(table, constraint_name);
	}

	[[nodiscard]]
	virtual std::string sql_create_unique(const TableState& table, const std::list<ColumnState>& cols) const;

	// SQL partial builders.

	// Hook to specialize column type alteration for different drivers.
	[[nodiscard]]
	virtual std::tuple<std::string, std::list<std::string>> partial_sql_alter_column_type(
		const TableState& table, const ColumnState& old_col, const ColumnState& new_col
	) const;

	// Returns the SQL to use in DEFAULT clause.
	[[nodiscard]]
	virtual inline std::string partial_sql_column_default(const ColumnState& column) const
	{
		return column.default_value;
	}

	// Hook to specialize column default alteration.
	[[nodiscard]]
	virtual std::string partial_sql_alter_column_default(
		const TableState& table, const ColumnState& old_col, const ColumnState& new_col, bool drop
	) const;

	// Hook to specialize column null alteration.
	[[nodiscard]]
	virtual std::string partial_sql_alter_column_null(
		const TableState& table, const ColumnState& old_col, const ColumnState& new_col
	) const;

	// Other sql builders.

	[[nodiscard]]
	virtual std::string sql_on_action_constraint(OnAction action) const;

	[[nodiscard]]
	virtual std::string sql_column_constraints(const Constraints& constraints, const std::string& default_value) const;

	[[nodiscard]]
	virtual std::string sql_type_string(SqlColumnType type) const;

	[[nodiscard]]
	virtual std::string sql_column(const ColumnState& column) const;

	[[nodiscard]]
	virtual std::string sql_foreign_key(
		const std::string& name,
		const std::string& parent, const std::string& parent_key,
		OnAction on_delete, OnAction on_update
	) const;

	// Helpers.

	[[nodiscard]]
	inline std::string quote_name(const std::string& s) const override
	{
		return s.starts_with('"') ? s : '"' + s + '"';
	}

	// Some database drivers don't accept default values for certain
	// columns types (i.e. MySQL longtext and longblob).
	[[nodiscard]]
	virtual bool skip_default(const ColumnState& column) const
	{
		return false;
	}

	virtual void execute(const std::string& sql, const IDatabaseConnection* connection) const
	{
		if (!connection)
		{
			throw NullPointerException("SQL backend is nullptr", _ERROR_DETAILS_);
		}

		connection->run_query(sql.ends_with(';') ? sql : (sql + ";"), nullptr, nullptr);
	}

	virtual void sql_column_autoincrement_check(SqlColumnType type, bool autoincrement, bool primary_key) const;

	[[nodiscard]]
	virtual bool sql_column_max_len_check(
		const std::string& name, SqlColumnType type, const std::optional<size_t>& max_len
	) const;

	virtual void delete_primary_key(
		const TableState& table, bool strict, const IDatabaseConnection* connection
	) const;

	[[nodiscard]]
	virtual inline std::list<std::string> constraint_names(const TableState& table, bool primary_key) const
	{
		// TODO: implement -> use driver introspection
		return {};
	}

	[[nodiscard]]
	virtual bool unique_should_be_added(const ColumnState& old_col, const ColumnState& new_col) const
	{
		return (!old_col.constraints.unique && new_col.constraints.unique) ||
			(old_col.constraints.primary_key && !new_col.constraints.primary_key && new_col.constraints.unique);
	}

	[[nodiscard]]
	virtual inline std::string create_unique_name(
		const TableState& table, const std::list<ColumnState>& cols, const std::string& suffix
	) const
	{
		return str::join("_", cols.begin(), cols.end(), [](const auto& col) -> auto { return col.name; }) + suffix;
	}
};

__ORM_DB_END__
