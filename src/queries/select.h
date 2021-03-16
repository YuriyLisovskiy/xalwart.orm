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
	q_value<bool> q_distinct;

	// Holds boolean condition for SQL 'WHERE' statement.
	q_value<q::condition_t> q_where;

	// Holds columns list for SQL 'ORDER BY' statement.
	q_value<std::initializer_list<q::ordering>> q_order_by;

	// Holds value for SQL 'LIMIT'. The default is -1.
	q_value<long int> q_limit;

	// Holds value for SQL 'OFFSET'. The default is -1.
	q_value<long int> q_offset;

	// Holds columns list for SQL 'GROUP BY' statement.
	q_value<std::initializer_list<std::string>> q_group_by;

	// Holds boolean condition for SQL 'HAVING' statement.
	q_value<q::condition_t> q_having;

	// Holds a list of conditions for SQL 'JOIN' statement.
	std::vector<q::join_t> joins;

	typedef std::function<void(ModelT& model)> relation_callable;

	// Holds a list of lambda-functions which must be
	// called for each selected object to set lazy
	// initializers.
	std::vector<relation_callable> relations;

public:

	// Retrieves table name and sets the default values.
	inline explicit select()
	{
		this->table_name = get_table_name<ModelT>();
		this->pk_name = get_pk_name<ModelT>();
		this->q_distinct.value = false;
		this->q_limit.value = -1;
		this->q_offset.value = -1;
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
			this->q_distinct.value,
			this->joins,
			this->q_where.value,
			this->q_order_by.value,
			this->q_limit.value,
			this->q_offset.value,
			this->q_group_by.value,
			this->q_having.value
		);
	}

	// Sets the distinct value.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual select& distinct()
	{
		if (this->q_distinct.is_set)
		{
			throw QueryError(
				"select: 'distinct' value is already set, check method call sequence",
				_ERROR_DETAILS_
			);
		}

		this->q_distinct.set(true);
		return *this;
	}

	// Sets SQL `join` condition of two tables. For more info,
	// check the `xw::q::join` class and related functions.
	inline select& join(q::join_t join)
	{
		this->joins.push_back(std::move(join));
		return *this;
	}

	// TESTME: one_to_many
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
	// `foreign_key`: is used for joining of `ModelT` with `OtherModelT`.
	// It is foreign key in `OtherModelT` to `ModelT` table.
	// If `foreign_key` is empty it will be generated automatically using
	// `ModelT::meta_table_name` without last char (usually 's')
	// and '_id' suffix. For example:
	//   `ModelT::meta_table_name` equals to 'persons', so, the result
	//   will be 'person_id'.
	template <ModelBasedType OtherModelT, typename PrimaryKeyT>
	inline select& one_to_many(
		const std::function<void(ModelT&, const xw::Lazy<std::vector<OtherModelT>>&)>& first,
		const std::function<void(OtherModelT&, const xw::Lazy<ModelT>&)>& second,
		std::string foreign_key=""
	)
	{
		if (foreign_key.empty())
		{
			foreign_key = this->table_name.substr(0, this->table_name.size() - 1) + "_id";
		}

		abc::ISQLDriver* driver = this->db;
		this->relations.push_back([driver, foreign_key, first, second](ModelT& model) -> void {
			auto pk_val = util::as<PrimaryKeyT>(
				model.__get_attr__(ModelT::meta_pk_name)->__str__().c_str()
			);
			first(model, xw::Lazy<std::vector<OtherModelT>>(
				[driver, foreign_key, pk_val, first, second]() -> std::vector<OtherModelT> {
					return select<OtherModelT>().use(driver)
						.template many_to_one<ModelT, PrimaryKeyT>(second, first, foreign_key)
						.where(q::c<OtherModelT>(foreign_key) == pk_val)
						.to_vector();
				}
			));
		});
		return *this;
	}

	// TESTME: one_to_many (simplified)
	// Simplified `one_to_many` method where lambdas are generated
	// automatically.
	//
	// For more details, read the above method's doc.
	template <typename PrimaryKeyT, ModelBasedType OtherModelT>
	inline select& one_to_many(
		xw::Lazy<std::vector<OtherModelT>> ModelT::*left,
		xw::Lazy<ModelT> OtherModelT::*right,
		std::string foreign_key=""
	)
	{
		return this->template one_to_many<OtherModelT, PrimaryKeyT>(
			[left](ModelT& model, const xw::Lazy<std::vector<OtherModelT>>& value) {
				model.*left = value;
			},
			[right](OtherModelT& model, const xw::Lazy<ModelT>& value) {
				model.*right = value;
			},
			foreign_key
		);
	}

	// TESTME: many_to_one
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
	// `foreign_key`: is used for joining of `ModelT` with `OtherModelT`.
	// It is foreign key in `ModelT` to `OtherModelT` table.
	// If `foreign_key` is empty it will be generated automatically using
	// `OtherModelT::meta_table_name` without last char (usually 's')
	// and '_id' suffix. For example:
	//   `OtherModelT::meta_table_name` equals to 'persons', so, the result
	//   will be 'person_id'.
	template <ModelBasedType OtherModelT, typename PrimaryKeyT>
	inline select& many_to_one(
		const std::function<void(ModelT&, const xw::Lazy<OtherModelT>&)>& first,
		const std::function<void(OtherModelT&, const xw::Lazy<std::vector<ModelT>>&)>& second,
		std::string foreign_key=""
	)
	{
		if (foreign_key.empty())
		{
			std::string other_table_name = OtherModelT::meta_table_name;
			foreign_key = other_table_name.substr(0, other_table_name.size() - 1) + "_id";
		}

		abc::ISQLDriver* driver = this->db;
		auto t_name = this->table_name;
		this->relations.push_back([driver, first, second, t_name, foreign_key](ModelT& model) -> void {
			auto model_pk_val = util::as<PrimaryKeyT>(model.__get_attr__(
				ModelT::meta_pk_name)->__str__().c_str()
			);
			first(model, xw::Lazy<OtherModelT>(
				[driver, first, second, t_name, foreign_key, model_pk_val]() -> OtherModelT {
					return select<OtherModelT>().use(driver)
						.join(q::left_on<OtherModelT, ModelT>(foreign_key))
						.template one_to_many<ModelT, PrimaryKeyT>(second, first, foreign_key)
						.where(q::c<ModelT>(ModelT::meta_pk_name) == model_pk_val)
						.first();
				}
			));
		});
		return *this;
	}

	// TESTME: many_to_one (simplified)
	// Simplified `many_to_one` method where lambdas are generated
	// automatically.
	//
	// For more details, read the above method's doc.
	template <typename PrimaryKeyT, ModelBasedType OtherModelT>
	inline select& many_to_one(
		xw::Lazy<OtherModelT> ModelT::*left,
		xw::Lazy<std::vector<ModelT>> OtherModelT::*right,
		std::string foreign_key=""
	)
	{
		return this->template many_to_one<OtherModelT, PrimaryKeyT>(
			[left](ModelT& model, const xw::Lazy<OtherModelT>& value) {
				model.*left = value;
			},
			[right](OtherModelT& model, const xw::Lazy<std::vector<ModelT>>& value) {
				model.*right = value;
			},
			foreign_key
		);
	}

	// TESTME: many_to_many
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
	// `left_fk`: is used for joining of `ModelT` with intermediate
	// table. It is foreign key in middle table to `ModelT` table.
	// If `left_fk` is empty it will be generated automatically
	// using `ModelT::meta_table_name` without last char (usually 's')
	// and '_id' suffix. For example:
	//   `ModelT::meta_table_name` equals to 'persons', so, the result
	//   will be 'person_id'.
	//
	// `right_fk`: is used for joining of `OtherModelT` with intermediate
	// table. It is foreign key in middle table to `OtherModelT` table.
	// If `right_fk` is empty it will be generated automatically
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
		std::string left_fk="", std::string right_fk="", std::string mid_table=""
	)
	{
		abc::ISQLDriver* driver = this->db;
		auto first_t_name = this->table_name;
		auto first_pk_name = this->pk_name;
		this->relations.push_back(
			[
				driver, first_t_name, first_pk_name, left_fk, right_fk, first, second, mid_table
			](ModelT& model) -> void {
				first(model, Lazy<std::vector<OtherModelT>>(
					[
						driver, first_t_name, first_pk_name, left_fk, right_fk, first, second, mid_table
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

						std::string s_pk = left_fk;
						if (s_pk.empty())
						{
							s_pk = first_t_name.substr(0, first_t_name.size() - 1) + "_id";
						}

						std::string o_pk = right_fk;
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

	// TESTME: many_to_many (simplified)
	// Simplified `many_to_many` method where lambdas are generated
	// automatically.
	//
	// For more details, read the above method's doc.
	template <ModelBasedType OtherModelT>
	inline select& many_to_many(
		Lazy<std::vector<OtherModelT>> ModelT::*left,
		Lazy<std::vector<ModelT>> OtherModelT::*right,
		std::string left_fk="", std::string right_fk="", std::string mid_table=""
	)
	{
		return this->template many_to_many<OtherModelT>(
			[left](ModelT& model, const Lazy<std::vector<OtherModelT>>& value) {
				model.*left = value;
			},
			[right](OtherModelT& model, const Lazy<std::vector<ModelT>>& value) {
				model.*right = value;
			},
			left_fk, right_fk, mid_table
		);
	}

	// Sets the condition for 'where' filtering.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual select& where(const q::condition_t& cond)
	{
		if (this->q_where.is_set)
		{
			throw QueryError(
				"select: 'where' condition is already set, check method call sequence",
				_ERROR_DETAILS_
			);
		}

		this->q_where.set(cond);
		return *this;
	}

	// Sets columns for ordering.
	//
	// Throws 'QueryError' if this method is called
	// more than once with non-empty columns list.
	inline virtual select& order_by(const std::initializer_list<q::ordering>& columns)
	{
		if (this->q_order_by.is_set)
		{
			throw QueryError(
				"select: columns for ordering is already set, check method call sequence",
				_ERROR_DETAILS_
			);
		}

		if (columns.size())
		{
			this->q_order_by.set(columns);
		}

		return *this;
	}

	// Sets the limit value.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual select& limit(size_t limit)
	{
		if (this->q_limit.is_set)
		{
			throw QueryError(
				"select: 'limit' value is already set, check method call sequence",
				_ERROR_DETAILS_
			);
		}

		this->q_limit.set(limit);
		return *this;
	}

	// Sets the offset value.
	//
	// Throws 'QueryError' if this method is called
	// more than once with positive value.
	inline virtual select& offset(size_t offset)
	{
		if (this->q_offset.is_set)
		{
			throw QueryError(
				"select: 'offset' value is already set, check method call sequence",
				_ERROR_DETAILS_
			);
		}

		if (offset > 0)
		{
			this->q_offset.set(offset);
		}

		return *this;
	}

	// Sets columns for grouping.
	//
	// Throws 'QueryError' if this method is called
	// more than once with non-empty columns list.
	inline virtual select& group_by(const std::initializer_list<std::string>& columns)
	{
		if (this->q_group_by.is_set)
		{
			throw QueryError(
				"select: columns for grouping is already set, check method call sequence",
				_ERROR_DETAILS_
			);
		}

		if (columns.size())
		{
			this->q_group_by.set(columns);
		}

		return *this;
	}

	// Sets the condition for 'having' filtering.
	//
	// Throws 'QueryError' if this method is called more than once.
	inline virtual select& having(const q::condition_t& cond)
	{
		if (this->q_having.is_set)
		{
			throw QueryError(
				"select: 'having' condition is already set, check method call sequence",
				_ERROR_DETAILS_
			);
		}

		this->q_having.set(cond);
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
		if (!this->q_limit.is_set)
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
