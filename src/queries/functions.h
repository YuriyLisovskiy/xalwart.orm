/**
 * queries/functions.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Set of basic SQL functions.
 */

#pragma once

// C++ libraries.
#include <string>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../db/meta.h"


__ORM_Q_BEGIN__

// Base structure for SQL function.
struct Function
{
	std::string name;
	std::string args;

	Function() = default;

	inline Function(std::string name, std::string args) : name(std::move(name)), args(std::move(args))
	{
	}

	inline explicit operator std::string() const
	{
		return this->name + "(" + this->args + ")";
	}
};

// Base structure for SQL aggregate function.
template <typename ReturnT>
struct AggregateFunction : public Function
{
	using return_type = ReturnT;

	AggregateFunction() = default;

	inline AggregateFunction(const std::string& name, const std::string& args) : Function(name, args)
	{
	};

	template <db::column_field_type_c ColumnT, typename ModelT>
	inline explicit AggregateFunction(const std::string& name, ColumnT ModelT::* column)
	{
		this->name = name;
		this->args = db::get_table_name<ModelT>(true) + "." + db::get_column_name(column, true);
	}
};

// TESTME: avg
// Builds SQL `avg` aggregate function.
template <db::column_field_type_c ColumnT, typename ModelT>
inline auto avg(ColumnT ModelT::* column)
{
	return AggregateFunction<double>{"avg", column};
}

// TESTME: count
// Builds SQL `count` aggregate function.
inline auto count()
{
	return AggregateFunction<size_t>("count", "*");
}

// TESTME: min
// Builds SQL `min` aggregate function.
template <db::column_field_type_c ColumnT, typename ModelT>
inline auto min(ColumnT ModelT::* column)
{
	return AggregateFunction<ColumnT>{"min", column};
}

// TESTME: max
// Builds SQL `max` aggregate function.
template <db::column_field_type_c ColumnT, typename ModelT>
inline auto max(ColumnT ModelT::* column)
{
	return AggregateFunction<ColumnT>{"max", column};
}

// TESTME: sum
// Builds SQL `sum` aggregate function.
template <db::column_field_type_c ColumnT, typename ModelT>
inline auto sum(ColumnT ModelT::* column)
{
	return AggregateFunction<ColumnT>{"sum", column};
}

__ORM_Q_END__
