/**
 * db/model.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Base class for representing the database model.
 */

#pragma once

// C++ libraries.
#include <map>

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
#include "./meta.h"


__ORM_DB_BEGIN__

// !IMPORTANT!
// Currently Model supports single pk only.
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
	static constexpr bool meta_omit_pk = true;

	// Tuple of mapped columns. At least, primary key
	// is required.
	//
	// Must be overwritten in child class.
	inline static constexpr std::tuple<> meta_columns = {};

protected:

	// Copies base class. Must be called when copy-constructor
	// is overridden.
	inline void copy_base(const Model& other)
	{
		this->_is_null_model = other._is_null_model;
	}

	template <typename TargetT, typename ...Columns>
	inline std::shared_ptr<const Object> get_attribute_from(
		const std::tuple<Columns...>& columns, const char* attr_name
	) const
	{
		std::shared_ptr<Object> obj;
		xw::orm::util::tuple_for_each(columns, [this, attr_name, &obj](auto& column)
		{
			if (column.name == std::string(attr_name))
			{
				using field_type = typename std::remove_reference<decltype(column)>::type;
				using model_type = typename field_type::model_type;
				using T = typename field_type::field_type;
				if constexpr (std::is_fundamental_v<T>)
				{
					obj = std::make_shared<xw::types::Fundamental<T>>(((model_type*)this)->*column.member_pointer);
				}
				else if constexpr (std::is_same_v<T, std::string>)
				{
					obj = std::make_shared<xw::types::String>(((model_type*)this)->*column.member_pointer);
				}

				return false;
			}

			return true;
		});

		if (!obj)
		{
			throw xw::core::AttributeError(
				"'" + this->__type__().name() + "' object has no attribute '" + std::string(attr_name) + "'",
				_ERROR_DETAILS_
			);
		}

		return obj;
	}

	template <typename TargetT, typename ...Columns>
	void set_attribute_for(
		const std::tuple<Columns...>& columns, const char* attr_name, const void* data
	)
	{
		bool is_set = false;
		xw::orm::util::tuple_for_each(columns, [this, attr_name, data, &is_set](auto& column)
		{
			if (column.name == std::string(attr_name))
			{
				using column_type = typename std::remove_reference<decltype(column)>::type;
				using model_type = typename column_type::model_type;
				using field_type = typename column_type::field_type;
				size_t len = std::strlen((char*)data);
				std::string str_val = {(char*)data, (char*)data + len + 1};
				((model_type*)this)->*column.member_pointer = xw::orm::util::as<field_type>(str_val.c_str());
				is_set = true;
				return false;
			}

			return true;
		});

		if (!is_set)
		{
			throw xw::core::AttributeError(
				"'" + this->__type__().name() + "' object has no attribute '" + std::string(attr_name) + "'",
				_ERROR_DETAILS_
			);
		}
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

	// Operator equals.
	inline bool operator== (const Model& other) const
	{
		return this->__cmp__(&other) == 0;
	}

	// Operator not equals.
	inline bool operator!= (const Model& other) const
	{
		return this->__cmp__(&other) != 0;
	}

	// Operator less.
	inline bool operator< (const Model& other) const
	{
		return this->__cmp__(&other) == -1;
	}

	// Operator less or equals.
	inline bool operator<= (const Model& other) const
	{
		auto res = this->__cmp__(&other);
		return res == 0 || res == -1;
	}

	// Operator greater.
	inline bool operator> (const Model& other) const
	{
		return this->__cmp__(&other) == 1;
	}

	// Operator greater or equals.
	inline bool operator>= (const Model& other) const
	{
		auto res = this->__cmp__(&other);
		return res == 0 || res == 1;
	}
};

// Used in templates where Model-based class is required.
template <typename T>
concept model_based_type_c = std::is_base_of_v<Model, T> && std::is_default_constructible_v<T>;

template <typename T>
concept model_based_iterator_type_c = std::is_base_of_v<Model, iterator_v_type<T>> &&
	std::is_default_constructible_v<iterator_v_type<T>>;

template <model_based_type_c M, column_field_type_c F>
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

__ORM_DB_END__
