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
#include <xalwart.core/utility.h>

// Orm libraries.
#include "../abc.h"
#include "../meta.h"


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

	void up(const abc::ISQLSchemaEditor* editor) const override
	{
		xw::util::require_non_null(
			editor, "xw::orm::db::ops::CreateTableOperation: schema editor is nullptr"
		)->create_table(this->table_name, this->columns_list, this->constraints_list);
	}

	void down(const abc::ISQLSchemaEditor* editor) const override
	{
		xw::util::require_non_null(
			editor, "xw::orm::db::ops::CreateTableOperation: schema editor is nullptr"
		)->drop_table(this->table_name);
	}

	template <column_migration_type_c T>
	void column(const std::string& name, const constraints_t& c={})
	{
		if (name.empty())
		{
			throw ValueError("'name' can not be empty", _ERROR_DETAILS_);
		}

		std::string default_;
		if (c.default_.has_value())
		{
			if (c.default_.type() != typeid(T))
			{
				throw TypeError(
					"type '" + xw::util::demangle(typeid(T).name()) +
					"' of default value is not the same as column type - '" +
					xw::util::demangle(c.default_.type().name()) + "'",
					_ERROR_DETAILS_
				);
			}

			default_ = field_as_column_v(std::any_cast<T>(c.default_));
		}

		auto col_type = col_t_from_type<T>::type;
		if (col_type == TEXT_T && c.max_len.has_value())
		{
			col_type = VARCHAR_T;
		}

		this->columns_list.push_back(
			xw::util::require_non_null(this->schema_editor)->sql_column(
				col_type, name,
				c.max_len, c.null, c.primary_key, c.unique,
				c.autoincrement, c.check, default_
			)
		);
	}

	void ForeignKey(const std::string& name, const foreign_key_constraints_t& c)
	{
		this->constraints_list.push_back(this->schema_editor->sql_foreign_key(
			name, c.to, c.key, c.on_delete, c.on_update
		));
	}
};

__ORM_DB_OPERATIONS_END__
