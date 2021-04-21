/**
 * db/operations/table.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * TODO: description
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Core libraries.
#include <xalwart.core/exceptions.h>

// Orm libraries.
#include "../abc.h"


__ORM_DB_OPERATIONS_BEGIN__

// TESTME: TableOperation
// Base class for migration operations.
class TableOperation : public abc::IOperation
{
protected:
	std::string table_name;

public:
	TableOperation() = default;

	inline explicit TableOperation(std::string name) : table_name(std::move(name))
	{
	}

	[[nodiscard]]
	inline const std::string& name() const
	{
		return this->table_name;
	}
};

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

// TESTME: CreateTableOperation
// Create a model's table.
class CreateTableOperation : public TableOperation
{
protected:
	abc::ISQLSchemaEditor* schema_editor;

	std::list<std::string> columns_list;
	std::list<std::string> constraints_list;

public:
	inline explicit CreateTableOperation(
		const std::string& name,
		abc::ISQLSchemaEditor* schema_editor
	) : TableOperation(name), schema_editor(schema_editor)
	{
	}

	void up(abc::ISQLSchemaEditor* editor) const override
	{
		if (!editor)
		{
			throw NullPointerException(
				"xw::orm::db::CreateModel: schema editor is nullptr",
				_ERROR_DETAILS_
			);
		}

		editor->create_table(this->table_name, this->columns_list, this->constraints_list);
	}

	void down(abc::ISQLSchemaEditor* editor) const override
	{
		if (!editor)
		{
			throw NullPointerException(
				"xw::orm::db::CreateModel: schema editor is nullptr",
				_ERROR_DETAILS_
			);
		}

		editor->drop_table(this->table_name);
	}

	// Columns.
	void Bool(const std::string& name, const col_constraints_t& constraints={})
	{
		this->columns_list.push_back(this->schema_editor->sql_data_column(
			BOOL_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		));
	}

	void String(const std::string& name, const string_col_constraints_t& constraints={})
	{
		this->columns_list.push_back(this->schema_editor->sql_text_column(
			VARCHAR_T, name, constraints.max_len,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		));
	}

	void Text(const std::string& name, const col_constraints_t& constraints={})
	{
		this->columns_list.push_back(this->schema_editor->sql_text_column(
			TEXT_T, name, -1,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		));
	}

	void ShortInt(const std::string& name, const int_col_constraints_t& constraints={})
	{
		this->columns_list.push_back(this->schema_editor->sql_data_column(
			constraints.autoincrement ? SMALL_SERIAL_T : SMALLINT_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		));
	}

	void Int(const std::string& name, const int_col_constraints_t& constraints={})
	{
		this->columns_list.push_back(this->schema_editor->sql_data_column(
			constraints.autoincrement ? SERIAL_T : INT_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		));
	}

	void LongLongInt(const std::string& name, const int_col_constraints_t& constraints={})
	{
		this->columns_list.push_back(this->schema_editor->sql_data_column(
			constraints.autoincrement ? BIG_SERIAL_T : BIGINT_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		));
	}

	void Float(const std::string& name, const col_constraints_t& constraints={})
	{
		this->columns_list.push_back(this->schema_editor->sql_data_column(
			REAL_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		));
	}

	void Double(const std::string& name, const col_constraints_t& constraints={})
	{
		this->columns_list.push_back(this->schema_editor->sql_data_column(
			DOUBLE_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		));
	}

	void Date(const std::string& name, const col_constraints_t& constraints={})
	{
		this->columns_list.push_back(this->schema_editor->sql_data_column(
			DATE_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		));
	}

	void Time(const std::string& name, const col_constraints_t& constraints={})
	{
		this->columns_list.push_back(this->schema_editor->sql_data_column(
			TIME_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		));
	}

	void Datetime(const std::string& name, const col_constraints_t& constraints={})
	{
		this->columns_list.push_back(this->schema_editor->sql_data_column(
			DATETIME_T, name,
			constraints.null, constraints.primary_key, constraints.unique, constraints.check
		));
	}

	// Constraints.
	void ForeignKey(
		const std::string& name, const foreign_key_constraints_t& info
	)
	{
		this->constraints_list.push_back(this->schema_editor->sql_foreign_key(
			name, info.to, info.key, info.on_delete, info.on_update
		));
	}
};

__ORM_DB_OPERATIONS_END__
