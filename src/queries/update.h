/**
 * queries/update.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Wrapper for SQL 'UPDATE' statement.
 */

#pragma once

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./abstract_query.h"
#include "../exceptions.h"


__ORM_Q_BEGIN__

template <db::model_based_type ModelType>
class Update final : public AbstractQuery<ModelType>
{
public:
	inline explicit Update(
		abc::IDatabaseConnection* connection, abc::ISQLQueryBuilder* builder, bool in_transaction=false
	) : AbstractQuery<ModelType>(connection, builder)
	{
		this->in_transaction = in_transaction;
		this->table_name = db::get_table_name<ModelType>();
	};

	// Throws 'QueryError' when driver is not set.
	[[nodiscard]]
	inline std::string to_sql() const override
	{
		require_non_null(this->query_builder, "SQL query builder is not initialized", _ERROR_DETAILS_);
		return str::join(
			" ", this->rows.begin(), this->rows.end(), [this](const auto& row) -> std::string {
				return this->query_builder->sql_update(this->table_name, row.first, row.second);
			}
		);
	}

	// Throws 'QueryError' if model is null.
	inline Update& model(const ModelType& model)
	{
		this->append_model(model);
		return *this;
	}

	// Updates single row in database.
	inline void commit_one() const
	{
		if (this->rows.size() > 1)
		{
			throw QueryError("Trying to update one model, but multiple models were set", _ERROR_DETAILS_);
		}

		this->db_connection->run_query(this->to_sql(), nullptr, nullptr);
	}

	// Updates multiple rows in database.
	inline void commit_batch() const
	{
		if (!this->in_transaction)
		{
			this->db_connection->begin_transaction();
		}

		this->db_connection->run_query(this->to_sql(), nullptr, nullptr);
		if (!this->in_transaction)
		{
			this->db_connection->end_transaction();
		}
	}

protected:
	// Marks if committing will be executed already in transaction.
	// If 'false', 'commit_batch()' will wrap an SQL query in transaction.
	bool in_transaction;

	// Name of the table which is retrieved from
	// `ModelType::meta_table_name` static member.
	std::string table_name;

	// Holds rows to update.
	//
	// `first`: columns with new values separated by comma.
	// Example: "name = 'Steve', age = 21".
	//
	// `second`: condition for 'WHERE' statement.
	//	Indicates what rows should be updated.
	std::vector<std::pair<std::string, q::Condition>> rows;

	inline void append_model(const ModelType& model)
	{
		if (model.is_null())
		{
			throw QueryError("Unable to update null model", _ERROR_DETAILS_);
		}

		std::string pk_name, pk_val;
		std::pair<std::string, q::Condition> row_data;
		util::tuple_for_each(ModelType::meta_columns, [model, &pk_name, &pk_val, &row_data](auto& column)
		{
			using field_type = typename std::remove_reference<decltype(column)>::type;
			using T = typename field_type::field_type;

			if (column.is_pk)
			{
				pk_val = column.as_string(model);
				pk_name = column.name;

				if constexpr (ModelType::meta_omit_pk)
				{
					return true;
				}
			}

			row_data.first += column.name + " = " + column.as_string(model) + ", ";
			return true;
		});

		row_data.first = str::rtrim(row_data.first, ", ");
		row_data.second = q::ColumnCondition(this->table_name, pk_name, "= " + pk_val);
		this->rows.push_back(row_data);
	}
};

__ORM_Q_END__
