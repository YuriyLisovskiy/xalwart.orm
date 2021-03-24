/**
 * queries/insert.h
 *
 * Copyright (c) 2021 Yuriy Lisovskiy
 *
 * Purpose: wrapper for SQL 'INSERT' statement.
 */

#pragma once

// Core libraries.
#include <xalwart.core/types/string.h>

// Module definitions.
#include "./_def_.h"

// Orm libraries.
#include "../abc.h"
#include "../exceptions.h"


__Q_BEGIN__

template <ModelBasedType ModelT>
class insert final
{
	static_assert(ModelT::meta_table_name != nullptr, "'meta_table_name' is not initialized");

protected:

	// Driver to perform an access to the database.
	abc::ISQLDriver* db = nullptr;

	// Holds columns names.
	// Generates during the first model appending.
	std::string columns_str;

	// Collection of rows to insert.
	std::vector<std::string> rows;

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

			using field_type = typename std::remove_reference<decltype(column)>::type;
//			using T = typename field_type::field_type;

			row += get_column_value_as_string<ModelT, typename field_type::field_type>(model, column) + ", ";

//			if constexpr (std::is_fundamental_v<T>)
//			{
//				row += std::to_string(model.*column.member_pointer);
//			}
//			else if constexpr (std::is_same_v<T, std::string>)
//			{
//				row += "'" + model.*column.member_pointer + "'";
//			}

//			row += ", ";
			return true;
		});

		str::rtrim(row, ", ");
		this->rows.push_back(row);
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

		this->columns_str = columns;
		str::rtrim(this->columns_str, ", ");
		this->append_row(model);
	};

	// Sets SQL driver.
	virtual insert& use(abc::ISQLDriver* driver)
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
	inline std::string query() const
	{
		if (!this->db)
		{
			throw QueryError("insert: database driver not set", _ERROR_DETAILS_);
		}

		return this->db->make_insert_query(
			meta::get_table_name<ModelT>(), this->columns_str, this->rows
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
	[[nodiscard]]
	inline std::string commit_one() const
	{
		if (this->rows.size() > 1)
		{
			throw QueryError(
				"insert: trying to insert one model, but multiple models were set",
				_ERROR_DETAILS_
			);
		}

		auto query = this->query();
		return this->db->run_insert(query);
	}

	// Inserts one row and sets inserted primary key
	// to `pk` as type T.
	template <typename T>
	inline void commit_one(T& pk) const
	{
		pk = util::as<T>(this->commit_one().c_str());
	}

	// Inserts row(s) into database.
	inline void commit_batch() const
	{
		auto query = this->query();
		this->db->run_insert(query);
	}
};

__Q_END__
