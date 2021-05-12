/**
 * db/state.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <string>
#include <map>
#include <any>
#include <optional>

// Core libraries.
#include <xalwart.core/utility.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./meta.h"


__ORM_DB_BEGIN__

enum sql_column_type
{
	BOOL_T,
	VARCHAR_T, TEXT_T,
	SMALLINT_T, INT_T, BIGINT_T,
	SMALL_SERIAL_T, SERIAL_T, BIG_SERIAL_T,
	REAL_T, DOUBLE_T, DATE_T, TIME_T, DATETIME_T
};

enum on_action
{
	SET_NULL, SET_DEFAULT, RESTRICT, NO_ACTION, CASCADE
};

template <typename T>
struct col_t_from_type { static const sql_column_type type; };

//template <typename T>
//const sql_column_type col_t_from_type<T>::type = BOOL_T;

template <>
inline const sql_column_type col_t_from_type<bool>::type = BOOL_T;

template <>
inline const sql_column_type col_t_from_type<std::string>::type = TEXT_T;

template <>
inline const sql_column_type col_t_from_type<const char*>::type = TEXT_T;

template <>
inline const sql_column_type col_t_from_type<short int>::type = SMALLINT_T;

template <>
inline const sql_column_type col_t_from_type<int>::type = INT_T;

template <>
inline const sql_column_type col_t_from_type<long int>::type = BIGINT_T;

template <>
inline const sql_column_type col_t_from_type<long long int>::type = BIGINT_T;

template <>
inline const sql_column_type col_t_from_type<float>::type = REAL_T;

template <>
inline const sql_column_type col_t_from_type<double>::type = DOUBLE_T;

template <>
inline const sql_column_type col_t_from_type<dt::Date>::type = DATE_T;

template <>
inline const sql_column_type col_t_from_type<dt::Time>::type = TIME_T;

template <>
inline const sql_column_type col_t_from_type<dt::Datetime>::type = DATETIME_T;

struct constraints_t
{
	std::optional<size_t> max_len;
	std::optional<bool> null;
	bool primary_key;
	bool unique;
	bool autoincrement;
	std::string check;
	std::any default_;
};

template <typename T>
concept column_migration_type_c = std::is_same_v<T, bool> ||
                                  std::is_same_v<T, bool> ||
                                  std::is_same_v<T, std::string> ||
                                  std::is_same_v<T, const char*> ||
                                  std::is_same_v<T, short int> ||
                                  std::is_same_v<T, int> ||
                                  std::is_same_v<T, long int> ||
                                  std::is_same_v<T, long long int> ||
                                  std::is_same_v<T, float> ||
                                  std::is_same_v<T, double> ||
                                  std::is_same_v<T, dt::Date> ||
                                  std::is_same_v<T, dt::Time> ||
                                  std::is_same_v<T, dt::Datetime>;

struct foreign_key_constraints_t
{
	std::string to;
	std::string key;
	on_action on_delete=NO_ACTION;
	on_action on_update=NO_ACTION;
};

struct column_state
{
	sql_column_type type;
	std::string name;
	std::string default_value;
	constraints_t constraints;

	template <column_migration_type_c T>
	inline static column_state create(const std::string& name, const constraints_t& c={})
	{
		if (name.empty())
		{
			throw ValueError(
				"column_state > create: 'name' can not be empty", _ERROR_DETAILS_
			);
		}

		std::string default_;
		if (c.default_.has_value())
		{
			if (c.default_.type() != typeid(T))
			{
				throw TypeError(
					"column_state > create: type '" + xw::util::demangle(typeid(T).name()) +
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

		return column_state{col_type, name, default_, c};
	}
};

struct table_state
{
	std::string name;
	std::unordered_map<std::string, column_state> columns{};
	std::unordered_map<std::string, foreign_key_constraints_t> foreign_keys{};

	[[nodiscard]]
	column_state get_column(const std::string& column_name) const
	{
		auto column = this->columns.find(column_name);
		if (column == this->columns.end())
		{
			throw KeyError(
				"table_state > get_column: column with name '" + column_name +
				"' does not exist in '" + this->name + "' table",
				_ERROR_DETAILS_
			);
		}

		return column->second;
	}

	[[nodiscard]]
	column_state& get_column_addr(const std::string& column_name)
	{
		auto column = this->columns.find(column_name);
		if (column == this->columns.end())
		{
			throw KeyError(
				"table_state > get_column: column with name '" + column_name +
				"' does not exist in '" + this->name + "' table",
				_ERROR_DETAILS_
			);
		}

		return column->second;
	}
};

struct project_state
{
	std::map<std::string, table_state> tables{};

	[[nodiscard]]
	table_state get_table(const std::string& name) const
	{
		auto table = this->tables.find(name);
		if (table == this->tables.end())
		{
			throw KeyError(
				"project_state > get_table: table with name '" + name + "' does not exist",
				_ERROR_DETAILS_
			);
		}

		return table->second;
	}

	[[nodiscard]]
	table_state& get_table_addr(const std::string& name)
	{
		auto table = this->tables.find(name);
		if (table == this->tables.end())
		{
			throw KeyError(
				"project_state > get_table: table with name '" + name + "' does not exist",
				_ERROR_DETAILS_
			);
		}

		return table->second;
	}
};

__ORM_DB_END__
