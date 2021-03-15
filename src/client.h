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
#include "./queries/delete.h"


__ORM_BEGIN__

template <typename IterT>
concept ModelTypeIterator = std::is_base_of_v<Model, typename std::iterator_traits<IterT>::value_type>;

class Client final
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
	template <ModelTypeIterator IterBegin, ModelTypeIterator IterEnd>
	inline void insert(IterBegin begin, IterEnd end) const
	{
		using ModelT = typename std::iterator_traits<IterBegin>::value_type;
		auto query = q::insert<ModelT>(this->db.get(), *begin++);
		std::for_each(begin, end, [&query](const ModelT& model) -> void {
			query.model(model);
		});
		query.bulk();
	}

	// Retrieve first row from database.
	template <ModelBasedType ModelT>
	inline ModelT get(const q::condition_t& cond) const
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
	inline q::select<ModelT> filter(const q::condition_t& cond) const
	{
		return q::select<ModelT>(this->db.get()).where(cond);
	}

	// Deletes list of models using iterator.
	template <ModelTypeIterator IteratorT>
	inline void delete_(IteratorT begin, IteratorT end) const
	{
		using ModelT = typename std::iterator_traits<IteratorT>::value_type;
		auto query = q::delete_<ModelT>().use(this->db.get());
		std::for_each(begin, end, [&query](const ModelT& model) -> void {
			query.model(model);
		});
		query.exec();
	}

	// Deletes list of models using initializer list.
	template <ModelBasedType ModelT>
	inline void delete_(const std::initializer_list<ModelT>& list) const
	{
		using ConstIterator = typename std::initializer_list<ModelT>::const_iterator;
		this->template delete_<ConstIterator>(list.begin(), list.end());
	}

	// Deletes single model.
	template <ModelBasedType ModelT>
	inline void delete_(const ModelT& model) const
	{
		q::delete_<ModelT>().use(this->db.get()).model(model).exec();
	}
};

__ORM_END__
