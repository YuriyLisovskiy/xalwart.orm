/**
 * model.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: base class for representing the database model.
 */

#pragma once

#include <iostream>

// Core libraries.
#include <xalwart.core/object/object.h>
#include <xalwart.core/exceptions.h>
#include <xalwart.core/types/string.h>
#include <xalwart.core/types/fundamental.h>
#include <xalwart.core/string_utils.h>
#include <xalwart.core/lazy.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./utility.h"


__ORM_BEGIN__

template <typename ModelT, typename FieldT>
struct column_t
{
	using field_type = FieldT;
	using model_type = ModelT;

	std::string name;

	FieldT ModelT::* member_pointer;

	column_t() = default;

	column_t(std::string name, FieldT ModelT::* member_ptr)
		: name(std::move(name)), member_pointer(member_ptr)
	{
	}

	column_t(const column_t& other)
	{
		if (this != &other)
		{
			this->name = other.name;
			this->member_pointer = other.member_pointer;
		}
	}
};

template <typename ModelT, typename FieldT>
inline column_t<ModelT, FieldT> make_column(
	const std::string& name, FieldT ModelT::* member_ptr
)
{
	return column_t<ModelT, FieldT>(name, member_ptr);
}

template <typename Derived, typename ...Columns>
class Model : public object::Object
{
private:

	// Null model indicator.
	// Used in queries when selecting one row and
	// 'SELECT' statement returns nothing.
	bool _is_null_model = false;

public:

	// Must be overwritten in child class.
	static constexpr const char* meta_table_name = nullptr;

	// Can be overwritten in child class.
	static constexpr const char* meta_pk_name = "id";

	// Omit primary key when inserting new models.
	//
	// Can be overwritten in child class.
	static constexpr bool meta_omit_pk = true;

	static const std::tuple<Columns...> meta_columns;

protected:
	inline void copy_base(const Model& other)
	{
		this->_is_null_model = other._is_null_model;
	}

public:

	// By default throws 'NotImplementedException'.
	// Overwriting is recommended in child classes
	// for models' comparison.
	[[nodiscard]]
	inline short __cmp__(const Object* other) const override
	{
		throw core::NotImplementedException(
			"'__cmp__' is not implemented", _ERROR_DETAILS_
		);
	}

	[[nodiscard]]
	inline std::string __str__() const override
	{
		if (this->_is_null_model)
		{
			return "null";
		}

		return object::Object::__str__();
	}

	[[nodiscard]]
	inline std::string __repr__() const override
	{
		if (this->_is_null_model)
		{
			return "Model{null}";
		}

		return object::Object::__repr__();
	}

	[[nodiscard]]
	std::shared_ptr<Object> __get_attr__(const char* attr_name) const override
	{
		std::shared_ptr<Object> obj;
		util::tuple_for_each(Derived::meta_columns, [this, attr_name, &obj](auto& column)
		{
			if (column.name == std::string(attr_name))
			{
				using field_type = typename std::remove_reference<decltype(column)>::type;
				using model_type = typename field_type::model_type;
				using T = typename field_type::field_type;
				if constexpr (std::is_fundamental_v<T>)
				{
					obj = std::make_shared<types::Fundamental<T>>(((model_type*)this)->*column.member_pointer);
				}
				else if constexpr (std::is_same_v<T, std::string>)
				{
					obj = std::make_shared<types::String>(((model_type*)this)->*column.member_pointer);
				}

				return false;
			}

			return true;
		});

		if (!obj)
		{
			throw core::AttributeError(
				"'" + this->__type__().name() + "' object has no attribute '" + std::string(attr_name) + "'",
				_ERROR_DETAILS_
			);
		}

		return obj;
	}

	void __set_attr__(const char* attr_name, const void* data) override
	{
		bool is_set = false;
		util::tuple_for_each(Derived::meta_columns, [this, attr_name, data, &is_set](auto& column)
		{
			if (column.name == std::string(attr_name))
			{
				using column_type = typename std::remove_reference<decltype(column)>::type;
				using model_type = typename column_type::model_type;
				using field_type = typename column_type::field_type;
				size_t len = std::strlen((char*)data);
				std::string str_val = {(char*)data, (char*)data + len + 1};
				((model_type*)this)->*column.member_pointer = util::as<field_type>(str_val.c_str());
				is_set = true;
				return false;
			}

			return true;
		});

		if (!is_set)
		{
			throw core::AttributeError(
				"'" + this->__type__().name() + "' object has no attribute '" + std::string(attr_name) + "'",
				_ERROR_DETAILS_
			);
		}
	}

	// Marks model as null-model. This action can not be undone.
	inline void mark_as_null()
	{
		this->_is_null_model = true;
	}

	// Returns whether model is null or not.
	[[nodiscard]]
	inline bool is_null() const
	{
		return this->_is_null_model;
	}

	inline void from_map(const std::map<std::string, char*>& fields)
	{
		std::map<std::string, std::map<std::string, char*>> sub_models;
		for (const auto& field : fields)
		{
			if (field.second)
			{
				this->__set_attr__(field.first.c_str(), field.second);
			}
		}
	}
};

template <typename Derived, typename ...Cs>
const std::tuple<Cs...> Model<Derived, Cs...>::meta_columns = {};

// Used in templates where Model-based class is required.
//template <typename T>
//concept ModelBasedType = std::is_base_of_v<Model, T> && std::is_default_constructible_v<T>;

__ORM_END__
