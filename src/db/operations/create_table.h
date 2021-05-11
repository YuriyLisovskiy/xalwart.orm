/**
 * db/operations/create_table.h
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
#include "./base.h"
#include "../state.h"


__ORM_DB_OPERATIONS_BEGIN__

// TESTME: CreateTable
// Creates a table in the database.
class CreateTable : public TableOperation
{
protected:
	std::map<std::string, std::tuple<sql_column_type, std::string, constraints_t>> columns{};
	std::map<std::string, foreign_key_constraints_t> foreign_keys{};

public:
	inline explicit CreateTable(const std::string& name) : TableOperation(name)
	{
	}

	inline void update_state(project_state& state) const override
	{
		state.tables[this->name_lower()] = table_state{this->columns, this->foreign_keys};
	}

	void forward(
		const abc::ISQLSchemaEditor* editor,
		const project_state& from_state, const project_state& to_state
	) const override;

	inline void backward(
		const abc::ISQLSchemaEditor* editor,
		const project_state& from_state, const project_state& to_state
	) const override
	{
		xw::util::require_non_null(
			editor, "CreateTable > backward: schema editor is nullptr"
		)->drop_table(this->table_name);
	}

	template <column_migration_type_c T>
	inline void column(const std::string& name, const constraints_t& c={})
	{
		if (name.empty())
		{
			throw ValueError(
				"CreateTable > backward: 'name' can not be empty", _ERROR_DETAILS_
			);
		}

		std::string default_;
		if (c.default_.has_value())
		{
			if (c.default_.type() != typeid(T))
			{
				throw TypeError(
					"CreateTable > backward: type '" + xw::util::demangle(typeid(T).name()) +
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

		this->columns[name] = std::make_tuple(col_type, default_, c);
	}

	inline void ForeignKey(const std::string& name, const foreign_key_constraints_t& c)
	{
		this->foreign_keys[name] = c;
	}
};

__ORM_DB_OPERATIONS_END__
