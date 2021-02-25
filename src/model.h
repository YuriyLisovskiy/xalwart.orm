/**
 * model.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

#include <iostream>

// Core libraries.
#include <xalwart.core/object/object.h>
#include <xalwart.core/exceptions.h>

// Module definitions.
#include "./_def_.h"


__ORM_BEGIN__

class Model : public object::Object
{
private:
	bool _is_null_model = false;

public:
	static constexpr const char* meta_table_name = nullptr;
	static constexpr const char* meta_pk_name = "id";

	// Omit primary key when inserting new models.
	static constexpr bool meta_omit_pk = true;

public:
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
			return "Model{null}";
		}

		return object::Object::__str__();
	}

	[[nodiscard]]
	inline std::string __repr__() const override
	{
		return this->__str__();
	}

	inline void mark_as_null()
	{
		this->_is_null_model = true;
	}

	[[nodiscard]]
	inline bool is_null() const
	{
		return this->_is_null_model;
	}
};

template <typename T>
concept ModelBasedType = std::is_base_of_v<Model, T> && std::is_default_constructible_v<T>;

__ORM_END__
