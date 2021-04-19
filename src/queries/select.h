/**
 * queries/select.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Wrapper for SQL 'SELECT' statement.
 */

#pragma once

// C++ libraries.
#include <list>

// Core libraries.
#include <xalwart.core/types/string.h>
#include <xalwart.core/lazy.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../abc.h"
#include "./functions.h"


__Q_BEGIN__

// TESTME: add tests with mocked driver
template <model_based_type_c ModelT>
class select final
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

protected:

	// Driver to perform an access to the database.
	abc::ISQLDriver* sql_driver = nullptr;

	abc::ISQLQueryBuilder* sql_builder = nullptr;

	// Retrieves automatically, check the default constructor.
	std::string table_name;

	// Retrieves automatically from Model meta,
	// check the default constructor.
	std::string pk_name;

	// Indicates whether to use distinction or not in SQL statement.
	// The default is false.
	bool q_distinct;

	// Holds boolean condition for SQL 'WHERE' statement.
	q_value<q::condition_t> q_where;

	// Holds columns list for SQL 'ORDER BY' statement.
	std::list<q::ordering> q_order_by;

	// Holds value for SQL 'LIMIT'. The default is -1.
	long int q_limit;

	// Holds value for SQL 'OFFSET'. The default is -1.
	long int q_offset;

	// Holds columns list for SQL 'GROUP BY' statement.
	std::list<std::string> q_group_by;

	// Holds boolean condition for SQL 'HAVING' statement.
	q_value<q::condition_t> q_having;

	// Holds a list of conditions for SQL 'JOIN' statement.
	std::list<q::join_t> joins;

	typedef std::function<void(ModelT& model)> relation_callable;

	// Holds a list of lambda-functions which must be
	// called for each selected object to set lazy
	// initializers.
	std::list<relation_callable> relations;

