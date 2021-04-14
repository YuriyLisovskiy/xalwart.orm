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
#include "../meta.h"
#include "../utility.h"


__Q_BEGIN__

// Base structure for SQL function.
struct function_t
{
	std::string name;
	std::string args;

	function_t() = default;

	inline function_t(
		std::string name, std::string args
	) : name(std::move(name)), args(std::move(args))
	{
	}

	inline explicit operator std::string() const
	{
		return this->name + "(" + this->args + ")";
	}
};

// Base structure for SQL aggregate function.
template <typename ReturnT>
struct aggregate_function_t : public function_t
{
	using return_type = ReturnT;

	aggregate_function_t() = default;

	inline aggregate_function_t(
		const std::string& name, const std::string& args
	) : function_t(name, args)
	{
	};

	template <typename ColumnT, typename ModelT>
	inline explicit aggregate_function_t(const std::string& name, ColumnT ModelT::* column)
	{
		this->name = name;
		this->args = meta::get_table_name<ModelT>(true) + "." + meta::get_column_name(column, true);
	}
};

// TESTME: avg
// Builds SQL `avg` aggregate function.
template <typename ColumnT, typename ModelT>
inline auto avg(ColumnT ModelT::* column)
{
	return aggregate_function_t<double>{"avg", column};
}

// TESTME: count
// Builds SQL `count` aggregate function.
inline auto count()
{
	return aggregate_function_t<size_t>("count", "*");
}

// TESTME: min
// Builds SQL `min` aggregate function.
template <typename ColumnT, typename ModelT>
inline auto min(ColumnT ModelT::* column)
{
	return aggregate_function_t<ColumnT>{"min", column};
}

// TESTME: max
// Builds SQL `max` aggregate function.
template <typename ColumnT, typename ModelT>
inline auto max(ColumnT ModelT::* column)
{
	return aggregate_function_t<ColumnT>{"max", column};
}

// TESTME: sum
// Builds SQL `sum` aggregate function.
template <typename ColumnT, typename ModelT>
inline auto sum(ColumnT ModelT::* column)
{
	return aggregate_function_t<ColumnT>{"sum", column};
}

__Q_END__
