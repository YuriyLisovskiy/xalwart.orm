/**
 * client.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// C++ libraries.
// TODO

// Module definitions.
#include <utility>

#include "./_def_.h"

// Orm libraries.
#include "./driver.h"
#include "./query/select.h"

__ORM_BEGIN__

class DbClient
{
protected:
	std::shared_ptr<DbDriver> driver;

public:
	inline explicit DbClient(std::shared_ptr<DbDriver> driver) : driver(std::move(driver))
	{
		if (!this->driver)
		{
			throw core::NullPointerException(
				"DbClient: driver must be instantiated", _ERROR_DETAILS_
			);
		}
	}

	template <ModelBasedType ModelT>
	inline q::select<ModelT> select() const
	{
		return q::select<ModelT>(this->driver.get());
	}

	template <ModelBasedType ModelT>
	inline q::select<ModelT> filter(const q::condition& cond) const
	{
		return q::select<ModelT>(this->driver.get()).where(cond);
	}

	template <ModelBasedType ModelT>
	inline std::shared_ptr<ModelT> get(const q::condition& cond) const
	{
		return q::select<ModelT>(this->driver.get()).where(cond).first();
	}
};

__ORM_END__
