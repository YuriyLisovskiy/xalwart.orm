/**
 * operators.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
#include <string>

// Module definitions.
#include "./_def_.h"


__ORM_BEGIN__

struct operator_base
{
protected:
	std::string repr;

public:
	virtual inline explicit operator std::string() const
	{
		return this->repr;
	};
};

template <OperatorValueType T>
struct comparison_operator : public operator_base
{
protected:
	inline void make(const std::string& column_name, const std::string& op, T value)
	{
		this->repr = column_name + " " + op + " " + std::to_string(value);
	}
};

template <>
struct comparison_operator<std::string> : public operator_base
{
protected:
	inline void make(const std::string& column_name, const std::string& op, const std::string& value)
	{
		this->repr = column_name + " " + op + " '" + value + "'";
	}
};

template <>
struct comparison_operator<const char*> : public operator_base
{
protected:
	inline void make(const std::string& column_name, const std::string& op, const char* value)
	{
		this->repr = column_name + " " + op + " '" + std::string(value) + "'";
	}
};

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
		this->repr = (std::string)left + " " + op + " " + (std::string)right;
	}
};

template <OperatorValueType L, OperatorValueType R>
struct and_ : public logical_operator<L, R>
{
	inline explicit and_(comparison_operator<L> left, comparison_operator<R> right)
	{
		this->make(left, "AND", right);
	}
};

// TODO: implement ALL, ANY, BETWEEN, EXISTS, IN, LIKE and NOT operators.

__ORM_END__
