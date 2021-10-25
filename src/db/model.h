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
#include <string>
#include <sstream>

// Base libraries.
#include <xalwart.base/exceptions.h>
#include <xalwart.base/types/utility.h>
#include <xalwart.base/string_utils.h>
#include <xalwart.base/lazy.h>
#include <xalwart.base/interfaces/base.h>
#include <xalwart.base/object/meta.h>
#include <xalwart.base/object/object.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./meta.h"


__ORM_DB_BEGIN__

// !IMPORTANT!
// Model supports single pk only!
// TESTME: Model
// TODO: docs for 'Model'
class Model : public xw::IStringSerializable
{
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

	Model() = default;

	[[nodiscard]]
	inline std::string to_string() const override
	{
		if (this->_is_null_model)
		{
			return "null";
		}

		std::stringstream oss;
		oss << static_cast<const void*>(this);
		return "<" + demangle(typeid(*this).name()) + " object at " + oss.str() + ">";
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
				this->__orm_set_column__(field.first, field.second);
			}
		}
	}

protected:
	// Copies base class. Must be called when copy-constructor
	// is overridden.
	inline void copy_base(const Model& other)
	{
		this->_is_null_model = other._is_null_model;
	}

	template <typename ...Columns>
	inline std::shared_ptr<const obj::Object> __orm_column_data_to_object__(
		const std::tuple<Columns...>& columns, const std::string& column_name
	) const
	{
		std::shared_ptr<const obj::Object> object;
		util::tuple_for_each(columns, [this, column_name, &object](auto& column)
		{
			if (column.name == column_name)
			{
				using column_type = typename std::remove_reference<decltype(column)>::type;
				using model_type = typename column_type::model_type;
				using field_type = typename column_type::field_type;
				if constexpr (std::is_same_v<field_type, dt::Date>)
				{
					object = std::make_shared<types::Date>(
						((model_type*)this)->*column.member_pointer, DEFAULT_DATE_FORMAT
					);
				}
				else if constexpr (std::is_same_v<field_type, dt::Time>)
				{
					object = std::make_shared<types::Time>(
						((model_type*)this)->*column.member_pointer, DEFAULT_TIME_FORMAT
					);
				}
				else if constexpr (std::is_same_v<field_type, dt::Datetime>)
				{
					object = std::make_shared<types::Datetime>(
						((model_type*)this)->*column.member_pointer, DEFAULT_DATETIME_FORMAT
					);
				}
				else
				{
					object = types::to_object(((model_type*)this)->*column.member_pointer);
				}

				return false;
			}

			return true;
		});

		if (!object)
		{
			this->_throw_column_not_found(column_name);
		}

		return object;
	}

	template <typename ...Columns>
	void __orm_set_column_data__(
		const std::tuple<Columns...>& columns, const std::string& column_name, const char* data
	)
	{
		bool is_set = false;
		util::tuple_for_each(columns, [this, column_name, data, &is_set](auto& column)
		{
			if (column.name == column_name)
			{
				using column_type = typename std::remove_reference<decltype(column)>::type;
				using model_type = typename column_type::model_type;
				size_t len = std::strlen(data);
				std::string str_val = {data, data + len + 1};
				((model_type*)this)->*column.member_pointer = column.as_field(str_val.c_str());
				is_set = true;
				return false;
			}

			return true;
		});

		if (!is_set)
		{
			this->_throw_column_not_found(column_name);
		}
	}

	virtual void __orm_set_column__(const std::string& column_name, const char* data) = 0;

private:
	// Null model indicator.
	// Used in queries when selecting one row and
	// 'SELECT' statement returns nothing.
	bool _is_null_model = false;

	inline void _throw_column_not_found(const std::string& column_name) const
	{
		throw AttributeError(
			"'" + demangle(typeid(*this).name()) + "' object has not column '" +
				column_name + "' mapped to the database",
			_ERROR_DETAILS_
		);
	}
};

// Used in templates where Model-based class is required.
template <typename T>
concept model_based_type = std::is_base_of_v<Model, T> && std::is_default_constructible_v<T>;

template <typename T>
concept model_based_iterator = std::is_base_of_v<Model, iterator_v_type<T>> &&
	std::is_default_constructible_v<iterator_v_type<T>>;

__ORM_DB_END__
