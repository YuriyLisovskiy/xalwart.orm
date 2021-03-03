/**
 * query/select.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: wrapper for SQL 'SELECT' statement.
 */

#pragma once

// Core libraries.
#include <xalwart.core/types/string.h>
#include <xalwart.core/object/utility.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../abc.h"
#include "../exceptions.h"
#include "./operations.h"
#include "./utility.h"


__Q_BEGIN__

template <ModelBasedType ModelT>
class select
{
protected:

	// Driver to perform an access to the database.
	abc::ISQLDriver* db = nullptr;

	// Retrieves automatically, check the default constructor.
	std::string table_name;

	// Retrieves automatically from Model meta,
	// check the default constructor.
	std::string pk_name;

	// Holds pair {value, is_set}:
	//  - value: actual value which will be forwarded to query generator;
	//  - is_set: indicates if the value is set or not.
	template <typename T>
	struct pair
	{
		T value;
		bool is_set = false;

		inline void set(T v)
		{
			this->value = std::move(v);
			this->is_set = true;
		}
	};

	// Indicates whether to use distinction or not in SQL statement.
	// The default is false.
	pair<bool> distinct_;

	// Holds boolean condition for SQL 'WHERE' statement.
	pair<q::condition> where_cond_;

	// Holds columns list for SQL 'ORDER BY' statement.
	pair<std::initializer_list<q::ordering>> order_by_cols_;

	// Holds value for SQL 'LIMIT'. The default is -1.
	pair<long int> limit_;

	// Holds value for SQL 'OFFSET'. The default is -1.
	pair<long int> offset_;

	// Holds columns list for SQL 'GROUP BY' statement.
	pair<std::initializer_list<std::string>> group_by_cols_;

	// Holds boolean condition for SQL 'HAVING' statement.
	pair<q::condition> having_cond_;

	// Holds a list of conditions for SQL 'JOIN' statement.
	std::vector<q::join> joins_;

	// TODO: write clear docs!
	// Holds a list of lambda-functions which must be
	// called for each selected object.
	typedef std::function<void(ModelT& model)> include_lambda;
	std::vector<include_lambda> included_relations;

public:

	// Retrieves table name and sets the default values.
	inline explicit select()
	{
		this->table_name = utility::get_table_name<ModelT>();
		this->pk_name = utility::get_pk_name<ModelT>();
		this->distinct_.value = false;
		this->limit_.value = -1;
		this->offset_.value = -1;
	};

	// Sets SQL driver and calls the default constructor.
	inline explicit select(abc::ISQLDriver* driver) : select()
	{
		this->db = driver;
	};

	// Sets the distinct value.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual select& distinct()
	{
		if (this->distinct_.is_set)
		{
			throw QueryError(
				"'distinct' value is already set, check method call sequence", _ERROR_DETAILS_
			);
		}

		this->distinct_.set(true);
		return *this;
	}

	// TODO: !experimental feature!
	template <ModelBasedType OtherModelT, typename PrimaryKeyT>
	inline select& one_to_many(
		std::function<void(ModelT&, const std::vector<OtherModelT>&)> lambda,
		std::string select_pk=""
	)
	{
		if (select_pk.empty())
		{
			select_pk = this->table_name.substr(0, this->table_name.size() - 1) + "_id";
		}

		this->included_relations.push_back([&](ModelT& model) -> void {
			lambda(model, select<OtherModelT>().using_(this->db)
				.where(q::equals(
					select_pk, object::as<PrimaryKeyT>(
						model.__get_attr__(ModelT::meta_pk_name)->__str__().c_str()
					)
				))
				.to_vector()
			);
		});
		return *this;
	}

	// TODO: !experimental feature!
	template <ModelBasedType OtherModelT, typename PrimaryKeyT>
	inline select& many_to_one(
		std::function<void(ModelT&, const OtherModelT&)> lambda,
		std::string select_pk=""
	)
	{
		if (select_pk.empty())
		{
			std::string other_table_name = OtherModelT::meta_table_name;
			select_pk = other_table_name.substr(0, other_table_name.size() - 1) + "_id";
		}

		this->included_relations.push_back([&](ModelT& model) -> void {
			auto model_pk = object::as<PrimaryKeyT>(model.__get_attr__(
				ModelT::meta_pk_name)->__str__().c_str()
			);
			lambda(model, select<OtherModelT>().using_(this->db)
				.join(q::left<OtherModelT, ModelT>(select_pk))
				.where(q::equals(this->table_name + "\".\"" + ModelT::meta_pk_name, model_pk))
				.first()
			);
		});
		return *this;
	}

	// Retrieves a list of models connected with `Many To Many`
	// relationship into std::vector<OtherModelT>.
	//
	// First argument, a lambda function, is used to set the vector
	// to each of the selected `ModelT` objects via an address to
	// class field.
	//
	// The second argument is `select_pk` which is used for joining
	// of `ModelT` with `OtherModelT`. Generally, it is foreign key
	// in `OtherModelT` to `ModelT` table. If `select_pk` is empty
	// it will be generated automatically using
	// `ModelT::meta_table_name` without last char (usually 's')
	// and '_id' suffix. For example:
	//   `ModelT::meta_table_name` equals to 'persons', so, the result
	//   will be 'person_id'.
	//
	// Middle table is created using `ModelT::meta_table_name` and
	// `OtherModelT::meta_table_name` in alphabetical order separated
	// by underscore ('_'). For example:
	//   `ModelT::meta_table_name` is 'persons' and `OtherModelT::meta_table_name`
	//   is 'cars', so, the result will be 'cars_persons'.
	template <ModelBasedType OtherModelT>
	inline select& many_to_many(
		std::function<void(ModelT&, const std::vector<OtherModelT>&)> lambda,
		std::string select_pk=""
	)
	{
		std::string left_prefix = OtherModelT::meta_table_name;
		std::string middle_table;
		if (this->table_name < left_prefix)
		{
			middle_table = this->table_name + "_" + left_prefix;
		}
		else
		{
			middle_table = left_prefix + "_" + this->table_name;
		}

		if (select_pk.empty())
		{
			select_pk = this->table_name.substr(0, this->table_name.size() - 1) + "_id";
		}

		this->included_relations.push_back(
			[this, left_prefix, middle_table, select_pk, lambda](ModelT& model) -> void {
				lambda(model, select<OtherModelT>().using_(this->db)
					.distinct()
					.join({"LEFT", middle_table, q::condition(
						'"' + left_prefix + "\".\"" + this->pk_name + "\" = \"" + middle_table + "\".\"" + select_pk + '"'
					)})
					.to_vector()
				);
			}
		);
		return *this;
	}

