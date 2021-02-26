/**
 * client.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: TODO
 */

#pragma once

// Core libraries.
#include <xalwart.core/object/utility.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./abc.h"
#include "./queries/insert.h"
#include "./queries/select.h"


__ORM_BEGIN__

class Client
{
protected:
	std::shared_ptr<abc::ISQLDriver> db;

public:
	inline explicit Client(std::shared_ptr<abc::ISQLDriver> driver) : db(std::move(driver))
	{
		if (!this->db)
		{
			throw core::NullPointerException(
				"DbClient: driver must be instantiated", _ERROR_DETAILS_
			);
		}
	}

	[[nodiscard]]
	inline abc::ISQLDriver* driver() const
	{
		return this->db.get();
	}

	// Insert into database.
	template <ModelBasedType ModelT>
	inline void insert_one(const ModelT& model) const
	{
		q::insert<ModelT>(this->db.get(), model).one();
	}

	template <ModelBasedType ModelT, typename PkT>
	inline void insert_one(const ModelT& model, PkT* pk) const
	{
		auto pk_str = q::insert<ModelT>(this->db.get(), model).one();
		if (pk != nullptr)
		{
			*pk = xw::object::as<PkT>((const void*)pk_str.c_str());
		}
	}

	template <ModelBasedType ModelT>
	inline q::insert<ModelT> insert(const ModelT& model) const
	{
		return q::insert<ModelT>(this->db.get(), model);
	}

	template <ModelBasedType ModelT, typename IteratorBegin, typename IteratorEnd>
	inline void insert_bulk(IteratorBegin begin, IteratorEnd end) const
	{
		auto query = q::insert<ModelT>(this->db.get(), *begin++);
		std::for_each(begin, end, [&query](const ModelT& model) -> void {
			query.model(model);
		});
		query.bulk();
	}

	// Retrieve from database.
	template <ModelBasedType ModelT>
	inline ModelT get(const q::condition& cond) const
	{
		return q::select<ModelT>(this->db.get()).where(cond).first();
	}

	template <ModelBasedType ModelT>
	inline q::select<ModelT> select() const
	{
		return q::select<ModelT>(this->db.get());
	}

	template <ModelBasedType ModelT>
	inline q::select<ModelT> filter(const q::condition& cond) const
	{
		return q::select<ModelT>(this->db.get()).where(cond);
	}
};

__ORM_END__
