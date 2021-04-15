/**
 * db/migration.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TODO: description
 */

#pragma once

// C++ libraries.
#include <list>
#include <memory>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../abc.h"
#include "./operations/table.h"


__DB_BEGIN__

struct col_constraints_t
{
	bool null = false;
	bool primary_key = false;
	bool unique = false;
	std::string check;
};

struct string_col_constraints_t
{
	long int max_len = -1;
	bool null = false;
	bool primary_key = false;
	bool unique = false;
	std::string check;
};

struct int_col_constraints_t
{
	bool null = false;
	bool primary_key = false;
	bool autoincrement = false;
	bool unique = false;
	std::string check;
};

struct foreign_key_constraints_t
{
	std::string to;
	std::string key;
	on_action on_delete=NO_ACTION;
	on_action on_update=NO_ACTION;
};

// TESTME: Migration
class Migration
{
protected:
	std::string identifier;

	std::list<std::shared_ptr<abc::IOperation>> operations;

	bool initial = false;

	// Whether to wrap the whole migration in a transaction.
	// Only has an effect on database drivers which support transactional DDL.
	bool atomic = true;

	// Database driver for running transactions.
	orm::abc::ISQLDriver* db;

	abc::ISchemaEditor* schema_editor;

protected:
	template <typename T>
	[[nodiscard]]
	std::string column(
		sql_column_type sql_type, const std::string& name,
		bool null=false, bool primary_key=false, bool unique=false, const std::string& check=""
	) const
	{
		return this->schema_editor->sql_data_column(
			sql_type, name, null, primary_key, unique, check
		);
	}

	template <typename T>
	[[nodiscard]]
	std::string text_column(
		sql_column_type sql_type, const std::string& name, long int max_len=-1,
		bool null=false, bool primary_key=false, bool unique=false, const std::string& check=""
	) const
	{
		return schema_editor->sql_text_column(
			sql_type, name, max_len, null, primary_key, unique, check
		);
	}

public:
	inline explicit Migration(
		orm::abc::ISQLDriver* db, std::string identifier, bool initial=false
	) : db(db), identifier(std::move(identifier)), initial(initial)
	{
		this->schema_editor = this->db->schema_editor();
	}

	bool up(abc::ISchemaEditor* schema_editor) const;

	bool down(abc::ISchemaEditor* schema_editor) const;

	[[nodiscard]]
	inline bool is_initial() const
	{
		return this->initial;
	}

	[[nodiscard]]
	inline bool is_atomic() const
	{
		return this->atomic;
	}

	// Operations.
	[[nodiscard]]
	std::shared_ptr<abc::IOperation> create_table(
		const std::string& name,
		const std::list<std::string>& columns,
		const std::list<std::string>& constraints={}
	) const
	{
		return std::make_shared<CreateTableOperation>(name, columns, constraints);
	}

	// Columns.
	[[nodiscard]]
	std::string Bool(
		const std::string& name, const col_constraints_t& constraints={}
	) const
	{
		return this->schema_editor->sql_data_column(
			BOOL_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		);
	}

	[[nodiscard]]
	std::string String(
		const std::string& name, const string_col_constraints_t& constraints={}
	) const
	{
		return this->schema_editor->sql_text_column(
			VARCHAR_T, name, constraints.max_len,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		);
	}

	[[nodiscard]]
	std::string Text(
		const std::string& name, const col_constraints_t& constraints={}
	) const
	{
		return this->schema_editor->sql_text_column(
			TEXT_T, name, -1,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		);
	}

	[[nodiscard]]
	std::string ShortInt(
		const std::string& name, const int_col_constraints_t& constraints={}
	) const
	{
		return this->schema_editor->sql_data_column(
			constraints.autoincrement ? SMALL_SERIAL_T : SMALLINT_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		);
	}

	[[nodiscard]]
	std::string Int(
		const std::string& name, const int_col_constraints_t& constraints={}
	) const
	{
		return this->schema_editor->sql_data_column(
			constraints.autoincrement ? SERIAL_T : INT_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		);
	}

	[[nodiscard]]
	std::string LongLongInt(
		const std::string& name, const int_col_constraints_t& constraints={}
	) const
	{
		return this->schema_editor->sql_data_column(
			constraints.autoincrement ? BIG_SERIAL_T : BIGINT_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		);
	}

	[[nodiscard]]
	std::string Float(
		const std::string& name, const col_constraints_t& constraints={}
	) const
	{
		return this->schema_editor->sql_data_column(
			REAL_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		);
	}

	[[nodiscard]]
	std::string Double(
		const std::string& name, const col_constraints_t& constraints={}
	) const
	{
		return this->schema_editor->sql_data_column(
			DOUBLE_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		);
	}

	// Constraints.
	[[nodiscard]]
	std::string ForeignKey(
		const std::string& name, const foreign_key_constraints_t& info
	) const
	{
		return this->schema_editor->sql_foreign_key(
			name, info.to, info.key, info.on_delete, info.on_update
		);
	}
};

__DB_END__
