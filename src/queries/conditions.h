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
#include "../meta.h"
#include "../utility.h"
#include "../exceptions.h"
#include "../model.h"


__Q_BEGIN__

template <typename T>
concept OperatorValueType = std::is_fundamental_v<T> ||
	std::is_same_v<T, std::string> || std::is_same_v<T, const char*>;

struct ordering final
{
private:
	std::string _table;
	std::string _column;
	bool _ascending = true;

public:
	inline ordering() = default;

	inline ordering(
		std::string table, std::string column, bool ascending
	) : _table(std::move(table)), _column(std::move(column)), _ascending(ascending)
	{
		if (this->_column.empty())
		{
			throw QueryError("ordering: 'column' is required", _ERROR_DETAILS_);
		}
	}

	inline explicit ordering(
		std::string table, std::string column
	) : _table(std::move(table)), _column(std::move(column))
	{
	}

	inline explicit operator std::string() const
	{
		auto result = util::quote_str(this->_column) + (this->_ascending ? " ASC" : " DESC");
		if (!this->_table.empty())
		{
			result = util::quote_str(this->_table) + "." + result;
		}

		return result;
	}
};

template <typename ColumnT, typename ModelT>
inline ordering asc(ColumnT ModelT::* column)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return ordering(ModelT::meta_table_name, meta::get_column_name(column), true);
}

template <typename ColumnT, typename ModelT>
inline ordering desc(ColumnT ModelT::* column)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return ordering(ModelT::meta_table_name, meta::get_column_name(column), false);
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

template <typename ModelT, OperatorValueType T>
struct comparison_op_t : public column_condition_t
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

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

template <typename ModelT>
struct comparison_op_t<ModelT, std::string> : public column_condition_t
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

public:
	inline comparison_op_t() = default;

	inline explicit comparison_op_t(
		const std::string& column_name, const std::string& op, const std::string& value
	) : column_condition_t(ModelT::meta_table_name, column_name, op + " '" + value + "'")
	{
	}
};

template <typename ModelT>
struct comparison_op_t<ModelT, const char*> : public column_condition_t
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

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
template <typename ModelT, typename ValueT>
struct column_t
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

protected:

	// Name of given column.
	std::string name;

public:
	inline explicit column_t(std::string name) : name(std::move(name))
	{
	}

	comparison_op_t<ModelT, ValueT> operator== (ValueT val)
	{
		return comparison_op_t<ModelT, ValueT>(this->name, "=", val);
	}

	comparison_op_t<ModelT, ValueT> operator!= (ValueT val)
	{
		return comparison_op_t<ModelT, ValueT>(this->name, "!=", val);
	}

	comparison_op_t<ModelT, ValueT> operator< (ValueT val)
	{
		return comparison_op_t<ModelT, ValueT>(this->name, "<", val);
	}

	comparison_op_t<ModelT, ValueT> operator> (ValueT val)
	{
		return comparison_op_t<ModelT, ValueT>(this->name, ">", val);
	}

	comparison_op_t<ModelT, ValueT> operator<= (ValueT val)
	{
		return comparison_op_t<ModelT, ValueT>(this->name, "<=", val);
	}

	comparison_op_t<ModelT, ValueT> operator>= (ValueT val)
	{
		return comparison_op_t<ModelT, ValueT>(this->name, ">=", val);
	}
};

template <typename ColumnT, typename ModelT>
inline column_t<ModelT, ColumnT> c(ColumnT ModelT::* member_pointer)
{
	return column_t<ModelT, ColumnT>(meta::get_column_name(member_pointer));
}

template <typename ColumnT, typename ModelT>
inline column_condition_t is_null(ColumnT ModelT::* member_pointer)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return column_condition_t(ModelT::meta_table_name, meta::get_column_name(member_pointer), "IS NULL");
}

template <typename ColumnT, typename ModelT>
inline column_condition_t is_not_null(ColumnT ModelT::* member_pointer)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return column_condition_t(ModelT::meta_table_name, meta::get_column_name(member_pointer), "IS NOT NULL");
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

template <typename ColumnT, typename ModelT>
inline column_condition_t between(ColumnT ModelT::* column, ColumnT lower, ColumnT upper)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return column_condition_t(
		ModelT::meta_table_name, meta::get_column_name(column),
		"BETWEEN " + std::to_string(lower) + " AND " + std::to_string(upper)
	);
}

template <typename ModelT>
inline column_condition_t between(
	std::string ModelT::* column, const std::string& lower, const std::string& upper
)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return column_condition_t(
		ModelT::meta_table_name, meta::get_column_name(column),
		"BETWEEN '" + lower + "' AND '" + upper + '\''
	);
}