public:

	// Retrieves table name and sets the default values.
	inline explicit select()
	{
		this->table_name = meta::get_table_name<ModelT>();
		this->pk_name = meta::get_pk_name<ModelT>();
		if (this->pk_name.empty())
		{
			throw QueryError("select: model requires pk column", _ERROR_DETAILS_);
		}

		this->q_distinct = false;
		this->q_limit = -1;
		this->q_offset = -1;
	};

	// Sets SQL driver.
	inline select& use(abc::ISQLDriver* driver)
	{
		if (driver)
		{
			this->sql_driver = driver;
			if (!this->sql_driver)
			{
				throw QueryError("select: SQL driver is not set", _ERROR_DETAILS_);
			}

			this->sql_builder = driver->query_builder();
			if (!this->sql_builder)
			{
				throw QueryError(
					"select: unable to retrieve not nullptr SQL query builder from driver",
					_ERROR_DETAILS_
				);
			}
		}

		return *this;
	}

	// Generates query using SQL driver.
	//
	// Throws 'QueryError' when driver is not set.
	[[nodiscard]]
	inline std::string query() const
	{
		if (!this->sql_builder)
		{
			throw QueryError("select: SQL query builder is not set", _ERROR_DETAILS_);
		}

		std::list<std::string> columns;
		util::tuple_for_each(ModelT::meta_columns, [&columns](auto& column) {
			columns.push_back(column.name);
		});
		return this->sql_builder->sql_select(
			this->table_name,
			columns,
			this->q_distinct,
			this->joins,
			this->q_where.value,
			this->q_order_by,
			this->q_limit,
			this->q_offset,
			this->q_group_by,
			this->q_having.value
		);
	}

	// TESTME: aggregate
	// Runs aggregate function for selected rows.
	//
	// Throws 'QueryError' when driver is not set.
	template <typename ReturnT>
	[[nodiscard]]
	inline ReturnT aggregate(const aggregate_function_t<ReturnT>& func) const
	{
		if (!this->sql_builder)
		{
			throw QueryError("select > " + func.name + ": SQL query builder is not set", _ERROR_DETAILS_);
		}

		auto query = this->sql_builder->sql_select_(
			this->table_name,
			(std::string)func + " AS agg_result",
			this->q_distinct,
			this->joins,
			this->q_where.value,
			this->q_order_by,
			this->q_limit,
			this->q_offset,
			this->q_group_by,
			this->q_having.value
		);

		if (!this->sql_driver)
		{
			throw QueryError("select > " + func.name + ": SQL driver is not set", _ERROR_DETAILS_);
		}

		using row_t = std::map<std::string, char*>;
		ReturnT result;
		this->sql_driver->run_select(query, &result, [](void* result_ptr, void* row_ptr) -> void {
			auto& row = *(row_t *)row_ptr;
			*(ReturnT *)result_ptr = util::as<ReturnT>(row["agg_result"]);
		});

		return result;
	}

	// TESTME: avg
	// Calculates average value of given column in selected rows.
	//
	// Throws 'QueryError' when driver is not set.
	template <column_type_c ColumnT>
	inline auto avg(ColumnT ModelT::* column) const
	{
		return this->template aggregate<double>(q::avg(column));
	}

	// TESTME: count
	// Calculates selected rows.
	//
	// Throws 'QueryError' when driver is not set.
	[[nodiscard]]
	inline auto count() const
	{
		return this->template aggregate<size_t>(q::count());
	}

	// TESTME: min
	// Calculates minimum value of given column in selected rows.
	//
	// Throws 'QueryError' when driver is not set.
	template <column_type_c ColumnT>
	inline auto min(ColumnT ModelT::* column) const
	{
		return this->template aggregate<ColumnT>(q::min(column));
	}

	// TESTME: max
	// Calculates maximum value of given column in selected rows.
	//
	// Throws 'QueryError' when driver is not set.
	template <column_type_c ColumnT>
	inline auto max(ColumnT ModelT::* column) const
	{
		return this->template aggregate<ColumnT>(q::max(column));
	}

	// TESTME: sum
	// Calculates sum by column of selected rows.
	//
	// Throws 'QueryError' when driver is not set.
	template <column_type_c ColumnT>
	inline auto sum(ColumnT ModelT::* column) const
	{
		return this->template aggregate<ColumnT>(q::sum(column));
	}

	// Sets the distinct value.
	inline select& distinct(bool v = true)
	{
		this->q_distinct = v;
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
	// `model_pk`: member pointer to primary key field of ModelT.
	// By default '&ModelT::id' is used.
	//
	// `foreign_key`: is used for joining of `ModelT` with `OtherModelT`.
	// It is foreign key in `OtherModelT` to `ModelT` table.
	// If `foreign_key` is empty it will be generated automatically using
	// `ModelT::meta_table_name` without last char (usually 's')
	// and '_id' suffix. For example:
	//   `ModelT::meta_table_name` equals to 'persons', so, the result
	//   will be 'person_id'.
	template <column_type_c PrimaryKeyT = size_t, typename OtherModelT>
	inline select& one_to_many(
		const std::function<void(ModelT&, const xw::Lazy<std::list<OtherModelT>>&)>& first,
		const std::function<void(OtherModelT&, const xw::Lazy<ModelT>&)>& second,
		PrimaryKeyT ModelT::* model_pk = &ModelT::id,
		std::string foreign_key=""
	)
	{
		auto fk_column = foreign_key.empty() ? meta::make_fk<ModelT>() : foreign_key;
		abc::ISQLDriver* driver = this->sql_driver;
		this->relations.push_back([driver, fk_column, first, second, model_pk](ModelT& model) -> void {
			auto pk_val = "'" + model.__get_attr__(meta::get_column_name(model_pk).c_str())->__str__() + "'";
			first(model, xw::Lazy<std::list<OtherModelT>>(
				[driver, fk_column, pk_val, first, second, model_pk]() -> std::list<OtherModelT> {
					return select<OtherModelT>().use(driver)
						.template many_to_one<PrimaryKeyT, ModelT>(second, first, model_pk, fk_column)
						.where(q::column_condition_t(
							meta::get_table_name<OtherModelT>(),
							util::quote_str(fk_column), "= " + pk_val
						))
						.all();
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
	template <column_type_c PrimaryKeyT = size_t, typename OtherModelT>
	inline select& one_to_many(
		xw::Lazy<std::list<OtherModelT>> ModelT::* left,
		xw::Lazy<ModelT> OtherModelT::* right,
		PrimaryKeyT ModelT::* model_pk = &ModelT::id,
		std::string foreign_key=""
	)
	{
		return this->template one_to_many<PrimaryKeyT, OtherModelT>(
			[left](ModelT& model, const xw::Lazy<std::list<OtherModelT>>& value) {
				model.*left = value;
			},
			[right](OtherModelT& model, const xw::Lazy<ModelT>& value) {
				model.*right = value;
			},
			model_pk,
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
	// `other_model_pk`: member pointer to primary key field of OtherModelT.
	// By default '&OtherModelT::id' is used.
	//
	// `foreign_key`: is used for joining of `ModelT` with `OtherModelT`.
	// It is foreign key in `ModelT` to `OtherModelT` table.
	// If `foreign_key` is empty it will be generated automatically using
	// `OtherModelT::meta_table_name` without last char (usually 's')
	// and '_id' suffix. For example:
	//   `OtherModelT::meta_table_name` equals to 'persons', so, the result
	//   will be 'person_id'.
	template <column_type_c PrimaryKeyT = size_t, typename OtherModelT>
	inline select& many_to_one(
		const std::function<void(ModelT&, const xw::Lazy<OtherModelT>&)>& first,
		const std::function<void(OtherModelT&, const xw::Lazy<std::list<ModelT>>&)>& second,
		PrimaryKeyT OtherModelT::* other_model_pk = &OtherModelT::id,
		const std::string& foreign_key=""
	)
	{
		auto fk_column = foreign_key.empty() ? meta::make_fk<OtherModelT>() : foreign_key;
		abc::ISQLDriver* driver = this->sql_driver;
		auto t_name = this->table_name;
		auto pk_name_str = this->pk_name;
		this->relations.push_back([
			driver, first, second, t_name, fk_column, other_model_pk, pk_name_str
		](ModelT& model) -> void {
			auto model_pk_val = "'" + model.__get_attr__(pk_name_str.c_str())->__str__() + "'";
			first(model, xw::Lazy<OtherModelT>(
				[driver, first, second, t_name, fk_column, model_pk_val, other_model_pk]() -> OtherModelT {
					return select<OtherModelT>().use(driver)
						.join(q::left_on<OtherModelT, ModelT>(fk_column))
						.template one_to_many<PrimaryKeyT, ModelT>(second, first, other_model_pk, fk_column)
						.where(q::column_condition_t(
							meta::get_table_name<ModelT>(),
							util::quote_str(meta::get_column_name(other_model_pk).c_str()), "= " + model_pk_val
						))
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
	template <column_type_c PrimaryKeyT = size_t, typename OtherModelT>
	inline select& many_to_one(
		xw::Lazy<OtherModelT> ModelT::* left,
		xw::Lazy<std::list<ModelT>> OtherModelT::* right,
		PrimaryKeyT OtherModelT::* other_model_pk = &OtherModelT::id,
		std::string foreign_key=""
	)
	{
		return this->template many_to_one<PrimaryKeyT, OtherModelT>(
			[left](ModelT& model, const xw::Lazy<OtherModelT>& value) {
				model.*left = value;
			},
			[right](OtherModelT& model, const xw::Lazy<std::list<ModelT>>& value) {
				model.*right = value;
			},
			other_model_pk,
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
	template <typename OtherModelT>
	inline select& many_to_many(
		const std::function<void(ModelT&, const Lazy<std::list<OtherModelT>>&)>& first,
		const std::function<void(OtherModelT&, const Lazy<std::list<ModelT>>&)>& second,
		std::string left_fk="", std::string right_fk="", std::string intermediate_table=""
	)
	{
		abc::ISQLDriver* driver = this->sql_driver;
		auto first_t_name = this->table_name;
		auto first_pk_name = this->pk_name;
		this->relations.push_back(
			[
				driver, first_t_name, first_pk_name, left_fk, right_fk, first, second, intermediate_table
			](ModelT& model) -> void {
				first(model, Lazy<std::list<OtherModelT>>(
					[
						driver, first_t_name, first_pk_name, left_fk, right_fk, first, second, intermediate_table
					]() -> std::list<OtherModelT> {
						std::string second_t_name = OtherModelT::meta_table_name;
						std::string m_table = intermediate_table;
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

						std::string s_pk = left_fk.empty() ? meta::make_fk<ModelT>() : left_fk;
						std::string o_pk = right_fk.empty() ? meta::make_fk<OtherModelT>() : right_fk;
						auto cond_str = '"' + second_t_name + "\".\"" + first_pk_name
							+ "\" = \"" + m_table + "\".\"" + s_pk + '"';

						return select<OtherModelT>().use(driver)
							.distinct()
							.join(join_t("LEFT", m_table, q::condition_t(cond_str)))
							.template many_to_many<ModelT>(second, first, o_pk, s_pk, m_table)
							.all();
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
	template <typename OtherModelT>
	inline select& many_to_many(
		Lazy<std::list<OtherModelT>> ModelT::*left,
		Lazy<std::list<ModelT>> OtherModelT::*right,
		std::string left_fk="", std::string right_fk="", std::string intermediate_table=""
	)
	{
		return this->template many_to_many<OtherModelT>(
			[left](ModelT& model, const Lazy<std::list<OtherModelT>>& value) {
				model.*left = value;
			},
			[right](OtherModelT& model, const Lazy<std::list<ModelT>>& value) {
				model.*right = value;
			},
			left_fk, right_fk, intermediate_table
		);
	}

	// Sets the condition for 'where' filtering.
	inline select& where(const q::condition_t& cond)
	{
		if (this->q_where.is_set)
		{
			this->q_where.set(this->q_where.value & cond);
		}
		else
		{
			this->q_where.set(cond);
		}

		return *this;
	}

	// Sets columns for ordering.
	inline select& order_by(const std::initializer_list<q::ordering>& columns)
	{
		if (columns.size())
		{
			this->q_order_by.insert(this->q_order_by.end(), columns.begin(), columns.end());
		}

		return *this;
	}

	// Sets the limit value.
	inline select& limit(size_t limit)
	{
		this->q_limit = limit;
		return *this;
	}

	// Sets the offset value.
	inline select& offset(size_t offset)
	{
		if (offset > 0)
		{
			this->q_offset = offset;
		}

		return *this;
	}

	// Sets columns for grouping.
	inline select& group_by(const std::initializer_list<std::string>& columns)
	{
		if (columns.size())
		{
			this->q_group_by.insert(this->q_group_by.end(), columns.begin(), columns.end());
		}

		return *this;
	}

	// Sets the condition for 'having' filtering.
	inline select& having(const q::condition_t& cond)
	{
		if (this->q_having.is_set)
		{
			this->q_having.set(this->q_having.value & cond);
		}
		else
		{
			this->q_having.set(cond);
		}

		return *this;
	}

	// Set limit value to 1, if it was not already set
	// and retrieves the first item of 'all()'
	// result. In case if 'all()' returns an empty
	// vector, returns null-model.
	//
	// Throws 'QueryError' when driver is not set.
	inline ModelT first()
	{
		this->limit(1);
		auto values = this->all();
		if (values.empty())
		{
			ModelT model;
			model.mark_as_null();
			return model;
		}

		return values.front();
	}

	// Performs an access to database. Runs SQL 'SELECT'
	// query and returns its result.
	//
	// Throws 'QueryError' when driver is not set.
	inline std::list<ModelT> all() const
	{
		auto query = this->query();
		using row_t = std::map<std::string, char*>;
		using data_t = std::pair<std::list<ModelT>, std::list<relation_callable>>;
		data_t collection{{}, this->relations};
		this->sql_driver->run_select(query, &collection, [](void* container_ptr, void* row_ptr) -> void {
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
