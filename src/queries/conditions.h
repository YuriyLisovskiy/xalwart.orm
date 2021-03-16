/**
 * query/operations.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: set of SQL arithmetic and logical operations.
 */

#pragma once

// C++ libraries.
#include <string>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../utility.h"
#include "../exceptions.h"
#include "../model.h"


__Q_BEGIN__

template <typename T>
concept OperatorValueType = std::is_fundamental_v<T> ||
	std::is_same_v<T, std::string> || std::is_same_v<T, const char*>;

struct ordering
{
public:
	std::string column;
	bool ascending = true;

public:
	inline ordering() = default;

	inline ordering(
		std::string column, bool ascending
	) : column(std::move(column)), ascending(ascending)
	{
	}

	inline explicit ordering(std::string column) : column(std::move(column))
	{
	}

	inline explicit ordering(const char* column) : ordering(std::string(column))
	{
	}

	virtual inline explicit operator std::string() const
	{
		return util::quote_str(this->column) + (this->ascending ? " ASC" : " DESC");
	}
};

inline ordering ascending(const std::string& column)
{
	return ordering(column, true);
}

inline ordering descending(const std::string& column)
{
	return ordering(column, false);
}

struct condition_t
{
public:

	// Condition string, example `"id" = 1`.
	std::string raw_condition;

public:
	inline condition_t() = default;

	inline explicit condition_t(std::string condition) : raw_condition(std::move(condition))
	{
	}

	inline explicit operator std::string() const
	{
		return this->raw_condition;
	};
};

struct column_condition_t : public condition_t
{
public:
	inline column_condition_t() = default;

	inline column_condition_t(
		const std::string& table_name,
		const std::string& column_name,
		const std::string& condition
	) : condition_t("")
	{
		if (column_name.empty())
		{
			throw QueryError("'column_name' is empty", _ERROR_DETAILS_);
		}

		this->raw_condition = util::quote_str(column_name) + " " + condition;
		if (!table_name.empty())
		{
			this->raw_condition = util::quote_str(table_name) + "." + this->raw_condition;
		}
	}
};

template <ModelBasedType ModelT, OperatorValueType T>
struct comparison_op_t : public column_condition_t
{
public:
	inline comparison_op_t() = default;

	inline explicit comparison_op_t(
		const std::string& column_name, const std::string& op, T value
	) : column_condition_t(
		ModelT::meta_table_name, column_name, op + " " + std::to_string(value)
	)
	{
	}
};

template <ModelBasedType ModelT>
struct comparison_op_t<ModelT, std::string> : public column_condition_t
{
public:
	inline comparison_op_t() = default;

	inline explicit comparison_op_t(
		const std::string& column_name, const std::string& op, const std::string& value
	) : column_condition_t(ModelT::meta_table_name, column_name, op + " '" + value + "'")
	{
	}
};

template <ModelBasedType ModelT>
struct comparison_op_t<ModelT, const char*> : public column_condition_t
{
public:
	inline comparison_op_t() = default;

	inline explicit comparison_op_t(
		const std::string& column_name, const std::string& op, const char* value
	) : column_condition_t(
		ModelT::meta_table_name, column_name, op + " '" + std::string(value) + "'"
	)
	{
	}
};

// SQL comparison operators for columns.
template <ModelBasedType ModelT>
struct c
{
	std::string column;

	inline explicit c(std::string column) : column(std::move(column))
	{
	}

	template <OperatorValueType T>
	comparison_op_t<ModelT, T> operator== (T val)
	{
		return comparison_op_t<ModelT, T>(this->column, "=", val);
	}

	template <OperatorValueType T>
	comparison_op_t<ModelT, T> operator!= (T val)
	{
		return comparison_op_t<ModelT, T>(this->column, "!=", val);
	}

	template <OperatorValueType T>
	comparison_op_t<ModelT, T> operator< (T val)
	{
		return comparison_op_t<ModelT, T>(this->column, "<", val);
	}

	template <OperatorValueType T>
	comparison_op_t<ModelT, T> operator> (T val)
	{
		return comparison_op_t<ModelT, T>(this->column, ">", val);
	}

	template <OperatorValueType T>
	comparison_op_t<ModelT, T> operator<= (T val)
	{
		return comparison_op_t<ModelT, T>(this->column, "<=", val);
	}

	template <OperatorValueType T>
	comparison_op_t<ModelT, T> operator>= (T val)
	{
		return comparison_op_t<ModelT, T>(this->column, ">=", val);
	}
};

template <ModelBasedType ModelT>
inline column_condition_t is_null(const std::string& column)
{
	return column_condition_t(ModelT::meta_table_name, column, "IS NULL");
}

template <ModelBasedType ModelT>
inline column_condition_t is_not_null(const std::string& column)
{
	return column_condition_t(ModelT::meta_table_name, column, "IS NOT NULL");
}

// SQL logical operators.
inline condition_t operator& (const condition_t& left, const condition_t& right)
{
	return condition_t(
		"(" + (std::string)left + " AND " + (std::string)right + ")"
	);
}

inline condition_t operator| (const condition_t& left, const condition_t& right)
{
	return condition_t(
		"(" + (std::string)left + " OR " + (std::string)right + ")"
	);
}

inline condition_t operator~ (const condition_t& cond)
{
	return condition_t("NOT (" + (std::string)cond + ")");
}

