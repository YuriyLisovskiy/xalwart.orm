/**
 * utility.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: ORM helpers.
 */

#pragma once

// Core libraries.
#include <xalwart.core/utility.h>
#include <xalwart.core/types/fundamental.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./model.h"


__ORM_UTIL_BEGIN__

template<typename T>
struct item_return{ typedef T type; };

template<typename T>
typename item_return<T>::type as(const void*);

template<>
struct item_return<int>{ typedef int type; };
template<>
inline int as<int>(const void* data)
{
	return std::stoi((const char*)data);
}

template<>
struct item_return<unsigned long int>{ typedef unsigned long int type; };
template<>
inline unsigned long int as<unsigned long int>(const void* data)
{
	return std::stoul((const char*)data);
}

template<>
struct item_return<double>{ typedef double type; };
template<>
inline double as<double>(const void* data)
{
	return std::stod((const char*)data);
}

template<>
struct item_return<std::string>{ typedef std::string type; };
template<>
inline std::string as<std::string>(const void* data)
{
	return std::string((const char*)data);
}

template <typename T>
inline object::Attribute field_accessor(T* field)
{
	return object::Attribute(
		[field]() -> std::shared_ptr<object::Object> {
			return std::make_shared<types::Fundamental<T>>(*field);
		},
			[field](const void* val) -> void {
			size_t len = std::strlen((char*)val);
			std::string str_val = {(char*)val, (char*)val + len + 1};
			*field = as<T>(str_val.c_str());
		}
	);
}

template<>
inline object::Attribute field_accessor<std::string>(std::string* field)
{
	return xw::object::Attribute(
		[field]() -> std::shared_ptr<object::Object> {
			return std::make_shared<types::String>(*field);
		},
		[field](const void* val) -> void {
			size_t len = std::strlen((char*)val);
			*field = {(char*)val, (char*)val + len + 1};
		}
	);
}

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

template <ModelBasedType ModelT>
inline std::string get_pk_name()
{
	return ModelT::meta_pk_name;
}

inline std::string quote_str(const std::string& s)
{
	return s.starts_with('"') ? s : '"' + s + '"';
}

__ORM_UTIL_END__
