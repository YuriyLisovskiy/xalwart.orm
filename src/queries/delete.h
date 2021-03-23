/**
 * queries/delete.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: wrapper for SQL 'DELETE' statement.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../abc.h"
#include "../exceptions.h"
#include "./conditions.h"


__Q_BEGIN__

template <typename PkT, ModelBasedType ModelT>
class delete_
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

protected:

	// Driver to perform an access to the database.
	abc::ISQLDriver* db = nullptr;

	// Holds condition for SQL 'WHERE' statement.
	q_value<condition_t> where_cond;

	// List of primary keys to delete. It will be used by
	// default if `where` is not called.
	std::vector<PkT> pks{};

	// Requires when generating condition.
	PkT ModelT::* pk_member_ptr;

protected:
	virtual inline void append_model(const ModelT& model)
	{
		if (model.is_null())
		{
			throw QueryError("delete: unable to delete null model", _ERROR_DETAILS_);
		}

		this->pks.push_back(model.*this->pk_member_ptr);
	}

public:
	inline explicit delete_(PkT ModelT::* pk = &ModelT::id) : pk_member_ptr(pk)
	{
		if (!this->pk_member_ptr)
		{
			throw QueryError("delete: 'pk' member pointer is nullptr", _ERROR_DETAILS_);
		}
	};

	// Appends model's pk to deletion list.
	inline explicit delete_(const ModelT& model, PkT ModelT::* pk = &ModelT::id) : pk_member_ptr(pk)
	{
		if (!this->pk_member_ptr)
		{
			throw QueryError("delete: 'pk' member pointer is nullptr", _ERROR_DETAILS_);
		}

		this->append_model(model);
	};

	// Sets SQL driver.
	inline virtual delete_& use(abc::ISQLDriver* driver)
	{
		if (driver)
		{
			this->db = driver;
		}

		return *this;
	}

	// Generates query using SQL driver.
	//
	// Throws 'QueryError' when driver is not set.
	[[nodiscard]]
	virtual inline std::string query()
	{
		if (!this->db)
		{
			throw QueryError("delete: database driver not set", _ERROR_DETAILS_);
		}

		if (!this->where_cond.is_set)
		{
			this->where_cond.set(in(this->pk_member_ptr, this->pks.begin(), this->pks.end()));
		}

		return this->db->make_delete_query(
			meta::get_table_name<ModelT>(), this->where_cond.value
		);
	}

	// Appends model's pk to deletion list.
	virtual inline delete_& model(const ModelT& model)
	{
		this->append_model(model);
		return *this;
	}

	// Sets the condition for 'where' filtering.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual delete_& where(const condition_t& cond)
	{
		if (this->where_cond.is_set)
		{
			throw QueryError(
				"delete: 'where' condition is already set, check method call sequence",
				_ERROR_DETAILS_
			);
		}

		this->where_cond.set(cond);
		return *this;
	}

	// Performs the deletion.
	// If no models were set, executes `DELETE` without
	// condition, otherwise generates it from primary keys
	// if it was not set manually.
	inline virtual void exec()
	{
		auto query = this->query();
		this->db->run_delete(query);
	}
};

__Q_END__
