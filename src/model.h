/**
 * model.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: base class for representing the database model.
 */

#pragma once

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

template <typename T>
concept column_field_type = std::is_fundamental_v<T> ||
	std::is_same_v<std::string, T> ||
	std::is_same_v<const char*, T>;

template <typename ModelT, column_field_type FieldT>
struct column_meta_t
{
	using field_type = FieldT;
	using model_type = ModelT;

	std::string name;
	bool is_pk;

	FieldT ModelT::* member_pointer;

	column_meta_t() = default;

	column_meta_t(std::string name, FieldT ModelT::* member_ptr, bool is_pk)
		: name(std::move(name)), member_pointer(member_ptr), is_pk(is_pk)
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

template <typename ModelT, column_field_type FieldT>
inline column_meta_t<ModelT, FieldT> make_column_meta(
	const std::string& name, FieldT ModelT::* member_ptr
)
{
	return column_meta_t<ModelT, FieldT>(name, member_ptr, false);
}

template <typename ModelT, column_field_type FieldT>
inline column_meta_t<ModelT, FieldT> make_pk_column_meta(
	const std::string& name, FieldT ModelT::* member_ptr
)
{
	return column_meta_t<ModelT, FieldT>(name, member_ptr, true);
}

// !IMPORTANT!
// Currently Model supports single pk only.
template <typename Derived, typename ...Columns>
class Model : public obj::Object
{
private:

	// Null model indicator.
	// Used in queries when selecting one row and
	// 'SELECT' statement returns nothing.
	bool _is_null_model = false;

public:

	// Must be overwritten in child class.
	static constexpr const char* meta_table_name = nullptr;

	// Omit primary key when inserting new models.
	//
	// Can be overwritten in child class.
	static constexpr bool meta_omit_pk = true;

	// Tuple of mapped columns. At least, primary key
	// is required.
	//
	// Must be overwritten in child class.
	static const std::tuple<Columns...> meta_columns;

protected:

	// Copies base class. Must be called when copy-constructor
	// is overridden.
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

		return obj::Object::__str__();
	}

	[[nodiscard]]
	inline std::string __repr__() const override
	{
		return this->__str__();
	}

	[[nodiscard]]
	std::shared_ptr<Object> __get_attr__(const char* attr_name) override
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
template <typename T>
concept ModelBasedType = std::is_base_of_v<Model<T>, T> && std::is_default_constructible_v<T>;

template <typename M, column_field_type F>
std::string get_column_value_as_string(const M& model, const column_meta_t<M, F>& column_meta)
{
	std::string result;
	if constexpr (std::is_fundamental_v<F>)
	{
		result = std::to_string(model.*column_meta.member_pointer);
	}
	else if constexpr (std::is_same_v<F, std::string>)
	{
		result = "'" + model.*column_meta.member_pointer + "'";
	}
	else if constexpr (std::is_same_v<F, const char*>)
	{
		result = "'" + std::string(model.*column_meta.member_pointer) + "'";
	}

	return result;
}

__ORM_END__
