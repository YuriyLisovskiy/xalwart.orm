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


__Q_BEGIN__

struct ordering
{
protected:
	std::string value;

public:
	inline ordering() = default;

	inline ordering(const std::string& column, const std::string& order)
	{
		this->value = column + " " + order;
	}

	inline ordering(const std::string& column)
	{
		this->value = column;
	}

	inline ordering(const char* column) : ordering(std::string(column))
	{
	}

	virtual inline explicit operator std::string() const
	{
		return this->value;
	}
};

inline ordering ascending(const std::string& column)
{
	return ordering(column, "ASC");
}

inline ordering asc(const std::string& column)
{
	return ascending(column);
}

inline ordering descending(const std::string& column)
{
	return ordering(column, "DESC");
}

inline ordering desc(const std::string& column)
{
	return descending(column);
}

struct condition
{
protected:
	std::string str;

public:
	inline condition() = default;

	inline condition(std::string str) : str(std::move(str))
	{
	}

	virtual inline explicit operator std::string() const
	{
		return this->str;
	};
};

template <OperatorValueType T>
struct comparison_operator : public condition
{
protected:
	inline void make(const std::string& column_name, const std::string& op, T value)
	{
		this->str = column_name + " " + op + " " + std::to_string(value);
	}

public:
	inline comparison_operator() = default;

	inline comparison_operator(std::string str) : condition(std::move(str))
	{
	}
};

template <>
struct comparison_operator<std::string> : public condition
{
protected:
	inline void make(const std::string& column_name, const std::string& op, const std::string& value)
	{
		this->str = column_name + " " + op + " '" + value + "'";
	}

public:
	inline comparison_operator() = default;

	inline comparison_operator(std::string str) : condition(std::move(str))
	{
	}
};

template <>
struct comparison_operator<const char*> : public condition
{
protected:
	inline void make(const std::string& column_name, const std::string& op, const char* value)
	{
		this->str = column_name + " " + op + " '" + std::string(value) + "'";
	}

public:
	inline comparison_operator() = default;

	inline comparison_operator(std::string str) : condition(std::move(str))
	{
	}
};

// SQL comparison operators.
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

// SQL logical operators.
inline condition operator& (const condition& left, const condition& right)
{
	return condition("(" + (std::string)left + " AND " + (std::string)right + ")");
}

inline condition operator| (const condition& left, const condition& right)
{
	return condition("(" + (std::string)left + " OR " + (std::string)right + ")");
}

inline condition operator! (const condition& cond)
{
	return condition("NOT (" + (std::string)cond + ")");
}

template <OperatorValueType T>
struct between : public condition
{
	inline explicit between(const std::string& column, T lower, T upper)
	{
		this->str = column + " BETWEEN " + std::to_string(lower) + " AND " + std::to_string(upper);
	}
};

template <>
struct between<std::string> : public condition
{
	inline explicit between(const std::string& column, const std::string& lower, const std::string& upper)
	{
		this->str = column + " BETWEEN '" + lower + "' AND '" + upper + '\'';
	}
};

template <>
struct between<const char*> : public condition
{
	inline explicit between(const std::string& column, const char* lower, const char* upper)
	{
		this->str = column + " BETWEEN '" + std::string(lower) + "' AND '" + std::string(upper) + '\'';
	}
};

// TODO: implement ALL, ANY, EXISTS, IN, and LIKE operators.

__Q_END__
