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
#include <optional>

// Base libraries.
#include <xalwart.base/types/string.h>
#include <xalwart.base/lazy.h>
#include <xalwart.base/utility.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./functions.h"
#include "./abstract_query.h"


__ORM_Q_BEGIN__

// TESTME: add tests with mocked driver
template <db::model_based_type ModelType>
class Select final : public AbstractQuery<ModelType>
{
public:
	inline explicit Select(
		xw::abc::orm::DatabaseConnection* connection, abc::SQLQueryBuilder* query_builder
	) : AbstractQuery<ModelType>(connection, query_builder), q_distinct(false), q_limit(-1), q_offset(-1)
	{
		this->table_name = db::get_table_name<ModelType>();
		this->pk_name = db::get_pk_name<ModelType>();
		if (this->pk_name.empty())
		{
			throw QueryError("Model requires pk column", _ERROR_DETAILS_);
		}
	};

	// Throws 'QueryError' when driver is not set.
	[[nodiscard]]
	inline std::string to_sql() const override
	{
		std::list<std::string> columns;
		util::tuple_for_each(ModelType::meta_columns, [&columns](auto& column) {
			columns.push_back(column.name);
		});
		auto where_condition = this->q_where.has_value() ? this->q_where.value() : Condition("");
		auto having_condition = this->q_having.has_value() ? this->q_having.value() : Condition("");
		return require_non_null(
			this->query_builder, "SQL query builder is not initialized", _ERROR_DETAILS_
		)->sql_select(
			this->table_name,
			columns,
			this->q_distinct,
			this->joins,
			where_condition,
			this->q_order_by,
			this->q_limit,
			this->q_offset,
			this->q_group_by,
			having_condition
		);
	}

	// TESTME: aggregate
	// Runs aggregate function for selected rows.
	//
	// Throws 'QueryError' when driver is not set.
	template <typename ReturnType>
	[[nodiscard]]
	inline ReturnType aggregate(const AggregateFunction<ReturnType>& func) const
	{
		std::string result_key = "agg_result";
		auto where_condition = this->q_where.has_value() ? this->q_where.value() : Condition("");
		auto having_condition = this->q_having.has_value() ? this->q_having.value() : Condition("");
		auto query = require_non_null(
			this->query_builder, func.name + ": SQL query builder is not initialized", _ERROR_DETAILS_
		)->sql_select_(
			this->table_name,
			(std::string)func + " AS " + result_key,
			this->q_distinct,
			this->joins,
			where_condition,
			this->q_order_by,
			this->q_limit,
			this->q_offset,
			this->q_group_by,
			having_condition
		);
		ReturnType result;
		require_non_null(
			this->db_connection, func.name + ": SQL driver is not initialized", _ERROR_DETAILS_
		)->run_query(query, [&result, result_key](const auto& map) -> void {
			result = xw::util::as<ReturnType>(map[result_key]);
		}, nullptr);
		return result;
	}

	// TESTME: avg
	// Calculates average value of given column in selected rows.
	//
	// Throws 'QueryError' when driver is not set.
	template <db::column_field_type ColumnType>
	inline auto avg(ColumnType ModelType::* column) const
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
	template <db::column_field_type ColumnType>
	inline auto min(ColumnType ModelType::* column) const
	{
		return this->template aggregate<ColumnType>(q::min(column));
	}

	// TESTME: max
	// Calculates maximum value of given column in selected rows.
	//
	// Throws 'QueryError' when driver is not set.
	template <db::column_field_type ColumnType>
	inline auto max(ColumnType ModelType::* column) const
	{
		return this->template aggregate<ColumnType>(q::max(column));
	}

	// TESTME: sum
	// Calculates sum by column of selected rows.
	//
	// Throws 'QueryError' when driver is not set.
	template <db::column_field_type ColumnType>
	inline auto sum(ColumnType ModelType::* column) const
	{
		return this->template aggregate<ColumnType>(q::sum(column));
	}

	inline Select& distinct(bool v = true)
	{
		this->q_distinct = v;
		return *this;
	}

	// Sets SQL `join` condition of two tables. For more info,
	// check the `xw::q::join` class and related functions.
	inline Select& join(q::Join join)
	{
		this->joins.push_back(std::move(join));
		return *this;
	}

