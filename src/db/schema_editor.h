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
#include "./abc.h"
#include "../abc.h"


__ORM_DB_BEGIN__

// TESTME: DefaultSchemaEditor
class DefaultSQLSchemaEditor : public abc::ISchemaEditor
{
protected:
	orm::abc::ISQLDriver* db;

protected:

	// SQL builders.

	[[nodiscard]]
	virtual inline std::string sql_create_table(
		const table_state& table,
		const std::list<column_state>& columns,
		const std::list<std::tuple<std::string, foreign_key_constraints_t>>& fks
	) const
	{
		auto s_columns = str::join(
			", ", columns.begin(), columns.end(),
			[this](const auto& col) -> auto { return this->sql_column(col); }
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
		return "CREATE TABLE " + table.name + "(" + s_columns +
			(s_constraints.empty() ? "" : ", " + s_constraints) + ")";
	}

	[[nodiscard]]
	virtual inline std::string sql_drop_table(const std::string& name) const
	{
		return "DROP TABLE " + name + " CASCADE";
	}

	[[nodiscard]]
	virtual inline std::string sql_add_column(
		const table_state& table, const column_state& column
	) const
	{
		return "ALTER TABLE " + this->quote_name(table.name) + " ADD COLUMN " + this->sql_column(column);
	}

	[[nodiscard]]
	virtual inline std::string sql_drop_column(
		const table_state& table, const column_state& column
	) const
	{
		return "ALTER TABLE " + this->quote_name(table.name) + " DROP COLUMN " + column.name;
	}

	[[nodiscard]]
	virtual inline std::string sql_update_with_default(
		const table_state& table, const column_state& col
	) const
	{
		auto col_name = this->quote_name(col.name);
		return "UPDATE " + this->quote_name(table.name) + " SET " +
	       col_name + " = " + col.default_value + " WHERE " + col_name + " IS NULL";
	}

	[[nodiscard]]
	virtual inline std::string sql_alter_column(
		const table_state& table, const std::string& actions
	) const
	{
		return "ALTER TABLE " + this->quote_name(table.name) + " " + actions;
	}

	[[nodiscard]]
	virtual inline std::string sql_delete_constraint(
		const table_state& table, const std::string& name
	) const
	{
		return this->sql_alter_column(table, "DROP CONSTRAINT " + name);
	}

	// Hook to specialize column renaming for different drivers.
	[[nodiscard]]
	virtual inline std::string sql_rename_column(
		const table_state& table, const column_state& old_col, const column_state& new_col
	) const
	{
		return this->sql_alter_column(
			table, "RENAME COLUMN " + old_col.name + " TO " + new_col.name
		);
	}

	[[nodiscard]]
	virtual inline std::string sql_delete_primary_key(
		const table_state& table, const std::string& constraint_name
	) const
	{
		return this->sql_delete_constraint(table, constraint_name);
	}

	[[nodiscard]]
	virtual inline std::string sql_create_unique(
		const table_state& table, const std::list<column_state>& cols
	) const
	{
		auto str_columns = str::join(", ", cols.begin(), cols.end(),
			[](const auto& col) -> auto { return col.name; }
		);
		return this->sql_alter_column(
			table,
			"ADD CONSTRAINT " + this->create_unique_name(table, cols, "_unique") +
			" UNIQUE (" + str_columns + ")");
	}

	// SQL partial builders.

	// Hook to specialize column type alteration for different drivers.
	[[nodiscard]]
	virtual inline std::tuple<std::string, std::list<std::string>> partial_sql_alter_column_type(
		const table_state& table, const column_state& old_col, const column_state& new_col
	) const
	{
		return std::make_tuple<std::string, std::list<std::string>>(
			"ALTER COLUMN " + this->quote_name(new_col.name) + " TYPE " + this->sql_type_string(new_col.type), {}
		);
	}

	// Returns the SQL to use in DEFAULT clause.
	[[nodiscard]]
	virtual inline std::string partial_sql_column_default(const column_state& column) const
	{
		return column.default_value;
	}

	// Hook to specialize column default alteration.
	[[nodiscard]]
	virtual inline std::string partial_sql_alter_column_default(
		const table_state& table,
		const column_state& old_col, const column_state& new_col,
		bool drop
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

	// Hook to specialize column null alteration.
	[[nodiscard]]
	virtual inline std::string partial_sql_alter_column_null(
		const table_state& table, const column_state& old_col, const column_state& new_col
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

	// Other sql builders.

	[[nodiscard]]
	virtual std::string sql_on_action_constraint(on_action action) const;

	[[nodiscard]]
	virtual std::string sql_column_constraints(
		const constraints_t& constraints, const std::string& default_value
	) const;

	[[nodiscard]]
	virtual std::string sql_type_string(sql_column_type type) const;

	[[nodiscard]]
	virtual std::string sql_column(const column_state& column) const;

	[[nodiscard]]
	virtual std::string sql_foreign_key(
		const std::string& name,
		const std::string& parent, const std::string& parent_key,
		on_action on_delete, on_action on_update
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
	virtual bool skip_default(const column_state& column) const
	{
		return false;
	}

	virtual void execute(const std::string& sql) const
	{
		this->db->run_query(sql.ends_with(';') ? sql : (sql + ";"));
	}

	virtual void sql_column_autoincrement_check(
		sql_column_type type, bool autoincrement, bool primary_key
	) const;

	[[nodiscard]]
	virtual bool sql_column_max_len_check(
		const std::string& name, sql_column_type type,
		const std::optional<size_t>& max_len
	) const;

	virtual inline void delete_primary_key(const table_state& table, bool strict) const
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

	[[nodiscard]]
	virtual inline std::list<std::string> constraint_names(
		const table_state& table, bool primary_key
	) const
	{
		// TODO: implement -> use driver introspection
		return {};
	}

	[[nodiscard]]
	virtual bool unique_should_be_added(const column_state& old_col, const column_state& new_col) const
	{
		return (!old_col.constraints.unique && new_col.constraints.unique) ||
			(old_col.constraints.primary_key && !new_col.constraints.primary_key && new_col.constraints.unique);
	}

	[[nodiscard]]
	virtual inline std::string create_unique_name(
		const table_state& table, const std::list<column_state>& cols, const std::string& suffix
	) const
	{
		return str::join("_", cols.begin(), cols.end(),
			[](const auto& col) -> auto { return col.name; }
		) + suffix;
	}

public:
	inline explicit DefaultSQLSchemaEditor(orm::abc::ISQLDriver* db) : db(db)
	{
		if (!this->db)
		{
			throw NullPointerException(
				"DefaultSQLSchemaEditor: database driver is nullptr", _ERROR_DETAILS_
			);
		}
	}

	inline void create_table(const table_state& table) const override
	{
		std::list<column_state> columns;
		for (const auto& col : table.columns)
		{
			columns.push_back(col.second);
		}

		std::list<std::tuple<std::string, foreign_key_constraints_t>> foreign_keys;
		for (const auto& fk : table.foreign_keys)
		{
			foreign_keys.emplace_back(fk.first, fk.second);
		}

		this->execute(this->sql_create_table(table, columns, foreign_keys));
	}

	inline void drop_table(const std::string& name) const override
	{
		this->execute(this->sql_drop_table(name));
	}

	inline void create_column(
		const table_state& table, const column_state& column
	) const override
	{
		this->execute(this->sql_add_column(table, column));
	}

	inline void drop_column(
		const table_state& table, const column_state& column
	) const override
	{
		this->execute(this->sql_drop_column(table, column));
	}

	inline void alter_column(
		const table_state& table,
		const column_state& old_column, const column_state& new_column, bool strict
	) const override
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
				actions.push_back(
					this->partial_sql_alter_column_default(table, old_column, new_column, false)
				);
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

		// TODO: implement logic for adding index.

//		if (needs_db_default)
//		{
//			auto partial_sql = this->partial_sql_alter_column_default(table, old_column, new_column, true);
//			this->execute(this->sql_alter_column(table, partial_sql));
//		}
	}
};

__ORM_DB_END__