template <typename ModelT>
inline column_condition_t between(
	const char* ModelT::* column, const char* lower, const char* upper
)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return column_condition_t(
		ModelT::meta_table_name, meta::get_column_name(column),
		"BETWEEN '" + std::string(lower) + "' AND '" + std::string(upper) + '\''
	);
}

template <typename ColumnT, typename ModelT>
inline column_condition_t like(ColumnT ModelT::* column, const std::string& pattern)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return column_condition_t(
		ModelT::meta_table_name, meta::get_column_name(column), "LIKE '" + pattern + "'"
	);
}

template <typename ColumnT, typename ModelT>
inline column_condition_t like(
	ColumnT ModelT::* column, const std::string& pattern, const std::string& escape
)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	return column_condition_t(
		ModelT::meta_table_name, meta::get_column_name(column),
		"LIKE '" + pattern + "' ESCAPE '" + escape + "'"
	);
}

template <types::fundamental_type ColumnT, typename IteratorT, typename ModelT>
inline column_condition_t in(ColumnT ModelT::* column, IteratorT begin, IteratorT end)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	if (begin == end)
	{
		throw QueryError("in: list is empty", _ERROR_DETAILS_);
	}

	std::string condition = "IN (" + str::join(
		begin, end, ", ", [](const ColumnT& item) -> std::string { return std::to_string(item); }
	) + ")";
	return column_condition_t(ModelT::meta_table_name, meta::get_column_name(column), condition);
}

template <typename ColumnT, types::fundamental_type RangeValueT, typename ModelT>
inline column_condition_t in(ColumnT ModelT::* column, const std::initializer_list<RangeValueT>& values)
{
	return in<ColumnT, typename std::initializer_list<RangeValueT>::const_iterator, ModelT>(
		column, values.begin(), values.end()
	);
}

template <typename ColumnT, StringIterType IteratorT, typename ModelT>
inline column_condition_t in(ColumnT ModelT::* column, IteratorT begin, IteratorT end)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

	if (begin == end)
	{
		throw QueryError("in: list is empty", _ERROR_DETAILS_);
	}

	std::string condition = "IN (" + str::join(
		begin, end, ", ", [](const ColumnT& item) -> std::string {
			if constexpr (std::is_same_v<ColumnT, const char*>)
			{
				return "'" + std::string(item) + "'";
			}
			else
			{
				return "'" + item + "'";
			}
		}
	) + ")";
	return column_condition_t(ModelT::meta_table_name, meta::get_column_name(column), condition);
}

template <typename ModelT>
inline column_condition_t in(std::string ModelT::* column, const std::initializer_list<std::string>& values)
{
	return in<std::string, std::initializer_list<std::string>::const_iterator, ModelT>(
		column, values.begin(), values.end()
	);
}

template <typename ModelT>
inline column_condition_t in(const char* ModelT::* column, const std::initializer_list<const char*>& values)
{
	return in<const char*, std::initializer_list<const char*>::const_iterator, ModelT>(
		column, values.begin(), values.end()
	);
}

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

template <typename LeftT, typename RightT>
inline join_t join_on(
	const std::string& type, const std::string& fk_to_left="", const q::condition_t& extra_condition={}
)
{
	static_assert(LeftT::meta_table_name != nullptr, "'meta_table_name' of left model is not initialized");
	static_assert(RightT::meta_table_name != nullptr, "'meta_table_name' of right model is not initialized");

	std::string left_table_name = LeftT::meta_table_name;
	const auto& table_name = RightT::meta_table_name;
	auto fk = fk_to_left.empty() ? meta::make_fk<LeftT>() : fk_to_left;
	auto left_pk = meta::get_pk_name<LeftT>();
	if (left_pk.empty())
	{
		throw QueryError("join_on: model requires pk column", _ERROR_DETAILS_);
	}

	auto condition_str = util::quote_str(left_table_name) + "." + util::quote_str(left_pk)
		+ " = " +
		util::quote_str(table_name) + "." + util::quote_str(fk);
	auto extra_cond_str = (std::string)extra_condition;
	if (!extra_cond_str.empty())
	{
		condition_str += " AND (" + extra_cond_str + ")";
	}

	return join_t(type, table_name, q::condition_t(condition_str));
}

template <typename LeftT, typename RightT>
inline join_t inner_on(const std::string& fk_to_left="", const q::condition_t& extra_condition={})
{
	return join_on<LeftT, RightT>("INNER", fk_to_left, extra_condition);
}

template <typename LeftT, typename RightT>
inline join_t left_on(const std::string& fk_to_left="", const q::condition_t& extra_condition={})
{
	return join_on<LeftT, RightT>("LEFT", fk_to_left, extra_condition);
}

template <typename LeftT, typename RightT>
inline join_t cross_on(const std::string& fk_to_left="", const q::condition_t& extra_condition={})
{
	return join_on<LeftT, RightT>("CROSS", fk_to_left, extra_condition);
}

__Q_END__
