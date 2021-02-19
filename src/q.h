/**
 * q.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: set of SQL arithmetic and logical operators.
 */

#pragma once

// C++ libraries.
#include <string>

// Module definitions.
#include "./_def_.h"


__Q_BEGIN__

struct operator_base
{
protected:
	std::string condition;

public:
	inline operator_base() = default;

	inline operator_base(std::string condition) : condition(std::move(condition))
	{
	}

	virtual inline explicit operator std::string() const
	{
		return this->condition;
	};
};

template <OperatorValueType T>
struct comparison_operator : public operator_base
{
protected:
	inline void make(const std::string& column_name, const std::string& op, T value)
	{
		this->condition = column_name + " " + op + " " + std::to_string(value);
	}

public:
	inline comparison_operator() = default;

	inline comparison_operator(std::string condition) : operator_base(std::move(condition))
	{
	}
};

template <>
struct comparison_operator<std::string> : public operator_base
{
protected:
	inline void make(const std::string& column_name, const std::string& op, const std::string& value)
	{
		this->condition = column_name + " " + op + " '" + value + "'";
	}

public:
	inline comparison_operator() = default;

	inline comparison_operator(std::string condition) : operator_base(std::move(condition))
	{
	}
};

template <>
struct comparison_operator<const char*> : public operator_base
{
protected:
	inline void make(const std::string& column_name, const std::string& op, const char* value)
	{
		this->condition = column_name + " " + op + " '" + std::string(value) + "'";
	}

public:
	inline comparison_operator() = default;

	inline comparison_operator(std::string condition) : operator_base(std::move(condition))
	{
	}
};

// SQLite comparison operators.
template <OperatorValueType T>
struct equals : public comparison_operator<T>
{
	inline explicit equals(const std::string& column, T value)
	{
		this->make(column, "=", value);
	}
};

template <OperatorValueType T>
struct eq : public equals<T>
{
	inline explicit eq(const std::string& column, T value)
		: equals<T>(column, value)
	{
	}
};

template <OperatorValueType T>
struct not_equals : public comparison_operator<T>
{
	inline explicit not_equals(const std::string& column, T value)
	{
		this->make(column, "!=", value);
	}
};

template <OperatorValueType T>
struct ne : public not_equals<T>
{
	inline explicit ne(const std::string& column, T value)
		: not_equals<T>(column, value)
	{
	}
};

template <OperatorValueType T>
struct less : public comparison_operator<T>
{
	inline explicit less(const std::string& column, T value)
	{
		this->make(column, "<", value);
	}
};

template <OperatorValueType T>
struct lt : public less<T>
{
	inline explicit lt(const std::string& column, T value)
		: less<T>(column, value)
	{
	}
};

template <OperatorValueType T>
struct greater : public comparison_operator<T>
{
	inline explicit greater(const std::string& column, T value)
	{
		this->make(column, ">", value);
	}
};

template <OperatorValueType T>
struct gt : public greater<T>
{
	inline explicit gt(const std::string& column, T value)
		: greater<T>(column, value)
	{
	}
};

template <OperatorValueType T>
struct less_or_equals : public comparison_operator<T>
{
	inline explicit less_or_equals(const std::string& column, T value)
	{
		this->make(column, "<=", value);
	}
};

template <OperatorValueType T>
struct le : public less_or_equals<T>
{
	inline explicit le(const std::string& column, T value)
		: less_or_equals<T>(column, value)
	{
	}
};

template <OperatorValueType T>
struct greater_or_equals : public comparison_operator<T>
{
	inline explicit greater_or_equals(const std::string& column, T value)
	{
		this->make(column, ">=", value);
	}
};

template <OperatorValueType T>
struct ge : public greater_or_equals<T>
{
	inline explicit ge(const std::string& column, T value)
		: greater_or_equals<T>(column, ">=", value)
	{
	}
};

// SQLite logical operators.
template <OperatorValueType L, OperatorValueType R>
struct logical_operator : public operator_base
{
protected:
	inline void make(
		const comparison_operator<L>& left,
		const std::string& op,
		const comparison_operator<R>& right
	)
	{
		this->condition = "(" + (std::string)left + " " + op + " " + (std::string)right + ")";
	}
};

inline operator_base operator& (const operator_base& left, const operator_base& right)
{
	return operator_base("(" + (std::string)left + " AND " + (std::string)right + ")");
}

inline operator_base operator| (const operator_base& left, const operator_base& right)
{
	return operator_base("(" + (std::string)left + " OR " + (std::string)right + ")");
}

inline operator_base operator! (const operator_base& condition)
{
	return operator_base("NOT (" + (std::string)condition + ")");
}

template <OperatorValueType T>
struct between : public operator_base
{
	inline explicit between(const std::string& column, T lower, T upper)
	{
		this->condition = column + " BETWEEN " + std::to_string(lower) + " AND " + std::to_string(upper);
	}
};

template <>
struct between<std::string> : public operator_base
{
	inline explicit between(const std::string& column, const std::string& lower, const std::string& upper)
	{
		this->condition = column + " BETWEEN '" + lower + "' AND '" + upper + '\'';
	}
};

template <>
struct between<const char*> : public operator_base
{
	inline explicit between(const std::string& column, const char* lower, const char* upper)
	{
		this->condition = column + " BETWEEN '" + std::string(lower) + "' AND '" + std::string(upper) + '\'';
	}
};

// TODO: implement ALL, ANY, EXISTS, IN, and LIKE operators.

__Q_END__
