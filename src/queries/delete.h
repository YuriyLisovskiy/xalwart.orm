/**
 * queries/delete.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Wrapper for SQL 'DELETE' statement.
 */

#pragma once

// C++ libraries.
#include <optional>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./abstract_query.h"
#include "./conditions.h"
#include "../exceptions.h"


__ORM_Q_BEGIN__

template <db::model_based_type ModelType>
class Delete final : public AbstractQuery<ModelType>
{
public:
	inline explicit Delete(
		xw::abc::orm::DatabaseConnection* connection, abc::SQLQueryBuilder* query_builder
	) : AbstractQuery<ModelType>(connection, query_builder)
	{
	}

	// Throws 'QueryError' when driver is not set.
	[[nodiscard]]
	inline std::string to_sql() const override
	{
		require_non_null(this->query_builder, "SQL builder is not initialized", _ERROR_DETAILS_);
		auto condition = this->where_condition;
		if (!condition.has_value())
		{
			util::tuple_for_each(ModelType::meta_columns, [this, &condition](auto& column)
			{
				if (column.is_pk)
				{
					condition = ColumnCondition(
						db::get_table_name<ModelType>(),
						column.name,
						"IN (" + str::join(", ", this->primary_keys.begin(), this->primary_keys.end()) + ")"
					);
					return false;
				}

				return true;
			});
		}

		return this->query_builder->sql_delete(db::get_table_name<ModelType>(), condition.value());
	}

	// Appends model's pk to deletion list.
	inline Delete& model(const ModelType& model)
	{
		this->append_model(model);
		return *this;
	}

	// Sets the condition for 'where' filtering.
	inline Delete& where(const Condition& condition)
	{
		if (this->where_condition.has_value())
		{
			this->where_condition = this->where_condition.value() & condition;
		}
		else
		{
			this->where_condition = condition;
		}

		return *this;
	}

	// Performs the deletion.
	// If no models were set, executes `DELETE` without
	// condition, otherwise generates it from primary keys
	// if it was not set manually.
	inline void commit() const
	{
		require_non_null(
			this->db_connection, "SQL Database connection is not initialized", _ERROR_DETAILS_
		)->run_query(this->to_sql(), nullptr, nullptr);
	}

protected:
	// Holds condition for SQL 'WHERE' statement.
	std::optional<Condition> where_condition;

	// List of primary keys to delete. It will be used by
	// default if `where` is not called.
	std::vector<std::string> primary_keys{};

	inline void append_model(const ModelType& model)
	{
		if (model.is_null())
		{
			throw QueryError("Unable to delete null model", _ERROR_DETAILS_);
		}

		util::tuple_for_each(ModelType::meta_columns, [this, model](auto& column)
		{
			if (column.is_pk)
			{
				this->primary_keys.push_back(column.as_string(model));
				return false;
			}

			return true;
		});
	}
};

__ORM_Q_END__
