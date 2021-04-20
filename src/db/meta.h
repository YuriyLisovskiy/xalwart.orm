/**
 * db/meta.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Utilities for Model based types.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../exceptions.h"
#include "../utility.h"


__ORM_DB_BEGIN__

template <typename ModelT>
inline std::string get_pk_name()
{
	std::string result;
	util::tuple_for_each(ModelT::meta_columns, [&result](auto& column)
	{
		if (column.is_pk)
		{
			result = column.name;
			return false;
		}

		return true;
	});
	return result;
}

template <typename ModelT>
inline std::string make_fk()
{
	static_assert(
		ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized"
	);
	std::string table_name = ModelT::meta_table_name;
	if (table_name.ends_with('s'))
	{
		table_name = table_name.substr(0, table_name.size() - 1);
	}

	auto pk_name = get_pk_name<ModelT>();
	if (pk_name.empty())
	{
		throw QueryError("make_fk: model requires pk column", _ERROR_DETAILS_);
	}

	return table_name + "_" + pk_name;
}

template <typename F, typename O>
inline std::string get_column_name(F O::* member_pointer, bool quote=false)
{
	std::string name;
	util::tuple_for_each(O::meta_columns, [&name, member_pointer](auto& column)
	{
		if (util::compare_any(column.member_pointer, member_pointer))
		{
			name = column.name;
			return false;
		}

		return true;
	});

	if (name.empty())
	{
		throw core::ValueError("column not found", _ERROR_DETAILS_);
	}

	return quote ? util::quote_str(name) : name;
}

template <typename ModelT>
inline std::string get_table_name(bool quote=false)
{
	static_assert(
		ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized"
	);
	return quote ? util::quote_str(ModelT::meta_table_name) : ModelT::meta_table_name;
}

template <typename T>
concept column_field_type_c = std::is_fundamental_v<T> ||
	std::is_same_v<std::string, T> ||
	std::is_same_v<const char*, T>;

template <typename ModelT, column_field_type_c FieldT>
struct column_meta_t
{
	using field_type = FieldT;
	using model_type = ModelT;

	std::string name;
	bool is_pk;

	FieldT ModelT::* member_pointer;

	column_meta_t() = default;

	column_meta_t(std::string name, FieldT ModelT::* member_ptr, bool is_pk) :
		name(std::move(name)), member_pointer(member_ptr), is_pk(is_pk)
	{
	}

	column_meta_t(const column_meta_t& other)
	{
		if (this != &other)
		{
			this->name = other.name;
			this->is_pk = other.is_pk;
			this->member_pointer = other.member_pointer;
		}
	}
};

template <typename ModelT, column_field_type_c FieldT>
inline column_meta_t<ModelT, FieldT> make_column_meta(
	const std::string& name, FieldT ModelT::* member_ptr
)
{
	return column_meta_t<ModelT, FieldT>(name, member_ptr, false);
}

template <typename ModelT, column_field_type_c FieldT>
inline column_meta_t<ModelT, FieldT> make_pk_column_meta(
	const std::string& name, FieldT ModelT::* member_ptr
)
{
	return column_meta_t<ModelT, FieldT>(name, member_ptr, true);
}

__ORM_DB_END__
