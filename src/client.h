/**
 * client.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: database client which uses SQL driver for
 * 	accessing the database.
 */

#pragma once

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

	// SQL driver for accessing the database.
	std::shared_ptr<abc::ISQLDriver> db;

public:

	// Sets driver.
	//
	// Throws 'NullPointerException' if the driver was not initialized.
	inline explicit Client(std::shared_ptr<abc::ISQLDriver> driver) : db(std::move(driver))
	{
		if (!this->db)
		{
			throw core::NullPointerException(
				"DbClient: driver must be instantiated", _ERROR_DETAILS_
			);
		}
	}

	// Returns pointer to the SQL driver.
	[[nodiscard]]
	inline abc::ISQLDriver* driver() const
	{
		return this->db.get();
	}

	// Inserts one model to the database.
	template <ModelBasedType ModelT>
	inline void insert_one(const ModelT& model) const
	{
		q::insert<ModelT>(this->db.get(), model).one();
	}

	// Inserts one model to the database and writes
	// last inserted primary key to 'pk' out argument.
	template <ModelBasedType ModelT, typename PkT>
	inline void insert_one(const ModelT& model, PkT* pk) const
	{
		auto pk_str = q::insert<ModelT>(this->db.get(), model).one();
		if (pk != nullptr)
		{
			*pk = util::as<PkT>((const void*)pk_str.c_str());
		}
	}

	// Creates 'insert' statement object with initialized driver.
	template <ModelBasedType ModelT>
	inline q::insert<ModelT> insert(const ModelT& model) const
	{
		return q::insert<ModelT>(this->db.get(), model);
	}

	// Inserts the list of models to the database.
	template <ModelBasedType ModelT, typename IteratorBegin, typename IteratorEnd>
	inline void insert_bulk(IteratorBegin begin, IteratorEnd end) const
	{
		auto query = q::insert<ModelT>(this->db.get(), *begin++);
		std::for_each(begin, end, [&query](const ModelT& model) -> void {
			query.model(model);
		});
		query.bulk();
	}

	// Retrieve first row from database.
	template <ModelBasedType ModelT>
	inline ModelT get(const q::condition& cond) const
	{
		return q::select<ModelT>(this->db.get()).where(cond).first();
	}

	// Creates 'select' statement object with initialized driver.
	template <ModelBasedType ModelT>
	inline q::select<ModelT> select() const
	{
		return q::select<ModelT>(this->db.get());
	}

	// Creates 'select' statement object with called 'where' method.
	// So, 'WHERE' condition is set and can not be changed.
	template <ModelBasedType ModelT>
	inline q::select<ModelT> filter(const q::condition& cond) const
	{
		return q::select<ModelT>(this->db.get()).where(cond);
	}
};

__ORM_END__
