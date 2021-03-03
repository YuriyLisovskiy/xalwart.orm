/**
 * queries/utility.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: helpers for queries.
 */

#pragma once

// Core libraries.
#include <xalwart.core/utility.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../model.h"


__Q_UTILITY_BEGIN__

// Retrieves table name of 'ModelT'. If ModelT::meta_table_name
// is nullptr, uses 'utility::demangle(...)' method to complete
// the operation.
template <ModelBasedType ModelT>
inline std::string get_table_name()
{
	if constexpr (ModelT::meta_table_name != nullptr)
	{
		return ModelT::meta_table_name;
	}
	else
	{
		auto table_name = xw::utility::demangle(typeid(ModelT).name());
		return table_name.substr(table_name.rfind(':') + 1);
	}
}

inline std::string quote_str(const std::string& s)
{
	return s.starts_with('"') ? s : '"' + s + '"';
}

template <ModelBasedType ModelT>
inline std::string get_pk_name()
{
	return ModelT::meta_pk_name;
}

__Q_UTILITY_END__
