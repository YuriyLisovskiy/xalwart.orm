/**
 * model.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// Core libraries.
#include <xalwart.core/object/object.h>
#include <xalwart.core/exceptions.h>

// Module definitions.
#include "./_def_.h"


__ORM_BEGIN__

class Model : public object::Object
{
public:
	[[nodiscard]]
	inline short __cmp__(const Object* other) const override
	{
		throw core::NotImplementedException(
			"'__cmp__' is not implemented", _ERROR_DETAILS_
		);
	}
};

template <typename T>
concept ModelBasedType = std::is_base_of_v<Model, T> && std::is_default_constructible_v<T>;

__ORM_END__
