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

// Core libraries.
#include <xalwart.core/object/utility.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./driver.h"
#include "./queries/insert.h"
#include "./queries/select.h"

__ORM_BEGIN__

class DbClient
{
protected:
	std::shared_ptr<DbDriver> driver_;

public:
	inline explicit DbClient(std::shared_ptr<DbDriver> driver) : driver_(std::move(driver))
	{
		if (!this->driver_)
		{
			throw core::NullPointerException(
				"DbClient: driver must be instantiated", _ERROR_DETAILS_
			);
		}
	}

	[[nodiscard]]
	inline DbDriver* driver() const
	{
		return this->driver_.get();
	}

	// Insert into database.
	template <ModelBasedType ModelT>
	inline void insert_one(const ModelT& model) const
	{
		q::insert<ModelT>(this->driver_.get(), model).one();
	}

	template <ModelBasedType ModelT, typename PkT>
	inline void insert_one(const ModelT& model, PkT* pk) const
	{
		auto pk_str = q::insert<ModelT>(this->driver_.get(), model).one();
		if (pk != nullptr)
		{
			*pk = xw::object::as<PkT>((const void*)pk_str.c_str());
		}
	}

	template <ModelBasedType ModelT>
	inline q::insert<ModelT> insert(const ModelT& model) const
	{
		return q::insert<ModelT>(this->driver_.get(), model);
	}

	template <ModelBasedType ModelT, typename IteratorBegin, typename IteratorEnd>
	inline void insert_bulk(IteratorBegin begin, IteratorEnd end) const
	{
		auto query = q::insert<ModelT>(this->driver_.get(), *begin++);
		std::for_each(begin, end, [&query](const ModelT& model) -> void {
			query.model(model);
		});
		query.bulk();
	}

	// Retrieve from database.
	template <ModelBasedType ModelT>
	inline ModelT get(const q::condition& cond) const
	{
		return q::select<ModelT>(this->driver_.get()).where(cond).first();
	}

	template <ModelBasedType ModelT>
	inline q::select<ModelT> select() const
	{
		return q::select<ModelT>(this->driver_.get());
	}

	template <ModelBasedType ModelT>
	inline q::select<ModelT> filter(const q::condition& cond) const
	{
		return q::select<ModelT>(this->driver_.get()).where(cond);
	}
};

__ORM_END__
