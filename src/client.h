/**
 * client.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Database client which uses SQL driver for accessing the database.
 */

#pragma once

// Base libraries.
#include <xalwart.base/utility.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./abc.h"
#include "./queries/insert.h"
#include "./queries/select.h"
#include "./queries/update.h"
#include "./queries/delete.h"
#include "./db/model.h"


__ORM_BEGIN__

// TESTME: Client
// TODO: docs for 'Client'
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
			throw NullPointerException("xw::orm::Client: driver must be instantiated", _ERROR_DETAILS_);
		}
	}

	// TESTME: driver
	// Returns pointer to the SQL driver.
	[[nodiscard]]
	inline abc::ISQLDriver* driver() const
	{
		return this->db.get();
	}

	// TESTME: insert_one
	// Inserts one model to the database.
	template <db::model_based_type ModelT>
	inline void insert_one(const ModelT& model) const
	{
		q::insert<ModelT>(model).use(this->driver()).commit_one();
	}

	// TESTME: insert_one (with pk)
	// Inserts one model to the database and writes
	// last inserted primary key to 'pk' out argument.
	template <db::model_based_type ModelT, typename PkT>
	inline void insert_one(ModelT& model, PkT ModelT::* pk_member_ptr) const
	{
		auto pk_str = q::insert<ModelT>(model).use(this->driver()).one();
		if (pk_member_ptr)
		{
			model.*pk_member_ptr = xw::util::as<PkT>((const void*)pk_str.c_str());
		}
	}

	// TESTME: insert
	// Creates 'insert' statement object with initialized driver.
	template <db::model_based_type ModelT>
	inline q::insert<ModelT> insert(const ModelT& model) const
	{
		return q::insert<ModelT>(model).use(this->driver());
	}

	// TESTME: insert (using iterator)
	// Inserts the list of models to the database.
	template <db::model_based_iterator IteratorT>
	inline void insert(IteratorT begin, IteratorT end) const
	{
		if (begin == end)
		{
			return;
		}

		using ModelT = iterator_v_type<IteratorT>;
		auto query = q::insert<ModelT>(*begin++).use(this->db.get());
		std::for_each(begin, end, [&query](const ModelT& model) -> void {
			query.model(model);
		});
		query.commit_batch();
	}

	// TESTME: get
	// Retrieve first row from database.
	template <db::model_based_type ModelT>
	inline ModelT get(const q::Condition& cond) const
	{
		return q::select<ModelT>().use(this->driver()).where(cond).first();
	}

	// TESTME: select
	// Creates 'select' statement object with initialized driver.
	template <db::model_based_type ModelT>
	inline q::select<ModelT> select() const
	{
		return q::select<ModelT>().use(this->driver());
	}

	// TESTME: filter
	// Creates 'select' statement object with called 'where' method.
	// So, 'WHERE' condition is set and can not be changed.
	template <db::model_based_type ModelT>
	inline q::select<ModelT> filter(const q::Condition& cond) const
	{
		return q::select<ModelT>().use(this->driver()).where(cond);
	}

	// TESTME: update
	// Creates 'update' statement object with initialized driver.
	template <db::model_based_type ModelT>
	inline q::update<ModelT> update(const ModelT& model) const
	{
		return q::update<ModelT>(model).use(this->driver());
	}

	// TESTME: update (using iterator)
	// Deletes list of models using iterator.
	template <
		db::model_based_iterator IteratorT, db::model_based_type ModelT = iterator_v_type<IteratorT>
	>
	inline void update(IteratorT begin, IteratorT end) const
	{
		if (begin == end)
		{
			return;
		}

		auto query = q::update<ModelT>(*begin++).use(this->db.get());
		std::for_each(begin, end, [&query](const ModelT& model) -> void {
			query.model(model);
		});
		query.commit_batch();
	}

	// TESTME: update (using initializer list)
	// Updates list of models using initializer list.
	template <db::model_based_type ModelT>
	inline void update(const std::initializer_list<ModelT>& list) const
	{
		using ConstIterator = typename std::initializer_list<ModelT>::const_iterator;
		this->template update<ConstIterator>(list.begin(), list.end());
	}

	// TESTME: delete_
	// Creates 'delete_' statement object with initialized driver.
	template <db::model_based_type ModelT>
	inline q::delete_<ModelT> delete_(const ModelT& model) const
	{
		return q::delete_<ModelT>(model).use(this->driver());
	}

	// TESTME: delete_ (using iterator)
	// Deletes list of models using iterator.
	template <
		db::model_based_iterator IteratorT, db::model_based_type ModelT = iterator_v_type<IteratorT>
	>
	inline void delete_(IteratorT begin, IteratorT end) const
	{
		if (begin == end)
		{
			return;
		}

		auto query = q::delete_<ModelT>(*begin++).use(this->driver());
		std::for_each(begin, end, [&query](const ModelT& model) -> void {
			query.model(model);
		});
		query.commit();
	}

	// TESTME: delete_ (using initializer list)
	// Deletes list of models using initializer list.
	template <db::model_based_type ModelT>
	inline void delete_(const std::initializer_list<ModelT>& list) const
	{
		using ConstIterator = typename std::initializer_list<ModelT>::const_iterator;
		this->template delete_<ConstIterator>(list.begin(), list.end());
	}

	// TESTME: avg
	// Calculates average value of given column in selected rows.
	template <db::model_based_type ModelT, db::column_field_type ColumnT>
	inline auto avg(ColumnT ModelT::* column) const
	{
		return q::select<ModelT>().use(this->driver()).avg(column);
	}

	// TESTME: count
	// Calculates selected rows.
	template <db::model_based_type ModelT>
	inline auto count() const
	{
		return q::select<ModelT>().use(this->driver()).count();
	}

	// TESTME: min
	// Calculates minimum value of given column in selected rows.
	template <db::model_based_type ModelT, db::column_field_type ColumnT>
	inline auto min(ColumnT ModelT::* column) const
	{
		return q::select<ModelT>().use(this->driver()).min(column);
	}

	// TESTME: max
	// Calculates maximum value of given column in selected rows.
	template <db::model_based_type ModelT, db::column_field_type ColumnT>
	inline auto max(ColumnT ModelT::* column) const
	{
		return q::select<ModelT>().use(this->driver()).max(column);
	}

	// TESTME: sum
	// Calculates sum by column of selected rows.
	template <db::model_based_type ModelT, db::column_field_type ColumnT>
	inline auto sum(ColumnT ModelT::* column) const
	{
		return q::select<ModelT>().use(this->driver()).sum(column);
	}
};

__ORM_END__