	// Sets SQL `join` condition of two tables. For more info,
	// check the `xw::q::join` class and related functions.
	inline select& join(q::join join_row)
	{
		this->joins_.push_back(std::move(join_row));
		return *this;
	}

	// Sets the condition for 'where' filtering.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual select& where(const q::condition& cond)
	{
		if (this->where_cond_.is_set)
		{
			throw QueryError(
				"'where' condition is already set, check method call sequence", _ERROR_DETAILS_
			);
		}

		this->where_cond_.set(cond);
		return *this;
	}

	// Sets columns for ordering.
	//
	// Throws 'QueryError' if this method is called
	// more than once with non-empty columns list.
	inline virtual select& order_by(const std::initializer_list<q::ordering>& columns)
	{
		if (this->order_by_cols_.is_set)
		{
			throw QueryError(
				"columns for ordering is already set, check method call sequence", _ERROR_DETAILS_
			);
		}

		if (columns.size())
		{
			this->order_by_cols_.set(columns);
		}

		return *this;
	}

	// Sets the limit value.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual select& limit(size_t limit)
	{
		if (this->limit_.is_set)
		{
			throw QueryError(
				"'limit' value is already set, check method call sequence", _ERROR_DETAILS_
			);
		}

		this->limit_.set(limit);
		return *this;
	}

	// Sets the offset value.
	//
	// Throws 'QueryError' if this method is called
	// more than once with positive value.
	inline virtual select& offset(size_t offset)
	{
		if (this->offset_.is_set)
		{
			throw QueryError(
				"'offset' value is already set, check method call sequence", _ERROR_DETAILS_
			);
		}

		if (offset > 0)
		{
			this->offset_.set(offset);
		}

		return *this;
	}

	// Sets columns for grouping.
	//
	// Throws 'QueryError' if this method is called
	// more than once with non-empty columns list.
	inline virtual select& group_by(const std::initializer_list<std::string>& columns)
	{
		if (this->group_by_cols_.is_set)
		{
			throw QueryError(
				"columns for grouping is already set, check method call sequence", _ERROR_DETAILS_
			);
		}

		if (columns.size())
		{
			this->group_by_cols_.set(columns);
		}

		return *this;
	}

	// Sets the condition for 'having' filtering.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual select& having(const q::condition& cond)
	{
		if (this->having_cond_.is_set)
		{
			throw QueryError(
				"'having' condition is already set, check method call sequence", _ERROR_DETAILS_
			);
		}

		this->having_cond_.set(cond);
		return *this;
	}

	// Sets SQL driver.
	inline virtual select& using_(abc::ISQLDriver* driver)
	{
		if (driver)
		{
			this->db = driver;
		}

		return *this;
	}

	// Set limit value to 1, if it was not already set
	// and retrieves the first item of 'to_vector()'
	// result. In case if 'to_vector()' returns an empty
	// vector, returns null-model.
	//
	// Throws 'QueryError' when driver is not set.
	inline virtual ModelT first()
	{
		// check if `limit(...)` was not called
		if (!this->limit_.is_set)
		{
			this->limit(1);
		}

		auto values = this->to_vector();
		if (values.empty())
		{
			ModelT model;
			model.mark_as_null();
			return model;
		}

		return values[0];
	}

	// Performs an access to database. Runs SQL 'SELECT'
	// query and returns its result.
	//
	// Throws 'QueryError' when driver is not set.
	inline virtual std::vector<ModelT> to_vector() const
	{
		auto query = this->query();
		using row_t = std::map<std::string, char*>;
		using data_t = std::pair<std::vector<ModelT>, std::vector<include_lambda>>;
		data_t collection{{}, this->included_relations};
		this->db->run_select(query, &collection, [](void* container_ptr, void* row_ptr) -> void {
			auto& container = *(data_t *)container_ptr;
			auto& row = *(row_t *)row_ptr;

			ModelT model;
			model.from_map(row);

			for (auto& lambda : container.second)
			{
				lambda(model);
			}

			container.first.push_back(model);
		});

		return collection.first;
	}

	// Generates query using SQL driver.
	//
	// Throws 'QueryError' when driver is not set.
	[[nodiscard]]
	inline virtual std::string query() const
	{
		if (!this->db)
		{
			throw QueryError("select: database client not set", _ERROR_DETAILS_);
		}

		return this->db->make_select_query(
			this->table_name,
			ModelT::meta_fields,
			this->distinct_.value,
			this->joins_,
			this->where_cond_.value,
			this->order_by_cols_.value,
			this->limit_.value,
			this->offset_.value,
			this->group_by_cols_.value,
			this->having_cond_.value
		);
	}
};

__Q_END__
