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
#include <xalwart.core/string_utils.h>

// Module definitions.
#include "./_def_.h"


__ORM_BEGIN__

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

	// TODO: !experimental feature!
	static constexpr std::initializer_list<const char*> meta_fields = {};

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

// Used in templates where Model-based class is required.
template <typename T>
concept ModelBasedType = std::is_base_of_v<Model, T> && std::is_default_constructible_v<T>;

__ORM_END__