	// TESTME: one_to_many
	// Retrieves models with lazy initialization connected
	// with one to many relationship.
	//
	// `first`: a lambda function, is used to set the vector
	// to each of the selected `ModelType` objects via an address to
	// class field of `OtherModelType`.
	//
	// `second`: a lambda function, is used to set an object
	// to each of the selected `OtherModelType` objects via an address to
	// class field of `ModelType`.
	//
	// `model_pk`: member pointer to primary key field of ModelType.
	// By default '&ModelT::id' is used.
	//
	// `foreign_key`: is used for joining of `ModelType` with `OtherModelType`.
	// It is foreign key in `OtherModelType` to `ModelType` table.
	// If `foreign_key` is empty it will be generated automatically using
	// `ModelType::meta_table_name` without last char (usually 's')
	// and '_id' suffix. For example:
	//   `ModelType::meta_table_name` equals to 'persons', so, the result
	//   will be 'person_id'.
	template <db::column_field_type PrimaryKeyT = size_t, typename OtherModelType>
	inline Select& one_to_many(
		const std::function<void(ModelType&, const xw::Lazy<std::list<OtherModelType>>&)>& first,
		const std::function<void(OtherModelType&, const xw::Lazy<ModelType>&)>& second,
		PrimaryKeyT ModelType::* model_pk = &ModelType::id,
		const std::string& foreign_key=""
	)
	{
		auto fk_column = foreign_key.empty() ? db::make_fk<ModelType>() : foreign_key;
		auto* connection = this->db_connection;
		auto* builder = this->query_builder;
		this->relations.push_back([connection, builder, fk_column, first, second, model_pk](ModelType& model) -> void {
			auto pk_val = "'" + model.__get_attr__(db::get_column_name(model_pk).c_str())->__str__() + "'";
			first(model, xw::Lazy<std::list<OtherModelType>>(
				[connection, builder, fk_column, pk_val, first, second, model_pk]() -> std::list<OtherModelType> {
					return select<OtherModelType>(connection, builder)
						.template many_to_one<PrimaryKeyT, ModelType>(second, first, model_pk, fk_column)
							.where(q::ColumnCondition(
							db::get_table_name<OtherModelType>(),
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
	template <db::column_field_type PrimaryKeyT = size_t, typename OtherModelType>
	inline Select& one_to_many(
		xw::Lazy<std::list<OtherModelType>> ModelType::* left,
		xw::Lazy<ModelType> OtherModelType::* right,
		PrimaryKeyT ModelType::* model_pk = &ModelType::id,
		const std::string& foreign_key=""
	)
	{
		return this->template one_to_many<PrimaryKeyT, OtherModelType>(
			[left](ModelType& model, const xw::Lazy<std::list<OtherModelType>>& value) {
				model.*left = value;
			},
			[right](OtherModelType& model, const xw::Lazy<ModelType>& value) {
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
	// to each of the selected `ModelType` objects via an address to
	// class field of `OtherModelType`.
	//
	// `second`: a lambda function, is used to set the vector
	// to each of the selected `OtherModelType` objects via an address to
	// class field of `ModelType`.
	//
	// `other_model_pk`: member pointer to primary key field of OtherModelType.
	// By default '&OtherModelT::id' is used.
	//
	// `foreign_key`: is used for joining of `ModelType` with `OtherModelType`.
	// It is foreign key in `ModelType` to `OtherModelType` table.
	// If `foreign_key` is empty it will be generated automatically using
	// `OtherModelType::meta_table_name` without last char (usually 's')
	// and '_id' suffix. For example:
	//   `OtherModelType::meta_table_name` equals to 'persons', so, the result
	//   will be 'person_id'.
	template <db::column_field_type PrimaryKeyT = size_t, typename OtherModelType>
	inline Select& many_to_one(
		const std::function<void(ModelType&, const xw::Lazy<OtherModelType>&)>& first,
		const std::function<void(OtherModelType&, const xw::Lazy<std::list<ModelType>>&)>& second,
		PrimaryKeyT OtherModelType::* other_model_pk = &OtherModelType::id,
		const std::string& foreign_key=""
	)
	{
		auto fk_column = foreign_key.empty() ? db::make_fk<OtherModelType>() : foreign_key;
		auto* connection = this->db_connection;
		auto* builder = this->query_builder;
		auto t_name = this->table_name;
		auto pk_name_str = this->pk_name;
		this->relations.push_back([
			connection, builder, first, second, t_name, fk_column, other_model_pk, pk_name_str
		](ModelType& model) -> void {
			auto model_pk_val = "'" + model.__get_attr__(pk_name_str.c_str())->__str__() + "'";
			first(model, xw::Lazy<OtherModelType>(
				[
					connection, builder, first, second, t_name, fk_column, model_pk_val, other_model_pk
				]() -> OtherModelType {
					return select<OtherModelType>(connection, builder)
						.join(q::left_on<OtherModelType, ModelType>(fk_column))
						.template one_to_many<PrimaryKeyT, ModelType>(second, first, other_model_pk, fk_column)
						.where(q::ColumnCondition(
							db::get_table_name<ModelType>(),
							util::quote_str(db::get_column_name(other_model_pk).c_str()), "= " + model_pk_val
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
	template <db::column_field_type PrimaryKeyT = size_t, typename OtherModelType>
	inline Select& many_to_one(
		xw::Lazy<OtherModelType> ModelType::* left,
		xw::Lazy<std::list<ModelType>> OtherModelType::* right,
		PrimaryKeyT OtherModelType::* other_model_pk = &OtherModelType::id,
		const std::string& foreign_key=""
	)
	{
		return this->template many_to_one<PrimaryKeyT, OtherModelType>(
			[left](ModelType& model, const xw::Lazy<OtherModelType>& value) {
				model.*left = value;
			},
			[right](OtherModelType& model, const xw::Lazy<std::list<ModelType>>& value) {
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
	// to each of the selected `ModelType` objects via an address to
	// class field of `OtherModelType`.
	//
	// `second`: a lambda function which is used to set the vector
	// to each of the selected `OtherModelType` objects via an address to
	// class field of `ModelType`.
	//
	// `left_fk`: is used for joining of `ModelType` with intermediate
	// table. It is foreign key in middle table to `ModelType` table.
	// If `left_fk` is empty it will be generated automatically
	// using `ModelType::meta_table_name` without last char (usually 's')
	// and '_id' suffix. For example:
	//   `ModelType::meta_table_name` equals to 'persons', so, the result
	//   will be 'person_id'.
	//
	// `right_fk`: is used for joining of `OtherModelType` with intermediate
	// table. It is foreign key in middle table to `OtherModelType` table.
	// If `right_fk` is empty it will be generated automatically
	// using `OtherModelType::meta_table_name` without last char
	// (usually 's') and '_id' suffix. For example:
	//   `OtherModelType::meta_table_name` equals to 'cars', so, the result
	//   will be 'car_id'.
	//
	// `mid_table`: an intermediate table for many to many relationship.
	// If it is empty, it will be created using `ModelType::meta_table_name`
	// and `OtherModelType::meta_table_name` in an alphabetical order separated
	// by underscore ('_'). For example:
	//   `ModelType::meta_table_name` is 'persons' and `OtherModelType::meta_table_name`
	//   is 'cars', so, the result will be 'cars_persons'.
	template <typename OtherModelType>
	inline Select& many_to_many(
		const std::function<void(ModelType&, const Lazy<std::list<OtherModelType>>&)>& first,
		const std::function<void(OtherModelType&, const Lazy<std::list<ModelType>>&)>& second,
		const std::string& left_fk="", const std::string& right_fk="", const std::string& intermediate_table=""
	)
	{
		auto* connection = this->db_connection;
		auto* builder = this->query_builder;
		auto first_t_name = this->table_name;
		auto first_pk_name = this->pk_name;
		this->relations.push_back(
			[
				connection, builder, first_t_name, first_pk_name, left_fk, right_fk, first, second, intermediate_table
			](ModelType& model) -> void {
				first(model, Lazy<std::list<OtherModelType>>(
					[
						connection, first_t_name, first_pk_name, left_fk, right_fk, first, second, intermediate_table
					]() -> std::list<OtherModelType> {
						std::string second_t_name = OtherModelType::meta_table_name;
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

						std::string s_pk = left_fk.empty() ? db::make_fk<ModelType>() : left_fk;
						std::string o_pk = right_fk.empty() ? db::make_fk<OtherModelType>() : right_fk;
						auto cond_str = '"' + second_t_name + "\".\"" + first_pk_name
							+ "\" = \"" + m_table + "\".\"" + s_pk + '"';

						return Select<OtherModelType>(connection, builder)
							.distinct()
							.join(Join("LEFT", m_table, q::Condition(cond_str)))
							.template many_to_many<ModelType>(second, first, o_pk, s_pk, m_table)
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
	template <typename OtherModelType>
	inline Select& many_to_many(
		Lazy<std::list<OtherModelType>> ModelType::*left,
		Lazy<std::list<ModelType>> OtherModelType::*right,
		const std::string& left_fk="", const std::string& right_fk="", const std::string& intermediate_table=""
	)
	{
		return this->template many_to_many<OtherModelType>(
			[left](ModelType& model, const Lazy<std::list<OtherModelType>>& value) {
				model.*left = value;
			},
			[right](OtherModelType& model, const Lazy<std::list<ModelType>>& value) {
				model.*right = value;
			},
			left_fk, right_fk, intermediate_table
		);
	}

	// Sets the condition for 'where' filtering.
	inline Select& where(const q::Condition& condition)
	{
		this->q_where = this->q_where.has_value() ? this->q_where.value() & condition : condition;
		return *this;
	}

	// Sets columns for ordering.
	inline Select& order_by(const std::initializer_list<q::Ordering>& columns)
	{
		if (columns.size())
		{
			this->q_order_by.insert(this->q_order_by.end(), columns.begin(), columns.end());
		}

		return *this;
	}

	// Sets the limit value.
	inline Select& limit(size_t limit)
	{
		this->q_limit = limit;
		return *this;
	}

	// Sets the offset value.
	inline Select& offset(size_t offset)
	{
		if (offset > 0)
		{
			this->q_offset = offset;
		}

		return *this;
	}

	// Sets columns for grouping.
	inline Select& group_by(const std::initializer_list<std::string>& columns)
	{
		if (columns.size())
		{
			this->q_group_by.insert(this->q_group_by.end(), columns.begin(), columns.end());
		}

		return *this;
	}

	// Sets the condition for 'having' filtering.
	inline Select& having(const q::Condition& condition)
	{
		this->q_having = this->q_having.has_value() ? this->q_having.value() & condition : condition;
		return *this;
	}

	// Set limit value to 1, if it was not already set
	// and retrieves the first item of 'all()'
	// result. In case if 'all()' returns an empty
	// vector, returns null-model.
	//
	// Throws 'QueryError' when driver is not set.
	inline ModelType first()
	{
		this->limit(1);
		auto values = this->all();
		if (values.empty())
		{
			ModelType model;
			model.mark_as_null();
			return model;
		}

		return values.front();
	}

	// Performs an access to database. Runs SQL 'SELECT'
	// query and returns its result.
	//
	// Throws 'QueryError' when driver is not set.
	[[nodiscard]]
	inline std::list<ModelType> all() const
	{
		std::pair<std::list<ModelType>, std::list<relation_callable>> collection{{}, this->relations};
		require_non_null(
			this->db_connection, "SQL Database connection is not initialized", _ERROR_DETAILS_
		)->run_query(this->to_sql(), [&collection](const auto& map) -> void {
			ModelType model;
			model.from_map(map);

			for (auto& callable : collection.second)
			{
				callable(model);
			}

			collection.first.push_back(model);
		}, nullptr);

		return collection.first;
	}

	// TESTME: delete_
	// Deletes selected rows without retrieving them from the database.
	inline void delete_() const
	{
		auto pk_col = util::quote_str(this->table_name) + "." + util::quote_str(this->pk_name);
		auto where_condition = this->q_where.has_value() ? this->q_where.value() : Condition("");
		auto having_condition = this->q_having.has_value() ? this->q_having.value() : Condition("");
		auto select_query = require_non_null(
			this->query_builder, "SQL query builder is not initialized", _ERROR_DETAILS_
		)->sql_select_(
			this->table_name,
			pk_col,
			this->q_distinct,
			this->joins,
			where_condition,
			this->q_order_by,
			this->q_limit,
			this->q_offset,
			this->q_group_by,
			having_condition
		);
		select_query.pop_back();
		auto query = this->query_builder->sql_delete(
			this->table_name, Condition(pk_col + " IN (" + select_query + ")")
		);
		require_non_null(
			this->db_connection, "SQL Database connection is not initialized", _ERROR_DETAILS_
		)->run_query(query, nullptr, nullptr);
	}

protected:
	// Retrieves automatically, check the default constructor.
	std::string table_name;

	// Retrieves automatically from Model meta,
	// check the default constructor.
	std::string pk_name;

	// Indicates whether to use distinction or not in SQL statement.
	// The default is false.
	bool q_distinct;

	// Holds boolean condition for SQL 'WHERE' statement.
	std::optional<q::Condition> q_where;

	// Holds columns list for SQL 'ORDER BY' statement.
	std::list<q::Ordering> q_order_by;

	// Holds value for SQL 'LIMIT'. The default is -1.
	long int q_limit;

	// Holds value for SQL 'OFFSET'. The default is -1.
	long int q_offset;

	// Holds columns list for SQL 'GROUP BY' statement.
	std::list<std::string> q_group_by;

	// Holds boolean condition for SQL 'HAVING' statement.
	std::optional<q::Condition> q_having;

	// Holds a list of conditions for SQL 'JOIN' statement.
	std::list<q::Join> joins;

	typedef std::function<void(ModelType& model)> relation_callable;

	// Holds a list of lambda-functions which must be
	// called for each selected object to set lazy
	// initializers.
	std::list<relation_callable> relations;
};

__ORM_Q_END__
