/**
 * db/abc.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Abstract base classes for `db` module.
 */

#pragma once

// C++ libraries.
#include <list>
#include <any>
#include <optional>

// Core libraries.
#include <xalwart.core/datetime.h>

// Module definitions.
#include "./_def_.h"


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

__ORM_DB_END__


__ORM_DB_ABC_BEGIN__

class ISQLSchemaEditor
{
public:
	virtual std::string sql_type_to_string(sql_column_type type) const = 0;

	// SQL builders.
	[[nodiscard]]
	virtual std::string sql_column(
		sql_column_type type, const std::string& name,
		const std::optional<size_t>& max_len,
		const std::optional<bool>& null,
		bool primary_key,
		bool unique,
		bool autoincrement,
		const std::string& check,
		const std::string& default_
	) const = 0;

	[[nodiscard]]
	virtual std::string sql_foreign_key(
		const std::string& name,
		const std::string& parent, const std::string& parent_key,
		on_action on_delete, on_action on_update
	) const = 0;

	// SQL executors.
	virtual void create_table(
		const std::string& name,
		const std::list<std::string>& columns,
		const std::list<std::string>& constraints
	) const = 0;

	virtual void drop_table(const std::string& name) const = 0;
};

class IOperation
{
public:
	virtual void up(const ISQLSchemaEditor* editor) const = 0;

	virtual void down(const ISQLSchemaEditor* editor) const = 0;
};

__ORM_DB_ABC_END__
