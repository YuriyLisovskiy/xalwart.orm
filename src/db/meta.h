/**
 * db/meta.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Utilities for Model based types.
 */

#pragma once

// Base libraries.
#include <xalwart.base/utility.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../exceptions.h"
#include "../utility.h"


__ORM_DB_BEGIN__

// TESTME: get_pk_name
// TODO: docs for 'get_pk_name'
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

// TESTME: make_fk
// TODO: docs for 'make_fk'
template <typename ModelT>
inline std::string make_fk()
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");
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

// TESTME: get_column_name
// TODO: docs for 'get_column_name'
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
		throw ValueError("column not found", _ERROR_DETAILS_);
	}

	return quote ? util::quote_str(name) : name;
}

// TESTME: get_table_name
// TODO: docs for 'get_table_name'
template <typename ModelT>
inline std::string get_table_name(bool quote=false)
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");
	return quote ? util::quote_str(ModelT::meta_table_name) : ModelT::meta_table_name;
}

template <typename T>
concept column_field_type = std::is_fundamental_v<T> ||
	std::is_same_v<std::string, T> ||
	std::is_same_v<const char*, T> ||
	std::is_same_v<dt::Date, T> ||
	std::is_same_v<dt::Time, T> ||
	std::is_same_v<dt::Datetime, T>;

// TESTME: ColumnMeta
// TODO: docs for 'ColumnMeta'
template <typename ModelT, column_field_type FieldT>
struct ColumnMeta
{
	using field_type = FieldT;
	using model_type = ModelT;

	std::string name;
	bool is_pk = false;

	FieldT ModelT::* member_pointer;

	using field_builder_type = std::function<FieldT(const void*)>;
	using string_builder_type = std::function<std::string(const ModelT&)>;

	field_builder_type as_field = nullptr;
	string_builder_type as_string = nullptr;

	ColumnMeta() = default;

	ColumnMeta(
		std::string name, FieldT ModelT::* member_ptr, bool is_pk,
		field_builder_type field_builder, string_builder_type string_builder
	) : name(std::move(name)), member_pointer(member_ptr), is_pk(is_pk),
		as_field(std::move(field_builder)), as_string(string_builder)
	{
	}

	ColumnMeta(const ColumnMeta& other)
	{
		if (this != &other)
		{
			this->name = other.name;
			this->is_pk = other.is_pk;
			this->member_pointer = other.member_pointer;
			this->as_field = other.as_field;
			this->as_string = other.as_string;
		}
	}
};

inline static const char* DEFAULT_DATE_FORMAT = "%Y-%m-%d";
inline static const char* DEFAULT_TIME_FORMAT = "%H:%M:%S";
inline static const char* DEFAULT_DATETIME_FORMAT = "%Y-%m-%d %H:%M:%S";

// TESTME: column_as_field
// TODO: docs for 'column_as_field'
template <column_field_type FieldT>
FieldT column_as_field(const void* data)
{
	if constexpr (std::is_same_v<FieldT, dt::Date>)
	{
		return util::as_date(data, DEFAULT_DATE_FORMAT);
	}
	else if constexpr (std::is_same_v<FieldT, dt::Time>)
	{
		return util::as_time(data, DEFAULT_TIME_FORMAT);
	}
	else if constexpr (std::is_same_v<FieldT, dt::Datetime>)
	{
		return util::as_datetime(data, DEFAULT_DATETIME_FORMAT);
	}

	return xw::util::as<FieldT>(data);
}

// TESTME: field_as_column_v
// TODO: docs for 'field_as_column_v'
template <column_field_type FieldT>
std::string field_as_column_v(const FieldT& field)
{
	if constexpr (std::is_fundamental_v<FieldT>)
	{
		return std::to_string(field);
	}
	else if constexpr (std::is_same_v<FieldT, std::string>)
	{
		return "'" + field + "'";
	}
	else if constexpr (std::is_same_v<FieldT, const char*>)
	{
		return "'" + std::string(field) + "'";
	}
	else if constexpr (std::is_same_v<FieldT, dt::Date>)
	{
		return "'" + field.strftime(DEFAULT_DATE_FORMAT) + "'";
	}
	else if constexpr (std::is_same_v<FieldT, dt::Time>)
	{
		return "'" + field.strftime(DEFAULT_TIME_FORMAT) + "'";
	}
	else if constexpr (std::is_same_v<FieldT, dt::Datetime>)
	{
		return "'" + field.strftime(DEFAULT_DATETIME_FORMAT) + "'";
	}

	return "";
}

// TESTME: make_column_meta
// TODO: docs for 'make_column_meta'
template <typename ModelT, column_field_type FieldT>
inline ColumnMeta<ModelT, FieldT> make_column_meta(
	const std::string& name, FieldT ModelT::* member_ptr, bool is_pk=false
)
{
	return ColumnMeta<ModelT, FieldT>(
		name, member_ptr, is_pk,
		[](const void* data) -> FieldT
		{
			return column_as_field<FieldT>(data);
		},
		[member_ptr](const ModelT& model) -> std::string
		{
			return field_as_column_v(model.*member_ptr);
		}
	);
}

// TESTME: make_pk_column_meta
// TODO: docs for 'make_pk_column_meta'
template <typename ModelT, typename FieldT>
inline ColumnMeta<ModelT, FieldT> make_pk_column_meta(const std::string& name, FieldT ModelT::* member_ptr)
{
	return make_column_meta<ModelT, FieldT>(name, member_ptr, true);
}

__ORM_DB_END__