template <ModelBasedType ModelT, types::fundamental_type T>
inline column_condition_t between(const std::string& column, T lower, T upper)
{
	return column_condition_t(
		ModelT::meta_table_name, column,
		"BETWEEN " + std::to_string(lower) + " AND " + std::to_string(upper)
	);
}

template <ModelBasedType ModelT>
inline column_condition_t between(
	const std::string& column, std::string lower, std::string upper
)
{
	return column_condition_t(
		ModelT::meta_table_name, column,
		"BETWEEN '" + std::move(lower) + "' AND '" + std::move(upper) + '\''
	);
}

template <ModelBasedType ModelT>
inline column_condition_t between(
	const std::string& column, const char* lower, const char* upper
)
{
	return column_condition_t(
		ModelT::meta_table_name, column,
		"BETWEEN '" + std::string(lower) + "' AND '" + std::string(upper) + '\''
	);
}

template <ModelBasedType ModelT>
inline column_condition_t like(const std::string& column, const std::string& pattern)
{
	return column_condition_t(ModelT::meta_table_name, column, "LIKE '" + pattern + "'");
}

template <ModelBasedType ModelT>
inline column_condition_t like(
	const std::string& column, const std::string& pattern, const std::string& escape
)
{
	return column_condition_t(
		ModelT::meta_table_name, column, "LIKE '" + pattern + "' ESCAPE '" + escape + "'"
	);
}

// TESTME: in (fundamental)
template <ModelBasedType ModelT, FundamentalIterType IteratorT>
inline column_condition_t in(const std::string& column, IteratorT begin, IteratorT end)
{
	if (begin == end)
	{
		throw QueryError("in: list is empty", _ERROR_DETAILS_);
	}

	std::string condition = " IN (" + str::join(begin, end, ", ", [](
		const typename std::iterator_traits<IteratorT>::value_type& item
	) -> std::string { return std::to_string(item); }) + ")";
	return column_condition_t(ModelT::meta_table_name, column, condition);
}

// TESTME: in (fundamental, initializer list)
template <ModelBasedType ModelT, types::fundamental_type RangeValueT>
inline column_condition_t in(const std::string& column, const std::initializer_list<RangeValueT>& values)
{
	return in<ModelT, std::initializer_list<RangeValueT>::const_iterator>(column, values.begin(), values.end());
}

// TESTME: in (string)
template <ModelBasedType ModelT, StringIterType IteratorT>
inline column_condition_t in(const std::string& column, IteratorT begin, IteratorT end)
{
	if (begin == end)
	{
		throw QueryError("in: list is empty", _ERROR_DETAILS_);
	}

	std::string condition = " IN (" + str::join(begin, end, ", ", [](
		const typename std::iterator_traits<IteratorT>::value_type& item
	) -> std::string { return item; }) + ")";
	return column_condition_t(ModelT::meta_table_name, column, condition);
}

// TESTME: in (string, initializer list)
template <ModelBasedType ModelT>
inline column_condition_t in(const std::string& column, const std::initializer_list<std::string>& values)
{
	return in<ModelT, std::initializer_list<std::string>::const_iterator>(column, values.begin(), values.end());
}

// TESTME: in (const char*, initializer list)
template <ModelBasedType ModelT>
inline column_condition_t in(const std::string& column, const std::initializer_list<const char*>& values)
{
	return in<ModelT, std::initializer_list<const char*>::const_iterator>(column, values.begin(), values.end());
}

// TODO: implement ALL, ANY, and EXISTS operators.

// TESTME: join_t
struct join_t
{
	std::string type;
	std::string table_name;
	q::condition_t condition;

	join_t(
		std::string type, std::string table_name, q::condition_t condition
	) : type(std::move(type)), table_name(std::move(table_name)), condition(std::move(condition))
	{
	}

	inline explicit operator std::string() const
	{
		return type + " JOIN \"" + table_name + "\" ON " + (std::string)condition;
	}
};

// TESTME: join
template <typename LeftT, typename RightT>
inline join_t join(
	const std::string& type, const std::string& join_pk, const q::condition_t& extra_condition={}
)
{
	std::string left_table_name = LeftT::meta_table_name;
	const auto& table_name = RightT::meta_table_name;
	auto condition_str = util::quote_str(left_table_name) + "." + util::quote_str(LeftT::meta_pk_name)
		+ " = " +
		util::quote_str(table_name) + "." + util::quote_str(join_pk);
	auto extra_cond_str = (std::string)extra_condition;
	if (!extra_cond_str.empty())
	{
		condition_str += " AND (" + extra_cond_str + ")";
	}

	return {type, table_name, q::condition_t(condition_str)};
}

// TESTME: inner (join)
template <typename LeftT, typename RightT>
inline join_t inner_on(const std::string& join_pk, const q::condition_t& extra_condition={})
{
	return join<LeftT, RightT>("INNER", join_pk, extra_condition);
}

// TESTME: left (join)
template <typename LeftT, typename RightT>
inline join_t left_on(const std::string& join_pk, const q::condition_t& extra_condition={})
{
	return join<LeftT, RightT>("LEFT", join_pk, extra_condition);
}

// TESTME: cross (join)
template <typename LeftT, typename RightT>
inline join_t cross_on(const std::string& join_pk, const q::condition_t& extra_condition={})
{
	return join<LeftT, RightT>("CROSS", join_pk, extra_condition);
}

__Q_END__
