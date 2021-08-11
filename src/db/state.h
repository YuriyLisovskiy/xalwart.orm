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

// Base libraries.
#include <xalwart.base/utility.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./meta.h"


__ORM_DB_BEGIN__

enum class SqlColumnType
{
	Bool,
	VarChar, Text,
	SmallInt, Int, BigInt,
	SmallSerial, Serial, BigSerial,
	Real, Double, Date, Time, DateTime
};

enum class OnAction
{
	SetNull, SetDefault, Restrict, NoAction, Cascade
};

template <typename T>
struct ColumnTFromType { static const SqlColumnType type; };

template <>
inline const SqlColumnType ColumnTFromType<bool>::type = SqlColumnType::Bool;

template <>
inline const SqlColumnType ColumnTFromType<std::string>::type = SqlColumnType::Text;

template <>
inline const SqlColumnType ColumnTFromType<const char*>::type = SqlColumnType::Text;

template <>
inline const SqlColumnType ColumnTFromType<short int>::type = SqlColumnType::SmallInt;

template <>
inline const SqlColumnType ColumnTFromType<int>::type = SqlColumnType::Int;

template <>
inline const SqlColumnType ColumnTFromType<long int>::type = SqlColumnType::BigInt;

template <>
inline const SqlColumnType ColumnTFromType<long long int>::type = SqlColumnType::BigInt;

template <>
inline const SqlColumnType ColumnTFromType<float>::type = SqlColumnType::Real;

template <>
inline const SqlColumnType ColumnTFromType<double>::type = SqlColumnType::Double;

template <>
inline const SqlColumnType ColumnTFromType<dt::Date>::type = SqlColumnType::Date;

template <>
inline const SqlColumnType ColumnTFromType<dt::Time>::type = SqlColumnType::Time;

template <>
inline const SqlColumnType ColumnTFromType<dt::Datetime>::type = SqlColumnType::DateTime;

struct Constraints
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

struct ForeignKeyConstraints
{
	std::string to;
	std::string key;
	OnAction on_delete=OnAction::NoAction;
	OnAction on_update=OnAction::NoAction;
};

struct ColumnState
{
	SqlColumnType type;
	std::string name;
	std::string default_value;
	Constraints constraints;

	template <column_migration_type_c T>
	inline static ColumnState create(const std::string& name, const Constraints& c={})
	{
		if (name.empty())
		{
			throw ValueError("ColumnState > create: 'name' can not be empty", _ERROR_DETAILS_);
		}

		std::string default_;
		if (c.default_.has_value())
		{
			if (c.default_.type() != typeid(T))
			{
				throw TypeError(
					"ColumnState > create: type '" + xw::util::demangle(typeid(T).name()) +
					"' of default value is not the same as column type - '" +
					xw::util::demangle(c.default_.type().name()) + "'",
					_ERROR_DETAILS_
				);
			}

			default_ = field_as_column_v(std::any_cast<T>(c.default_));
		}

		auto col_type = ColumnTFromType<T>::type;
		if (col_type == SqlColumnType::Text && c.max_len.has_value())
		{
			col_type = SqlColumnType::VarChar;
		}

		return ColumnState{col_type, name, default_, c};
	}
};

struct TableState
{
	std::string name;
	std::unordered_map<std::string, ColumnState> columns{};
	std::unordered_map<std::string, ForeignKeyConstraints> foreign_keys{};

	[[nodiscard]]
	ColumnState get_column(const std::string& column_name) const
	{
		auto column = this->columns.find(column_name);
		if (column == this->columns.end())
		{
			throw KeyError(
				"TableState > get_column: column with name '" + column_name +
				"' does not exist in '" + this->name + "' table",
				_ERROR_DETAILS_
			);
		}

		return column->second;
	}

	[[nodiscard]]
	ColumnState& get_column_addr(const std::string& column_name)
	{
		auto column = this->columns.find(column_name);
		if (column == this->columns.end())
		{
			throw KeyError(
				"TableState > get_column: column with name '" + column_name +
				"' does not exist in '" + this->name + "' table",
				_ERROR_DETAILS_
			);
		}

		return column->second;
	}

	[[nodiscard]]
	const ColumnState& get_column_addr(const std::string& column_name) const
	{
		auto column = this->columns.find(column_name);
		if (column == this->columns.end())
		{
			throw KeyError(
				"TableState > get_column: column with name '" + column_name +
				"' does not exist in '" + this->name + "' table",
				_ERROR_DETAILS_
			);
		}

		return column->second;
	}
};

struct ProjectState
{
	std::map<std::string, TableState> tables{};

	[[nodiscard]]
	TableState get_table(const std::string& name) const
	{
		auto table = this->tables.find(name);
		if (table == this->tables.end())
		{
			throw KeyError(
				"ProjectState > get_table: table with name '" + name + "' does not exist", _ERROR_DETAILS_
			);
		}

		return table->second;
	}

	[[nodiscard]]
	TableState& get_table_addr(const std::string& name)
	{
		auto table = this->tables.find(name);
		if (table == this->tables.end())
		{
			throw KeyError(
				"ProjectState > get_table: table with name '" + name + "' does not exist", _ERROR_DETAILS_
			);
		}

		return table->second;
	}

	[[nodiscard]]
	const TableState& get_table_addr(const std::string& name) const
	{
		auto table = this->tables.find(name);
		if (table == this->tables.end())
		{
			throw KeyError(
				"ProjectState > get_table: table with name '" + name + "' does not exist", _ERROR_DETAILS_
			);
		}

		return table->second;
	}
};

__ORM_DB_END__
