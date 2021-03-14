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
protected:
	std::string str;

public:
	inline condition_t() = default;

	inline explicit condition_t(std::string str) : str(std::move(str))
	{
	}

	virtual inline explicit operator std::string() const
	{
		return this->str;
	};
};

template <ModelBasedType ModelT, OperatorValueType T>
struct comparison_operator : public condition_t
{
public:
	inline comparison_operator() = default;

	inline explicit comparison_operator(
		const std::string& column_name, const std::string& op, T value
	)
	{
		auto table = util::quote_str(ModelT::meta_table_name);
		auto column = util::quote_str(column_name);
		this->str = table + "." + column + " " + op + " " + std::to_string(value);
	}
};

template <ModelBasedType ModelT>
struct comparison_operator<ModelT, std::string> : public condition_t
{
public:
	inline comparison_operator() = default;

	inline explicit comparison_operator(
		const std::string& column_name, const std::string& op, const std::string& value
	)
	{
		auto table = util::quote_str(ModelT::meta_table_name);
		auto column = util::quote_str(column_name);
		this->str = table + "." + column + " " + op + " '" + value + "'";
	}
};

template <ModelBasedType ModelT>
struct comparison_operator<ModelT, const char*> : public condition_t
{
public:
	inline comparison_operator() = default;

	inline explicit comparison_operator(
		const std::string& column_name, const std::string& op, const char* value
	)
	{
		auto table = util::quote_str(ModelT::meta_table_name);
		auto column = util::quote_str(column_name);
		this->str = table + "." + column + " " + op + " '" + std::string(value) + "'";
	}
};

// SQL comparison operators.
template <ModelBasedType ModelT>
struct c
{
	std::string column;

	inline explicit c(std::string column) : column(std::move(column))
	{
	}

	template <OperatorValueType T>
	comparison_operator<ModelT, T> operator== (T val)
	{
		return comparison_operator<ModelT, T>(this->column, "=", val);
	}

	template <OperatorValueType T>
	comparison_operator<ModelT, T> operator!= (T val)
	{
		return comparison_operator<ModelT, T>(this->column, "!=", val);
	}

	template <OperatorValueType T>
	comparison_operator<ModelT, T> operator< (T val)
	{
		return comparison_operator<ModelT, T>(this->column, "<", val);
	}

	template <OperatorValueType T>
	comparison_operator<ModelT, T> operator> (T val)
	{
		return comparison_operator<ModelT, T>(this->column, ">", val);
	}

	template <OperatorValueType T>
	comparison_operator<ModelT, T> operator<= (T val)
	{
		return comparison_operator<ModelT, T>(this->column, "<=", val);
	}

	template <OperatorValueType T>
	comparison_operator<ModelT, T> operator>= (T val)
	{
		return comparison_operator<ModelT, T>(this->column, ">=", val);
	}
};

// SQL logical operators.
inline condition_t operator& (const condition_t& left, const condition_t& right)
{
	return condition_t("(" + (std::string)left + " AND " + (std::string)right + ")");
}

inline condition_t operator| (const condition_t& left, const condition_t& right)
{
	return condition_t("(" + (std::string)left + " OR " + (std::string)right + ")");
}

inline condition_t operator~ (const condition_t& cond)
{
	return condition_t("NOT (" + (std::string)cond + ")");
}

template <typename T>
concept FundamentalType = std::is_fundamental_v<T>;

template <ModelBasedType ModelT, OperatorValueType T>
struct between : public condition_t
{
	inline explicit between(const std::string& column, T lower, T upper)
	{
		this->str = util::quote_str(ModelT::meta_table_name) + "." + util::quote_str(column) +
			" BETWEEN " + std::to_string(lower) + " AND " + std::to_string(upper);
	}
};

template <ModelBasedType ModelT>
struct between<ModelT, std::string> : public condition_t
{
	inline explicit between(const std::string& column, const std::string& lower, const std::string& upper)
	{
		this->str = util::quote_str(ModelT::meta_table_name) + "." + util::quote_str(column) +
			" BETWEEN '" + lower + "' AND '" + upper + '\'';
	}
};

template <ModelBasedType ModelT>
struct between<ModelT, const char*> : public condition_t
{
	inline explicit between(const std::string& column, const char* lower, const char* upper)
	{
		this->str = util::quote_str(ModelT::meta_table_name) + "." + util::quote_str(column) +
			" BETWEEN '" + std::string(lower) + "' AND '" + std::string(upper) + '\'';
	}
};

// TODO: test it
template <ModelBasedType ModelT>
condition_t like(const std::string& column, const std::string& pattern)
{
	return util::quote_str(ModelT::meta_table_name) + "." + util::quote_str(column) +
		" LIKE '" + pattern + "'";
}

// TODO: test it
template <ModelBasedType ModelT>
condition_t like(
	const std::string& column, const std::string& pattern, const std::string& escape
)
{
	return like<ModelT>(column, pattern) + " ESCAPE '" + escape + "'";
}

// TODO: test it
template <ModelBasedType ModelT, FundamentalIterType IterBegin, FundamentalIterType IterEnd>
condition_t in(const std::string& column, IterBegin begin, IterEnd end)
{
	if (begin == end)
	{
		throw QueryError("in: list is empty", _ERROR_DETAILS_);
	}

	std::string condition_str = util::quote_str(ModelT::meta_table_name) + "." + util::quote_str(column) +
		" IN (" + str::join(begin, end, ", ", [](
			const typename std::iterator_traits<IterBegin>::value_type& item
		) -> std::string { return std::to_string(item); }) + ")";
	return condition_t(condition_str);
}

// TODO: test it
template <ModelBasedType ModelT, StringIterType IterBegin, StringIterType IterEnd>
condition_t in(const std::string& column, IterBegin begin, IterEnd end)
{
	if (begin == end)
	{
		throw QueryError("in: list is empty", _ERROR_DETAILS_);
	}

	std::string condition_str = util::quote_str(ModelT::meta_table_name) + "." + util::quote_str(column) +
		" IN (" + str::join(begin, end, ", ", [](
			const typename std::iterator_traits<IterBegin>::value_type& item
		) -> std::string { return item; }) + ")";
	return condition_t(condition_str);
}

// TODO: implement ALL, ANY, and EXISTS operators.

struct join
{
	std::string type;
	std::string table_name;
	q::condition_t condition;

	inline explicit operator std::string() const
	{
		return type + " JOIN \"" + table_name + "\" ON " + (std::string)condition;
	}
};

template <typename LeftT, typename RightT>
join left(const std::string& join_pk, const q::condition_t& extra_condition={})
{
	std::string left_table_name = LeftT::meta_table_name;
	const auto& table_name = RightT::meta_table_name;
	auto condition_str = util::quote_str(left_table_name) + "." + util::quote_str(LeftT::meta_pk_name)
		+ " = " +
		util::quote_str(table_name) + "." + util::quote_str(join_pk);
	auto extra_cond_str = (std::string)extra_condition;
	if (!extra_cond_str.empty())
	{
		condition_str += " AND " + extra_cond_str;
	}

	return {"LEFT", table_name, q::condition_t(condition_str)};
}

__Q_END__
