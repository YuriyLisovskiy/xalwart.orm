/**
 * queries/insert.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Wrapper for SQL 'INSERT' statement.
 */

#pragma once

// Base libraries.
#include <xalwart.base/utility.h>
#include <xalwart.base/types/string.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "./abstract_query.h"
#include "../exceptions.h"


__ORM_Q_BEGIN__

template <db::model_based_type ModelType>
class Insert final : public AbstractQuery<ModelType>
{
public:
	inline explicit Insert(
		IDatabaseConnection* connection, ISQLQueryBuilder* builder
	) : AbstractQuery<ModelType>(connection, builder)
	{
	}

	// Throws 'QueryError' when driver is not set.
	[[nodiscard]]
	inline std::string to_sql() const override
	{
		return require_non_null(
			this->query_builder, "SQL query builder is not initialized", _ERROR_DETAILS_
		)->sql_insert(db::get_table_name<ModelType>(), this->columns_line, this->rows);
	}

	// Throws 'QueryError' if model is null.
	inline Insert& model(const ModelType& model)
	{
		if (model.is_null())
		{
			throw QueryError("Unable to insert null model", _ERROR_DETAILS_);
		}

		if (this->rows.empty())
		{
			this->build_columns_line(model);
		}

		this->append_model(model);
		return *this;
	}

	// Inserts one row and returns inserted pk as string.
	//
	// Throws 'QueryError' if more than one model was set.
	inline void commit_one() const
	{
		if (this->rows.size() > 1)
		{
			throw QueryError("Trying to insert one model, but multiple models were set", _ERROR_DETAILS_);
		}

		this->db_connection->run_query(this->to_sql(), nullptr, nullptr);
	}

	// Inserts one row and sets inserted primary key
	// to `pk` as type T.
	template <db::column_field_type T>
	inline void commit_one(T& pk) const
	{
		if (this->rows.size() > 1)
		{
			throw QueryError("Trying to insert one model, but multiple models were set", _ERROR_DETAILS_);
		}

		auto query = this->to_sql();
		std::string raw_pk;
		this->db_connection->run_query(query, raw_pk);
		if (!raw_pk.empty())
		{
			pk = xw::util::as<T>(raw_pk.c_str());
		}
	}

	inline void commit_batch() const
	{
		this->db_connection->run_query(this->to_sql(), nullptr, nullptr);
	}

protected:
	// Holds columns names.
	// Generates during the first model appending.
	std::string columns_line;

	// Collection of rows to insert.
	std::list<std::string> rows;

	// Converts model into row (string) and appends it to `rows`.
	//
	// `is_first`: used to indicate if `append` is called for the
	// first time. If true, then generates `columns_line`.
	inline void append_model(const ModelType& model)
	{
		std::string row;
		util::tuple_for_each(ModelType::meta_columns, [&row, model](auto& column)
		{
			if constexpr (ModelType::meta_omit_pk)
			{
				if (column.is_pk)
				{
					return true;
				}
			}

			row += column.as_string(model) + ", ";
			return true;
		});

		this->rows.push_back(str::rtrim(row, ", "));
	}

	inline void build_columns_line(const ModelType& model)
	{
		util::tuple_for_each(ModelType::meta_columns, [this](auto& column)
		{
			if constexpr (ModelType::meta_omit_pk)
			{
				if (column.is_pk)
				{
					return true;
				}
			}

			this->columns_line += column.name + ", ";
			return true;
		});

		this->columns_line = str::rtrim(this->columns_line, ", ");
	}
};

__ORM_Q_END__
