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
#include "../abc.h"
#include "../exceptions.h"


__ORM_Q_BEGIN__

template <db::model_based_type_c ModelT>
class insert final
{
	static_assert(ModelT::meta_table_name != nullptr, "insert: 'meta_table_name' is not initialized");

protected:

	// Driver to perform an access to the database.
	abc::ISQLDriver* sql_driver = nullptr;

	// Holds columns names.
	// Generates during the first model appending.
	std::string columns_str;

	// Collection of rows to insert.
	std::list<std::string> rows;

protected:

	// Converts model into row (string) and appends it to `rows`.
	//
	// `is_first`: used to indicate if `append` is called for the
	// first time. If true, than generates `columns_str`.
	inline void append_row(const ModelT& model)
	{
		std::string row;
		util::tuple_for_each(ModelT::meta_columns, [&row, model](auto& column)
		{
			if constexpr (ModelT::meta_omit_pk)
			{
				if (column.is_pk)
				{
					return true;
				}
			}

//			using field_type = typename std::remove_reference<decltype(column)>::type;
//			row += db::get_column_value_as_string<ModelT, typename field_type::field_type>(model, column) + ", ";
			row += column.as_string(model) + ", ";
			return true;
		});

		this->rows.push_back(str::rtrim(row, ", "));
	}

public:

	// Appends model to insertion list.
	//
	// Throws 'QueryError' if model is null.
	inline explicit insert(const ModelT& model)
	{
		if (model.is_null())
		{
			throw QueryError(
				"insert: unable to insert null model", _ERROR_DETAILS_
			);
		}

		std::string columns;
		util::tuple_for_each(ModelT::meta_columns, [&columns](auto& column)
		{
			if constexpr (ModelT::meta_omit_pk)
			{
				if (column.is_pk)
				{
					return true;
				}
			}

			columns += column.name + ", ";
			return true;
		});

		this->columns_str = str::rtrim(columns, ", ");
		this->append_row(model);
	};

	// Sets SQL driver.
	virtual insert& use(abc::ISQLDriver* driver)
	{
		if (driver)
		{
			this->sql_driver = driver;
		}

		return *this;
	}

	// Generates query using SQL driver.
	//
	// Throws 'QueryError' when driver is not set.
	[[nodiscard]]
	inline std::string query() const
	{
		if (!this->sql_driver)
		{
			throw QueryError("insert: SQL driver is not set", _ERROR_DETAILS_);
		}

		auto sql_builder = this->sql_driver->query_builder();
		if (!sql_builder)
		{
			throw QueryError("insert: SQL query builder is initialized", _ERROR_DETAILS_);
		}

		return sql_builder->sql_insert(
			db::get_table_name<ModelT>(), this->columns_str, this->rows
		);
	}

	// Appends model to insertion list.
	//
	// Throws 'QueryError' if model is null.
	inline insert& model(const ModelT& model)
	{
		if (model.is_null())
		{
			throw QueryError(
				"insert: unable to insert null model", _ERROR_DETAILS_
			);
		}

		this->append_row(model);
		return *this;
	}

	// Inserts one row and returns inserted pk as string.
	//
	// Throws 'QueryError' if more than one model was set.
	inline void commit_one() const
	{
		if (this->rows.size() > 1)
		{
			throw QueryError(
				"insert: trying to insert one model, but multiple models were set",
				_ERROR_DETAILS_
			);
		}

		auto query = this->query();
		this->sql_driver->run_insert(query);
	}

	// Inserts one row and sets inserted primary key
	// to `pk` as type T.
	template <db::column_field_type_c T>
	inline void commit_one(T& pk) const
	{
		if (this->rows.size() > 1)
		{
			throw QueryError(
				"insert: trying to insert one model, but multiple models were set",
				_ERROR_DETAILS_
			);
		}

		auto query = this->query();
		std::string raw_pk;
		this->sql_driver->run_insert(query, raw_pk);
		pk = xw::util::as<T>(raw_pk.c_str());
	}

	// Inserts row(s) into database.
	inline void commit_batch() const
	{
		auto query = this->query();
		this->sql_driver->run_insert(query);
	}
};

__ORM_Q_END__
