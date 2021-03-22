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
#include "./queries/update.h"
#include "./queries/delete.h"


__ORM_BEGIN__

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
	template <typename ModelT>
	inline void insert_one(const ModelT& model) const
	{
		q::insert<ModelT>(model).use(this->db.get()).one();
	}

	// Inserts one model to the database and writes
	// last inserted primary key to 'pk' out argument.
	template <typename ModelT, typename PkT>
	inline void insert_one(ModelT& model, PkT ModelT::* pk_member_ptr) const
	{
		auto pk_str = q::insert<ModelT>(model).use(this->db.get()).one();
		if (pk_member_ptr)
		{
			model.*pk_member_ptr = util::as<PkT>((const void*)pk_str.c_str());
		}
	}

	// Creates 'insert' statement object with initialized driver.
	template <typename ModelT>
	inline q::insert<ModelT> insert(const ModelT& model) const
	{
		return q::insert<ModelT>(model).use(this->db.get());
	}

	// Inserts the list of models to the database.
	template <typename IterBegin, typename IterEnd>
	inline void insert(IterBegin begin, IterEnd end) const
	{
		using ModelT = typename std::iterator_traits<IterBegin>::value_type;
		auto query = q::insert<ModelT>(*begin++).use(this->db.get());
		std::for_each(begin, end, [&query](const ModelT& model) -> void {
			query.model(model);
		});
		query.bulk();
	}

	// Retrieve first row from database.
	template <typename ModelT>
	inline ModelT get(const q::condition_t& cond) const
	{
		return q::select<ModelT>().use(this->db.get()).where(cond).first();
	}

	// Creates 'select' statement object with initialized driver.
	template <typename ModelT>
	inline q::select<ModelT> select() const
	{
		return q::select<ModelT>().use(this->db.get());
	}

	// Creates 'select' statement object with called 'where' method.
	// So, 'WHERE' condition is set and can not be changed.
	template <typename ModelT>
	inline q::select<ModelT> filter(const q::condition_t& cond) const
	{
		return q::select<ModelT>().use(this->db.get()).where(cond);
	}

	// Updates single model.
	template <typename ModelT>
	inline void update(const ModelT& model) const
	{
		q::update<ModelT>(model).use(this->db.get()).exec();
	}

	// Creates 'delete_' statement object with initialized driver.
	template <typename PkT, typename ModelT>
	inline q::delete_<PkT, ModelT> delete_(PkT ModelT::* pk = &ModelT::id) const
	{
		return q::delete_<PkT, ModelT>(pk).use(this->db.get());
	}

	// Deletes list of models using iterator.
	template <
		typename PkT, typename IteratorT,
		typename ModelT = typename std::iterator_traits<IteratorT>::value_type
	>
	inline void delete_(IteratorT begin, IteratorT end, PkT ModelT::* pk = &ModelT::id) const
	{
		auto query = q::delete_<PkT, ModelT>().use(this->db.get());
		std::for_each(begin, end, [&query](const ModelT& model) -> void {
			query.model(model);
		});
		query.exec();
	}

	// Deletes list of models using initializer list.
	template <typename PkT, typename ModelT>
	inline void delete_(const std::initializer_list<ModelT>& list, PkT ModelT::* pk = &ModelT::id) const
	{
		using ConstIterator = typename std::initializer_list<ModelT>::const_iterator;
		this->template delete_<PkT, ConstIterator>(list.begin(), list.end());
	}

	// Deletes single model.
	template <typename PkT, typename ModelT>
	inline void delete_(const ModelT& model, PkT ModelT::* pk = &ModelT::id) const
	{
		q::delete_<PkT, ModelT>().use(this->db.get()).model(model).exec();
	}
};

__ORM_END__
