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
#include <xalwart.core/lazy.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../abc.h"


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

	// Indicates whether to use distinction or not in SQL statement.
	// The default is false.
	q_value<bool> distinct_;

	// Holds boolean condition for SQL 'WHERE' statement.
	q_value<q::condition_t> where_cond_;

	// Holds columns list for SQL 'ORDER BY' statement.
	q_value<std::initializer_list<q::ordering>> order_by_cols_;

	// Holds value for SQL 'LIMIT'. The default is -1.
	q_value<long int> limit_;

	// Holds value for SQL 'OFFSET'. The default is -1.
	q_value<long int> offset_;

	// Holds columns list for SQL 'GROUP BY' statement.
	q_value<std::initializer_list<std::string>> group_by_cols_;

	// Holds boolean condition for SQL 'HAVING' statement.
	q_value<q::condition_t> having_cond_;

	// Holds a list of conditions for SQL 'JOIN' statement.
	std::vector<q::join> joins;

	// Holds a list of lambda-functions which must be
	// called for each selected object to set lazy
	// initializers.
	typedef std::function<void(ModelT& model)> relation_callable;
	std::vector<relation_callable> relations;

public:

	// Retrieves table name and sets the default values.
	inline explicit select()
	{
		this->table_name = get_table_name<ModelT>();
		this->pk_name = get_pk_name<ModelT>();
		this->distinct_.value = false;
		this->limit_.value = -1;
		this->offset_.value = -1;
	};

	// Sets SQL driver and calls the default constructor.
	inline explicit select(abc::ISQLDriver* driver) : select()
	{
		this->db = driver;
	};

	// Sets SQL driver.
	inline virtual select& use(abc::ISQLDriver* driver)
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
	inline virtual std::string query() const
	{
		if (!this->db)
		{
			throw QueryError("select: database driver not set", _ERROR_DETAILS_);
		}

		return this->db->make_select_query(
			this->table_name,
			ModelT::meta_fields,
			this->distinct_.value,
			this->joins,
			this->where_cond_.value,
			this->order_by_cols_.value,
			this->limit_.value,
			this->offset_.value,
			this->group_by_cols_.value,
			this->having_cond_.value
		);
	}

	// Sets the distinct value.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual select& distinct()
	{
		if (this->distinct_.is_set)
		{
			throw QueryError(
				"select: 'distinct' value is already set, check method call sequence",
				_ERROR_DETAILS_
			);
		}

		this->distinct_.set(true);
		return *this;
	}

	// Sets SQL `join` condition of two tables. For more info,
	// check the `xw::q::join` class and related functions.
	inline select& join(q::join join_row)
	{
		this->joins.push_back(std::move(join_row));
		return *this;
	}

	// Retrieves models with lazy initialization connected
	// with one to many relationship.
	//
	// `first`: a lambda function, is used to set the vector
	// to each of the selected `ModelT` objects via an address to
	// class field of `OtherModelT`.
	//
	// `second`: a lambda function, is used to set an object
	// to each of the selected `OtherModelT` objects via an address to
	// class field of `ModelT`.
	//
	// `select_pk`: is used for joining of `ModelT` with `OtherModelT`.
	// It is foreign key in `OtherModelT` to `ModelT` table.
	// If `select_pk` is empty it will be generated automatically using
	// `ModelT::meta_table_name` without last char (usually 's')
	// and '_id' suffix. For example:
	//   `ModelT::meta_table_name` equals to 'persons', so, the result
	//   will be 'person_id'.
	template <ModelBasedType OtherModelT, typename PrimaryKeyT>
	inline select& one_to_many(
		const std::function<void(ModelT&, const xw::Lazy<std::vector<OtherModelT>>&)>& first,
		const std::function<void(OtherModelT&, const xw::Lazy<ModelT>&)>& second,
		std::string select_pk=""
	)
	{
		if (select_pk.empty())
		{
			select_pk = this->table_name.substr(0, this->table_name.size() - 1) + "_id";
		}

		abc::ISQLDriver* driver = this->db;
		this->relations.push_back([driver, select_pk, first, second](ModelT& model) -> void {
			auto pk_val = util::as<PrimaryKeyT>(
				model.__get_attr__(ModelT::meta_pk_name)->__str__().c_str()
			);
			first(model, xw::Lazy<std::vector<OtherModelT>>(
				[driver, select_pk, pk_val, first, second]() -> std::vector<OtherModelT> {
					return select<OtherModelT>().use(driver)
						.template many_to_one<ModelT, PrimaryKeyT>(second, first, select_pk)
						.where(q::c<ModelT>(select_pk) == pk_val)
						.to_vector();
				}
			));
		});
		return *this;
	}

	// Retrieves models with lazy initialization connected
	// with one to many relationship.
	//
	// `first`: a lambda function, is used to set an object
	// to each of the selected `ModelT` objects via an address to
	// class field of `OtherModelT`.
	//
	// `second`: a lambda function, is used to set the vector
	// to each of the selected `OtherModelT` objects via an address to
	// class field of `ModelT`.
	//
	// `select_pk`: is used for joining of `ModelT` with `OtherModelT`.
	// It is foreign key in `ModelT` to `OtherModelT` table.
	// If `select_pk` is empty it will be generated automatically using
	// `OtherModelT::meta_table_name` without last char (usually 's')
	// and '_id' suffix. For example:
	//   `OtherModelT::meta_table_name` equals to 'persons', so, the result
	//   will be 'person_id'.
	template <ModelBasedType OtherModelT, typename PrimaryKeyT>
	inline select& many_to_one(
		const std::function<void(ModelT&, const xw::Lazy<OtherModelT>&)>& first,
		const std::function<void(OtherModelT&, const xw::Lazy<std::vector<ModelT>>&)>& second,
		std::string other_pk=""
	)
	{
		if (other_pk.empty())
		{
			std::string other_table_name = OtherModelT::meta_table_name;
			other_pk = other_table_name.substr(0, other_table_name.size() - 1) + "_id";
		}

		abc::ISQLDriver* driver = this->db;
		auto t_name = this->table_name;
		this->relations.push_back([driver, first, second, t_name, other_pk](ModelT& model) -> void {
			auto model_pk_val = util::as<PrimaryKeyT>(model.__get_attr__(
				ModelT::meta_pk_name)->__str__().c_str()
			);
			first(model, xw::Lazy<OtherModelT>(
				[driver, first, second, t_name, other_pk, model_pk_val]() -> OtherModelT {
					return select<OtherModelT>().use(driver)
						.join(q::left<OtherModelT, ModelT>(other_pk))
						.template one_to_many<ModelT, PrimaryKeyT>(second, first, other_pk)
						.where(q::c<ModelT>(ModelT::meta_pk_name) == model_pk_val)
						.first();
				}
			));
		});
		return *this;
	}

	// Retrieves lists of models with lazy initialization
	// connected with many to many relationship.
	//
	// `first`: a lambda function which is used to set the vector
	// to each of the selected `ModelT` objects via an address to
	// class field of `OtherModelT`.
	//
	// `second`: a lambda function which is used to set the vector
	// to each of the selected `OtherModelT` objects via an address to
	// class field of `ModelT`.
	//
	// `select_pk`: is used for joining of `ModelT` with intermediate
	// table. It is foreign key in middle table to `ModelT` table.
	// If `select_pk` is empty it will be generated automatically
	// using `ModelT::meta_table_name` without last char (usually 's')
	// and '_id' suffix. For example:
	//   `ModelT::meta_table_name` equals to 'persons', so, the result
	//   will be 'person_id'.
	//
	// `other_pk`: is used for joining of `OtherModelT` with intermediate
	// table. It is foreign key in middle table to `OtherModelT` table.
	// If `other_pk` is empty it will be generated automatically
	// using `OtherModelT::meta_table_name` without last char
	// (usually 's') and '_id' suffix. For example:
	//   `OtherModelT::meta_table_name` equals to 'cars', so, the result
	//   will be 'car_id'.
	//
	// `mid_table`: an intermediate table for many to many relationship.
	// If it is empty, it will be created using `ModelT::meta_table_name`
	// and `OtherModelT::meta_table_name` in an alphabetical order separated
	// by underscore ('_'). For example:
	//   `ModelT::meta_table_name` is 'persons' and `OtherModelT::meta_table_name`
	//   is 'cars', so, the result will be 'cars_persons'.
	template <ModelBasedType OtherModelT>
	inline select& many_to_many(
		const std::function<void(ModelT&, const Lazy<std::vector<OtherModelT>>&)>& first,
		const std::function<void(OtherModelT&, const Lazy<std::vector<ModelT>>&)>& second,
		std::string select_pk="", std::string other_pk="", std::string mid_table=""
	)
	{
		abc::ISQLDriver* driver = this->db;
		auto first_t_name = this->table_name;
		auto first_pk_name = this->pk_name;
		this->relations.push_back(
			[
				driver, first_t_name, first_pk_name, select_pk, other_pk, first, second, mid_table
			](ModelT& model) -> void {
				first(model, Lazy<std::vector<OtherModelT>>(
					[
						driver, first_t_name, first_pk_name, select_pk, other_pk, first, second, mid_table
					]() -> std::vector<OtherModelT> {
						std::string second_t_name = OtherModelT::meta_table_name;
						std::string m_table = mid_table;
						if (m_table.empty())
						{
							if (first_t_name < second_t_name)
							{
								m_table = first_t_name + "_" + second_t_name;
							}
							else
							{
								m_table = second_t_name + "_" + first_t_name;
							}
						}

						std::string s_pk = select_pk;
						if (s_pk.empty())
						{
							s_pk = first_t_name.substr(0, first_t_name.size() - 1) + "_id";
						}

						std::string o_pk = other_pk;
						if (o_pk.empty())
						{
							o_pk = second_t_name.substr(0, second_t_name.size() - 1) + "_id";
						}

						auto cond_str = '"' + second_t_name + "\".\"" + first_pk_name
							+ "\" = \"" + m_table + "\".\"" + s_pk + '"';

						return select<OtherModelT>().use(driver)
							.distinct()
							.join({"LEFT", m_table, q::condition_t(cond_str)})
							.template many_to_many<ModelT>(second, first, o_pk, s_pk, m_table)
							.to_vector();
					}
				));
			}
		);
		return *this;
	}

	// Sets the condition for 'where' filtering.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual select& where(const q::condition_t& cond)
	{
		if (this->where_cond_.is_set)
		{
			throw QueryError(
				"select: 'where' condition is already set, check method call sequence",
				_ERROR_DETAILS_
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
				"select: columns for ordering is already set, check method call sequence",
				_ERROR_DETAILS_
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
				"select: 'limit' value is already set, check method call sequence",
				_ERROR_DETAILS_
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
				"select: 'offset' value is already set, check method call sequence",
				_ERROR_DETAILS_
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
				"select: columns for grouping is already set, check method call sequence",
				_ERROR_DETAILS_
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
	inline virtual select& having(const q::condition_t& cond)
	{
		if (this->having_cond_.is_set)
		{
			throw QueryError(
				"select: 'having' condition is already set, check method call sequence",
				_ERROR_DETAILS_
			);
		}

		this->having_cond_.set(cond);
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
		using data_t = std::pair<std::vector<ModelT>, std::vector<relation_callable>>;
		data_t collection{{}, this->relations};
		this->db->run_select(query, &collection, [](void* container_ptr, void* row_ptr) -> void {
			auto& container = *(data_t *)container_ptr;
			auto& row = *(row_t *)row_ptr;

			ModelT model;
			model.from_map(row);

			for (auto& callable : container.second)
			{
				callable(model);
			}

			container.first.push_back(model);
		});

		return collection.first;
	}
};

__Q_END__
